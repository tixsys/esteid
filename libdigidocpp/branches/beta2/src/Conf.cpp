#include "Conf.h"
#include "log.h"

//std::auto_ptr<digidoc::Conf> digidoc::Conf::INSTANCE = std::auto_ptr<digidoc::Conf>(new digidoc::Conf());
digidoc::Conf* digidoc::Conf::INSTANCE = NULL;

digidoc::Conf* digidoc::Conf::getInstance() throw(IOException)
{
    if (INSTANCE == NULL)
    {
        THROW_IOEXCEPTION("Conf is not initialized");
    }
	return INSTANCE;
}

///
/// Init global Conf with conf
/// @param conf implementation to use
///
void digidoc::Conf::init(Conf* conf)
{
    //INSTANCE = std::auto_ptr<Conf>(conf);
    INSTANCE = conf;
}

bool digidoc::Conf::isInitialized()
{
    return INSTANCE != NULL;
}

void digidoc::Conf::destroy()
{
    delete INSTANCE;
    INSTANCE = NULL;
}