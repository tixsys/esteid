//test1
var estEidLoader = {
  pluginTimeout: 10,
  waitForObjectStart: null,

  keyString: "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",
	
  uTF8Encode: function(string) {
		string = string.replace(/\x0d\x0a/g, "\x0a");
		var output = "";
		for (var n = 0; n < string.length; n++) {
			var c = string.charCodeAt(n);
			if (c < 128) {
				output += String.fromCharCode(c);
			} else if ((c > 127) && (c < 2048)) {
				output += String.fromCharCode((c >> 6) | 192);
				output += String.fromCharCode((c & 63) | 128);
			} else {
				output += String.fromCharCode((c >> 12) | 224);
				output += String.fromCharCode(((c >> 6) & 63) | 128);
				output += String.fromCharCode((c & 63) | 128);
			}
		}
		return output;
	},
	
	base64Encode: function(input) {
		var output = "";
		var chr1, chr2, chr3, enc1, enc2, enc3, enc4;
		var i = 0;
		input = uTF8Encode(input);
		while (i < input.length) {
			chr1 = input.charCodeAt(i++);
			chr2 = input.charCodeAt(i++);
			chr3 = input.charCodeAt(i++);
			enc1 = chr1 >> 2;
			enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
			enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
			enc4 = chr3 & 63;
			if (isNaN(chr2)) {
				enc3 = enc4 = 64;
			} else if (isNaN(chr3)) {
				enc4 = 64;
			}
			output = output + keyString.charAt(enc1) + keyString.charAt(enc2) + keyString.charAt(enc3) + keyString.charAt(enc4);
		}
		return output;
	},
	base64Decode: function(input) {
		var output = "";
		var chr1, chr2, chr3;
		var enc1, enc2, enc3, enc4;
		var i = 0;
		input = input.replace(/[^A-Za-z0-9\+\/\=]/g, "");
		while (i < input.length) {
			enc1 = keyString.indexOf(input.charAt(i++));
			enc2 = keyString.indexOf(input.charAt(i++));
			enc3 = keyString.indexOf(input.charAt(i++));
			enc4 = keyString.indexOf(input.charAt(i++));
			chr1 = (enc1 << 2) | (enc2 >> 4);
			chr2 = ((enc2 & 15) << 4) | (enc3 >> 2);
			chr3 = ((enc3 & 3) << 6) | enc4;
			output = output + String.fromCharCode(chr1);
			if (enc3 != 64) {
				output = output + String.fromCharCode(chr2);
			}
			if (enc4 != 64) {
				output = output + String.fromCharCode(chr3);
			}
		}
		output = uTF8Decode(output);
		return output;
	},
  /**
   * Log a message to DOM element with htmlLog
   *
   * @param {String} msg a message to log
   */
  htmlLog: function(msg) {
    var e = document.getElementById("htmlLog");
    if(e) e.innerHTML = "[" + (new Date()) + "] " + msg + "<br />\n" + e.innerHTML;
    if (window.console != undefined) {
        console.log(msg);
    }
  },

  /**
   * Append element to documents "body" tag
   *
   * @param {DOMObject} e an element to add
   * @return a reference to the added object
   */
  appendToBody: function(e) {
    var b = document.getElementsByTagName("body")[0];
    return b.appendChild(e);
  },

  /**
   * Remove element from documents "body" tag
   * @param {DOMObject} e an element to remove
   */
  removeFromBody: function(e) {
    try {
        var b = document.getElementsByTagName("body")[0];
        b.removeChild(e);
    } catch(err) {
        estEidLoader.htmlLog("Failed to remove object from body: " + err.message);
    }
  },

  /**
   * Wait for an object to start responding.
   *
   * @param {String} id id of the object to poll
   * @param {int} timeout how many seconds to wait
   * @param {Object} callbacks an object that has following properties:
   *   isWorking a function to validate the loaded object
   *   onSuccess a function to be called on success
   *   onFail a function to be called on failure
   */
  waitForObject: function(id, timeout, callbacks) {
    var e = document.getElementById(id);
    var ok = false;

    /* Return immediately if everything is OK */
    try { ok = callbacks.isWorking(e); } catch(err) { }
    if(ok) {
      estEidLoader.waitForObjectStart = null;
      callbacks.onSuccess(e);
      return;
    }

    /* Wait for plugin only on browsers that support plugin registry
     * because then we are sure that the plugin actually has a chance
     * to become active. Othervise we are just wasting valuable time. */
    if(!navigator.mimeTypes || navigator.mimeTypes.length <= 0) {
      callbacks.onFail(e);
      return;
    }

    /* Set starting timestamp */
    if(!estEidLoader.waitForObjectStart) {
      estEidLoader.waitForObjectStart = (new Date()).getTime();
    }

    /* Check time */
    var now = (new Date()).getTime();
    var millis = now - estEidLoader.waitForObjectStart
    if(millis > timeout * 1000) {
      estEidLoader.waitForObjectStart = null;
      callbacks.onFail(e);
      return;
    }

    /* Schedule another check */
    estEidLoader.htmlLog("Waiting for plugin to load ...");
    window.setTimeout(function() {
      estEidLoader.waitForObject(id, timeout, callbacks);
    }, 1000);
  },

  /**
   * Inject an object tag into DOM with specified id and mimeType.
   * The object will not be created if the specified MIME type is not
   * listed in plugin registry othervise Firefox will show a
   * "missing plugin" notification bar.
   *
   * @parem {String} id id for the new object
   * @param {String} mimeType MIME type of the new object
   * @param {Object} callbacks an object that has following properties:
   *   isWorking a function to validate the loaded object
   *   onSuccess a function to be called on success
   *   onFail a function to be called on failure
   */
  createMimeObject: function(id, mimeType, callbacks) {
    var e = null;
    var errpfx = "Failed to load plugin for " + mimeType + ": ";

    /* Check if the requested MIME type is available in the plugin registry.
     * If we try to create an object for MIME type that is not available,
     * Firefox will show a notification bar asking the user to install it.
     *
     * For browsers that do not expose plugin registry, we try to load
     * the object anyway. There is another quirk that must be taken into
     * account: many browsers implement the registry in Firefox way,
     * but IE exposes an empty list. That's why we must check
     * if the length of the list is greater than zero below.
     */
    if(navigator.mimeTypes && navigator.mimeTypes.length > 0 &&
       !(navigator.mimeTypes[mimeType])) {
      estEidLoader.htmlLog("Plugin for " + mimeType + " is not installed");
      callbacks.onFail(e);
      return;
    }

    try {
      e = document.createElement('span');
      e.innerHTML = '<object style="width: 1px; height: 1px;" id="' + id +
                          '" type="' + mimeType + '" />';
      e = estEidLoader.appendToBody(e);
    } catch(err) {
      estEidLoader.htmlLog(errpfx + err.message);
      callbacks.onFail(e);
      return;
    }

    /* Wait for object to become active */
    estEidLoader.waitForObject(id, estEidLoader.pluginTimeout, {
      isWorking: callbacks.isWorking,
      onSuccess: callbacks.onSuccess,
      onFail: function(e) {
        estEidLoader.htmlLog(errpfx + "timed out while waiting");
        callbacks.onFail(e);
      }
    });
  },
  getCert: function(){
	  return estEidLoader.card.signCert.cert;
  },
  getCertBase64: function(){
	  return estEidLoader.base64Encode(estEidLoader.getCert());
  },
  doSign:function(hash, url, callbacks){
	  estEidLoader.htmlLog("Starting sign asychronously");
	  estEidLoader.card.signAsync(hash, url, 
		{
			onSuccess:function(hex) {
				estEidLoader.htmlLog("Sign succeeded:"+hex);
				callbacks.onSuccess(hex);
			},
			onError:function(msg) {
				estEidLoader.htmlLog("Sign failed:"+msg);
				callbacks.onError(msg);
			}
		}
	  );
  },
  
  /**
   * Inject an esteid plugin object into DOM using a specified id
   *
   * @param {String} id id of the new object
   * @param {Object} callbacks an object wirh following properties
   *   onSuccess called on successful load
   *   onFail called on failure
   */
  createEstEidObject: function(id, callbacks) {
    if(estEidLoader.disable_new) {
      estEidLoader.htmlLog("New plugin support disabled by config");
      callbacks.onFail(null);
      return;
    }

    estEidLoader.htmlLog("Trying to load esteid browser plugin");

    estEidLoader.createMimeObject(id, "application/x-esteid", {
      isWorking: function(e) { return (e.getVersion()) ? true : false; },
      onSuccess: function(e) {
        if(typeof(e.signAsync) == "undefined") {
          estEidLoader.htmlLog("Failed to utilize plugin, You are using an experimental software version, please update!");
          callbacks.onFail(e);
        } else {
          estEidLoader.htmlLog("Using browser plugin: " + e.getVersion());
          estEidLoader.card=document.getElementById(id);
          try {
        	  if(typeof(callbacks.onCardInserted) != "undefined") 
        		  addEvent(estEidLoader.card, "CardInserted", callbacks.onCardInserted);
        	  
        	  if(typeof(callbacks.onCardRemoved) != "undefined")
        		  addEvent(estEidLoader.card, "CardRemoved", callbacks.onCardRemoved);
        	  
        	  if(typeof(callbacks.onSignSuccess) != "undefined")
        		  addEvent(estEidLoader.card, "SignSuccess", callbacks.onSignSuccess);
        	  
        	  if(typeof(callbacks.onSignFailure) != "undefined")
        		  addEvent(estEidLoader.card, "SignFailure", callbacks.onSignFailure);
              
        	  callbacks.onSuccess(e);
          }
          catch(err) {
        	  callbacks.onFail(err);
          }
          
        }
      },
      onFail: callbacks.onFail
    });
  },
  card:null,
  isCardIn:false,
  /**
   * Load esteid signing plugin into document.
   * The function will attempt to find the esteid browser plugin
   * or load some legacy signing plugin if it's not available.
   * Will call pluginReady when plugin is loaded or pluginFail
   * when no plugin could be activated.
   */
  loadPlugin: function(id, callbacks) {
    estEidLoader.createEstEidObject(id, {
      onSuccess: function(e) { 
    	  
    	  //try to read card info
    	  try{
    		  if(typeof(estEidLoader.card.signCert)=="undefined" || typeof(estEidLoader.card.signCert)=="unknown")
			  {
			  	callbacks.onCardRemoved();
			  	estEidLoader.isCardIn=false;
			  	 callbacks.pluginReady(null);
			  	 return;
			  }
    		  estEidLoader.isCardIn=true;  
    		  callbacks.pluginReady(estEidLoader.getCert());
    		  return;
    	  }catch(err)
    	  {
    		  if(err.message.indexOf("No cards found")> -1){	  	
    			  callbacks.onCardRemoved();
    		  	  estEidLoader.isCardIn=false;
    		  }  
    		  else{
    			  estEidLoader.htmlLog(err);
        		  callbacks.pluginFail('Error on plugin initialization: '+err);
        		  estEidLoader.isCardIn=false;  
        		  return;
    		  }
    		  
    	  }
    	  callbacks.pluginReady(null);
    	  
      },
      onFail: function(e) {
        if(e) estEidLoader.removeFromBody(e);

        if(estEidLoader.loadLegacySigner) {
          estEidLoader.loadLegacySigner(id, callbacks);
        } else {
          callbacks.pluginFail('Unable to load EstEid plugin...');
        }
      },
	  onCardInserted:function(reader){
		  estEidLoader.isCardIn=true;
		   callbacks.onCardInserted(estEidLoader.getCert());
	  },
	  onCardRemoved:function(reader){
		  estEidLoader.isCardIn=false;
		  callbacks.onCardRemoved();
	  },
	  onSignSuccess:function(hex){
		  callbacks.onSignSuccess(hex);
	  },
	  onSignFailure:function(msg){
		  callbacks.onSignFailure(msg);
	  }
    });

    return;
  }, 
  readPersonInfo:function(){
	  estEidLoader.htmlLog("readPersonInfo");

	  var methodNames=[ "firstName", "lastName", "middleName", "sex", "citizenship", "birthDate", "personalID", "documentID", "expiryDate", "placeOfBirth", "issuedDate", "residencePermit", "comment1", "comment2", "comment3", "comment4"];
	  var personInfo={};
	  
		  var isOldCard=typeof(estEidLoader.card.personalData)!="undefined";
		  for (var prop in methodNames) {
			  var method=methodNames[prop];
			  if(isOldCard)
				  personInfo[method]=estEidLoader.personalData[method];
			  else
				  personInfo[method]=estEidLoader.card[method];
			  estEidLoader.htmlLog("gotPersonInfo:"+method+" "+personInfo[method]);
		  }	
	    
	  
	  return personInfo;
  }
};

