/*
 * Copyright (C) 2009-2010  Estonian Informatics Centre
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

using System;
using System.Collections;
using System.IO;
#if WIN32
using Microsoft.Win32; // registry support
#endif
using System.Xml;

namespace EstEIDSigner
{
    class XmlConf : 
        ErrorContainer
    {
        private string DEFAULT_CONF_LOC = string.Empty;
        private string USER_CONF_LOC    = string.Empty;
        private string CONF_PATH        = string.Empty;
        private string digestUri        = string.Empty;
        private string pkcs11DriverPath = string.Empty;
        private string certStorePath    = string.Empty;
        private string manifestXsdPath  = string.Empty;
        private string xadesXsdPath     = string.Empty;
        private string dsigXsdPath      = string.Empty;
        private string proxyHost        = string.Empty;
        private string proxyPort        = string.Empty;
        private string proxyUser        = string.Empty;
        private string proxyPass        = string.Empty;
        private string pkcs12Cert       = string.Empty;
        private string pkcs12Pass       = string.Empty;
        private Hashtable certs         = null;
#if WIN32
        const string REGISTRY_KEY       = @"SOFTWARE\Estonian ID Card\digidocpp";
#else
        const string DIGIDOCPP_CONFIG_DIR = "/etc/digidocpp";
#endif
        const string DIGIDOCPP_ENV      = "DIGIDOCPP_OVERRIDE_CONF";
        const string DIGEST_URI         = "digest.uri";
        const string PKCS11_DRIVER_PATH = "pkcs11.driver.path";
        const string CERT_STORE_PATH    = "cert.store.path";
        const string MANIFEST_XSD_PATH  = "manifest.xsd.path";
        const string XADES_XSD_PATH     = "xades.xsd.path";
        const string DSIG_XSD_PATH      = "dsig.xsd.path";
        const string PROXY_HOST         = "proxy.host";
        const string PROXY_PORT         = "proxy.port";
        const string PROXY_USER         = "proxy.user";
        const string PROXY_PASS         = "proxy.pass";
        const string PKCS12_CERT        = "pkcs12.cert";
        const string PKCS12_PASS        = "pkcs12.pass";

        public XmlConf()
        {
            certs = new Hashtable();
        }
        
        public string DefaultConfDir
        {
            get
            {
                // the file path in conf is relative to the conf file's location
                string env = DEFAULT_CONF_LOC;
                if (env.Length > 0)
                    return Path.GetDirectoryName(env);
                return (Directory.GetCurrentDirectory());
            }
        }

        public string DigestUri
        {
            get { return digestUri; }
        }

        public string PKCS11DriverPath
        {
            get { return pkcs11DriverPath; }
        }

        public string CertStorePath
        {
            get { return Path.Combine(DefaultConfDir, certStorePath); }
        }

        public string ManifestXsdPath
        {
            get { return Path.Combine(DefaultConfDir, manifestXsdPath); }
        }

        public string XadesXsdPath
        {
            get { return Path.Combine(DefaultConfDir, xadesXsdPath); }
        }

        public string DsigXsdPath
        {
            get { return Path.Combine(DefaultConfDir, dsigXsdPath); }
        }

        public string ProxyHost
        {
            get { return proxyHost; }
        }

        public string ProxyPort
        {
            get { return proxyPort; }
        }

        public string ProxyUser
        {
            get { return proxyUser; }
        }

        public string ProxyPass
        {
            get { return proxyPass; }
        }

        public string Pkcs12Cert
        {
            get { return pkcs12Cert; }
        }

        public string Pkcs12Pass
        {
            get { return pkcs12Pass; }
        }

        private string DigidocppOverrideConf
        {
            get { return Environment.GetEnvironmentVariable(DIGIDOCPP_ENV); }
        }

        public bool Initialize()
        {
            if (!SetDefaultConfPath())
                return (false);

            if (!SetUserConfPath())
                return (false);

            return (true);
        }

        public bool Open()
        {
            bool res = false;

            if (File.Exists(DEFAULT_CONF_LOC))
                res = Init(DEFAULT_CONF_LOC);
            else
            {
                this.lastError = "XML configuration file doesn't exist: " + DEFAULT_CONF_LOC;
                return (false);
            }

            string overrideConf = DigidocppOverrideConf;            
            if ((overrideConf == null) && File.Exists(USER_CONF_LOC))
                res = res && Init(USER_CONF_LOC);

            return (res);
        }

        private bool SetDefaultConfPath()
        {
            string overrideConf = DigidocppOverrideConf;

            if (overrideConf != null && overrideConf.Length > 0)
            {
                if (File.Exists(overrideConf))
                {
                    DEFAULT_CONF_LOC = overrideConf;
                    return (true);
                }

                this.lastError = "Configuration file doesn't exist: " + overrideConf;
                return (false);
            }
            else
            {
#if WIN32
                RegistryKey regkey;

                regkey = Registry.LocalMachine.OpenSubKey(REGISTRY_KEY);
                if (regkey == null)
                {
                    this.lastError = "Registry key doesn't exist: " + REGISTRY_KEY;
                    return (false);
                }

                DEFAULT_CONF_LOC = (string)regkey.GetValue("ConfigFile");
#else
                DEFAULT_CONF_LOC = DIGIDOCPP_CONFIG_DIR "/digidocpp.conf";
#endif
                if (DEFAULT_CONF_LOC.Length == 0)
                {
                    this.lastError = "Registry key value doesn't exist: " + REGISTRY_KEY + "ConfigFile";
                    return (false);
                }

                return (true);
            }
        }

        private bool SetUserConfPath()
        {
#if WIN32
            USER_CONF_LOC = Environment.GetEnvironmentVariable("APPDATA");
            if (USER_CONF_LOC.Length > 0)
                USER_CONF_LOC += "\\digidocpp\\digidocpp.conf";   
#else
            USER_CONF_LOC = Environment.GetEnvironmentVariable("HOME");
            if (USER_CONF_LOC.Length > 0)
                USER_CONF_LOC += "/.digidocpp/digidocpp.conf"; 
#endif
            if (USER_CONF_LOC.Length == 0)
            {
                this.lastError = "User config path variable is missing";
                return (false);
            }

            return (true);
        }        

        private bool Init(string path)
        {
            XmlDocument doc = new XmlDocument();
            
            try
            {
                doc.Load(path);
            }
            catch (System.Xml.XmlException ex)
            {
                this.lastError = string.Format("XML Load failed ({0}):\n\n{1}", path, ex.ToString());
                return (false);
            }

            XmlNode root = doc.DocumentElement;
            IEnumerator e = root.GetEnumerator();
            string s;
            XmlNodeList memberNodes = doc.SelectNodes("//param");
            foreach (XmlNode node in memberNodes)
            {
                if (node.Attributes["name"] != null)
                {
                    s = node.Attributes["name"].Value;

                    if (DIGEST_URI.Equals(s))
                    {
                        digestUri = node.InnerText;
                    }
                    else if (MANIFEST_XSD_PATH.Equals(s))
                    {
                        manifestXsdPath = node.InnerText;
                    }
                    else if (XADES_XSD_PATH.Equals(s))
                    {
                        xadesXsdPath = node.InnerText;
                    }
                    else if (DSIG_XSD_PATH.Equals(s))
                    {
                        dsigXsdPath = node.InnerText;
                    }
                    else if (PKCS11_DRIVER_PATH.Equals(s))
                    {
                        pkcs11DriverPath = node.InnerText;
                    }
                    else if (CERT_STORE_PATH.Equals(s))
                    {
                        certStorePath = node.InnerText;
                    }
                    else if (PROXY_HOST.Equals(s))
                    {
                        proxyHost = node.InnerText;
                    }
                    else if (PROXY_PORT.Equals(s))
                    {
                        proxyPort = node.InnerText;
                    }
                    else if (PROXY_USER.Equals(s))
                    {
                        proxyUser = node.InnerText;
                    }
                    else if (PROXY_PASS.Equals(s))
                    {
                        proxyPass = node.InnerText;
                    }
                    else if (PKCS12_CERT.Equals(s))
                    {
                        pkcs12Cert = node.InnerText;
                    }
                    else if (PKCS12_PASS.Equals(s))
                    {
                        pkcs12Pass = node.InnerText;
                    }                    
                }
            }

            string issuer = string.Empty, url = string.Empty, cert = string.Empty;
            string fullPath = DefaultConfDir;
            memberNodes = doc.SelectNodes("//ocsp");

            foreach (XmlNode node in memberNodes)
            {
                if (node.Attributes["issuer"] != null)
                {
                    issuer = node.Attributes["issuer"].Value;

                    foreach (XmlNode child in node.ChildNodes)
                    {
                        if (child.Name.Equals("url"))
                        {
                            url = child.InnerText;
                        }
                        else if (child.Name.Equals("cert"))
                        {
                            cert = child.InnerText;
                        }
                    }

                    if (issuer.Length > 0 && url.Length > 0 && cert.Length > 0)
                    {                        
                        cert = Path.Combine(fullPath, cert);
                        certs.Add(issuer, new OCSPConf(issuer, url, cert));
                        issuer = string.Empty;
                        url = string.Empty;
                        cert = string.Empty;
                    }
                }
            }

            if (certs.Count == 0)
            {
                this.lastError = "Failed to load OCSP certificates from directory: " + path;
                return (false);
            }

            return (true);
        }

        public OCSPConf GetOCSPConf(string key)
        {
            return ((OCSPConf)certs[key]);
        }
    }
}
