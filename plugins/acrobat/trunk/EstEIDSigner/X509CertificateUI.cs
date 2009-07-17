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
using System.Windows.Forms;
using System.Security.Cryptography.X509Certificates;

namespace EstEIDSigner
{
    class X509CertificateUI
    {
        public static X509Certificate2Collection SelectFromCollection(X509Certificate2Collection certificates, 
            string title, string message)
        {
            FormSelectCertificate form = new FormSelectCertificate(title, message);            
            Dictionary<int, X509Certificate2> map = new Dictionary<int, X509Certificate2>();
            X509Certificate2Collection coll = null;
            int index;

            foreach (X509Certificate2 x509 in certificates)
            {
                string name = x509.GetNameInfo(X509NameType.SimpleName, false);
                string issuer = x509.GetNameInfo(X509NameType.SimpleName, true);
                
                index = form.Add(name, issuer, x509.GetExpirationDateString());
                map.Add(index, x509);                
            }
            
            DialogResult dr = form.ShowDialog();
            if (dr == DialogResult.OK)
            {
                index = form.Selected;
                if (index != -1 && map.ContainsKey(index))
                {
                    coll = new X509Certificate2Collection(map[index]);
                }
            }

            return coll;
        }
    }
}
