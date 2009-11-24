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
using System.IO;
using Microsoft.Win32;

using EstEIDSigner.Properties;

namespace EstEIDSigner
{
    /// <summary>PKCS12 settings container.</summary>
    /// <remarks>On WIN platform it uses registry, otherwise INI file.</remarks>    
    class PKCS12Settings
        : ErrorContainer
    {
        private string filename;
        private string password;

        public string Filename
        {
            get { return filename; }
        }

        public string Password
        {
            get { return password; }
        }

        public bool Load()
        {
#if WIN32
#warning WIN32 is defined: using Registry to load PKCS12 variables
            RegistryKey regkey ;

            regkey = Registry.CurrentUser.OpenSubKey(@"Software\Estonian ID Card\OrganizationDefaults\Client");
            if (regkey == null)
            {
                this.lastError = Resources.REGISTRY_SETTING_MISSING;
                return (false);
            }

            filename = (string)regkey.GetValue("pkcs12Cert");
            password = (string)regkey.GetValue("pkcs12Pass");
#else
#warning WIN32 is not defined: using Environment/INI to load PKCS12 variables

            string path = Environment.GetEnvironmentVariable("XDG_CONFIG_HOME");
            if (path == null || path.Length == 0)
                path = Environment.GetEnvironmentVariable("HOME");
            path = path + "/.config/Estonian ID Card.conf";

            if (!File.Exists(path))
            {
                this.lastError = Resources.MISSING_CONFIG_FILE + path;
                return (false);
            }

            IniFile ini = new IniFile(path);
            filename = ini.ReadValue("Client", "pkcs12Cert");
            password = ini.ReadValue("Client", "pkcs12Pass");
#endif

            if ((filename == null) || (filename.Length == 0))
            {
                this.lastError = Resources.PKCS12_FILENAME_MISSING;
                return false;
            }

            return true;
        }
    }
}
