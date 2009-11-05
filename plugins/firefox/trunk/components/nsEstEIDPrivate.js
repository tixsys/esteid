try {
  Components.utils.import("resource://gre/modules/XPCOMUtils.jsm");
} catch(e) {
  // Ugly, ugly copy-paste from modules/XPCOMUtils.jsm to support
  // thunderbird 2.0 that does not have XPCOMUtils.
  // Remove when this old beast is abandoned for good

// FIXME: Remove this code as soon as possible
// ======================   CUT HERE   =============================
const Ci = Components.interfaces;
const Cr = Components.results;

var XPCOMUtils = {
  generateQI: function(interfaces) {
    return makeQI([i.name for each(i in interfaces)]);
  },
  generateModule: function(componentsArray, postRegister, preUnregister) {
    let classes = [];
    for each (let component in componentsArray) {
      classes.push({
        cid:          component.prototype.classID,
        className:    component.prototype.classDescription,
        contractID:   component.prototype.contractID,
        factory:      this._getFactory(component),
        categories:   component.prototype._xpcom_categories
      });
    }

    return { // nsIModule impl.
      getClassObject: function(compMgr, cid, iid) {
        // We only support nsIFactory queries, not nsIClassInfo
        if (!iid.equals(Ci.nsIFactory))
          throw Cr.NS_ERROR_NOT_IMPLEMENTED;

        for each (let classDesc in classes) {
          if (classDesc.cid.equals(cid))
            return classDesc.factory;
        }

        throw Cr.NS_ERROR_FACTORY_NOT_REGISTERED;
      },

      registerSelf: function(compMgr, fileSpec, location, type) {
        var componentCount = 0;
        debug("*** registering " + fileSpec.leafName + ": [ ");
        compMgr.QueryInterface(Ci.nsIComponentRegistrar);

        for each (let classDesc in classes) {
          debug((componentCount++ ? ", " : "") + classDesc.className);
          compMgr.registerFactoryLocation(classDesc.cid,
                                          classDesc.className,
                                          classDesc.contractID,
                                          fileSpec,
                                          location,
                                          type);
          if (classDesc.categories) {
            let catMan = XPCOMUtils.categoryManager;
            for each (let cat in classDesc.categories) {
              let defaultValue = (cat.service ? "service," : "") +
                                 classDesc.contractID;
              catMan.addCategoryEntry(cat.category,
                                      cat.entry || classDesc.className,
                                      cat.value || defaultValue,
                                      true, true);
            }
          }
        }

        if (postRegister)
          postRegister(compMgr, fileSpec, componentsArray);
        debug(" ]\n");
      },

      unregisterSelf: function(compMgr, fileSpec, location) {
        var componentCount = 0;
        debug("*** unregistering " + fileSpec.leafName + ": [ ");
        compMgr.QueryInterface(Ci.nsIComponentRegistrar);
        if (preUnregister)
          preUnregister(compMgr, fileSpec, componentsArray);

        for each (let classDesc in classes) {
          debug((componentCount++ ? ", " : "") + classDesc.className);
          if (classDesc.categories) {
            let catMan = XPCOMUtils.categoryManager;
            for each (let cat in classDesc.categories) {
              catMan.deleteCategoryEntry(cat.category,
                                         cat.entry || classDesc.className,
                                         true);
            }
          }
          compMgr.unregisterFactoryLocation(classDesc.cid, fileSpec);
        }
        debug(" ]\n");
      },

      canUnload: function(compMgr) {
        return true;
      }
    };
  },
  _getFactory: function(component) {
    var factory = component.prototype._xpcom_factory;
    if (!factory) {
      factory = {
        createInstance: function(outer, iid) {
          if (outer)
            throw Cr.NS_ERROR_NO_AGGREGATION;
          return (new component()).QueryInterface(iid);
        }
      }
    }
    return factory;
  }
};
function makeQI(interfaceNames) {
  return function XPCOMUtils_QueryInterface(iid) {
    if (iid.equals(Ci.nsISupports))
      return this;
    for each(let interfaceName in interfaceNames) {
      if (Ci[interfaceName].equals(iid))
        return this;
    }

    throw Cr.NS_ERROR_NO_INTERFACE;
  };
}
}
// ======================   CUT HERE   =============================

