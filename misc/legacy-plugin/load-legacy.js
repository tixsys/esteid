var load_legacy_esteid = true;
var myuri = '.';
//var myuri = "/legacy";

var err_notloaded = "Applet not loaded yet!";
var err_cancel    = "User Cancelled request";
var err_notresp   = "Applet not responding";
var err_nocert    = "Unable to read certificate";
var err_sign      = "Signing failed";

function hex2pem(input) {
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
}

function pollCert(id, f) {
    htmlLog("Creating certificate poller for '" + id + "'");
    document.ePoller = { id: id, getter: f, worker: function() {
        var e = document.getElementById(id);
        if(!e) return; // Our work is done, when our element is gone

        if(!e.signCert) {
            htmlLog("Polling for cert for '" + id + "'");
            try {
                e.signCert = document.ePoller.getter();
            } catch(err) {
                htmlLog("Polling error: " + err);
                e.signCert = null;
            }
        }

        window.setTimeout(document.ePoller.worker, 1000);
    }};
    window.setTimeout(document.ePoller.worker, 100);
}

function defineCertGetter(id, getter) {
    var e = document.getElementById(id);

    try {
        e.__defineGetter__("signCert", getter);
    } catch(err) {
        // Use polling for old IE-s that have no support for getters
        pollCert(id, getter);
    }
}

function getCertActiveX() {
    htmlLog("Loading cert data via ActiveX control");

    var s = document.getElementById("iSign");
    var cert = s.getSigningCertificate();
    if(cert) {
        return { cert: hex2pem(cert) };
    } else {
        throw(err_nocert);
    }
}

function createActiveX(id) {
    if(esteid_config && esteid_config.disable_ax) return false;

    htmlLog("Trying to create ActiveX control");

    var e = document.getElementById(id);

    try { 
        new ActiveXObject("EIDCard.Sign");

        var s = document.createElement('span');
        s.innerHTML = '<object id="iSign" classid="clsid:FC5B7BD2-584A-4153-92D7-4C5840E4BC28" style="width: 1px; height: 1px;" />';
        appendToBody(s);
        s = document.getElementById("iSign");

        htmlLog("Loaded ActiveX control " + s.getInfo()); // Loaded OK?

        e.signAsync = function(hash, url, callback) {
            var s = document.getElementById("iSign");
            htmlLog("Trying to sign with ActiveX control");
            var sig = s.getSignedHash(hash, s.selectedCertNumber);
            if (sig) {
                htmlLog("ActiveX sign successful");
                callback.onSuccess(sig);
            } else {
                htmlLog("ActiveX sign failed");
                callback.onError(err_cancel);
            }
            document.getElementById(id).signCert = null;
        }
        defineCertGetter(id, getCertActiveX);
        return true;
    } catch(err) {
        htmlLog("Failed to load ActiveX control " + err.message);
        return false;
    }
}

function getCertMoz() {
    htmlLog("Loading cert data via Mozilla plugin");

    var s = document.getElementById("mSign");
    var r = eval(s.getCertificates());
    if(r.returnCode != 0 || r.certificates.length < 1) {
        throw(err_nocert + "[" + r.returnCode + "]");
    }

    /* Find correct certificate */
    var reg = new RegExp("(^| |,)Non-Repudiation($|,)");
    var cert = null;
    for (var i in r.certificates) {
        cert = r.certificates[i];
        if (reg.exec(cert.keyUsage)) break;
    }

    if(!cert) throw(err_nocert + " no suitable cert on card");

    cert.cert = hex2pem(cert.cert);
    s.certID = cert.id;

    return cert;
}

