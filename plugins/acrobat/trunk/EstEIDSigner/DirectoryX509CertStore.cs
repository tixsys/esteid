/*
 * Copyright (C) 2009  Manuel Matonin <manuel@smartlink.ee>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;

using Org.BouncyCastle.X509;

namespace EstEIDSigner
{
    class X509CertStoreEntry
    {
        private string filename = string.Empty;
        private X509Certificate certificate = null;
        private string ocspUrl = string.Empty;
        private bool usedInOCSP;

        public X509CertStoreEntry(string filename, X509Certificate certificate, string url, bool usedInOCSP)
        {
            this.filename = filename;
            this.certificate = certificate;
            this.ocspUrl = url;
            this.usedInOCSP = usedInOCSP;
        }

        public X509Certificate Certificate
        {
            set { this.certificate = value; }
            get { return certificate; }
        }

        public string Filename
        {
            set { this.filename = value; }
            get { return filename; }
        }

        public string OCSPUrl
        {
            set { this.ocspUrl = value; }
            get { return ocspUrl; }
        }

        public bool IsOCSPCert
        {
            get { return usedInOCSP; }
        }
    }

    class DirectoryX509CertStore
    {
        private string certPath;
        private Hashtable certs;
        private string ocspUrl;

        /// <summary>Class that scans certain directory to find X509 certs (*.crt).</summary>
        /// <param name="path">Directory to scan.</param>
        /// <param name="url">OCSP URL, will be used in creating X509CertStoreEntry object.</param>
        public DirectoryX509CertStore(string path, string url)
        {
            this.certPath = path;
            this.certs = new Hashtable();
            this.ocspUrl = url;
        }

        public bool Open()
        {
            string fullPath = EstEIDSignerGlobals.BDocLibConf;
            bool res = false;

            // if we have BDOC conf file then use it
            // these certs will be used in 1st order in OCSP request
            if (File.Exists(fullPath))
                res = LoadBDocStore(fullPath);

            return (res && LoadDirectoryStore());
        }

        public IEnumerator GetCerts()
        {
            if (certs != null)
                return certs.Values.GetEnumerator();

            return null;
        }

        public X509CertStoreEntry GetResponderCert(string key)
        {
            string name;
            X509CertStoreEntry entry, match = null;
            IDictionaryEnumerator e = certs.GetEnumerator();

            while (e.MoveNext())
            {
                entry = (X509CertStoreEntry)e.Value;
                name = X509Utils.GetIssuerFields(entry.Certificate, "CN");
                if (String.Compare(name, key, true) == 0)
                {
                    match = entry;
                    if (match.IsOCSPCert)
                        return (match);
                }
            }            

            // fall through: return non-best match if exists
            return (match);
        }

        public X509CertStoreEntry GetIssuerCert(string key)
        {
            return ((X509CertStoreEntry)certs[key]);
        }

        private bool LoadBDocStore(string path)
        {
            XmlDocument documentation = new XmlDocument();
            documentation.Load(path);
            XmlNodeList memberNodes = documentation.SelectNodes("//ocsp");
            string url = string.Empty;
            string fullpath;

            foreach (XmlNode node in memberNodes)
            {
                if (node.Attributes["issuer"] != null)
                {
                    foreach (XmlNode child in node.ChildNodes)
                    {
                        if (child.Name.Equals("url"))
                        {
                            url = child.InnerText;
                        }
                        else if (child.Name.Equals("cert"))
                        {
                            fullpath = Path.GetFullPath(
                                Path.Combine(EstEIDSignerGlobals.ConfigDirectory, child.InnerText));
                            if (File.Exists(fullpath))
                                AddFileToCollection(true, fullpath, url);
                            url = "";
                        }
                    }
                }
            }

            return (certs.Keys.Count > 0);
        }

        private bool LoadDirectoryStore()
        {
            DirectoryInfo di = new DirectoryInfo(this.certPath);
            FileInfo[] files = di.GetFiles("*.crt");

            foreach (FileInfo fi in files)
                AddFileToCollection(false, fi.FullName, this.ocspUrl);

            return (certs.Keys.Count > 0);
        }

        private void AddFileToCollection(bool usedInOCSP, string path, string url)
        {
            int i;
            string key;

            byte[] raw = EstEIDUtils.ReadFile(path);
            X509Certificate[] chain = X509Utils.LoadCertificate(raw);

            for (i = 0; i < chain.Length; i++)
            {
                key = X509Utils.GetSubjectFields(chain[i], "CN");
                // better safe than sorry, who knows what the dir content consists of...
                if (!certs.Contains(key))
                    certs.Add(key, new X509CertStoreEntry(path, chain[i], url, usedInOCSP));
            }
        }
    }
}
