const ESTEID_CONTRACTID = "@id.eesti.ee/esteid;1";

function EstEIDtest() {
    // document.getElementById('esteidpanel').label = "Jeee!";
    try {
        // this is needed to generally allow usage of components in javascript
        netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
        var com = Components.classes[ESTEID_CONTRACTID]
                            .createInstance(Components.interfaces.nsIEstEID);

        document.getElementById('esteidpanel').label = com.getVersion();

//	registerGlobalObject();
    } catch (anError) {
        dump("ERROR: " + anError);
    }
}

function registerGlobalObject() {
    var catman = Components.classes["@mozilla.org/categorymanager;1"]
                           .getService(Components.interfaces.nsICategoryManager);

    // var rv = catman.addCategoryEntry("JavaScript global property",
    //                         "EstEID",
    //                         ESTEID_CONTRACTID,
    //                         true,  /* Persist this entry */
    //                         true); /* Replace existing entry */

    // var rv = catman.addCategoryEntry("JavaScript global constructor",
    //                         "EstEID",
    //                         ESTEID_CONTRACTID,
    //                         true,  /* Persist this entry */
    //                         true); /* Replace existing entry */
    // alert(rv);
}

window.addEventListener("load", EstEIDtest, false);