function addEvent(obj, name, func)
{
    if (window.addEventListener) {
        obj.addEventListener(name, func, false); 
    } else {
        obj.attachEvent("on"+name, func);
    }
}
/* Compatibility function. This shall be removed soon */
function loadEstEidPlugin(id, pluginReady) {
  estEidLoader.htmlLog("WARNING You are using loadEstEidPlugin compatibility function that will be removed soon");

  if(!load_legacy_esteid)
    estEidLoader.loadLegacySigner = null;

  if(esteid_config)
    for(i in esteid_config)
      estEidLoader[i] = esteid_config[i];

  estEidLoader.loadPlugin(id, pluginReady, {
      pluginReady: pluginReady,
      pluginFail: function() {
        estEidLoader.htmlLog("Signing is impossible");
      }
  });
}



function sign(id, hash){
	var esteid = document.getElementById(id);
	return esteid.sign();
}


/* Legacy plugin loader / API converter. Include this file only if
 * You need support for legacy signing plugins.
 */

estEidLoader.myuri         = '.';
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
        e.__defineGetter__("signCert", getter);
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

    /* TODO: Improve error handling */
    if(!c.certificateAsPEM) throw("no cert data");

    return { cert: c.certificateAsPEM,
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
      var id = s.getCertificate().id;
      var sig = s.sign(id, hash, { language: 'en' });
      estEidLoader.htmlLog("x-digidoc sign successful");
      callback.onSuccess(sig);
    } catch(e) {
      estEidLoader.htmlLog("x-digidoc sign failed");
      callback.onError(estEidLoader.err_sign + "[" + e.message + "]");
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

  var myuri = estEidLoader.myuri;

  estEidLoader.htmlLog("Trying to create Java Applet");

  var e = document.getElementById(id);

  try {
    var s = document.createElement('span');
    s.innerHTML = '<object id="jSign" classid="clsid:8AD9C840-044E-11D1-B3E9-00805F499D93" width="1" height="1" codebase="http://java.sun.com/products/plugin/autodl/jinstall-1_4_0-win.cab#Version=1,4,0,0">' +
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
    if(!s) callback.onError(estEidLoader.err_notloaded);
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
  e.id = 'esteid';
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
          cb.pluginFail();
        });
      });
    }
  });
};

/* Compatibility flag. This shall be removed soon */
var load_legacy_esteid = true;