var nsEstEIDPrivateInterface_list =
    [Components.interfaces.nsIEstEIDPrivate,
//     Components.interfaces.nsISecurityCheckedComponent,
     Components.interfaces.nsIClassInfo];

function nsEstEIDPrivate() {
/*  try {
    this._private = Components.classes["@id.eesti.ee/esteidprivate;1"]
                  .createInstance(Components.interfaces.nsIEstEIDPrivate);

    var JSONtext = this._private.getPersonalDataAttributeNames();
    if(JSONtext != null)
      this._personalDataAttributeNames = eval('(' + JSONtext + ')');

  } catch (anError) {
    dump("ERROR: " + anError);
  }
*/
  dump("EsteidPrivate created\n");
}

nsEstEIDPrivate.prototype = {
  classDescription: "Estonian ID card private XPCOM Component",
  classID:          Components.ID("{be28a086-2234-429b-9bc4-74a652538c64}"),
  contractID:       "@id.eesti.ee/esteid-private;1",
  service:          true,
  QueryInterface: XPCOMUtils.generateQI(nsEstEIDPrivateInterface_list),

  // nsIClassInfo
  flags: Components.interfaces.nsIClassInfo.SINGLETON,
  classDescription: "EstEIDPrivate",
  implementationLanguage:
    Components.interfaces.nsIProgrammingLanguage.JAVASCRIPT,
  getInterfaces: function(count) {
    count.value = nsEstEIDPrivateInterface_list.length;
    return nsEstEIDPrivateInterface_list;
  },
  getHelperForLanguage: function(count) { return null; },

/*
  // nsISecurityCheckedComponent
  canCreateWrapper: function(aIID) {
    return "AllAccess";
  },
  canCallMethod: function(aIID, methodName) {
    dump("canCallMethod: " + methodName + "\n");
    return "AllAccess";
  },
  canGetProperty: function(aIID, propertyName) {
    dump("canGetProperty: " + propertyName + "\n");
    return "AllAccess";
  },
  canSetProperty: function(aIID, propertyName) {
    return "NoAccess";
  },
*/

  /* Phew! Now we can start to do USEFUL things. About time! */
  _log: "",
  _wl: ["www.swedbank.ee"],
  _sdlg: null,
  _nbIcon: "chrome://esteid/skin/id-16.png",

  _getNotificationBox: function(aWindow) {
    /* https://developer.mozilla.org/en/Code_snippets/Tabbed_browser
     * browser/base/content/browser.js
     * https://developer.mozilla.org/en/XUL%3anotificationbox
     */
    var wm = Components.classes["@mozilla.org/appshell/window-mediator;1"]
                       .getService(Components.interfaces.nsIWindowMediator);
    var mw = wm.getMostRecentWindow("navigator:browser");
    var gb = mw.getBrowser();
    var db = gb.getBrowserForDocument(aWindow.document);
    var nb = gb.getNotificationBox(db);

    return nb;
  },


  _urlToHost: function(url) {
    // We only support https URLs so we can parse the URL like this
    var re = new RegExp("^https://(.*?)/");
    var m = re.exec(url);
    if(m)
      return m[1];
    else
      return null;
  },

  _addToWhitelist: function(url) {
    var host = this._urlToHost(url);
    if(host) this._wl.push(host);
  },

  isWhiteListed: function(url) {
    var wl = this._wl;
    var host = this._urlToHost(url);

    if(!host)
      return false;

    for(var i = 0; i < wl.length; i++)
      if(wl[i] == host)
        return true;

    return false;
  },

  promptForSignPIN: function(aParent, subject, docUrl, docHash, pageUrl,
                             pinPadTimeout, retry, tries) {
    var params = {subject:subject, docUrl:docUrl,
                  docHash:docHash, pageUrl:pageUrl,
                  retry:retry, tries:tries, aParent:aParent,
                  timeout:pinPadTimeout, out:null};

    var dlg = aParent.openDialog("chrome://esteid/content/signprompt.xul",
                        "esteidpin2", "modal,centerscreen,chrome,titlebar",
                        params);

    return params.out;
  },

  closePinPrompt: function(aParent) {
    try {
      var ww = Components.classes["@mozilla.org/embedcomp/window-watcher;1"]
                         .getService(Components.interfaces.nsIWindowWatcher);
      //var win = ww.getWindowByName("esteidpin2", aParent);
      var win = ww.getWindowByName("esteidpin2", null);
      win.arguments[0].forceClose = true;
    } catch(e) {
      this.errorMessage("Can't close PIN prompt: " + e);
    }
  },

  showSettings: function(aParent, pageUrl) {
    // Do not show multiple windows
    if(this._sdlg && !this._sdlg.closed) {
      this._sdlg.focus();
      return;
    }

    var params = {wl: this._wl, log: this._log,
                  host: this._urlToHost(pageUrl), out:null};
    this._sdlg = aParent.openDialog("chrome://esteid/content/prefs.xul",
                       "_blank", "chrome,centerscreen", params);
  },

  logMessage: function(msg) {
    this._log += msg + "\n";
  },

  debugMessage: function(msg) {
    this.logMessage("DEBUG: " + msg);
  },

  errorMessage: function(msg) {
    this.logMessage("ERROR: " + msg);
  },

  showNotification: function(aWindow, pageUrl) {
    var jsSucks = this;
    var nb = this._getNotificationBox(aWindow);
    var sbs = Components.classes["@mozilla.org/intl/stringbundle;1"].
                getService(Components.interfaces.nsIStringBundleService);
    var sb = sbs.createBundle("chrome://esteid/locale/esteid.properties");

    /* Remove a previous notification if any (happens with page reloads) */
    var tmp = nb.getNotificationWithValue("esteid-blocked");
    if(tmp) nb.removeNotification(tmp);

    /* Show Error notification if the URL can't be added (eg. not https) */
    if(!this._urlToHost(pageUrl)) {
        var messageString = sb.GetStringFromName("blockedInsecure");
        nb.appendNotification(messageString, "esteid-blocked", this._nbIcon,
                              nb.PRIORITY_WARNING_MEDIUM);
      return;
    }

    /* Show normal notification */
    var messageString = sb.GetStringFromName("blocked");
    let buttons = [{
        label: sb.GetStringFromName("allowButton"),
        accessKey: sb.GetStringFromName("allowAccessKey"),
        popup: null,
        callback: function() { jsSucks._addToWhitelist(pageUrl); }
      },{
        label: sb.GetStringFromName("denyButton"),
        accessKey: sb.GetStringFromName("denyAccessKey"),
        popup: null,
        callback: function() { return; }
      },{
        label: sb.GetStringFromName("prefsButton"),
        accessKey: sb.GetStringFromName("prefsAccessKey"),
        popup: null,
        callback: function() { jsSucks.showSettings(aWindow, pageUrl); }
    }];
    nb.appendNotification(messageString, "esteid-blocked", this._nbIcon,
                          nb.PRIORITY_WARNING_MEDIUM , buttons);
  },
  createPluginInstance: function() {
    return Components.classes["@id.eesti.ee/esteid;1"].
           createInstance(Components.interfaces.nsIEstEID);

  },
  getVersion: function() { return "0.1.0"; }
};

var components = [nsEstEIDPrivate];
function NSGetModule(compMgr, fileSpec) {
  return XPCOMUtils.generateModule(components);
}
