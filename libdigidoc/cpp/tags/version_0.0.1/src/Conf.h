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

      protected:
          //Conf();
          //Conf(const Conf& conf);
          //Const& operator= (const Conf& conf);

      private:
    	  static std::auto_ptr<Conf> INSTANCE;

    };
}

#endif // !defined(__CONF_H_INCLUDED__)
