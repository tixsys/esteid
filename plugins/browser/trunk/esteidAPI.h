#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHostWrapper.h"

class esteidAPI : public FB::JSAPIAuto
{
public:
    esteidAPI(FB::BrowserHostWrapper *host);
    virtual ~esteidAPI();

    /** get plugin version 
      * @returns string version number
      */
    std::string getVersion();

    /** Sign hash with digital signature certificate
     * Browser will automatically prompt for PIN2
     * @param hash HEX encoded document hash to sign
     * @param url an URL to the document itself
     */
/*
    std::string sign(std::string hash, std::string url);
*/

    /** Certificates (read-only properties) */
/*
    esteidAPI::Certificate get_authCert();
    esteidAPI::Certificate get_signCert();
*/

    /** Personal data file attributes (read-only properties) */
    std::string get_lastName();
/*
    std::string get_firstName();
    std::string get_middleName();
    std::string get_sex();
    std::string get_citizenship();
    std::string get_birthDate();
    std::string get_personalID();
    std::string get_documentID();
    std::string get_expiryDate();
    std::string get_placeOfBirth();
    std::string get_issuedDate();
    std::string get_residencePermit();
    std::string get_comment1();
    std::string get_comment2();
    std::string get_comment3();
    std::string get_comment4();
*/

   /*  Event handlers
   *        - OnCardInserted
   *        - OnCardRemoved
   *        - OnReadersChanged
   */

private:
    FB::AutoPtr<FB::BrowserHostWrapper> m_host;
};
