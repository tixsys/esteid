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
using System.Collections;
using System.Configuration;
using System.Text;

using EstEIDSigner.Properties;

namespace EstEIDSigner
{
    public class EstEIDSettings 
        : ErrorContainer
    {
        private Configuration config = null;
        public static readonly string SignaturePage = "1";
        public static readonly string SignatureX = "100";
        public static readonly string SignatureY = "100";
        public static readonly string SignatureW = "400";
        public static readonly string SignatureH = "200";
        public static readonly string SignatureDescription = "0";

        public bool Create(string filename, string data)
        {
            FileStream f;
            
            try
            {
                f = File.Create(filename);
                f.Write(Encoding.UTF8.GetBytes(data), 0, Encoding.UTF8.GetByteCount(data));
                f.Close();
            }
            catch (Exception ex)
            {
                this.lastError = ex.Message;
            }

            return (this.lastError.Length == 0);
        }

        public bool Open(string filename)
        {
            try
            {
                ExeConfigurationFileMap fileMap = new ExeConfigurationFileMap();
                fileMap.ExeConfigFilename = @filename;
                config = ConfigurationManager.OpenMappedExeConfiguration(fileMap, ConfigurationUserLevel.None);
                
                // it may happen that conf file doesn't exist even if OpenExeConfiguration succeeds...
                if (!config.HasFile)
                    lastError = Resources.MISSING_CONFIG_FILE + config.FilePath;
            }
            catch (ConfigurationErrorsException ex)
            {
                this.lastError = ex.Message;
            }

            return (this.lastError.Length == 0);
        }

        public void Save()
        {
            if (config != null)
                config.Save();
        }

        public KeyValueConfigurationCollection Value
        {
            get { return config.AppSettings.Settings; }
        }

        public bool ToBoolean(string key)
        {
            if (Value[key] != null)
                return System.Convert.ToBoolean(Value[key].Value);

            return (false);
        }

        public string ToString(string key)
        {
            if (Value[key] != null)
                return Value[key].Value;

            return (string.Empty);
        }

        public string ToString(string key, string defValue)
        {
            if (Value[key] != null)
                return Value[key].Value;

            return (defValue);
        }

        public uint ToUInt(string key, string defValue)
        {
            uint v = 0;
            string s = defValue;

            if ((Value[key] != null) && (Value[key].Value.Length > 0))
                s = Value[key].Value;

            try
            {
                v = Convert.ToUInt32(s);
            }
            catch (FormatException) { }
            catch (OverflowException) { }
            finally
            {
                if ((v >= UInt32.MaxValue) || (v <= UInt32.MinValue))
                    v = 0;
            }

            return (v);
        }

        public void AddOrReplace(string key, string value)
        {
            if (Value[key] == null)
                Value.Add(key, "");

            Value[key].Value = value;
        }
    }
}
