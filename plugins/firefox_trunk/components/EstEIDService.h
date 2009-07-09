/*
 * This template class helps to implement Card Services.
 * Typical usage:
 *
 * class MyCardService : public EstEIDService<MyCardService> {
 * protected:
 *     void Lock() { ... };
 *     void UnLock() { ... };
 *     void PostMessage(msgType type,
 *	                    readerID reader,
 *	                    std::string msg) { ... }
 * private:
 *     friend class EstEIDService<MyCardService>;
 *     MyCardService() {};
 *     ~MyCardService() {};
 * }
 *
 * MyCardService *service = MyCardService::getInstance();
 */

#ifndef ESTEIDSERVICE_H_
#define ESTEIDSERVICE_H_

#include "EstEIDServiceBase.h"

template <class T>
class EstEIDService : public EstEIDServiceBase {
public:
    static T* getInstance();
protected:
    EstEIDService();
    virtual ~EstEIDService();
    //!< singleton class instance
    static T* sEstEIDService;
private:
    //! Copy constructor.
    EstEIDService(const EstEIDService& source){};
};

/**
 * Class constructor.
 * Right now we are making this a singleton.
 * It's generally a good idea to have only a single connection
 * to SmartCard manager service.
 * We might reconsider when times change :P
 */
template <class T> EstEIDService<T>::EstEIDService()
{
	if(sEstEIDService)
		throw std::runtime_error(">1 EstEIDService object created");

    sEstEIDService = (T*) this;
}

template <class T> EstEIDService<T>::~EstEIDService() {
    sEstEIDService = NULL;
}

/**
 * Singleton "constructor". Will return an existing instance
 * or create a new one if none exists.
 */

template <class T> T* EstEIDService<T>::getInstance() {
    if(!sEstEIDService) {
        T* ni = new T();
        if (!ni)
            return NULL;
        if(!sEstEIDService)
        	throw std::runtime_error("Memory corrupt?");
    }
    return sEstEIDService;
}

/* Zero out our singleton instance variable */
template <class T> T* EstEIDService<T>::sEstEIDService = NULL;

#endif /* ESTEIDSERVICE_H_ */
