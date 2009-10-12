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
using Microsoft.Win32;

namespace EstEIDSigner
{
    class EstEIDSignerGlobals
    {
        public static string ConfigDirectory
        {
            get 
            {
#if WIN32
#warning WIN32 is defined: using Registry to load configuration file location
            RegistryKey regkey ;

            regkey = Registry.CurrentUser.OpenSubKey(@"Software\Estonian ID Card");
            if (regkey == null)
                return Environment.CurrentDirectory + "\\";

            string path = (string)regkey.GetValue("Installed");

            if (path == null || path.Length == 0)
                return Environment.CurrentDirectory + "\\";

            return path;
#else
#warning WIN32 is not defined: using Environment to load configuration file location
                
                string path = Environment.GetEnvironmentVariable("XDG_CONFIG_HOME");
                if (path == null || path.Length == 0)
                    path = Environment.GetEnvironmentVariable("HOME");

                if (path == null || path.Length == 0)
                    return "./";

                return path;
#endif
            }
        }
    }
}
