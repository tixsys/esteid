function EstEIDtest() {
    const Ci = Components.interfaces;
    const ESTEID_CONTRACTID = "@id.eesti.ee/esteid;1";

    try {
	// this is needed to generally allow usage of components in javascript
	netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
        var com = Components.classes[ESTEID_CONTRACTID]
                            .createInstance(Ci.nsIEstEID);

        document.getElementById('esteidpanel').label = com.getVersion();
    } catch (anError) {
        dump("ERROR: " + anError);
    }
    try {
        var obj = document.getElementById('buttplug');
        dump(obj.version);
        obj.about();
    } catch (anError) {
        dump("ERROR: " + anError);
    }
}

// Configure EstEID certificates and modules
window.addEventListener("load", ConfigureEstEID, false);
// Load plugin to show it's version on status bar
window.addEventListener("load", EstEIDtest, false);
