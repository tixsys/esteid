/*
 * libdigidocpp - a C++ library for creating and validating BDOC-1.0 documents
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#if defined(_WIN32)
#include <conio.h>
#endif

#if !defined(_WIN32)
#include <unistd.h>
#endif 

#include <string.h>

#include "../../log.h"
#include "../../util/String.h"
#include "EstEIDSigner.h"


/**
 * Initializes base class PKCS11Signer.
 *
 * @param driver full path to the PKCS #11 driver (e.g. /usr/lib/opensc-pkcs11.so)
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
digidoc::EstEIDSigner::EstEIDSigner(const std::string& driver) throw(SignException)
 : PKCS11Signer(driver)
{
}

/**
 *
 */
digidoc::EstEIDSigner::~EstEIDSigner()
{
}

/**
 * Implements signing certificate selector for EstEID ID-Cards.
 *
 * @param certificates list of certificates to choose from. List of all certificates
 *        found ID-card.
 * @return should return the selected certificate.
 * @throws SignException throws exception if no suitable certificate was found.
 */
digidoc::PKCS11Signer::PKCS11Cert digidoc::EstEIDSigner::selectSigningCertificate(
        std::vector<PKCS11Signer::PKCS11Cert> certificates) throw(SignException)
{
    // Find EstEID signing certificate
    if(certificates.empty())
        THROW_SIGNEXCEPTION("Could not find certificate with label 'Allkirjastamine'.");
    return certificates[0];
}

/**
 * EstEID ID-Card signer with interactive acquisition from console.
 *
 * @param driver full path to the PKCS #11 driver (e.g. /usr/lib/opensc-pkcs11.so)
 * @param prompt prompt that is printed to console if PIN code is acquired. The prompt
 *        must cotain <code>%s</code>, it is replaced with token label. For example
 *        with <pre>ID-kaart (PIN2, Allkirjastamine)</pre>.
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
digidoc::EstEIDConsolePinSigner::EstEIDConsolePinSigner(const std::string& driver, const std::string& prompt) throw(SignException)
 : EstEIDSigner(driver)
 , prompt(prompt)
{
}

/**
 * EstEID ID-Card signer with interactive acquisition from console.
 *
 * @param driver full path to the PKCS #11 driver (e.g. /usr/lib/opensc-pkcs11.so)
 * @throws SignException exception is thrown if the provided PKCS #11 driver
 *         loading failed.
 */
digidoc::EstEIDConsolePinSigner::EstEIDConsolePinSigner(const std::string& driver) throw(SignException)
 : EstEIDSigner(driver)
 , prompt("Please enter PIN for token '%s' or <enter> to cancel: ")
{
}

/**
 *
 */
digidoc::EstEIDConsolePinSigner::~EstEIDConsolePinSigner()
{
}

/**
 * @param prompt sets the prompt value.
 */
void digidoc::EstEIDConsolePinSigner::setPrompt(const std::string& prompt)
{
    this->prompt = prompt;
}

/**
 * Asks PIN code from the user using console and returns it.
 *
 * @param certificate the certificate, which PIN code is asked.
 * @return returns PIN code.
 * @throws SignException throws an exception if the login process is canceled.
 */
std::string digidoc::EstEIDConsolePinSigner::getPin(PKCS11Signer::PKCS11Cert certificate) throw(SignException)
{
    char pin[16];
    size_t pinMax = 16;

#if defined(_WIN32)
	// something that acts wildly similarily with getpass()
	{
		printf( prompt.c_str(), certificate.token.label.c_str() );
		size_t i = 0;
		int c;
		while ( (c = _getch()) != '\r' ) 
		{
			switch ( c )
			{
			default:
				if ( i >= pinMax-1 || iscntrl( c ) ) 
				{
					// can't be a part of password
					fputc( '\a', stdout );
					break;
				}
				pin[i++] = static_cast<char>(c);
				fputc( '*', stdout );
				break;
			case EOF:
			{
				fputs( "[EOF]\n", stdout );
				SignException e( __FILE__, __LINE__, "PIN acquisition canceled with [EOF].");
				e.setCode( Exception::PINCanceled );
				throw e;
				break;
			}
			case 0: 
			case 0xE0:  // FN Keys (0 or E0) start of two-character FN code
				c = ( c << 4 ) | _getch();
				if ( c != 0xE53 && c != 0xE4B && c != 0x053 && c != 0x04b ) 
				{
					// not {DELETE}, {<--}, Num{DEL} and Num{<--}  
					fputc( '\a', stdout );
					break;
				}
				// NO BREAK, fall through to the one-character deletes  
			case '\b':
			case '127':
				if ( i == 0 )
				{
					// nothing to delete
					fputc( '\a', stdout );
					break;
				}
				pin[--i] = '\0';
				fputs( "\b \b", stdout );
				break;
			case  3: // CTRL+C 
			{
				fputs( "^C\n", stdout );
				SignException e( __FILE__, __LINE__, "PIN acquisition canceled with ^C.");
				e.setCode( Exception::PINCanceled );
				throw e;
				break;
			}
			case  26: // CTRL+Z
			{
				fputs( "^Z\n", stdout );
				SignException e( __FILE__, __LINE__, "PIN acquisition canceled with ^Z.");
				e.setCode( Exception::PINCanceled );
				throw e;
				break;
			}
			case  27: // ESC 
				fputc('\n', stdout );
				printf( prompt.c_str(), certificate.token.label.c_str() );
				i = 0;
				break;
			}
		}
		fputc( '\n', stdout );
		pin[i] = '\0';
	}
#else
    char* pwd = getpass(util::String::format(prompt.c_str(), certificate.token.label.c_str()).c_str());
    strncpy(pin, pwd, pinMax);
#endif

    pin[pinMax-1] = '\0';

    std::string result(pin);
    if(result.empty())
    {
		SignException e( __FILE__, __LINE__, "PIN acquisition canceled.");
		e.setCode( Exception::PINCanceled );
		throw e;
    }

    return result;
}
