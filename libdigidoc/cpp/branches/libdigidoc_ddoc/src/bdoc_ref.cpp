#if defined(_WIN32)
// for converting between DOS and Windows code pages with OemToChar()
#include <windows.h> 
#else
namespace 
{
inline bool	OemToChar( char const*, char* )
{
	// other platforms do not have special console code pages?
	return true;
}
}
#endif

#include <iostream>
#include <algorithm>
#include <list>
#include "log.h"
#include "BDoc.h"
#include "Conf.h"
#include "crypto/crypt/RSACrypt.h"
#include "crypto/signer/Signer.h"
#include "crypto/signer/RSASigner.h"
#include "crypto/signer/EstEIDSigner.h"
#include "crypto/cert/DirectoryX509CertStore.h"
#include "io/ZipSerialize.h"

namespace digidoc
{
namespace
{
/// Commander is local class for high level BDoc manipulation.
class Commander
{
public:
	Commander( char const* progName )
			: _progName( progName )
			, _bDoc( NULL )
	{
		digidoc::initialize();
		digidoc::X509CertStore::init( new DirectoryX509CertStore() );
	}

	~Commander()
	{
		delete _bDoc;
		digidoc::X509CertStore::destroy();
		digidoc::terminate();
	}

	enum
	{
		COMMAND_COUNT = 16,
		MAX_ARGUMENT_COUNT = 10,
		OK = 0,
		NOT_IMPLEMENTED = 255,
		INVALID_ARGUMENTS = 254,
		BDOC_EXCEPTION = 253,
		IO_EXCEPTION = 252,
		SIGN_EXCEPTION = 251,
		PLAIN_EXCEPTION = 250,
		UNKNOWN_EXCEPTION = 249,
		INVALID_COMMAND = 248,
	};

	/// Each command has Executor that takes arguments in style of C main() function.
	/// @returns int, meaning of value depends on command.
	/// @param increment returns how many arguments the command took from argument list.
	/// @param quitting returns if the execution has to stop after this command.
	/// @throws may throw whatever it decides to throw
	typedef int ( Commander::*Executor )( size_t argc, char* argv[], bool& quitting, size_t& increment );
	struct Command
	{
		const char* str;
		size_t params;
		Executor exec;
		const char* help;
	};
	static Command _commands[COMMAND_COUNT];

	/// Lists valid command names to std::cout.
	static void listCommands()
	{
		std::cout << "Valid commands are:" << std::endl;
		for ( size_t i = 0, l = 0; i < COMMAND_COUNT; ++i )
		{
			std::cout << _commands[i].str << "  ";
			l += std::string( _commands[i].str ).size() + 2;
			if ( l >= 65 )
			{
				l = 0;
				std::cout << std::endl;
			}
		}
		std::cout << std::endl
		<< "Type -help \"-help\" for further aid. :D" << std::endl;
	}

	/// predicate for finding command that matches the argument
	static bool matchesArgument( Command cmd, char* arg )
	{
		return ( std::string( arg ) == std::string( cmd.str ) );
	}

	/// Executor of -help command.
	/// without arguments lists valid command names to std::cout.
	/// if argv[1] is present describes specific command.
	int cmdHelp( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		if ( argc == 1 )
		{
			increment = 1;
			listCommands();
			return OK;
		}
		increment = 2;

		// argv[1] should be commands name
		Command* cmd = std::find_first_of( _commands, _commands + COMMAND_COUNT, argv + 1, argv + 2, matchesArgument );
		if ( cmd == _commands + COMMAND_COUNT )
		{
			std::cout << "No help for a command like \"" << argv[1] << "\"." << std::endl;
			listCommands();
			quitting = true;
			return INVALID_ARGUMENTS;
		}

		std::cout << "Command: " << cmd->str << " Parameters: " << cmd->params << std::endl;
		std::cout << cmd->help << std::endl;
		return OK;
	}

	/// Executor of -quit command.
	/// returns that the execution has to stop after this command.
	int cmdQuit( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 1;
		quitting = true;
		std::cout << "Bye." << std::endl;
		return OK;
	}

