// Headers required for module defintion.
#include <nsIGenericFactory.h>
#include <nsIClassInfoImpl.h>
#include <nsServiceManagerUtils.h>
#include <nsIScriptNameSpaceManager.h>
#include <nsICategoryManager.h>
#include <nsCOMPtr.h>

// Headers of module's components.
#include "nsEstEID.h"

//NS_GENERIC_FACTORY_CONSTRUCTOR(nsEstEID);
NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsEstEID, Init);
//NS_GENERIC_FACTORY_SINGLETON_CONSTRUCTOR(nsEstEID,
//                                         nsEstEID::getInstance);



NS_DECL_CLASSINFO(nsEstEID);

/* Module self registration */
static NS_METHOD
nsEstEIDRegisterSelf(nsIComponentManager *compMgr,
                             nsIFile *path,
                             const char *loaderStr,
                             const char *type,
                             const nsModuleComponentInfo *info)
{
    nsresult rv;

    nsCOMPtr<nsICategoryManager> cat =
        do_GetService(NS_CATEGORYMANAGER_CONTRACTID);
    NS_ENSURE_STATE(cat);

    // We could also register to "app-startup"
    // to make sure we will be instantiated earlier

    rv = cat->AddCategoryEntry(JAVASCRIPT_GLOBAL_PROPERTY_CATEGORY,
                          "esteid",
//                          "service," NS_ESTEID_CONTRACTID,
                          NS_ESTEID_CONTRACTID,
                          PR_TRUE, PR_TRUE, nsnull);

#if 0
    rv = cat->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY,
                          "EstEID",
//                          "service," NS_ESTEID_CONTRACTID,
                          NS_ESTEID_CONTRACTID,
                          PR_TRUE, PR_TRUE, nsnull);
#endif

    return NS_OK;
}

static const nsModuleComponentInfo gComponents[] = {
        {
		"Estonian ID card digital signature support",
                NS_IESTEID_IID,
                NS_ESTEID_CONTRACTID,
                nsEstEIDConstructor,
                nsEstEIDRegisterSelf,
                NULL, /* No Unregistration Proc */
                NULL, /* No Factory destructor */
                NS_CI_INTERFACE_GETTER_NAME(nsEstEID),
                NULL, /* No language helper */
                &NS_CLASSINFO_NAME(nsEstEID),
                nsIClassInfo::DOM_OBJECT
//		nsIClassInfo::SINGLETON | nsIClassInfo::DOM_OBJECT
//		nsIClassInfo::SINGLETON | nsIClassInfo::MAIN_THREAD_ONLY
        }
};

NS_IMPL_NSGETMODULE(nsEstEID, gComponents)
