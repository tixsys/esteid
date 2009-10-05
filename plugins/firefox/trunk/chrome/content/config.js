/* Configure Estonian ID Card support.
 * This JavaScript will handle CA certificate import and load required
 * security modules into browser.
 */

const nsNSSCertCache          = "@mozilla.org/security/nsscertcache;1";
const nsModDB                 = "@mozilla.org/security/pkcs11moduledb;1"
const nsX509CertDB            = "@mozilla.org/security/x509certdb;1"
const nsLocalFile             = "@mozilla.org/file/local;1";
const nsFileInputStream       = "@mozilla.org/network/file-input-stream;1";
const nsScriptableInputStream = "@mozilla.org/scriptableinputstream;1";
const nsPKCS11                = "@mozilla.org/security/pkcs11;1";
const nsHttpProtocolHandler   = "@mozilla.org/network/protocol;1?name=http";
const nsWindowsRegKey         = "@mozilla.org/windows-registry-key;1";

const EstEidModName = "Estonian ID Card";
const EstEidLegacyNames = [ "opensc-pkcs11", "ID-Kaart" ];
const soName = "onepin-opensc-pkcs11";

var logmsg = "";

function log(a) {
    logmsg += a + "\n";
}

function debug(a) {
    log("DEBUG: " + a);
}


function PEM2Base64(pem) {
    var beginCert = "-----BEGIN CERTIFICATE-----";
    var endCert = "-----END CERTIFICATE-----";
    pem = pem.replace(/[\r\n]/g, "");
    var begin = pem.indexOf(beginCert);
    var end = pem.indexOf(endCert);
    return pem.substring(begin + beginCert.length, end);
}