	/// Lets to execute the commands interactively one after other.
	/// Takes the command and arguments from std::cin.
	/// @returns the result of command
	/// @param quitting returns if the application has to exit after this command.
	int interact( bool& quitting )
	{
		std::string line;
		std::list<std::string> elements; // list, since vector.push_back() may relocate it
		std::getline( std::cin, line );
		OemToChar( &line[0], &line[0] ); // fix console code page

		char* argv[MAX_ARGUMENT_COUNT];
		int argc( 0 );

		if ( line.empty() )
		{
			return OK;
		}

		size_t pos( 0 ); // parsing position

		// parse command
		if ( line[pos] != '-' )
		{
			std::cout << "Start commands with minus sign (-)." << std::endl;
			return INVALID_ARGUMENTS;
		}
		++pos; //minus sign

		while ( pos < line.size() && !isspace( line[pos] ) )
		{
			if ( !isalnum( line[pos] ) && line[pos] != '_' )
			{
				std::cout << "Unsupported symbol in command \'" << line[pos] << "\'." << std::endl
				          << "Separate arguments with whitespace." << std::endl;
				return INVALID_ARGUMENTS;
			}
			++pos; //rest of command
		}
		elements.push_back( line.substr( 0, pos ) );
		argv[argc] = &elements.back()[0];
		++argc;

		// parse arguments
		while ( pos < line.size() && isspace( line[pos] ) )
		{
			// just pass the whitespace, do is more readable than std:
			// pos = std::find_if( line.begin()+pos, line.end(), std::not1( std::ptr_fun( &isspace ) ) ) - line.begin();
			do
			{
				++pos;
			}
			while ( pos < line.size() && isspace( line[pos] ) );
			// were these trailing spaces?
			if ( pos == line.size() )
			{
				break;
			}

			if ( line[pos] != '\"' )
			{
				std::cout << "Put command arguments inside quotation marks (\")."  << std::endl;
				return INVALID_ARGUMENTS;
			}

			++pos; // first quotation mark

			size_t argStart( pos );
			pos = line.find_first_of( "\"" , argStart );
			if ( pos == std::string::npos )
			{
				std::cout << "Argument should end with quotation mark (\")."  << std::endl;
				return INVALID_ARGUMENTS;
			}
			if ( argc >= MAX_ARGUMENT_COUNT )
			{
				std::cout << "Too many arguments."  << std::endl;
				return INVALID_ARGUMENTS;
			}

			elements.push_back( line.substr( argStart, pos - argStart ) );
			argv[argc] = &elements.back()[0];
			++argc;

			++pos; //second quotation mark
		}

		if ( pos != line.size() )
		{
			std::cout << "Unexpected symbol '" << line[pos] << "\' after arguments." << std::endl
			          << " Separate arguments with whitespace." << std::endl;
			return INVALID_ARGUMENTS;
		}

		size_t increment( 0 );
		bool leaving( false );
		int ret( OK );

		ret = execute( argc, argv, leaving, increment );

		quitting = ( ret == OK && leaving );

		return ret;
	}

	/// Generic interface to execute and handle exceptions of all commands.
	/// Has same interface as Executor but does not throw, instead returns specific values.
	int execute( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		// argv[0] should be commands name
		Command* cmd = std::find_first_of( _commands, _commands + COMMAND_COUNT, argv, argv + 1, matchesArgument );
		if ( cmd == _commands + COMMAND_COUNT )
		{
			quitting = true;
			std::cout << "unknown command." << std::endl;
			return INVALID_ARGUMENTS;
		}

		int ret( OK );
		try
		{
			ret = ( this->*cmd->exec )( argc, argv, quitting, increment );
		}
		catch ( const digidoc::BDocException& e )
		{
			ERR( "Caught BDocException: %s", e.getMsg().c_str() );
			ret = BDOC_EXCEPTION;
			quitting = true;
		}
		catch ( const digidoc::IOException& e )
		{
			ERR( "Caught IOException: %s", e.getMsg().c_str() );
			ret = IO_EXCEPTION;
			quitting = true;
		}
		catch ( const digidoc::SignException& e )
		{
			ERR( "Caught SignException: %s", e.getMsg().c_str() );
			ret = SIGN_EXCEPTION;
			quitting = true;
		}
		catch ( const digidoc::Exception& e )
		{
			ERR( "Caught Exception: %s", e.getMsg().c_str() );
			ret = PLAIN_EXCEPTION;
			quitting = true;
		}
		catch ( ... )
		{
			ERR( "Caught unknown exception" );
			ret = UNKNOWN_EXCEPTION;
			quitting = true;
		}
		return ret;
	}

