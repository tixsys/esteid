Components.utils.import("resource://gre/modules/XPCOMUtils.jsm");

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
                             retry, tries) {
    var params = {subject:subject,docUrl:docUrl,
                  docHash:docHash,pageUrl:pageUrl,
                  retry:retry, tries:tries, aParent:aParent,
                  out:null};

    var dlg = aParent.openDialog("chrome://esteid/content/signprompt.xul",
                        "_blank", "modal,centerscreen,chrome,titlebar",
                        params);

    return params.out;
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
