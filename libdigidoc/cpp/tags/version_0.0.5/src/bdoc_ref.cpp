//#if defined(_WIN32)
//#include <stdio>
//#endif

#include <iostream>
#include "log.h"
#include "BDoc.h"

class Commander 
{
public:
	Commander( char const* progName )
	    : _progName( progName )
	    , _bDoc( NULL )
	{
	}

	~Commander() {}
	
	enum 
	{
		COMMAND_COUNT = 14,
		OK = 0,
		NOT_IMPLEMENTED = -1,
		INVALID_ARGUMENTS = -2,
		BDOC_EXCEPTION = -3,
		IO_EXCEPTION = -4,
		SIGN_EXCEPTION = -5,
		PLAIN_EXCEPTION = -6,
		UNKNOWN_EXCEPTION = -7,
	};

	typedef int (Commander::*Executor)( int argc, char* argv[], bool& quiting, size_t& increment );
	struct Command
	{
		const char* str;
		size_t params;
		Executor exec;
		const char* help;
	};
	static Command _commands[COMMAND_COUNT];

	void listCommands()
	{
		std::cout << "Valid commands are:" << std::endl;
		for ( size_t i = 0, l = 0; i < COMMAND_COUNT; ++i) 
		{
			std::cout << _commands[i].str << "  ";
			l += std::string( _commands[i].str ).size() + 2;
			if ( l >= 65 ) {
				l = 0;
				std::cout << std::endl;
			}
		}
		std::cout << std::endl 
		    << "Type -help \"-help\" for further aid. :D" << std::endl;
	}

	int cmdHelp( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		if ( argc == 1 ) {
			increment = 1;
			listCommands();
			return OK;
		}
		increment = 2;
		for ( size_t i = 0; i < COMMAND_COUNT; ++i) 
		{
			if ( std::string( argv[1] ) == std::string( _commands[i].str ) )
			{
				std::cout << "Command: " << _commands[i].str << " Parameters: " << _commands[i].params << std::endl;
				std::cout << _commands[i].help  << std::endl;
				return OK;
			}
		}
		std::cout << "No help for a command like \"" << argv[1] << "\"." << std::endl;
		listCommands();
		quiting = true;
		return INVALID_ARGUMENTS;
	}

	int cmdQuit( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		increment = 1;
		quiting = true;
		std::cout << "Bye." << std::endl;
		return OK;
	}

	int interact( bool& quiting )
	{
		std::vector<char> line( 1000 );
		std::cin.getline( &line[0], 999 );
		char* argv[100];
		int argc( 0 );

		if ( line[0] == 0 )
		{
			return OK;
		}

		if ( line[0] != '-' )
		{
			std::cout << "Start commands with minus sign (-)." << std::endl;
			listCommands();
			return INVALID_ARGUMENTS;
		}

		argv[argc] = &line[0];
		++argc;
		size_t pos( 1 );
		while ( pos < 999 && line[pos] != 0 && !isspace( line[pos] ) )
		{
			if ( !isalnum( line[pos] ) )
			{
				std::cout << "Separate commands and arguments with whitespace." << std::endl;
				listCommands();
				return INVALID_ARGUMENTS;
			}
			++pos;
		}

		while ( pos < 999 && line[pos] != 0 && isspace( line[pos] ) )
		{
			line[pos] = 0;
			++pos;

			while ( pos < 998 && line[pos] != 0 && isspace( line[pos] ) )
			{
				++pos;
			}

			if ( line[pos] != '\"' || line[pos] == 0 || pos == 998 )
			{
				std::cout << "Put command arguments inside quotation marks (\")."  << std::endl;
				listCommands();
				return INVALID_ARGUMENTS;
			}
			++pos;
			argv[argc] = &line[pos];
			++argc;

			while ( pos < 998 && line[pos] != 0 && line[pos] != '\"' )
			{
				++pos;
			}

			if ( line[pos] != '\"' )
			{
				std::cout << "Put command arguments inside quotation marks (\")."  << std::endl;
				listCommands();
				return INVALID_ARGUMENTS;
			}

			line[pos] = 0;
			++pos;
		}

		size_t increment( 0 );
		bool leaving( false );
		int ret( OK );

		ret = execute( argc, argv, leaving, increment );

		quiting = (ret == OK && leaving );

		return ret;
	}