	/// Executor of -create command.
	/// Closes current BDoc if any. Creates new BDoc. Sets it as current.
	int cmdCreate( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 1;
		delete _bDoc;
		_bDoc = new BDoc;
		std::cout << "New BDoc created successfully." << std::endl;
		return OK;
	}

	/// Executor of -open command.
	/// Closes current BDoc if any. Opens BDoc given in path. Sets it as current.
	/// argv[1] path to file.
	int cmdOpen( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 2;
		if ( argc < increment )
		{
			quitting = true;
			std::cout << "-open requires an argument." << std::endl;
			return INVALID_COMMAND;
		}
		delete _bDoc;
		_bDoc = NULL;
		std::auto_ptr<ISerialize> serializer( new ZipSerialize( argv[1] ) );
		_bDoc = new BDoc( serializer );
		std::cout << "Existing BDoc file opened successfully." << std::endl;
		return OK;
	}

	/// Executor of -save_as command.
	/// Saves current BDoc at indicated location.
	/// argv[1] Path where to save BDoc.
	int cmdSaveAs( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 2;
		if ( argc < increment )
		{
			quitting = true;
			std::cout << "-save requires an argument." << std::endl;
			return INVALID_COMMAND;
		}
		if ( _bDoc == NULL )
		{
			quitting = true;
			std::cout << "Cannot save unexisting BDoc." << std::endl;
			return INVALID_COMMAND;
		}

		std::auto_ptr<ISerialize> serializer( new ZipSerialize( argv[1] ) );
		std::cout << "Saving BDoc" << std::endl;
		_bDoc->saveTo( serializer );
		std::cout << "BDoc saved." << std::endl;
		return OK;
	}

	/// Executor of -insert command.
	/// Inserts Document file into current BDoc.
	/// argv[1] path to file.
	int cmdInsert( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 2;
		if ( argc < increment )
		{
			quitting = true;
			std::cout << "-insert requires an argument." << std::endl;
			return INVALID_COMMAND;
		}
		if ( _bDoc == NULL )
		{
			quitting = true;
			std::cout << "Cannot insert Documents into unexisting BDoc." << std::endl;
			return INVALID_COMMAND;
		}

		_bDoc->addDocument( Document( argv[1], "file" ) );
		std::cout << "Document inserted into current bDoc." << std::endl;
		return OK;
	}

	/// Executor of -doc_count command.
	/// Returns Document count in current BDoc.
	/// If this command is given from command line quits application returning it.
	int cmdDocCount( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 1;
		if ( _bDoc == NULL )
		{
			quitting = true;
			std::cout << "Cannot return Document count in unexisting BDoc." << std::endl;
			return INVALID_COMMAND;
		}

		quitting = true;
		int ret  = _bDoc->documentCount();
		std::cout << "Document count " << ret << "." << std::endl;
		return ret;
	}

	/// Executor of -doc_info command.
	/// Displays Document type, path and size.
	/// argv[1] Number of the Document (1-based).
	int cmdDocInfo( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 2;
		if ( argc < increment )
		{
			quitting = true;
			std::cout << "-doc_info requires an argument." << std::endl;
			return INVALID_COMMAND;
		}
		if ( _bDoc == NULL )
		{
			quitting = true;
			std::cout << "Cannot display Document info for unexisting BDoc." << std::endl;
			return INVALID_COMMAND;
		}

		int i = atoi( argv[1] );
		Document doc = _bDoc->getDocument( i - 1 );
		std::cout << "Document " << i << " (" << doc.getMediaType().c_str() << "). \"" << doc.getPath().c_str() << "\" (" << doc.getSize() << " bytes)" << std::endl;
		return OK;
	}