function createMoz(id) {
    if(esteid_config && esteid_config.disable_moz) return false;

    if(!navigator.mimeTypes['application/x-idcard-plugin']) {
        htmlLog("Mozilla plugin (application/x-idcard-plugin) is not installed");
        return;
    }

    htmlLog("Trying to load Mozilla plugin");
    var e = document.getElementById(id);

    try {
        var s = document.createElement("embed");
        s.id           = 'mSign';
        s.type         = 'application/x-idcard-plugin';
        s.style.width  = "1px";
        s.style.height = "1px";
        appendToBody(s);
        var s = document.getElementById('mSign');

        htmlLog("Loaded Mozilla plugin " + s.getVersion()); // Loaded OK?

        e.signAsync = function(hash, url, callback) {
            var s = document.getElementById("mSign");
            htmlLog("Trying to sign with Mozilla plugin");
            var r = eval(s.sign(s.certID, hash));
            if (r.returnCode == 0) {
                htmlLog("Mozilla plugin sign successful");
                callback.onSuccess(r.signature);
            } else {
                htmlLog("Mozilla plugin sign failed: #" + r.returnCode);
                if(r.returnCode == 1) {
                    callback.onError(err_cancel);
                } else {
                    callback.onError(err_sign + "[" + r.returnCode + "]");
                }
            }
            document.getElementById(id).signCert = null;
        }
        defineCertGetter(id, getCertMoz);
        return true;
    } catch(err) {
        htmlLog("Failed to load Mozilla plugin: " + err.message);
        return false;
    }
}

function execJavaPrep(s) {
    s.prepare('window.jCrap.cert', 'window.jCrap.cancel',
              'window.jCrap.error');
}

function getCertJava() {
    htmlLog("Loading cert data via Java Applet");

    var s = window.jCrap.plugin;
    if(!s) throw(err_notloaded);
    window.jCrap.callback = null;
    window.jCrap.waiting = true;
    execJavaPrep(s);
    var start = (new Date()).getTime();
    while(window.jCrap.waiting) {
        var now = (new Date()).getTime();
        if(now - start > 10000) {
            window.jCrap.waiting = false;
            htmlLog("Loading of cert data failed: " + err_notresp);
            throw(err_notresp);
        }
    }
    if(window.jCrap.pem) {
        return { cert: window.jCrap.pem };
    } else {
        htmlLog("Loading of cert data failed: " + window.jCrap.errmsg);
        throw(window.jCrap.errmsg);
    }
}

function createJava(id, pluginReady) {
    if(esteid_config && esteid_config.disable_java) return false;

    htmlLog("Trying to create Java Applet");

    var e = document.getElementById(id);

/*
    e.innerHTML = '<applet id="jSign" code="SignatureApplet.class"' +
        ' width="1" height="1" archive="' + myuri + '/SignApplet_sig.jar,' +
        myuri + '/iaikPkcs11Wrapper_sig.jar" name="SignatureApplet">' +
        '<param name="mayscript" value="true">' +
        '<param name="debug_level" value="4">' +
        '<param name="lang" value="ENG">' +
        '</applet>';
*/

    e.innerHTML = '<object id="jSign" classid="clsid:8AD9C840-044E-11D1-B3E9-00805F499D93" width="1" height="1" codebase="http://java.sun.com/products/plugin/autodl/jinstall-1_4_0-win.cab#Version=1,4,0,0">' +
      '<param name="code" value="SignatureApplet.class">' +
      '<param name ="archive" value="' + myuri + '/SignApplet_sig.jar,' + myuri + '/iaikPkcs11Wrapper_sig.jar">' +
      '<param name="type" value="application/x-java-applet;version=1.4">' +
      '<param name="mayscript" value="true">' +
      '<param name="name" value="SignatureApplet">' +
      '<param name="debug_level" value="4">' +
      '<param name="lang" value="ENG">' +
      '<embed id="ejSign" type="application/x-java-applet;version=1.4" width="1" height="1"' +
      'pluginspage="http://java.sun.com/products/plugin/index.html#download" code="SignatureApplet.class"' +
      'archive="' + myuri + '/SignApplet_sig.jar,' + myuri +'/iaikPkcs11Wrapper_sig.jar" ' +
      'name="SignatureApplet" mayscript="true" lang="ENG" debug_level="4">' +
      '</embed></object>'

    window.jCrap = {
        waiting: false, pem: '', errmsg: '',
        slot: 10, plugin: null, callback: null,
        cert: function(slot, crt) {
            this.waiting = false;
            this.slot = slot;
            this.pem = hex2pem(crt);
            this.errmsg = '';
            htmlLog("Cert data received from Java Applet");
        },
        sign: function(hex) {
            this.errmsg = '';
            htmlLog("Java Applet sign successful");
            this.callback.onSuccess(hex);
            document.getElementById(id).signCert = null;
        },
        error: function(errcode, errmsg) {
            this.waiting = false;
            htmlLog("Java Applet error " + errcode + ":" + errmsg);
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
            htmlLog("Cancel pressed on Java Applet");
            if(this.callback) {
                this.callback.onError(err_cancel);
            } else {
                this.pem = this.hex = '';
                this.errmsg = err_cancel;
            }
        },
        poll: function() {
            htmlLog("Waiting for Java Applet to load ...");

            try {
                var s = document.getElementById("jSign");
                execJavaPrep(s);
                window.jCrap.plugin = s;
            } catch(err) { }
            try { 
                s = document.getElementById("ejSign");
                execJavaPrep(s);
                window.jCrap.plugin = s;
            } catch(err) { }

            // Loaded OK?
            if(window.jCrap.plugin) {
                defineCertGetter(id, getCertJava);
                htmlLog("Java Applet loaded successfully");
                pluginReady();
            } else {
                window.setTimeout(window.jCrap.poll, 500);
            }
        }
    };
    e.signAsync = function(hash, url, callback) {
        htmlLog("Trying to sign with Java Applet");

        var s = window.jCrap.plugin;
        if(!s) callback.onError(err_notloaded);
        window.jCrap.callback = callback;
        s.finalize(window.jCrap.slot, hash, 'window.jCrap.sign',
                   'window.jCrap.cancel', 'window.jCrap.error');
    }
    window.setTimeout('window.jCrap.poll();', 500);
}

