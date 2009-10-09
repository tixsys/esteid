/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Based on Mozilla simple plugin example.
 * See LICENSE.MOZ for upstream licensing details.
 */

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "plugingate/pluginbase.h"
#include "nsIEstEID.h"

#include <nsCOMPtr.h>

#define ESTEID_DEBUG printf

class EstEIDPluginInstance : public nsPluginInstanceBase
{
public:
  EstEIDPluginInstance(NPP aInstance);
  ~EstEIDPluginInstance();

  NPBool init(NPWindow* aWindow);
  void shut();
  NPBool isInitialized();

  // we need to provide implementation of this method as it will be
  // used by Mozilla to retrieve the scriptable peer
  // and couple of other things on Unix
  NPError	GetValue(NPPVariable variable, void *value);

  NPBool initXPCOM();

private:
  NPP mInstance;
  NPBool mInitialized;
  nsIEstEID* mScriptablePeer;
};

#endif // __PLUGIN_H__
