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
using System.Text;

namespace EstEIDSigner
{
    class OCSPConf
    {
        private string issuer;
        private string url;
        private string cert;

        public OCSPConf(string issuer, string url, string cert)
        {
            this.issuer = issuer;
            this.url = url;
            this.cert = cert;
        }

        public string Issuer
        {
            get { return issuer; }
            set { issuer = value; }
        }

        public string Url
        {
            get { return url; }
            set { url = value; }
        }

        public string Cert
        {
            get { return cert; }
            set { cert = value; }
        }
    }
}