	/// Executor of -remove_doc command.
	/// Removes Document file from current BDoc.
	/// argv[1] Number of the Document file (1-based).
	int cmdRemoveDoc( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 2;
		if ( argc < increment )
		{
			quitting = true;
			std::cout << "-remove_doc requires an argument." << std::endl;
			return INVALID_COMMAND;
		}
		if ( _bDoc == NULL )
		{
			quitting = true;
			std::cout << "Cannot remove Documents from unexisting BDoc." << std::endl;
			return INVALID_COMMAND;
		}

		int i = atoi( argv[1] );
		_bDoc->removeDocument( i - 1 );
		std::cout << "Document removed from BDoc." << std::endl;
		return OK;
	}

	/// Executor of -extract_doc command.
	/// Extracts Document file from current BDoc.
	/// argv[1] Number of the Document file (1-based).
	/// argv[2] Path where to save Document file.
	int cmdExtractDoc( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 3;
		if ( argc < increment )
		{
			quitting = true;
			std::cout << "-extract_doc requires two arguments." << std::endl;
			return INVALID_COMMAND;
		}
		if ( _bDoc == NULL )
		{
			quitting = true;
			std::cout << "Cannot extract Documents from unexisting BDoc." << std::endl;
			return INVALID_COMMAND;
		}

		int i = atoi( argv[1] );
		Document doc = _bDoc->getDocument( i - 1 );
		doc.saveAs( argv[2] );
		std::cout << "Document #" << i << " saved to \"" << argv[2] << "\"." << std::endl;
		return OK;
	}

	/// Executor of -sign command.
	/// Signs current BDoc.
	/// argv[1] Path to signing certificate or "id-kaart".
	/// argv[2] Type "BES" or "TM".
	/// argv[3] Location city.
	/// argv[4] Location state or province.
	/// argv[5] Location postal code.
	/// argv[6] Location country name.
	/// argv[7] Signer's role.
	int cmdSign( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 8;
		if ( argc < increment )
		{
			quitting = true;
			std::cout << "-sign requires seven arguments." << std::endl;
			return INVALID_COMMAND;
		}
		if ( _bDoc == NULL )
		{
			quitting = true;
			std::cout << "Cannot sign unexisting BDoc." << std::endl;
			return INVALID_COMMAND;
		}

		quitting = false;

		//Signer* signer;
		Signature::Type profile;
		Signer::SignatureProductionPlace spp( argv[3], argv[4], argv[5], argv[6] );
		Signer::SignerRole role( argv[7] );


		if ( std::string( "TM" ) == argv[2] )
		{
			profile = Signature::TM;
		}
		else
		{
			profile = Signature::BES;
		}

		if ( std::string( "id-kaart" ) == argv[1] )//id-kaart
		{
			//FIXME: implement pin as argument
			std::string pkcs11Driver = Conf::getInstance()->getPKCS11DriverPath();
			EstEIDConsolePinSigner signer( pkcs11Driver );
			signer.setSignatureProductionPlace( spp );
			signer.setSignerRole( role );
			_bDoc->sign( &signer, profile );
		}
		else//RSA signer
		{
			// Load X.509 cert and RSA private key.
			//std::string keyPath = "/home/janari/projects/smartlink/digidoc/test/data/cert/cert+priv_key.pem";
			std::cout << "Loading:" << argv[1] << std::endl;
			std::string keyPath = argv[1];
			X509* signingCert = X509Cert::loadX509( keyPath );
			X509_scope signingCertScope( &signingCert );
			std::cout << "Loaded:" << argv[1] << std::endl;
			RSA* privateKey = RSACrypt::loadRSAPrivateKey( keyPath );
			RSA_scope privateKeyScope( &privateKey );

			X509Cert x509( signingCert );

			RSASigner signer( signingCert, privateKey );
			signer.setSignatureProductionPlace( spp );
			signer.setSignerRole( role );
			_bDoc->sign( &signer, profile );
		}

		std::cout << "Signed BDoc." << std::endl;
		return OK;
	}

	/// Executor of -sign_count command.
	/// Returns Signature count in current BDoc.
	/// If this command is given from command line quits application returning it.
	int cmdSignCount( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 1;
		if ( _bDoc == NULL )
		{
			quitting = true;
			std::cout << "Cannot return Signature count in unexisting BDoc." << std::endl;
			return INVALID_COMMAND;
		}

		quitting = true;
		int ret  = _bDoc->signatureCount();
		std::cout << "Signature count " << ret << "." << std::endl;
		return ret;
	}

