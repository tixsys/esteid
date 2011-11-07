/* Legacy plugin loader / API converter. Include this file only if
 * You need support for legacy signing plugins.
 */

estEidLoader.myuri         = '.';
estEidLoader.jaruri        = estEidLoader.myuri + '/jar';
estEidLoader.javaTimeout   = 30;
estEidLoader.err_notloaded = "Applet not loaded yet!";
estEidLoader.err_cancel    = "User Cancelled request";
estEidLoader.err_notresp   = "Applet not responding";
estEidLoader.err_nocert    = "Unable to read certificate";
estEidLoader.err_sign      = "Signing failed";

estEidLoader.waitForJavaStart = null;

estEidLoader.hex2pem = function(input) {
    var _keyStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    var output = "-----BEGIN CERTIFICATE-----\n";
    var chr1, chr2, chr3, enc1, enc2, enc3, enc4;
    var i = 0, j = 0;

    // Base64 encoder
    while (i < input.length) {
        chr1 = '0x' + input.charAt(i++) + input.charAt(i++);
        chr2 = '0x' + input.charAt(i++) + input.charAt(i++);
        chr3 = '0x' + input.charAt(i++) + input.charAt(i++);
 
        enc1 = chr1 >> 2;
        enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
        enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
        enc4 = chr3 & 63;
 
        if (isNaN(chr2)) {
            enc3 = enc4 = 64;
        } else if (isNaN(chr3)) {
            enc4 = 64;
        }
 
        output += _keyStr.charAt(enc1) + _keyStr.charAt(enc2) +
            _keyStr.charAt(enc3) + _keyStr.charAt(enc4);
        j += 4;
        if(j % 64 == 0) output += "\n";
 
    }

    output += "\n-----END CERTIFICATE-----\n";
 
    return output;
};

estEidLoader.pollCert = function(id, f) {
    estEidLoader.htmlLog("Creating certificate poller for '" + id + "'");
    document.ePoller = { id: id, getter: f, worker: function() {
        var e = document.getElementById(id);
        if(!e) return; // Our work is done, when our element is gone

        if(!e.signCert) {
            estEidLoader.htmlLog("Polling for cert for '" + id + "'");
            try {
                e.signCert = document.ePoller.getter();
            } catch(err) {
                estEidLoader.htmlLog("Polling error: " + err);
                e.signCert = null;
            }
        }

        window.setTimeout(document.ePoller.worker, 1000);
    }};
    window.setTimeout(document.ePoller.worker, 100);
};

estEidLoader.defineCertGetter = function(id, getter) {
    var e = document.getElementById(id);

    try {
        /* Define ES5 getter if possible */
        if(Object.defineProperty) {
            /* Check if getters actually work, IE8 has broken implementation. */
            var t = new Object();
            Object.defineProperty(t,"a",{get:function(){return {a: "ok"};}});
            if(t.a.a !== "ok") throw("Broken defineProperty implementation");

            Object.defineProperty(e, "signCert", { get : getter });
        } else {
            e.__defineGetter__("signCert", getter);
        }
    } catch(err) {
        // Use polling for old IE-s that have no support for getters
        estEidLoader.pollCert(id, getter);
    }
};

/* Support for SK-s x-digidoc plugin.
 * Available only for Mac and Linux as of 06.10.2010
 */
estEidLoader.getCertSKplug = function() {
    estEidLoader.htmlLog("Loading cert data via x-digidoc plugin");

    var s = document.getElementById("sSign");
    var c = s.getCertificate();

    /* SK Plugin has certificateAsPEM field, but it's broken in IE */

    /* TODO: Improve error handling */
    if(!c.certificateAsHex) throw("no cert data");

    s.certID = c.id;

    return { cert: estEidLoader.hex2pem(c.certificateAsHex),
             CN: c.CN,
             validFrom: c.validFrom,
             validTo: c.validUntil };
};
estEidLoader.setupSKplug = function(id) {
  var e = document.getElementById(id);

  e.signAsync = function(hash, url, callback) {
    var s = document.getElementById("sSign");
    estEidLoader.htmlLog("Trying to sign with x-digidoc plugin");
    try { 
      if(!s.certID) s.certID = s.getCertificate().id;
      var sig = s.sign(s.certID, hash, { language: 'en' });
      estEidLoader.htmlLog("x-digidoc sign successful");
      callback.onSuccess(sig);
    } catch(err) {
      estEidLoader.htmlLog("x-digidoc sign failed");
      callback.onError(estEidLoader.err_sign + "[" + err.message + "]");
    }
    document.getElementById(id).signCert = null;
  };
  e.getVersion = function() {
    var s = document.getElementById("sSign");
    return "0.0.0 " + s.version + " (legacy x-digidoc)";
  };
  estEidLoader.defineCertGetter(id, estEidLoader.getCertSKplug);
};
estEidLoader.createSKplug = function(id, pluginReady, pluginFail) {
  if(estEidLoader.disable_sk) { pluginFail(); return; }

  estEidLoader.htmlLog("Trying to create x-digidoc (id.eesti.ee)");

  estEidLoader.createMimeObject("sSign", "application/x-digidoc", {
    isWorking: function(e) { return (e.version) ? true : false; },
    onSuccess: function(e) { 
      estEidLoader.htmlLog("Loaded x-digidoc " + e.version);
      try {
        estEidLoader.setupSKplug(id);
        pluginReady();
      } catch(err) {
        estEidLoader.htmlLog("Failed to set up x-digidoc plugin " + err.message);
        pluginFail();
      }
    },
    onFail: function(e) { pluginFail(); }
  });
};

