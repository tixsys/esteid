var logmsg = "";

function log(a) {
  try {
    eidui.logMessage(a);
  } catch(e) {
    logmsg += a + "\n";
  }
}

function debug(a) {
  try {
    eidui.debugMessage(a);
  } catch(e) {
    logmsg += "DEBUG: " + a + "\n";
  }
}

function error(a) {
  try {
    eidui.errorMessage(a);
  } catch(e) {
    logmsg += "ERROR: " + a + "\n";
  }
}

function showNormalIcon() {
  var elt = document.getElementById('eidicon');
  elt.src = "chrome://esteid/skin/id-16.png";
}

function showErrorIcon() {
  var elt = document.getElementById('eidicon');
  elt.src = "chrome://global/skin/icons/error-16.png";
}

function showWarnIcon() {
  var elt = document.getElementById('eidicon');
  elt.src = "chrome://global/skin/icons/warning-16.png";
}

function LoadEstEID() {
    try {
        var com = Components.classes["@id.eesti.ee/esteid;1"]
                            .createInstance(Components.interfaces.nsIEstEID);
        // var com = document.getElementById('eidplugin');

        log("Plugin Version: " + com.getVersion() + "\n");
        var elt = document.getElementById('esteidpanel');
	elt.setAttribute("tooltiptext", "EstEID plugin " + com.getVersion());
        showNormalIcon();
    } catch (anError) {
        error(anError);
	showErrorIcon();
    }
}

var eidui = Components.classes["@id.eesti.ee/esteid-private;1"]
                      .getService(Components.interfaces.nsIEstEIDPrivate);

// Load plugin to show it's version on status bar
window.addEventListener("load", LoadEstEID, false);
// Configure EstEID certificates and modules
window.addEventListener("load", ConfigureEstEID, false);