function createSKplug(id) {
    if(esteid_config && esteid_config.disable_sk) return false;

    /* Many browsers implement it the FF way, but IE shows an empty list */
    if(navigator.mimeTypes && navigator.mimeTypes.length > 0 &&
       !(navigator.mimeTypes['application/x-digidoc'])) {
        /* Do not attempt to load plugin if it's not there.
         * Othervise Firefox will show a yellow bar */
        htmlLog("x-digidoc (application/x-digidoc) is not installed");
        return false;
    }

    htmlLog("Trying to create x-digidoc (id.eesti.ee)");

    var e = document.getElementById(id);

    try { 
        var s = document.createElement('span');
        s.innerHTML = '<object id="sSign" type="application/x-digidoc" style="width: 1px; height: 1px;" />';
        appendToBody(s);
        s = document.getElementById("sSign");

        if(s.version == null) throw(err_notresp); // Loaded OK?

        htmlLog("Loaded x-digidoc " + s.version);

        e.signAsync = function(hash, url, callback) {
            var s = document.getElementById("sSign");
            htmlLog("Trying to sign with x-digidoc plugin");
            try { 
              var cn = s.getCertificate().CN;
              var sig = s.sign(cn, hash, { language: 'en' });
              htmlLog("x-digidoc sign successful");
              callback.onSuccess(sig);
            } catch(e) {
                htmlLog("x-digidoc sign failed");
                callback.onError(err_sign + "[" + e.message + "]");
            }
            document.getElementById(id).signCert = null;
        }
        defineCertGetter(id, getCertSKplug);
        return true;
    } catch(err) {
        htmlLog("Failed to load x-digidoc plugin " + err.message);
        return false;
    }
}

function getCertSKplug() {
    htmlLog("Loading cert data via x-digidoc plugin");

    var s = document.getElementById("sSign");
    var c = s.getCertificate();
    return { cert: c.certificateAsPEM,
             CN: c.CN,
             validFrom: c.validFrom,
             validTo: c.validUntil };
}

function loadLegacySigner(id, pluginReady) {
    htmlLog("Trying to find and load some legacy signing plugin");

    /* Create a dummy DOM object to attach our wrapper functions to */
    var e = document.createElement("span");
    e.id = 'esteid';
    e = appendToBody(e);

    if(!createSKplug(id) && !createActiveX(id) && !createMoz(id))
        createJava(id, pluginReady);
    else
        pluginReady();
}