/* Support for old ActiveX signing control.
 * This was available only for Internet Explorer.
 */
estEidLoader.getCertActiveX = function() {
  estEidLoader.htmlLog("Loading cert data via ActiveX control");

  var s = document.getElementById("iSign");
  var cert = s.getSigningCertificate();
  if(cert) {
    return { cert: estEidLoader.hex2pem(cert) };
  } else {
    throw(estEidLoader.err_nocert);
  }
};
estEidLoader.createActiveX = function(id) {
  if(estEidLoader.disable_ax) return false;

  estEidLoader.htmlLog("Trying to create ActiveX control");

  var e = document.getElementById(id);

  try { 
    new ActiveXObject("EIDCard.Sign");

    var s = document.createElement('span');
    s.innerHTML = '<object id="iSign" classid="clsid:FC5B7BD2-584A-4153-92D7-4C5840E4BC28" style="width: 1px; height: 1px;" />';
    estEidLoader.appendToBody(s);
    s = document.getElementById("iSign");

    estEidLoader.htmlLog("Loaded ActiveX control " + s.getInfo()); // Loaded OK?

    e.signAsync = function(hash, url, callback) {
      var s = document.getElementById("iSign");
      estEidLoader.htmlLog("Trying to sign with ActiveX control");
      var sig = s.getSignedHash(hash, s.selectedCertNumber);
      if (sig) {
        estEidLoader.htmlLog("ActiveX sign successful");
        callback.onSuccess(sig);
      } else {
        estEidLoader.htmlLog("ActiveX sign failed");
        callback.onError(estEidLoader.err_cancel);
      }
      document.getElementById(id).signCert = null;
    };
    e.getVersion = function() {
      var s = document.getElementById("iSign");
      return "0.0.0 " + s.getInfo() + " (legacy ActiveX control)";
    };
    estEidLoader.defineCertGetter(id, estEidLoader.getCertActiveX);
    return true;
  } catch(err) {
    estEidLoader.htmlLog("Failed to load ActiveX control " + err.message);
    return false;
  }
};

/* Support for old Mozilla signing plugin. This was originally developed by
 * Swedbank and is available only for Windows.
 */
