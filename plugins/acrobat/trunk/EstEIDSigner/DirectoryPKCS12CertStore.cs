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
using System.Collections.Generic;
using System.Text;
using System.IO;

using Org.BouncyCastle.Pkcs;
using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.X509;

using EstEIDSigner.Properties;

namespace EstEIDSigner
{
    class DirectoryPKCS12CertStore 
        : ErrorContainer
    {
        private Pkcs12Store store = null;
        private ICipherParameters key = null;
        private string filename;
        private string password;
        private X509Certificate[] chain;

        /// <summary>Wrapper class that allows to open PKCS#12 container.</summary>
        /// <remarks>This constructor initializes private variables needed to open PKCS12 Store.</remarks>
        /// <param name="filename">PKCS#12 filename.</param>
        /// <param name="password">PKCS#12 password.</param>
        public DirectoryPKCS12CertStore(string filename, string password)
        {
            this.filename = filename;
            this.password = password;
        }

        public ICipherParameters Key
        {
            get { return key;  }
        }

        public X509Certificate[] Chain
        {
            get { return chain; }
        }

        public bool Open()
        {
            if ((filename == null) || (filename.Length == 0))
            {
                this.lastError = Resources.PKCS12_FILENAME_MISSING;
                return false;
            }

            if (!File.Exists(filename))
            {
                this.lastError = Resources.PKCS12_FILE_MISSING + filename;
                return false;
            }

            FileStream fs = new FileStream(filename, FileMode.Open);
            store = new Pkcs12Store(fs, password.ToCharArray());

            string alias = null;
            foreach (string al in store.Aliases)
            {
                if (store.IsKeyEntry(al) && store.GetKey(al).Key.IsPrivate)
                {
                    alias = al;
                    break;
                }
            }
            fs.Close();
            key = store.GetKey(alias).Key;

            X509CertificateEntry[] x = store.GetCertificateChain(alias);
            chain = new X509Certificate[x.Length];

            for (int k = 0; k < x.Length; ++k)
            {
                chain[k] = x[k].Certificate;
            }  

            if (key == null)
                this.lastError = Resources.INVALID_PKCS12_CERT_PRI_KEY_MISSING;;
            if (chain == null || chain.Length == 0)
                this.lastError = Resources.INVALID_PKCS12_CERT_PUB_KEY_MISSING;

            return (key != null && chain != null && chain.Length > 0);
        }
    }
}
