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

var EsteidPrefs =
{
  plugin: null,

  onLoad: function()
  {
    var sbs = Components.classes["@mozilla.org/intl/stringbundle;1"]
                        .getService(Components.interfaces.nsIStringBundleService);
    var sb = sbs.createBundle("chrome://esteid/locale/esteid.properties");

    var logmsg;
    try {
      Components.utils.import("resource://esteid/global.jsm");
      logmsg = esteidglobal.log;
    } catch(e) {
      var params = window.arguments[0];

      if (params.log)
        logmsg = params.log;
      else
        logmsg = "";
      logmsg += e + "\n";
    }

    var elt = document.getElementById('log');
    elt.value = logmsg;

    this.plugin = this.getPlugin();
    if (this.plugin != null)
      document.getElementById('wlbtn').disabled = false;
  },

  doOK: function()
  {
  },

  getPlugin: function()
  {
    return document.getElementById('eidplugin');
  },

  openPluginSettings: function()
  {
    this.plugin.showSettings();
  }
};
