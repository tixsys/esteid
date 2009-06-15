#if !defined(__CONF_H_INCLUDED__)
#define __CONF_H_INCLUDED__

#include <memory>
#include <string>

namespace digidoc
{
    /**
     * TODO: implement and add description
     */
    class Conf
    {

      public:
          static Conf* getInstance();
          static void init(Conf* conf);
          virtual std::string getDigestUri() const;
          virtual std::string getManifestXsdPath() const;
          virtual std::string getXadesXsdPath() const;
          virtual std::string getDsigXsdPath() const;
          virtual std::string getPKCS11DriverPath() const;
          virtual std::string getOCSPUrl() const;
          virtual std::string getOCSPCertPath() const;
          virtual std::string getCertStorePath() const;

      protected:
          Conf();
          Conf(const Conf& conf);
          Conf& operator= (const Conf& conf);

      private:
          //static std::auto_ptr<Conf> INSTANCE;
          static Conf* INSTANCE;

    };
}

#endif // !defined(__CONF_H_INCLUDED__)