estEidLoader.getCertMoz = function() {
  estEidLoader.htmlLog("Loading cert data via Mozilla plugin");

  var s = document.getElementById("mSign");
  var r = eval(s.getCertificates());
  if(r.returnCode != 0 || r.certificates.length < 1) {
    throw(estEidLoader.err_nocert + "[" + r.returnCode + "]");
  }

  /* Find correct certificate */
  var reg = new RegExp("(^| |,)Non-Repudiation($|,)");
  var cert = null;
  for (var i in r.certificates) {
    cert = r.certificates[i];
    if (reg.exec(cert.keyUsage)) break;
  }

  if(!cert) throw(estEidLoader.err_nocert + " no suitable cert on card");

  cert.cert = estEidLoader.hex2pem(cert.cert);
  s.certID = cert.id;

  return cert;
};
estEidLoader.setupMoz = function(id) {
  var e = document.getElementById(id);

  e.signAsync = function(hash, url, callback) {
    var s = document.getElementById("mSign");
    estEidLoader.htmlLog("Trying to sign with Mozilla plugin");
    var r = eval(s.sign(s.certID, hash));
    if (r.returnCode == 0) {
      estEidLoader.htmlLog("Mozilla plugin sign successful");
      callback.onSuccess(r.signature);
    } else {
      estEidLoader.htmlLog("Mozilla plugin sign failed: #" + r.returnCode);
      if(r.returnCode == 1) {
        callback.onError(estEidLoader.err_cancel);
      } else {
        callback.onError(estEidLoader.err_sign + "[" + r.returnCode + "]");
      }
    }
    document.getElementById(id).signCert = null;
  };
  e.getVersion = function() {
    var s = document.getElementById("mSign");
    return "0.0.0 " + s.getVersion() + " (legacy Mozilla plugin)";
  };
  estEidLoader.defineCertGetter(id, estEidLoader.getCertMoz);
};
estEidLoader.createMoz = function(id, pluginReady, pluginFail) {
  if(estEidLoader.disable_moz) { pluginFail(); return; }

  var mt = "application/x-idcard-plugin";

  if(!navigator.mimeTypes[mt]) {
    estEidLoader.htmlLog("Mozilla plugin (" + mt + ") is not installed");
    pluginFail();
    return;
  }

  estEidLoader.htmlLog("Trying to load Mozilla plugin");

  estEidLoader.createMimeObject("mSign", mt, {
    isWorking: function(e) { return (e.getVersion()) ? true : false; },
    onSuccess: function(e) { 
      estEidLoader.htmlLog("Loaded Mozilla plugin " + e.getVersion());
      try {
        estEidLoader.setupMoz(id);
        pluginReady();
      } catch(err) {
        estEidLoader.htmlLog("Failed to set up Mozilla plugin " + err.message);
        pluginFail();
      }
    },
    onFail: function(e) { pluginFail(); }
  });
};

/* Support for old Java signing applet. This was originally developed by
 * SEB bank and is available for all platforms. However the Java browser plugin
 * is slow and unstable at times, also the applet may require that the user
 * manually sets up a JNI PKCS#11 bridge before it can talk to the card.
 */
