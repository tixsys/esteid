// Headers required for module defintion.
#include <nsIGenericFactory.h>
#include <nsIClassInfoImpl.h>
#include <nsServiceManagerUtils.h>
#include <nsIScriptNameSpaceManager.h>
#include <nsICategoryManager.h>
#include <nsCOMPtr.h>

// Headers of module's components.
#include "nsEstEID.h"
#include "nsEstEIDCertificate.h"

NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsEstEID, Init);

NS_DECL_CLASSINFO(nsEstEID);
NS_DECL_CLASSINFO(nsEstEIDCertificate);

static const nsModuleComponentInfo gComponents[] = {
        {
		"Estonian ID card digital signature support",
                NS_IESTEID_IID,
                NS_ESTEID_CONTRACTID,
                nsEstEIDConstructor,
                NULL, /* No Registration Proc */
                NULL, /* No Unregistration Proc */
                NULL, /* No Factory destructor */
                NS_CI_INTERFACE_GETTER_NAME(nsEstEID),
                NULL, /* No language helper */
                &NS_CLASSINFO_NAME(nsEstEID),
                nsIClassInfo::PLUGIN_OBJECT | nsIClassInfo::DOM_OBJECT
        }
};

NS_IMPL_NSGETMODULE(nsEstEID, gComponents)