	/// Executor of -sign_info command.
	/// Displays Signature type, production place and signers role.
	/// argv[1] Number of the Signature (1-based).
	int cmdSignInfo( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 2;
		if ( argc < increment )
		{
			quitting = true;
			std::cout << "-sign_info requires an argument." << std::endl;
			return INVALID_COMMAND;
		}
		if ( _bDoc == NULL )
		{
			quitting = true;
			std::cout << "Cannot display Signature info for unexisting BDoc." << std::endl;
			return INVALID_COMMAND;
		}

		int i = atoi( argv[1] );
		const Signature* sig = _bDoc->getSignature( i - 1 );
		// type
		std::cout << "Signature " << i << " (" << sig->getMediaType().c_str() << ")" << std::endl;
		// production place
		Signer::SignatureProductionPlace spp = sig->getProductionPlace();
		if ( !spp.isEmpty() )
		{
			std::cout << "  Signature production place:" << std::endl;
			std::cout << "    Country:           " << spp.countryName.c_str() << std::endl;
			std::cout << "    State or Province: " << spp.stateOrProvince.c_str() << std::endl;
			std::cout << "    City:              " << spp.city.c_str() << std::endl;
			std::cout << "    Postal code:       " << spp.postalCode.c_str() << std::endl;
		}

		// signer role
		Signer::SignerRole roles = sig->getSignerRole();
		if ( !roles.isEmpty() )
		{
			std::cout << "  Signer role(s):" << std::endl;
			for ( Signer::SignerRole::TRoles::const_iterator iter = roles.claimedRoles.begin(); iter != roles.claimedRoles.end(); iter++ )
			{
				std::cout << "    " << iter->c_str() << std::endl;
			}
		}
		return OK;
	}

	/// Executor of -remove_sign command.
	/// Removes Signature from current BDoc.
	/// argv[1] Number of the Signature (1-based).
	int cmdRemoveSign( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 2;
		if ( argc < increment )
		{
			quitting = true;
			std::cout << "-remove_sign requires an argument." << std::endl;
			return INVALID_COMMAND;
		}
		if ( _bDoc == NULL )
		{
			quitting = true;
			std::cout << "Cannot remove Signatures from unexisting BDoc." << std::endl;
			return INVALID_COMMAND;
		}

		int i = atoi( argv[1] );
		_bDoc->removeSignature( i - 1 );
		std::cout << "Signature removed from BDoc." << std::endl;
		return OK;
	}

	/// Executor of -valid_offline command.
	/// Validates Signature offline.
	/// argv[1] Number of the Signature (1-based).
	int cmdValidOffline( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 2;
		if ( argc < increment )
		{
			quitting = true;
			std::cout << "-valid_offline requires an argument." << std::endl;
			return INVALID_COMMAND;
		}
		if ( _bDoc == NULL )
		{
			quitting = true;
			std::cout << "Cannot validate Signatures in unexisting BDoc." << std::endl;
			return INVALID_COMMAND;
		}
		int i = atoi( argv[1] );
		const Signature* sig = _bDoc->getSignature( i - 1 );
		sig->validateOffline();
		std::cout << "Signature " << i << " validated offline." << std::endl;
		return OK;
	}

