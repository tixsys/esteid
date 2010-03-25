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
            return LoadDirectoryStore();
        }

        public IEnumerator GetCerts()
        {
            if (certs != null)
                return certs.Values.GetEnumerator();

            return null;
        }        

        public X509CertStoreEntry GetIssuerCert(string key)
        {
            return ((X509CertStoreEntry)certs[key]);
        }        

        private bool LoadDirectoryStore()
        {
            DirectoryInfo di = new DirectoryInfo(this.certPath);
            FileInfo[] files = di.GetFiles("*.crt");

            foreach (FileInfo fi in files)
                AddFileToCollection(fi.FullName);

            return (certs.Keys.Count > 0);
        }

        private void AddFileToCollection(string path)
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
                    certs.Add(key, new X509CertStoreEntry(path, chain[i]));
            }
        }
    }
}
