/*
 * esteid-browser-plugin - a browser plugin for Estonian EID card
 *
 * Copyright (C) 2010  Estonian Informatics Centre
 * Copyright (C) 2010  Smartlink OÜ
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

try {
  Components.utils.import("resource://esteid/global.jsm");
} catch(e) {
  // Too old firefox / thunderbird. Default to window-local storage.
  var esteidglobal = {  
    log : "",
    errflag : false,
    isConfigured : false,
  };
  esteid_debug("No global JSM available, logs will be window specific");
}

function esteid_log(a) {
  esteidglobal.log += a + "\n";
}

function esteid_debug(a) {
  esteid_log("DEBUG: " + a);
}

function esteid_error(a) {
  esteid_log("ERROR: " + a);
  esteidglobal.errflag = true;
}

var Esteid =
{
    showNormalIcon: function()
    {
        var elt = document.getElementById('eidicon');
        elt.src = "chrome://esteid/skin/id-16.png";
    },

    showErrorIcon: function()
    {
        var elt = document.getElementById('eidicon');
        elt.src = "chrome://esteid/skin/id-error-16.png";
    },

    showWarnIcon: function()
    {
        var elt = document.getElementById('eidicon');
        elt.src = "chrome://esteid/skin/id-warning-16.png";
    },

    isBrowser: function()
    {
        var com = document.getElementById('esteidbrowserpanel');
        return (com != null);
    },

    showPrefs: function()
    {
        // Do not show multiple windows
        var ww = Components.classes["@mozilla.org/embedcomp/window-watcher;1"]
                           .getService(Components.interfaces.nsIWindowWatcher);
        var win = ww.getWindowByName("esteidprefs", null);

        if (win && !win.closed) {
          win.focus();
          return;
        }

        var params = { log: esteidglobal.log, out:null};
        window.openDialog("chrome://esteid/content/prefs.xul",
                           "esteidprefs", "chrome,centerscreen", params);
    },

    init: function()
    {
        if (this.isBrowser()) {
            var mimeTypeObject = navigator.mimeTypes
                                 .namedItem("application/x-esteid");

            if (mimeTypeObject) {
                try {
                    var eidver = mimeTypeObject.enabledPlugin.version;

                    if (!esteidglobal.isConfigured)
                        esteid_log("Plugin Version: " + eidver);

                    var elt = document.getElementById('esteidbrowserpanel');
                    if (eidver)
                        elt.setAttribute("tooltiptext", "EstEID plugin " + eidver);
                } catch(anError) {
                    /* Obtaining plugin version is not supported on
                     * FF3.5 and older browsers. It's OK. We don't care. */
                }
            } else {
                esteid_error("Open EstEID browser plugin is not installed");
            }
            try {
                EsteidConvertLegacy.init();
            } catch (anError) {
                esteid_error("Can't register legacy converter: " + anError);
            }
        }

        try {
            if (!esteidglobal.isConfigured)
                EsteidConfig.init();
        } catch (anError) {
            esteid_error(anError);
        }

        if (esteidglobal.errflag)
            this.showErrorIcon();
        else
            this.showNormalIcon();

        esteidglobal.errflag = false;
        esteidglobal.isConfigured = true;
    }
};

window.addEventListener("load", function() { Esteid.init(); }, false);
//window.addEventListener("unload", UnLoadEstEID, false);
