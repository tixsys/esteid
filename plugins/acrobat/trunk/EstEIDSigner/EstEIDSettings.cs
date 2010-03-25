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
        private string configPath = string.Empty;

        const string DEFAULT_X          = "100";
        const string DEFAULT_Y          = "100";
        const string DEFAULT_W          = "400";
        const string DEFAULT_H          = "200";
        const string DEFAULT_RENDER     = "0";
        const string DEFAULT_PAGE       = "1";
        const string DEFAULT_SECTOR     = "9";

        const string TSA_ENABLED        = "enable_tsa";
        const string TSA_URL            = "tsa_url";
        const string TSA_USER           = "tsa_user";
        const string TSA_PASSWORD       = "tsa_password";
        const string SIGNATURE_PAGE     = "signature_page";
        const string SIGNATURE_RENDER   = "signature_render";
        const string DEBUG_ENABLED      = "debug";
        const string VISIBLE_SIGNATURE  = "visible_signature";
        const string SIGNATURE_SECTOR   = "signature_sector";
        const string SIGN_USE_SECTOR    = "signature_use_sector";
        const string SIGN_USE_COORDS    = "signature_use_coords";
        const string SIGNATURE_X        = "signature_x";
        const string SIGNATURE_Y        = "signature_y";
        const string SIGNATURE_W        = "signature_w";
        const string SIGNATURE_H        = "signature_h";
        const string LANGUAGE           = "language";
        const string CONTACT            = "contact";
        const string REASON             = "reason";
        const string CITY               = "city";
        const string COUNTY             = "county";
        const string COUNTRY            = "country";
        const string INDEX              = "index";
        const string HELP_URL           = "help_url";

        public string ConfigPath
        {
            get { return (configPath); }
        }

        public bool TsaEnabled
        {
            get { return (ToBoolean(TSA_ENABLED)); }
            set { AddOrReplace(TSA_ENABLED, value.ToString()); }
        }

        public string TsaUrl
        {
            get { return (ToString(TSA_URL)); }
            set { AddOrReplace(TSA_URL, value); }
        }

        public string TsaUser
        {
            get { return (ToString(TSA_USER)); }
            set { AddOrReplace(TSA_USER, value); }
        }

        public string TsaPassword
        {
            get { return (ToString(TSA_PASSWORD)); }
            set { AddOrReplace(TSA_PASSWORD, value); }
        }

        public string SignaturePage
        {
            get { return (ToString(SIGNATURE_PAGE, DEFAULT_PAGE)); }
            set { AddOrReplace(SIGNATURE_PAGE, value); }
        }

        public string SignatureRender
        {
            get { return (ToString(SIGNATURE_RENDER, DEFAULT_RENDER)); }
            set { AddOrReplace(SIGNATURE_RENDER, value); }
        }

        public bool DebugEnabled
        {
            get { return (ToBoolean(DEBUG_ENABLED)); }
        }

        public bool SignatureUseSector
        {
            get { return (ToBoolean(SIGN_USE_SECTOR)); }
            set { AddOrReplace(SIGN_USE_SECTOR, value.ToString()); }
        }

        public bool SignatureUseCoordinates
        {
            get { return (ToBoolean(SIGN_USE_COORDS)); }
            set { AddOrReplace(SIGN_USE_COORDS, value.ToString()); }
        }

        public string SignatureSector
        {
            get { return (ToString(SIGNATURE_SECTOR, DEFAULT_SECTOR)); }
            set { AddOrReplace(SIGNATURE_SECTOR, value); }
        }

        public uint SignatureX
        {
            get { return (ToUInt(SIGNATURE_X, DEFAULT_X)); }
            set { AddOrReplace(SIGNATURE_X, value.ToString()); }
        }

        public uint SignatureY
        {
            get { return (ToUInt(SIGNATURE_Y, DEFAULT_Y)); }
            set { AddOrReplace(SIGNATURE_Y, value.ToString()); }
        }

        public uint SignatureW
        {
            get { return (ToUInt(SIGNATURE_W, DEFAULT_W)); }
            set { AddOrReplace(SIGNATURE_W, value.ToString()); }
        }

        public uint SignatureH
        {
            get { return (ToUInt(SIGNATURE_H, DEFAULT_H)); }
            set { AddOrReplace(SIGNATURE_H, value.ToString()); }
        }

        public bool VisibleSignature
        {
            get { return (ToBoolean(VISIBLE_SIGNATURE)); }
            set { AddOrReplace(VISIBLE_SIGNATURE, value.ToString()); }
        }

        public string Language
        {
            get { return (ToString(LANGUAGE, "")); }
            set { AddOrReplace(LANGUAGE, value); }
        }

        public string Contact
        {
            get { return (ToString(CONTACT)); }
            set { AddOrReplace(CONTACT, value); }
        }

        public string Reason
        {
            get { return (ToString(REASON)); }
            set { AddOrReplace(REASON, value); }
        }

        public string City
        {
            get { return (ToString(CITY)); }
            set { AddOrReplace(CITY, value); }
        }

        public string County
        {
            get { return (ToString(COUNTY)); }
            set { AddOrReplace(COUNTY, value); }
        }

        public string Country
        {
            get { return (ToString(COUNTRY)); }
            set { AddOrReplace(COUNTRY, value); }
        }

        public string Index
        {
            get { return (ToString(INDEX)); }
            set { AddOrReplace(INDEX, value); }
        }

        public string HelpUrl
        {
            get { return (ToString(HELP_URL)); }
        }

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
                this.lastError = string.Format("Error creating config file: {0}\n\n{1}", filename, ex.Message);
            }

            return (this.lastError.Length == 0);
        }

        public bool Initialize()
        {
            string fullConfigPath = EstEIDSignerGlobals.LocalAppPath;

            try
            {
                if (!Directory.Exists(fullConfigPath))
                    Directory.CreateDirectory(fullConfigPath);
            }
            catch (System.IO.IOException ex)
            {
                this.lastError = string.Format("Error creating application directory: {0}\n\n{1}", fullConfigPath, ex.Message);
                return (false);
            }

            string fullConfigName = Path.Combine(fullConfigPath, EstEIDSignerGlobals.AppName + ".config");

            // create default config file if needed
            if (!File.Exists(fullConfigName))
            {
                if (!Create(fullConfigName, Resources.DEFAULT_CONFIG))
                    return (false);                    
            }

            configPath = fullConfigName;

            return (true);
        }

        public bool Open()
        {
            try
            {
                ExeConfigurationFileMap fileMap = new ExeConfigurationFileMap();
                fileMap.ExeConfigFilename = @configPath;
                config = ConfigurationManager.OpenMappedExeConfiguration(fileMap, ConfigurationUserLevel.None);
                
                // it may happen that conf file doesn't exist even if OpenExeConfiguration succeeds...
                if (!config.HasFile)
                    lastError = Resources.MISSING_CONFIG_FILE + config.FilePath;
            }
            catch (ConfigurationErrorsException ex)
            {
                this.lastError = string.Format("Error opening configuration file: {0}\n\n{1}", configPath, ex.Message);
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
            get 
            { 
                if (config != null)
                    return config.AppSettings.Settings;

                return (null);
            }
        }

        private bool ToBoolean(string key)
        {
            if (Value != null && Value[key] != null)
                return System.Convert.ToBoolean(Value[key].Value);

            return (false);
        }

        private string ToString(string key)
        {
            if (Value != null && Value[key] != null)
                return Value[key].Value;

            return (string.Empty);
        }

        public string ToString(string key, string defValue)
        {
            if (Value != null && Value[key] != null)
                return Value[key].Value;

            return (defValue);
        }

        public uint ToUInt(string key, string defValue)
        {
            uint v = 0;
            string s = defValue;

            if ((Value != null) && (Value[key] != null) && (Value[key].Value.Length > 0))
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
            if (Value != null)
            {
                if (Value[key] == null)
                    Value.Add(key, "");

                Value[key].Value = value;
            }
        }
    }
}