function ConfigureEstEID() {
    const Ci = Components.interfaces;
    const Cc = Components.classes;

    /* Detect platform.
     * We could use navigator.platform or navigator.oscpu, but unfortunately
     * those can be overriden by preferences so we have to go to the source
     * of this information which is calculated here 
     * mozilla/netwerk/protocol/http/src/nsHttpHandler.cpp
     */
    var httpHandler = Cc[nsHttpProtocolHandler]
                        .getService(Ci.nsIHttpProtocolHandler);
    var oscpu = httpHandler.oscpu;
    var platform = httpHandler.platform;

    debug("OSCPU: " + oscpu);
    debug("Platform: " + platform);

    var bits = (oscpu.indexOf(" x86_64") < 0 &&
                oscpu.indexOf(" x64") < 0) ? 32 : 64;

    debug("Detected " + bits + " bit browser");

    var certDir;
    var moduleDlls;

    /* Set platform specific paths */
    if(platform == "Windows") {
        var EstEidInstallDir;
        var win32Dir;

        /* Load sysdir location from registry */
        try {
            var key = Cc[nsWindowsRegKey].createInstance(Ci.nsIWindowsRegKey);
            key.open(Ci.nsIWindowsRegKey.ROOT_KEY_LOCAL_MACHINE,
                     "Software\\Microsoft\\Windows NT\\CurrentVersion",
                     Ci.nsIWindowsRegKey.ACCESS_READ);
            win32Dir = key.readStringValue("SystemRoot");
        } catch(e) {
            var win32Dir = "C:\\WINDOWS";
        }
        win32Dir += "\\System32\\";

        /* Load EstEID stuff location from registry */
        try {
            var key = Cc[nsWindowsRegKey].createInstance(Ci.nsIWindowsRegKey);
            key.open(Ci.nsIWindowsRegKey.ROOT_KEY_LOCAL_MACHINE,
                     "Software\\Estonian ID Card",
                     Ci.nsIWindowsRegKey.ACCESS_READ);
            EstEidInstallDir = key.readStringValue("Installed");
        } catch(e) {
            log("ERROR: Unable to read EstEID path from registry: " + e);
            EstEidInstallDir = "C:\\Program Files\\Estonian ID Card\\";
        }
        certDir = EstEidInstallDir + "certs";
        moduleDlls = [ EstEidInstallDir + soName + ".dll",
                       win32Dir + soName + ".dll" ];
    } else if(platform == "Macintosh") {
        certDir = "/usr/local/etc/digidocpp/certs";
        moduleDlls = [ "/Library/OpenSC/lib/" + soName + ".so" ];
    } else if(platform == "X11") { // Linux/Unix
        var libdir = (bits == 64) ? "/usr/lib64/" : "/usr/lib/";
        certDir = "/etc/digidocpp/certs";
        moduleDlls = [ libdir + soName + ".so",
                       libdir + "pkcs11/" + soName + ".so" ];
    } else {
        log("ERROR: Unknown plaform " + platform);
        return;
    }

    debug("Cert Dir: " + certDir);
    debug("Module Dlls: " + moduleDlls);

    var EstEidDll;
    var needtoload = false;

    /* Go through module path list and pick the first that actually exists */
    for(i in moduleDlls) {
        var file = Cc[nsLocalFile].createInstance(Ci.nsILocalFile);
        var readable = false;
        try {
            file.initWithPath(moduleDlls[i]);
            var readable = file.isReadable();
        } catch (e) { };

        if(readable) {
            EstEidDll = moduleDlls[i];
            needtoload = true;
            break;
        }
    }
    debug("Picked Module: " + EstEidDll);

    var modulesToRemove = new Array();

    /* Search security module database */
    var secmoddb = Cc[nsModDB].createInstance(Ci.nsIPKCS11ModuleDB);
    var modules = secmoddb.listModules();

    for(;;) {
      var module = modules.currentItem()
                          .QueryInterface(Ci.nsIPKCS11Module);
        /* Remove modules that have recognized names but different DLLs */
        if (module) {
            if(module.libName == EstEidDll)
                needtoload = false;
            else if(module.name == EstEidModName)
                modulesToRemove.push(module.name);
            else if(EstEidLegacyNames.indexOf(module.name) >= 0)
                modulesToRemove.push(module.name);
        }
        try { modules.next(); } catch(e) { break; }
    }

    /* Actually remove modules marked for removal */
    for(i in modulesToRemove) {
        var modName = modulesToRemove[i];

        /* Try to load sane interface (Firefox 3.5 and later) first */
        try {
            var pkcs11 = Cc[nsPKCS11].getService(Ci.nsIPKCS11);
            try {
                pkcs11.deleteModule(modName);
            } catch(e) {
                log("ERROR removing module: " + e);
            }
        } catch(e) {
            var pkcs11 = window.pkcs11;
            var res;
            do {
                res = pkcs11.deletemodule(modName);
            } while(res == -2); // Bug user until he/she clicks OK
            if(res < 0) log("Error removing module");
        }
    }

    /* Load EstEID module */
    if(needtoload) {
        /* Try to load sane interface (Firefox 3.5 and later) first */
        try {
            var pkcs11 = Cc[nsPKCS11].getService(Ci.nsIPKCS11);
            try {
                pkcs11.addModule(EstEidModName, EstEidDll, 0x1<<28, 0);
            } catch(e) {
                log("ERROR loading module: " + e);
            }
        } catch(e) {
            var pkcs11 = window.pkcs11;
            var res;
            do {
                res = pkcs11.addmodule(EstEidModName, EstEidDll, 0x1<<28, 0);
            } while(res == -2); // Bug user until he/she clicks OK
            if(res < 0) log("Error loading module");
        }
    }

    /* Find CA Certificates and construct a list of fingerprints */
    var certdb    = Cc[nsX509CertDB]  .createInstance(Ci.nsIX509CertDB);
    var certdb2   = certdb.QueryInterface(Ci.nsIX509CertDB2);
    var certcache = Cc[nsNSSCertCache].createInstance(Ci.nsINSSCertCache);

    certcache.cacheAllCerts();
    var certList = certcache.getX509CachedCerts().getEnumerator();

    var caCertIDs = new Array();
    while (certList.hasMoreElements()) {
        var next = certList.getNext();
        var cert = next.QueryInterface(Ci.nsIX509Cert);
        var cert2 = next.QueryInterface(Ci.nsIX509Cert2);
        if (cert && cert2 && cert2.certType == Ci.nsIX509Cert.CA_CERT) {
            caCertIDs.push(cert.sha1Fingerprint);
        }
    }

    var EstEIDcertIDs = new Array();

    /* Open Certificate directory */
    var dir = Cc[nsLocalFile].createInstance(Ci.nsILocalFile);
    dir.initWithPath(certDir);
    var i = dir.directoryEntries;

    /* Read files, one by one */
    while(i.hasMoreElements()) {
        var file = i.getNext().QueryInterface(Ci.nsILocalFile);
        if(file.isReadable()) {
            var fS = Cc[nsFileInputStream]
                              .createInstance(Ci.nsIFileInputStream);
            fS.init(file, 1, 0, false);
            var sS = Cc[nsScriptableInputStream]
                              .getService(Ci.nsIScriptableInputStream);
            sS.init(fS);
            var data = sS.read(sS.available());
            sS.close();
            fS.close();

            /* Parse file contents */
            try {
                var pem = PEM2Base64(data);
                var cert = certdb.constructX509FromBase64(pem);
                var cert2 = cert.QueryInterface(Ci.nsIX509Cert2);

                /* If NOT a CA cert go to next file */
                if(cert2.certType != Ci.nsIX509Cert.CA_CERT) continue;

                EstEIDcertIDs.push(cert.sha1Fingerprint);
                if(caCertIDs.indexOf(cert.sha1Fingerprint) >= 0) continue;
                debug("Adding to store: " + file.leafName);
                certdb2.addCertFromBase64(pem, "C,C,P", "");
            } catch(e) {
                log("Unable to parse " + file.leafName + ": " + e);
            }
        }
    }

    debug("Esteid Certs: " + EstEIDcertIDs.join("\n"));
}

//delay of popups until after the browser is loaded, kind of a hack
//window.setTimeout('start()', 5000);