	/// Executor of -valid_online command.
	/// Validates Signature online.
	/// argv[1] Number of the Signature (1-based).
	int cmdValidOnline( size_t argc, char* argv[], bool& quitting, size_t& increment )
	{
		increment = 2;
		if ( argc < increment )
		{
			quitting = true;
			std::cout << "-valid_online requires an argument." << std::endl;
			return INVALID_COMMAND;
		}
		if ( _bDoc == NULL )
		{
			quitting = true;
			std::cout << "Cannot validate Signatures in unexisting BDoc." << std::endl;
			return INVALID_COMMAND;
		}

		int i = atoi( argv[1] );
		const Signature* sig = _bDoc->getSignature( i - 1 );
		int ret = sig->validateOnline();
		const char* STATUS[3] = { "GOOD", "REVOKED", "UNKNOWN" };
		std::cout << "Signature " << i << " validated online. Certificate status " << STATUS[ret] << "." << std::endl;
		return ret;
	}

private:
	char const* _progName;
	digidoc::BDoc* _bDoc;

};


Commander::Command Commander::_commands[COMMAND_COUNT] =
{
	{"-create",        0, &Commander::cmdCreate,       "Closes current BDoc if any. Creates new BDoc. Sets it as current.\n"
	                                                   "No parameters." },
	{"-open",          1, &Commander::cmdOpen,         "Closes current BDoc if any. Opens BDoc. Sets it as current.\n"
	                                                   "Parameter: Path to existing BDoc." },
	{"-save_as",       1, &Commander::cmdSaveAs,       "Saves current BDoc at indicated location.\n"
	                                                   "Parameter: Path where to save BDoc." },
	{"-insert",        1, &Commander::cmdInsert,       "Inserts Document file into current BDoc.\n"
	                                                   "Parameter: Path to file." },
	{"-doc_count",     0, &Commander::cmdDocCount,     "Returns Document count in current BDoc.\n"
	                                                   "If this command is given from command line quits application returning it.\n"
	                                                   "No parameters." },
	{"-doc_info",      1, &Commander::cmdDocInfo,      "Displays Document type, path and size.\n"
	                                                   "Parameter: Number of the Document (1-based)." },
	{"-remove_doc",    1, &Commander::cmdRemoveDoc,    "Removes Document file from current BDoc.\n"
	                                                   "Parameter: Number of the Document file (1-based)." },
	{"-extract_doc",   2, &Commander::cmdExtractDoc,   "Extracts Document file from current BDoc. Parameters:\n"
	                                                   "1) Number of the Document file (1-based).\n"
	                                                   "2) Path where to save Document file." },
	{"-sign",          7, &Commander::cmdSign,         "Signs current BDoc. Parameters:\n"
	                                                   "1) Path to signing certificate or \"id-kaart\".\n"
	                                                   "2) Signature type \"BES\" or \"TM\".\n"
	                                                   "3) Location city.\n"
	                                                   "4) Location state or province.\n"
	                                                   "5) Location postal code.\n"
	                                                   "6) Location country name.\n"
	                                                   "7) Signer's role." },
	{"-sign_count",    0, &Commander::cmdSignCount,    "Returns Signature count in current BDoc.\n"
	                                                   "If this command is given from command line quits application returning it.\n"
	                                                   "No parameters." },
	{"-sign_info",     1, &Commander::cmdSignInfo,     "Displays Signature type, production place and signers role.\n"
	                                                   "Parameter: Number of the Signature (1-based)." },
	{"-remove_sign",   1, &Commander::cmdRemoveSign,   "Removes Signature from current BDoc.\n"
	                                                   "Parameter: Number of the Signature (1-based)." },
	{"-valid_offline", 1, &Commander::cmdValidOffline, "Validates Signature offline.\n"
	                                                   "Parameter: Number of the Signature (1-based)." },
	{"-valid_online",  1, &Commander::cmdValidOnline,  "Validates Signature online.\n"
	                                                   "Parameter: Number of the Signature (1-based)." },
	{"-quit",          0, &Commander::cmdQuit,         "Quits application with return value 0.\n"
	                                                   "No parameters." },
	{"-help",          1, &Commander::cmdHelp,         "Describes specific command.\n"
	                                                   "Parameter: Command name." },
};

} // "anonymous" namespace
} // digidoc namespace

/// Executes digidoc reference application.
///
/// @param argc number of command line arguments.
/// @param argv command line arguments.
/// @return depends on input. May return positive values that are not errors in testing sense.

int main( int argc, char* argv[] )
{
	bool returning( false );
	bool isInteractive( argc < 2 );
	int ret( 0 );
	char const* progName = argv[0];
	++argv;
	--argc;
	size_t increment( 0 );
	digidoc::Commander commander( progName );

	if ( isInteractive )
	{
		std::cout << "Hello! Working with you in interactive mode." << std::endl;
		for ( ;; )
		{
			std::cout << ret << ">";
			ret = commander.interact( returning );
			if ( returning )
			{
				return ret;
			}
		}
	}
	else
	{
		while ( argc >= 1 )
		{
			ret = commander.execute( argc, argv, returning, increment );
			if ( returning )
			{
				return ret;
			}
			argv += increment;
			argc -= increment;
		}
	}
	return 0;
}

