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
using System.Text;
using System.IO;

using Org.BouncyCastle.X509;

namespace EstEIDSigner
{
    class X509CertStoreEntry
    {
        private string filename = string.Empty;
        private X509Certificate certificate = null;

        public X509CertStoreEntry(string filename, X509Certificate certificate)
        {
            this.filename = filename;
            this.certificate = certificate;
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
    }

    class DirectoryX509CertStore
    {
        private string certPath;
        private Hashtable certs;

        /// <summary>Class that scans certain directory to find X509 certs (*.crt).</summary>        
        /// <param name="path">Directory to scan.</param>        
        public DirectoryX509CertStore(string path)
        {
            this.certPath = path;
            this.certs = new Hashtable();
        }

        public bool Open()
        {
            int i;
            string key;
            DirectoryInfo di = new DirectoryInfo(this.certPath);
            FileInfo[] files = di.GetFiles("*.crt");

            foreach (FileInfo fi in files)
            {
                byte[] raw = EstEIDUtils.ReadFile(fi.FullName);
                X509Certificate[] chain = X509Utils.LoadCertificate(raw);

                for (i = 0; i < chain.Length; i++)
                {
                    key = X509Utils.GetSubjectFields(chain[i], "CN");                    
                    // better safe than sorry, who knows what the dir content consists of...
                    if (!certs.ContainsKey(key))
                        certs.Add(key, new X509CertStoreEntry(fi.FullName, chain[i]));
                }
            }

            return certs.Keys.Count > 0;
        }

        public IEnumerator GetCerts()
        {
            if (certs != null)
                return certs.Values.GetEnumerator();

            return null;
        }

        private X509CertStoreEntry GetCert(string issuer)
        {
            string name;
            X509CertStoreEntry entry;
            IDictionaryEnumerator e = certs.GetEnumerator();

            while (e.MoveNext())
            {
                entry = (X509CertStoreEntry)e.Value;
                name = X509Utils.GetIssuerFields(entry.Certificate, "CN");
                if (String.Compare(name, issuer, true) == 0)
                    return entry;
            }

            return null;
        }

        public X509CertStoreEntry this[string key, bool subjectKey]
        {
            get
            {
                return (subjectKey) ? (X509CertStoreEntry)certs[key] : GetCert(key);             
            }
        }
    }
}
