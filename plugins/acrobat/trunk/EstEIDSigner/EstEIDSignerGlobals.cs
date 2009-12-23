﻿/*
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
        private const string organizationDir = "Estonian ID Card";
        private const string programDir = "EstEIDSigner";
        private const string certDir = "certs";
        private const string bdoclibConfXml = "digidocpp.conf";
#if WIN32
        private const string registryKey = @"Software\Estonian ID Card";
#else
        private const string configDir = "/etc/digidocpp";
#endif

        public static string AppName
        {
            get
            {
                string fullPath = Environment.GetCommandLineArgs()[0];
                
                return (System.IO.Path.GetFileName(fullPath));
            }
        }

        public static string LocalAppPath
        {
            get 
            {
                string fullPath = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
                fullPath = System.IO.Path.Combine(fullPath, organizationDir);
                fullPath = System.IO.Path.Combine(fullPath, programDir);

                return (fullPath);
            }
        }

        public static string ConfigDirectory
        {
            get
            {
#if WIN32
#warning WIN32 is defined: using Registry to load configuration file location
                RegistryKey regkey;

                regkey = Registry.CurrentUser.OpenSubKey(registryKey);
                if (regkey == null)
                    return Environment.CurrentDirectory + "\\";

                string path = (string)regkey.GetValue("Installed");

                if (path == null || path.Length == 0)
                    return Environment.CurrentDirectory + "\\";

                return (path);
#else
#warning WIN32 is not defined: using predefined configuration file location
                string path = configDir;

                return (path);
#endif
            }
        }

        public static string CertDirectory
        {            
            get
            {
                string fullDir = System.IO.Path.Combine(ConfigDirectory, certDir);

                return (fullDir);
            }
        }

        public static string BDocLibConf
        {
            get
            {
                string fullDir = System.IO.Path.Combine(ConfigDirectory, bdoclibConfXml);

                return (fullDir);
            }
        }
    }
}
