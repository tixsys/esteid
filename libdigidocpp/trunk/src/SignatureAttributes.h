#if !defined(__SIGNATUREATTRIBUTES_H_INCLUDED__)
#define __SIGNATUREATTRIBUTES_H_INCLUDED__

#include "Exports.h"

#include <string>
#include <vector>

namespace digidoc {

    class EXP_DIGIDOC SignatureProductionPlace
    {
    public:
        SignatureProductionPlace();
        SignatureProductionPlace(std::string city, std::string stateOrProvince, std::string postalCode, std::string countryName);
        bool isEmpty();

        std::string city;
        std::string stateOrProvince;
        std::string postalCode;
        std::string countryName;
    };

    class EXP_DIGIDOC SignerRole
    {
    public:
        SignerRole();
        SignerRole(const std::string& claimedRole);
        bool isEmpty();

        typedef std::vector<std::string> TRoles;
//        TRoles claimedRoles;
        std::vector<std::string> claimedRoles;
    };
}

#endif // !defined(__SIGNATUREATTRIBUTES_H_INCLUDED__)
