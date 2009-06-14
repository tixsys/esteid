/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Based on Mozilla simple plugin example.
 * See LICENSE.MOZ for upstream licensing details.
 */

#include "plugin.h"
#include "nsIServiceManager.h"
#include "nsIMemory.h"
#include "nsISupportsUtils.h" // this is where some useful macros defined
#include "nsStringAPI.h"
#include "nsIEstEID.h"

// service manager which will give the access to all public browser services
nsIServiceManager * gServiceManager = NULL;

// Unix needs this
#ifdef XP_UNIX
#define MIME_TYPES_HANDLED  "application/esteid"
#define PLUGIN_NAME         "Estonian ID-Card Plugin for Mozilla"
#define MIME_TYPES_DESCRIPTION  MIME_TYPES_HANDLED"::"PLUGIN_NAME
#define PLUGIN_DESCRIPTION  PLUGIN_NAME " (XPCOM bridge)"

char* NPP_GetMIMEDescription(void)
{
    return((char *)MIME_TYPES_DESCRIPTION);
}

// get values per plugin
NPError NS_PluginGetValue(NPPVariable aVariable, void *aValue)
{
  NPError err = NPERR_NO_ERROR;
  switch (aVariable) {
    case NPPVpluginNameString:
      *((const char **)aValue) = PLUGIN_NAME;
      break;
    case NPPVpluginDescriptionString:
      *((const char **)aValue) = PLUGIN_DESCRIPTION;
      break;
    default:
      err = NPERR_INVALID_PARAM;
      break;
  }
  return err;
}
#endif //XP_UNIX

//////////////////////////////////////
//
// general initialization and shutdown
//
NPError NS_PluginInitialize()
{
  // this is probably a good place to get the service manager
  // note that Mozilla will add reference, so do not forget to release
  nsISupports * sm = NULL;

  NPN_GetValue(NULL, NPNVserviceManager, &sm);

  // Mozilla returns nsIServiceManager so we can use it directly; doing QI on
  // nsISupports here can still be more appropriate in case something is changed
  // in the future so we don't need to do casting of any sort.
  if(sm) {
    sm->QueryInterface(NS_GET_IID(nsIServiceManager), (void**)&gServiceManager);
    NS_RELEASE(sm);
    ESTEID_DEBUG("EstEIDPluginInstance: Service manager initialized\n");
  }

  return NPERR_NO_ERROR;
}

void NS_PluginShutdown()
{
  // we should release the service manager
  NS_IF_RELEASE(gServiceManager);
  gServiceManager = NULL;
}

/////////////////////////////////////////////////////////////
//
// construction and destruction of our plugin instance object
//
nsPluginInstanceBase * NS_NewPluginInstance(nsPluginCreateData * aCreateDataStruct)
{
  if(!aCreateDataStruct)
    return NULL;

  EstEIDPluginInstance * plugin = new EstEIDPluginInstance(aCreateDataStruct->instance);

  /* Make Sure we can load EstEID XPCOM component */
  if(plugin->initXPCOM()) {
    return plugin;
  } else {
    delete plugin;
    return NULL;
  }
}

void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin)
{
  if(aPlugin)
    delete (EstEIDPluginInstance *)aPlugin;
}

////////////////////////////////////////
//
// EstEIDPluginInstance class implementation
//
EstEIDPluginInstance::EstEIDPluginInstance(NPP aInstance) : nsPluginInstanceBase(),
  mInstance(aInstance),
  mInitialized(FALSE),
  mScriptablePeer(NULL)
{
  ESTEID_DEBUG("EstEIDPluginInstance()\n");
}

EstEIDPluginInstance::~EstEIDPluginInstance()
{
  ESTEID_DEBUG("~EstEIDPluginInstance()\n");
  NS_IF_RELEASE(mScriptablePeer);
}


NPBool EstEIDPluginInstance::initXPCOM() {
  /* Get a pointer to EstEID XPCOM object so we can expose it
     for scripting */
  if (gServiceManager) {
    ESTEID_DEBUG("EstEIDPluginInstance: Trying to aquire %s\n", NS_ESTEID_CONTRACTID);
    gServiceManager->GetServiceByContractID(NS_ESTEID_CONTRACTID,
        NS_GET_IID(nsIEstEID), (void **)&mScriptablePeer);
    if(mScriptablePeer) {
        nsCString ver;
        mScriptablePeer->GetVersion(ver);
        ESTEID_DEBUG("EstEIDPluginInstance: Loaded EstEID XPCOM version %s\n", ver.get());
        return TRUE;
    } else {
      ESTEID_DEBUG("EstEIDPluginInstance: Failed to load EstEID XPCOM component\n");
      return FALSE;
    }
  } else {
    ESTEID_DEBUG("EstEIDPluginInstance: No Service manager available\n");
    return FALSE;
  }
}

NPBool EstEIDPluginInstance::init(NPWindow* aWindow)
{
  ESTEID_DEBUG("EstEIDPluginInstance::init()\n");
  // FIXME: Is this needed?
  if(aWindow == NULL)
    return FALSE;

  mInitialized = TRUE;
  return TRUE;
}

void EstEIDPluginInstance::shut()
{
  ESTEID_DEBUG("EstEIDPluginInstance::shut()\n");
  mInitialized = FALSE;
}

NPBool EstEIDPluginInstance::isInitialized()
{
  ESTEID_DEBUG("EstEIDPluginInstance::isInitialized()\n");
  return mInitialized;
}

// ==============================
// ! Scriptability related code !
// ==============================
//
// here the plugin is asked by Mozilla to tell if it is scriptable
// we should return a valid interface id and a pointer to
// nsScriptablePeer interface which we should have implemented
// and which should be defined in the corressponding *.xpt file
// in the bin/components folder
NPError	EstEIDPluginInstance::GetValue(NPPVariable aVariable, void *aValue)
{
  NPError rv = NPERR_NO_ERROR;

  switch (aVariable) {
    case NPPVpluginScriptableInstance: {
      ESTEID_DEBUG("EstEIDPluginInstance::GetValue(NPPVpluginScriptableInstance)\n");
      if (mScriptablePeer) {
        // add reference for the caller requesting the object
        NS_ADDREF(mScriptablePeer);
        *(nsISupports **)aValue = mScriptablePeer;
      } else {
        ESTEID_DEBUG("EstEID XPCOM not initialized\n");
        rv = NPERR_OUT_OF_MEMORY_ERROR;
      }
    }
    break;

    case NPPVpluginScriptableIID: {
      ESTEID_DEBUG("EstEIDPluginInstance::GetValue(NPPVpluginScriptableIID)\n");
      static nsIID scriptableIID = NS_IESTEID_IID;
      nsIID* ptr = (nsIID *)NPN_MemAlloc(sizeof(nsIID));
      if (ptr) {
          *ptr = scriptableIID;
          *(nsIID **)aValue = ptr;
      } else
        rv = NPERR_OUT_OF_MEMORY_ERROR;
    }
    break;

    default:
      break;
  }

  return rv;
}
