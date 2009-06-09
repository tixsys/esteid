#if !defined(__CONF_H_INCLUDED__)
#define __CONF_H_INCLUDED__

#include <memory>
#include <string>

#include "Exports.h"

namespace digidoc
{
    /**
     * TODO: implement and add description
     */
    class EXP_DIGIDOC Conf
    {

      public:
          static void init(Conf* conf);
          static Conf* getInstance();
          virtual std::string getDigestUri() const = 0;
          virtual std::string getManifestXsdPath() const = 0;
          virtual std::string getXadesXsdPath() const = 0;
          virtual std::string getDsigXsdPath() const = 0;
          virtual std::string getPKCS11DriverPath() const = 0;
          virtual std::string getOCSPUrl() const = 0;
          virtual std::string getOCSPCertPath() const = 0;
          virtual std::string getCertStorePath() const = 0;
// Fixme should be protected
          Conf() {}
      private:
// Fixme should be protected
          Conf( const Conf& conf );
          Conf& operator= ( const Conf& conf );

      private:
          //static std::auto_ptr<Conf> INSTANCE;
          static Conf* INSTANCE;

    };
}

#endif // !defined(__CONF_H_INCLUDED__)
