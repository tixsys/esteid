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

/* Fix braindead web pages by loading specific JS hacks
 * into HTML page in order to change broken behaviour.
 */
var esteidHackTable = [
  { url: "^https://id.seb.ee", js: "sebJava.js" }
];

var EsteidConvertLegacy =
{
  /* Shamelessly stolen from:
   * http://forums.mozillazine.org/viewtopic.php?p=921150#921150 */
  getContents: function(aURL)
  {
    var ioService=Components.classes["@mozilla.org/network/io-service;1"]
      .getService(Components.interfaces.nsIIOService);
    var scriptableStream=Components
      .classes["@mozilla.org/scriptableinputstream;1"]
      .getService(Components.interfaces.nsIScriptableInputStream);

    var channel=ioService.newChannel(aURL,null,null);
    var input=channel.open();
    scriptableStream.init(input);
    var str=scriptableStream.read(input.available());
    scriptableStream.close();
    input.close();
    return str;
  },

  injectJS: function(doc, js)
  {
    var se = doc.createElement('script');
    se.type = "text/javascript";
    var pe = doc.documentElement;
    se.innerHTML = js;
    pe.appendChild(se);
  },

  loadHacks: function(doc)
  {
    for (var i in esteidHackTable) {
      var o = esteidHackTable[i];
      try {
        var urlreg = new RegExp(o.url, "i");
        if (urlreg.exec(doc.location.href)) {
          esteid_log('Loading ' + o.js + ' on page ' + doc.location.href);
          this.injectJS(doc, this.getContents("chrome://esteid/content/" + o.js));
        }
      } catch(e) { esteid_log("Error in EsteidConvertLegacy.loadHacks: " + e); }
    }
  },

  convertObject: function(o, doc)
  {
    var p    = o.parentNode;
    var id   = o.getAttribute("id");
    var name = o.getAttribute("name");

    /* Support ancient Java Applets "parameter driven" mode */
    var op = null, fInit = null, fSig, fCert, fCancel, fError, hash, slot;
    var params = o.getElementsByTagName('param');
    for (var i in params) {
      try {
        var pname = params[i].getAttribute("name");
        var value = params[i].getAttribute("value");
        switch(pname) {
          case "name"            : name    = value; break;
          case "OPERATION"       : op      = value; break;
          case "HASH"            : hash    = value; break;
          case "TOKEN_ID"        : slot    = value; break;
          case "FUNC_SET_CERT"   : fCert   = value; break;
          case "FUNC_SET_SIGN"   : fSig    = value; break;
          case "FUNC_DRIVER_ERR" : fError  = value; break;
          case "FUNC_CANCEL"     : fCancel = value; break;
          case "FUNC_INIT"       : fInit   = value; break;
          default: break;
        }
      } catch(err) { }
    }

    esteid_log('Converting ' + (id ? id : name) + ' to new plugin on page ' +
               doc.location.href);

    o = p.removeChild(o);
    delete o;

    if (id == null)
      id = "Converted" + Math.floor(Math.random() * 1000);

    var e = doc.createElement('object');
    e.id           = id;
    e.name         = name;
    e.type         = 'application/x-esteid';
    e.style.width  = '1px';
    e.style.height = '1px';

    e = p.appendChild(e);

    /* Execute "parameter driven" mode functions */
    if (fInit) {
      this.injectJS(doc, fInit + "();");
    }

    if (op) {
      var cmd = "document.getElementById('" + id + "')";

      if (op == "FINALIZE") {
        cmd += ".finalize('" + slot + "','" + hash +
               "','" + fSig + "','" + fCancel + "','" + fError + "');";
      } else {
        var rnd = Math.floor(Math.random() * 1000);
        cmd += ".prepare('window.CertHack" + rnd +
               "','" + fCancel + "','" + fError + "');";
        cmd += "function CertHack" + rnd + "(a, b) { " + fCert + "(b, a); }";
      }
      this.injectJS(doc, cmd);
    }
  },

  /* Try to find Java code attribute
   * NB! Please keep this code in sync with components/OldJavaBlocker.js
   */
  findJavaCodeAttr: function(elem)
  {
    var code = elem.getAttribute("code");

    if (!code)
      code = elem.getAttribute("java_code");
    if (!code) {
      var clsid = elem.getAttribute("classid");
      if (clsid) {
        var cre = new RegExp("^java:(.*)$", "i");
        var match = cre.exec(clsid);
        if (match) code = match[1];
      }
    }

    if (!code) {
      var params = elem.getElementsByTagName('param');
      for (var i in params) {
        try {
          if (params[i].getAttribute("name") == "java_code")
            code = params[i].getAttribute("value");
        } catch(err) { }
      }
    }

    return code;
  },

  convert: function(doc)
  {
    if (!doc.location) return; // Optimize

    doc.esteidConvertLegacyRunning = true;

    /* Regex to find ActiveX tags (YES some sites send those to Firefox too) */
    var axreg = new RegExp("FC5B7BD2-584A-4153-92D7-4C5840E4BC28", "i");

    /* Find candidate objects */
    var ot = [ "applet", "embed", "object" ];
    for (var i in ot) {
      var els = doc.getElementsByTagName(ot[i]);
      for (var j = 0; j < els.length; j++) {
        var e = els[j];
        var code = this.findJavaCodeAttr(e);

        if (code == "SignatureApplet.class" || code == "SignApplet.class" ||
           code == "XMLSignApplet.class")
          this.convertObject(e, doc);
        else if (axreg.exec(e.getAttribute("classid")))
          this.convertObject(e, doc);
      }
    }
    doc.esteidConvertLegacyRunning = false;
  },

  init: function()
  {
    esteid_log('Activating legacy plugin tag converter');

    var appcontent = document.getElementById("appcontent");

    appcontent.addEventListener("DOMContentLoaded", function(aEvent) {
      var doc = aEvent.originalTarget;

      doc.addEventListener("DOMNodeInserted", function() {
        var doc = aEvent.originalTarget;
        if (!doc.esteidConvertLegacyRunning)
          EsteidConvertLegacy.convert(doc);
      }, false);
      EsteidConvertLegacy.convert(doc); }, false);

    // https://developer.mozilla.org/en/XUL_School/Intercepting_Page_Loads
    gBrowser.addEventListener("load", function(aEvent) {
      var doc = aEvent.originalTarget;
      if (doc instanceof HTMLDocument) {
        if (doc.defaultView.frameElement) {
          while (doc.defaultView.frameElement) {
            doc = doc.defaultView.frameElement.ownerDocument;
          }
        }
        EsteidConvertLegacy.loadHacks(doc);
      }
    }, true);
  }
};
