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
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Threading;
using System.Resources;
using System.Reflection;
using System.Diagnostics;
using System.Globalization;

using iTextSharp.text.pdf;

using EstEIDSigner.Properties;

namespace EstEIDSigner
{
    public partial class Form1 : Form
    {
        private DirectoryX509CertStore store = null;
        private EstEIDSettings config = null;
        private PKCS12Settings credentials = null;
        private PdfReader reader = null;
        private string inputFile = string.Empty;
        private string outputFile = string.Empty;

        public Form1()
        {
            this.Font = new Font("Verdana", (float)8.25);            

            InitializeComponent();
            
            config = new EstEIDSettings();

            LoadConfig();
        }

        private void debug(string txt)
        {
            DebugBox.AppendText(txt + System.Environment.NewLine);
            Application.DoEvents();
        }

        private void status(string txt, bool error)
        {
            if (error)            
                statusBox.ForeColor = System.Drawing.Color.Red;                
            else            
                statusBox.ForeColor = SystemColors.ControlText;

            statusBox.Text = txt;
            debug(txt);
        }

        private void InitReader()
        {
            string inputFile = this.inputFile;
            if (!File.Exists(inputFile))
                return;

            reader = new PdfReader(inputFile);

            MetaData md = new MetaData();
            md.Info = reader.Info;

            authorBox.Text = md.Author;
            titleBox.Text = md.Title;
            subjectBox.Text = md.Subject;
            kwBox.Text = md.Keywords;
            creatorBox.Text = md.Creator;
            prodBox.Text = md.Producer; 
        }

        private void linkInput_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            OpenFileDialog openFile;

            openFile = new System.Windows.Forms.OpenFileDialog();
            openFile.Filter = Resources.UI_INPUT_FILEDIALOG_FILTER;
            openFile.Title = Resources.UI_INPUT_FILEDIALOG_TITLE;
            if (openFile.ShowDialog() != DialogResult.OK)
                return;

            status("", false);            
            linkInput.Text = System.IO.Path.GetFileName(openFile.FileName);
            this.inputFile = openFile.FileName;
            
            InitReader();
        }

        private void linkOutput_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            SaveFileDialog saveFile;

            saveFile = new System.Windows.Forms.SaveFileDialog();
            saveFile.Filter = Resources.UI_OUTPUT_FILEDIALOG_FILTER;
            saveFile.Title = Resources.UI_OUTPUT_FILEDIALOG_TITLE;
            if (saveFile.ShowDialog() != DialogResult.OK)
                return;
                        