	int execute( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		for ( size_t i = 0; i < COMMAND_COUNT; ++i) 
		{
			if ( std::string( argv[0] ) == std::string( _commands[i].str ) )
			{
				int ret( OK );
				try
				{
					ret = (this->*_commands[i].exec)( argc, argv, quiting, increment );
				}
				catch(const digidoc::BDocException& e)
				{
					ERR("Caught BDocException: %s", e.getMsg().c_str());
					ret = BDOC_EXCEPTION;
					quiting = true;
				}
				catch(const digidoc::IOException& e)
				{
					ERR("Caught IOException: %s", e.getMsg().c_str());
					ret = IO_EXCEPTION;
					quiting = true;
				}
				catch(const digidoc::SignException& e)
				{
					ERR("Caught SignException: %s", e.getMsg().c_str());
					ret = SIGN_EXCEPTION;
					quiting = true;
				}
				catch(const digidoc::Exception& e)
				{
					ERR("Caught Exception: %s", e.getMsg().c_str());
					ret = PLAIN_EXCEPTION;
					quiting = true;
				}
				catch(...)
				{
					ERR("Caught unknown exception");
					ret = UNKNOWN_EXCEPTION;
					quiting = true;
				}
				return ret;

			}
		}
		quiting = true;
		std::cout << "unknown command." << std::endl;
		return INVALID_ARGUMENTS;
	}

	int cmdCreate( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		delete _bDoc;
		_bDoc = new ( digidoc::BDoc );
		return OK;
	}

	int cmdOpen( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		quiting = true;
		std::cout << "open is not implemented." << std::endl;
		return NOT_IMPLEMENTED;
	}

	int cmdSaveAs( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		quiting = true;
		std::cout << "save_as is not implemented." << std::endl;
		return NOT_IMPLEMENTED;
	}

	int cmdInsert( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		quiting = true;
		std::cout << "insert is not implemented." << std::endl;
		return NOT_IMPLEMENTED;
	}

	int cmdDocCount( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		quiting = true;
		std::cout << "doc_count is not implemented." << std::endl;
		return NOT_IMPLEMENTED;
	}

	int cmdRemoveDoc( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		quiting = true;
		std::cout << "remove_doc is not implemented." << std::endl;
		return NOT_IMPLEMENTED;
	}

	int cmdExtractDoc( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		quiting = true;
		std::cout << "extract_doc is not implemented." << std::endl;
		return NOT_IMPLEMENTED;
	}

	int cmdSign( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		quiting = true;
		std::cout << "sign is not implemented." << std::endl;
		return NOT_IMPLEMENTED;
	}

	int cmdSignCount( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		quiting = true;
		std::cout << "sign_count is not implemented." << std::endl;
		return NOT_IMPLEMENTED;
	}

	int cmdRemoveSign( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		quiting = true;
		std::cout << "remove_sign is not implemented." << std::endl;
		return NOT_IMPLEMENTED;
	}

	int cmdValidOffline( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		quiting = true;
		std::cout << "valid_offline is not implemented." << std::endl;
		return NOT_IMPLEMENTED;
	}

	int cmdValidOnline( int argc, char* argv[], bool& quiting, size_t& increment )
	{
		quiting = true;
		std::cout << "valid_online not is implemented." << std::endl;
		return NOT_IMPLEMENTED;
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
	{"-doc_count",     0, &Commander::cmdDocCount,     "Quits application returning Document count in current BDoc.\n"
	                                                   "No parameters." },
	{"-remove_doc",    1, &Commander::cmdRemoveDoc,    "Removes Document file from current BDoc.\n"
	                                                   "Parameter: Number of the Document file (1-based)." },
	{"-extract_doc",   2, &Commander::cmdExtractDoc,   "Extracts Document file from current BDoc. Parameters:\n"
	                                                   "1) Number of the Document file (1-based).\n"
	                                                   "2) Path where to save Document file." },
	{"-sign",          7, &Commander::cmdSign,         "Signs current BDoc. Parameters:\n"
	                                                   "1) Signing \"dummy\" or \"console\".\n"
													   "2) Type \"BES\" or \"TM\".\n"
													   "3) Location city.\n"
													   "4) Location state or province.\n"
													   "5) Location postal code.\n"
													   "6) Location country name.\n"
                                                       "7) Signer's role." },
	{"-sign_count",    0, &Commander::cmdSignCount,    "Quits application returning Signature count in current BDoc.\n"
	                                                   "No parameters." },
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


/**
 * Executes digidoc reference application.
 *
 * @param argc number of command line arguments.
 * @param argv command line arguments.
 * @return depends on input
 */

int main(int argc, char* argv[])
{
	bool returning( false );
	bool isInteractive( argc < 2 );
	int ret( 0 );
	char const* progName = argv[0];
	++argv;
	--argc;
	size_t increment( 0 );
	Commander commander( progName );

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