estEidLoader.execJavaPrep = function(s) {
  s.prepare('window.jCrap.cert', 'window.jCrap.cancel',
            'window.jCrap.error');
};
estEidLoader.getCertJava = function() {
  estEidLoader.htmlLog("Loading cert data via Java Applet");

  var s = window.jCrap.plugin;
  if(!s) throw(estEidLoader.err_notloaded);
  window.jCrap.callback = null;
  window.jCrap.waiting = true;
  estEidLoader.execJavaPrep(s);
  var start = (new Date()).getTime();
  while(window.jCrap.waiting) {
    var now = (new Date()).getTime();
    if(now - start > 10000) {
      window.jCrap.waiting = false;
      estEidLoader.htmlLog("Loading of cert data failed: " + estEidLoader.err_notresp);
      throw(estEidLoader.err_notresp);
    }
  }
  if(window.jCrap.pem) {
    return { cert: window.jCrap.pem };
  } else {
    estEidLoader.htmlLog("Loading of cert data failed: " + window.jCrap.errmsg);
    throw(window.jCrap.errmsg);
  }
};
estEidLoader.createJava = function(id, pluginReady, pluginFail) {
  if(estEidLoader.disable_java) { pluginFail(); return; }

  var jaruri = estEidLoader.jaruri;

  estEidLoader.htmlLog("Trying to create Java Applet");

  var e = document.getElementById(id);

  try {
    var s = document.createElement('span');
    s.innerHTML = '<object id="jSign" classid="clsid:8AD9C840-044E-11D1-B3E9-00805F499D93" width="1" height="1" codebase="http://java.sun.com/products/plugin/autodl/jinstall-1_4_0-win.cab#Version=1,4,0,0">' +
      '<param name="code" value="SignatureApplet.class">' +
      '<param name ="archive" value="' + jaruri + '/SignApplet_sig.jar,' + jaruri + '/iaikPkcs11Wrapper_sig.jar">' +
      '<param name="type" value="application/x-java-applet;version=1.4">' +
      '<param name="mayscript" value="true">' +
      '<param name="name" value="SignatureApplet">' +
      '<param name="debug_level" value="4">' +
      '<param name="lang" value="ENG">' +
      '<embed id="ejSign" type="application/x-java-applet;version=1.4" width="1" height="1"' +
      'pluginspage="http://java.sun.com/products/plugin/index.html#download" code="SignatureApplet.class"' +
      'archive="' + jaruri + '/SignApplet_sig.jar,' + jaruri +'/iaikPkcs11Wrapper_sig.jar" ' +
      'name="SignatureApplet" mayscript="true" lang="ENG" debug_level="4">' +
      '</embed></object>';
    estEidLoader.appendToBody(s);
  } catch(err) {
    estEidLoader.htmlLog("Failed to create applet: " + err.message);
    pluginFail();
    return;
  }

  window.jCrap = {
    waiting: false, pem: '', errmsg: '',
    slot: 10, plugin: null, callback: null,
    cert: function(slot, crt) {
      this.waiting = false;
      this.slot = slot;
      this.pem = estEidLoader.hex2pem(crt);
      this.errmsg = '';
      estEidLoader.htmlLog("Cert data received from Java Applet");
    },
    sign: function(hex) {
      this.errmsg = '';
      estEidLoader.htmlLog("Java Applet sign successful");
      this.callback.onSuccess(hex);
      document.getElementById(id).signCert = null;
    },
    error: function(errcode, errmsg) {
      this.waiting = false;
      estEidLoader.htmlLog("Java Applet error " + errcode + ":" + errmsg);
      if(this.callback) {
        this.callback.onError(errcode + ": " + errmsg);
      } else {
        this.pem = this.hex = '';
        this.errmsg = errcode + ": " + errmsg;
      }
      document.getElementById(id).signCert = null;
    },
    cancel: function() {
      this.waiting = false;
      estEidLoader.htmlLog("Cancel pressed on Java Applet");
      if(this.callback) {
        this.callback.onError(estEidLoader.err_cancel);
      } else {
        this.pem = this.hex = '';
        this.errmsg = estEidLoader.err_cancel;
      }
    },
    poll: function() {
      estEidLoader.htmlLog("Waiting for Java Applet to load ...");

      try {
        var s = document.getElementById("jSign");
        estEidLoader.execJavaPrep(s);
        window.jCrap.plugin = s;
      } catch(err) { }
      try { 
        s = document.getElementById("ejSign");
        estEidLoader.execJavaPrep(s);
        window.jCrap.plugin = s;
      } catch(err) { }

      // Loaded OK?
      if(window.jCrap.plugin) {
        estEidLoader.defineCertGetter(id, estEidLoader.getCertJava);
        estEidLoader.htmlLog("Java Applet loaded successfully");
        estEidLoader.waitForJavaStart = null;
        pluginReady();
      } else {
        var now = (new Date()).getTime();
        var millis = now - estEidLoader.waitForJavaStart;
        if(millis > estEidLoader.javaTimeout * 1000) {
          estEidLoader.htmlLog("Timed out waiting for Java Applet to load");
          estEidLoader.waitForJavaStart = null;
          pluginFail();
        } else {
          window.setTimeout(window.jCrap.poll, 1000);
        }
      }
    }
  };
  e.signAsync = function(hash, url, callback) {
    estEidLoader.htmlLog("Trying to sign with Java Applet");

    var s = window.jCrap.plugin;
    if(!s) {
      callback.onError(estEidLoader.err_notloaded);
      return;
    }
    window.jCrap.callback = callback;
    s.finalize(window.jCrap.slot, hash, 'window.jCrap.sign',
               'window.jCrap.cancel', 'window.jCrap.error');
  };
  e.getVersion = function() {
    return "0.0.0 (legacy Java Applet)";
  };
  estEidLoader.waitForJavaStart = (new Date()).getTime();
  window.setTimeout('window.jCrap.poll();', 500);
};

/**
 * Try to load some legacy signing method and set up an API converter
 * so it can be used exactly the same way as the new universal plugin.
 *
 * @param {String} id Id for the plugin
 * @param {Function} pluginReady a function that will be called on success
 * @param {Function} pluginFail a function that will be called on failure
 */
estEidLoader.loadLegacySigner = function(id, cb) {
  estEidLoader.htmlLog("Trying to find and load some legacy signing plugin");

  /* Create a dummy DOM object to attach our wrapper functions to */
  var e = document.createElement("span");
  e.id = id;
  e.isLegacy = true;
  e = estEidLoader.appendToBody(e);

  estEidLoader.createSKplug(id, cb.pluginReady, function() {
    if(estEidLoader.createActiveX(id)) {
      cb.pluginReady();
    } else {
      estEidLoader.createMoz(id, cb.pluginReady, function() {
        estEidLoader.createJava(id, cb.pluginReady, function() {
          if(e) estEidLoader.removeFromBody(e);
          estEidLoader.htmlLog("Failed to find any legacy plugin");
          cb.pluginFail("Unable to find any supported signing plugin");
        });
      });
    }
  });
};

/* Compatibility flag. This shall be removed soon */
var load_legacy_esteid = true;
