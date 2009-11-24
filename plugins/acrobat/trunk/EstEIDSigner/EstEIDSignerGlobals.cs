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

namespace EstEIDSigner
{
    class EstEIDSignerGlobals
    {
        public static string OrganizationDir = "Estonian ID Card";
        public static string ProgramDir = "EstEIDSigner";

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
#if WIN32
#warning WIN32 is defined: using Windows slashes
                string format = @"{0}\{1}\{2}\";
#else
#warning WIN32 is not defined: using Unix slashes
                string format = @"{0}/{1}/{2}/";
#endif
                string appPath = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
                string fullPath = string.Format(format, appPath, OrganizationDir, ProgramDir);

                return (fullPath);
            }
        }
    }
}