            linkOutput.Text = System.IO.Path.GetFileName(saveFile.FileName);
            this.outputFile = saveFile.FileName;
        }

        private void OpenCertStore()
        {
            if (store == null)
            {                
                string path = config.ToString("cert_path");
                if (path == null || path.Length == 0)
                    path = EstEIDSignerGlobals.CertDirectory;

                if (!Directory.Exists(path))
                    throw new Exception(Resources.DIR_CERT_ERROR + path);

                store = new DirectoryX509CertStore(path);
                if (store.Open() == false)
                    throw new Exception(Resources.DIR_CERT_ERROR + path);
            }
        }

        private void LoadPKCS12Config()
        {
            if (credentials == null)
            {
                credentials = new PKCS12Settings();

                if (credentials.Load() == false)             
                    throw new Exception(Resources.PKCS12_LOAD_ERROR + credentials.lastError);
            }
        }

        private void sign_Click(object sender, EventArgs e)
        {
            string inputFile = this.inputFile;

            if (!File.Exists(inputFile))
            {
                MessageBox.Show(Resources.MISSING_INPUT, Resources.ERROR);
                return;
            }
         
            string outputFile = this.outputFile;

            if ((outputFile == null) || (outputFile.Length == 0))
            {
                MessageBox.Show(Resources.MISSING_OUTPUT, Resources.ERROR);
                return;
            }            

            Timestamp stamp = null;
            if (config.ToBoolean("enable_tsa") == true)
                stamp = new Timestamp(config.ToString("tsa_url"), 
                    config.ToString("tsa_user"), 
                    config.ToString("tsa_password"));

            //Adding Meta Datas
            MetaData metaData = new MetaData();
            metaData.Author = authorBox.Text;
            metaData.Title = titleBox.Text;
            metaData.Subject = subjectBox.Text;
            metaData.Keywords = kwBox.Text;
            metaData.Creator = creatorBox.Text;
            metaData.Producer = prodBox.Text;

            Appearance app = new Appearance();
            app.Contact = Contacttext.Text;
            app.Reason = Reasontext.Text;
            app.Location = Locationtext.Text;
            app.Visible = SigVisible.Checked;

            PDFSigner pdfSigner = null;

            try
            {
                this.signButton.Enabled = false;

                status(Resources.READ_CERTS, false);
                OpenCertStore();

                status(Resources.READ_SETTINGS, false);
                LoadPKCS12Config();

                // if user copy-pasted filename into PDF input field then force init...
                if (reader == null)
                    InitReader();

                status(Resources.ADD_SIGNATURE, false);

                pdfSigner = new PDFSigner(reader, inputFile, outputFile, stamp, metaData, app, store);
                pdfSigner.StatusHandler = this.status;
                pdfSigner.Settings = this.config;
                pdfSigner.Sign(credentials);

                status(Resources.DOCUMENT_SIGNED, false);
            }
            catch (CancelException)
            {
                // user action was canceled
                status(Resources.SIGNING_CANCELED, true);
            }
            catch (RevocationException ex)
            {
                status(Resources.OCSP_CERT_ERROR, true);
                MessageBox.Show(ex.Message, Resources.ERROR);
            }
            catch (DocVerifyException ex)
            {
                status(Resources.DOC_VERIFY_FAILURE, true);
                MessageBox.Show(ex.Message, Resources.ERROR);
            }
            catch (Exception ex)
            {
                status(Resources.GENERAL_EXCEPTION, true);
                debug(ex.ToString());
                MessageBox.Show(ex.Message, Resources.ERROR);
            }

            this.signButton.Enabled = true;
            // force Garbage Collector to close pkcs11 bridge
            GC.Collect();
        }

        private void LoadConfig()
        {
            bool b, configExisted = true;

            string fullConfigPath = EstEIDSignerGlobals.LocalAppPath;
            if (!Directory.Exists(fullConfigPath))
                Directory.CreateDirectory(fullConfigPath);

            string fullConfigName = System.IO.Path.Combine(fullConfigPath, EstEIDSignerGlobals.AppName + ".config");

            // create default config file if needed
            if (!File.Exists(fullConfigName))
            {
                if (!config.Create(fullConfigName, Resources.DEFAULT_CONFIG))
                    MessageBox.Show(config.lastError, Resources.ERROR);

                configExisted = false;
            }

            if (!config.Open(fullConfigName))
                MessageBox.Show(config.lastError, Resources.ERROR);

            try
            {
                // set Estonian ID card cert dir if new installation or
                // invalid cert path
                if ((!configExisted)
                    || (!Path.IsPathRooted(config.ToString("cert_path")))
                    || (!Directory.Exists(config.ToString("cert_path"))))
                    config.AddOrReplace("cert_path", EstEIDSignerGlobals.CertDirectory);

                b = config.ToBoolean("debug");
                if (!b)
                {
                    DebugBox.Hide();
                    MaximumSize = new Size(this.Width, tablePanel3.Bottom + 50);
                }                

                b = config.ToBoolean("visible_signature");
                this.SigVisible.Checked = b;
            }
            catch (Exception) { }           

            // UI
            string lang = config.ToString("language");            
            comboLanguage.SelectedIndex = GetCulture(lang);
        }

        private int GetCulture(string name)
        {
            string[] cultureCodes = {string.Empty, "en-US", "ru-RU"};
            int index = 0;

            foreach (string cultureCode in cultureCodes)
            {
                try
                {
                    if (cultureCode.CompareTo(name) == 0)
                    {
                        CultureInfo ci = new CultureInfo(cultureCode);

                        return (index);                        
                    }
                }
                catch {}

                index++;
            }

            return (-1);
        }

        private void SetCulture(int comboIndex)
        {
            CultureInfo culture;

            switch (comboIndex)
            {
                case 1:
                    culture = new CultureInfo("en-US");
                    linkSettings.Location = new Point(422, 16);
                    break;
                case 2:
                    culture = new CultureInfo("ru-RU");
                    linkSettings.Location = new Point(430, 16);
                    break;
                default:
                    culture = new CultureInfo(string.Empty);
                    linkSettings.Location = new Point(430, 16);
                    break;
            }

            Thread.CurrentThread.CurrentCulture = culture;
            Thread.CurrentThread.CurrentUICulture = culture;
            Resources.Culture = culture;

            config.AddOrReplace("language", culture.Name);

            // PDF Document box
            label1.Text = Resources.UI_PDF_DOC;
            label2.Text = Resources.UI_INPUT;
            label3.Text = Resources.UI_OUTPUT;
            label4.Text = Resources.UI_STATUS;

            linkInput.Text = (this.inputFile.Length > 0) ? System.IO.Path.GetFileName(this.inputFile) : 
                Resources.UI_CHOOSE_INPUT;
            linkOutput.Text = (this.outputFile.Length > 0) ? System.IO.Path.GetFileName(this.outputFile) :
                Resources.UI_CHOOSE_OUTPUT;

            // PDF Metadata box
            label5.Text = Resources.UI_PDF_METADATA;
            label6.Text = Resources.UI_AUTHOR;
            label7.Text = Resources.UI_TITLE;
            label8.Text = Resources.UI_SUBJECT;
            label9.Text = Resources.UI_KEYWORDS;
            label10.Text = Resources.UI_CREATOR;
            label11.Text = Resources.UI_SOFTWARE;

            // Signature box
            label12.Text = Resources.UI_SIGNATURE;
            label13.Text = Resources.UI_SIGNATURE_DESCRIPTION;
            label14.Text = Resources.UI_SIGNATURE_NAME;
            label15.Text = Resources.UI_SIGNATURE_LOCATION;
            SigVisible.Text = Resources.UI_SIGNATURE_VISIBLE;

            // Other fields
            signButton.Text = Resources.UI_SIGN_BUTTON;
            linkSettings.Text = Resources.UI_SETTINGS_LINK;            
            linkHelp.Text = Resources.UI_HELP_LINK;
        }

        private void SigVisible_CheckedChanged(object sender, EventArgs e)
        {
            config.AddOrReplace("visible_signature", this.SigVisible.Checked.ToString());
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (config != null)
                config.Save();
        }

        private void linkSettings_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            FormSettings settings = new FormSettings(config);
            
            settings.ShowDialog();
        }

        private void linkHelp_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            string helpUrl = config.ToString("help_url");

            if (helpUrl != string.Empty)
            {
                Process process = new Process();
                process.StartInfo.FileName = helpUrl;
                process.Start();
            }            
        }

        private void comboLanguage_SelectedIndexChanged(object sender, EventArgs e)
        {
            SetCulture(comboLanguage.SelectedIndex);
        }  
    }
}