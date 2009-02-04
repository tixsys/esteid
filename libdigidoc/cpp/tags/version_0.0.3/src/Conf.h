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
          std::string getManifestXsdPath() const;
          std::string getXadesXsdPath() const;
          std::string getDsigXsdPath() const;
          std::string getOCSPUrl() const;
          std::string getOCSPCertPath() const;
          std::string getCertStorePath() const;

      protected:
          //Conf();
          //Conf(const Conf& conf);
          //Const& operator= (const Conf& conf);

      private:
          static std::auto_ptr<Conf> INSTANCE;

    };
}

#endif // !defined(__CONF_H_INCLUDED__)
