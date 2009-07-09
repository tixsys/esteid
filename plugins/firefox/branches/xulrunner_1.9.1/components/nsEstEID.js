Components.utils.import("resource://gre/modules/XPCOMUtils.jsm");

var nsEstEIDInterface_list =
    [Components.interfaces.nsIEstEID,
     Components.interfaces.nsISecurityCheckedComponent,
     Components.interfaces.nsIClassInfo];

function nsEstEID() {
  try {
    // this is needed to generally allow usage of components in javascript
    // netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
    this._private = Components.classes["@id.eesti.ee/esteidprivate;1"]
                  .createInstance(Components.interfaces.nsIEstEID);

    var JSONtext = this._private.getPersonalDataAttributeNames();
    if(JSONtext != null)
      this._personalDataAttributeNames = eval('(' + JSONtext + ')');

  } catch (anError) {
    dump("ERROR: " + anError);
  }
  dump("Esteid created\n");
}

nsEstEID.prototype = {
  classDescription: "Estonian ID card access public XPCOM Component",
  classID:          Components.ID("{fc4a799c-04e6-11de-b3d7-72c955d89593}"),
  contractID:       "@id.eesti.ee/esteid;1",
  _xpcom_categories: [{
    category: "JavaScript global property",
    entry: "esteid",
  }],
  service: true,
  QueryInterface: XPCOMUtils.generateQI(nsEstEIDInterface_list),

  // nsIClassInfo
  //flags: Components.interfaces.nsIClassInfo.DOM_OBJECT | 
  //       Components.interfaces.nsIClassInfo.SINGLETON,
  flags: Components.interfaces.nsIClassInfo.SINGLETON,
  classDescription: "EstEID",
  implementationLanguage:
    Components.interfaces.nsIProgrammingLanguage.JAVASCRIPT,
  getInterfaces: function(count) {
    count.value = nsEstEIDInterface_list.length;
    return nsEstEIDInterface_list;
  },
  getHelperForLanguage: function(count) { return null; },

  /* get lastName() {
    return this._kala;
  }, */

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

  /* Phew! Now we can start to do USEFUL things. About time! */

  /** Pointer to native XPCOM object to interact with the physical card */
  _private: null,
  /** personal Data from card */
  _personalData: null,
  _listener: null,
  /**  function to read personal data off our card */ 
  readPersonalData: function() {
    var JSONtext = this._private.readPersonalData();
    if(JSONtext) {
      this._personalData = eval('(' + JSONtext + ')');
      if(this._listener != null) this._listener.handleEvent(89);
      // this.firstName = this._personalData.firstName;
    }
/*
    // Copy all properties to this
    for( var prop in personalData) {
      if( prop == "constructor" ) continue;
      var data = personalData[prop];
      if( typeof(data) != "string" ) continue;
      this[prop] = data;
    }
*/
  },
/*  get firstName() {
    return this._personalData.firstName;
  },
*/
  addEventListener: function(type, listener) {
    this._listener = listener;
  },
  personalData: null,
  getVersion: function() { return "0.1.0"; }
};

/* Generate getters. Don't you just love dynamic languages :P */
// nsEstEID.prototype.__defineGetter__("lastName", function() { return "Pakiraam"; });
// nsEstEID.prototype.__defineGetter__("lastName", nsEstEID.prototype.getter);

var components = [nsEstEID];
function NSGetModule(compMgr, fileSpec) {
  return XPCOMUtils.generateModule(components);
}
