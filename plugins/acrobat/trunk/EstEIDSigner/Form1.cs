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
        private DirectoryX509CertStore store    = null;
        private EstEIDSettings settings         = null;        
        private PdfReader reader                = null;
        private XmlConf conf                    = null;
        private string inputFile                = string.Empty;
        private string outputFile               = string.Empty;

        public Form1()
        {
            this.Font = new Font("Verdana", (float)8.25);

            InitializeComponent();

            settings = new EstEIDSettings();
            conf = new XmlConf();

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
            InputFile = openFile.FileName;
            
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

            OutputFile = saveFile.FileName;
        }

        private string InputFile
        {
            set
            {
                UIInputFile = value;
                this.inputFile = value;
            }
            get
            {
                return this.inputFile;
            }
        }

        private string OutputFile
        {
            set
            {
                UIOutputFile = value;
                this.outputFile = value;
            }
            get
            {
                return this.outputFile;
            }
        }

        private string UIInputFile
        {
            set
            {
                string s = System.IO.Path.GetFileName(value);
                if (s.Length > 32)
                    s = s.Substring(0, 29) + "...";
                this.linkInput.Text = s;
            }
        }

        private string UIOutputFile
        {
            set
            {
                string s = System.IO.Path.GetFileName(value);
                if (s.Length > 32)
                    s = s.Substring(0, 29) + "...";
                this.linkOutput.Text = s;
            }
        }

        private string LocationText
        {
            get
            {
                StringBuilder buf = new StringBuilder();
                if (Citytext.Text.Length > 0)
                    buf.Append(Citytext.Text);
                if (Countytext.Text.Length > 0)
                {
                    if (buf.Length > 0)
                        buf.Append(", ");
                    buf.Append(Countytext.Text);
                }
                if (Countrytext.Text.Length > 0)
                {
                    if (buf.Length > 0)
                        buf.Append(", ");
                    buf.Append(Countrytext.Text);
                }
                if (Indextext.Text.Length > 0)
                {
                    if (buf.Length > 0)
                        buf.Append(", ");
                    buf.Append(Indextext.Text);
                }
                return buf.ToString();
            }
        }

        private void OpenCertStore(string path)
        {
            if (store == null)
            {
                if (!Directory.Exists(path))
                    throw new Exception(Resources.DIR_CERT_ERROR + path);

                store = new DirectoryX509CertStore(path);
                if (store.Open() == false)
                    throw new Exception(Resources.DIR_CERT_ERROR + path);                
            }
        }        

        private void sign_Click(object sender, EventArgs e)
        {
            string inputFile = this.InputFile;

            if (!File.Exists(inputFile))
            {
                MessageBox.Show(Resources.MISSING_INPUT, Resources.ERROR);
                return;
            }
         
            string outputFile = this.OutputFile;

            if ((outputFile == null) || (outputFile.Length == 0))
            {
                MessageBox.Show(Resources.MISSING_OUTPUT, Resources.ERROR);
                return;
            }

            Timestamp stamp = null;
            if (settings.TsaEnabled)
                stamp = new Timestamp(settings.TsaUrl, settings.TsaUser, settings.TsaPassword);

            //Adding Meta Datas
            MetaData metaData = new MetaData();
            metaData.Author = authorBox.Text;
            metaData.Title = titleBox.Text;
            metaData.Subject = subjectBox.Text;
            metaData.Keywords = kwBox.Text;
            metaData.Creator = creatorBox.Text;
            metaData.Producer = prodBox.Text;

            Appearance app = new Appearance();
            app.Contact = Contacttext.Text; // PDF field "contact" <-> DigiDoc field "role"
            app.Reason = Reasontext.Text;   // PDF field "reason" <-> DigiDoc field "resolution"
            app.Location = LocationText;
            app.Visible = SigVisible.Checked;
            app.SigLocation = new SignatureLocation(settings);
            app.Page = uint.Parse(settings.SignaturePage);
            app.SignatureRender = (PdfSignatureAppearance.SignatureRender)uint.Parse(settings.SignatureRender);

            PDFSigner pdfSigner = null;

            try
            {
                this.signButton.Enabled = false;
                status(Resources.ADD_SIGNATURE, false);
                pdfSigner = new PDFSigner(reader, inputFile, outputFile);
                pdfSigner.StatusHandler = this.status;
                pdfSigner.Settings = this.settings;
                pdfSigner.DirectoryX509CertStore = this.store;
                pdfSigner.XmlConf = this.conf;
                pdfSigner.Timestamp = stamp;
                pdfSigner.MetaData = metaData;
                pdfSigner.Appearance = app;

                pdfSigner.Sign();                

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
            if (!settings.Initialize())
                MessageBox.Show(settings.lastError, Resources.ERROR);

            if (!settings.Open())
                MessageBox.Show(settings.lastError, Resources.ERROR);

            try
            {
                if (!settings.DebugEnabled)
                {
                    int w = this.Width;                    
                    DebugBox.Hide();                    
                    MaximumSize = new Size(w, tablePanel3.Bottom + 50);
                }

                this.SigVisible.Checked = settings.VisibleSignature;
            }
            catch (Exception) { }

            // UI
            comboLanguage.SelectedIndex = GetCulture(settings.Language);

            // Signature box content
            Contacttext.Text = settings.Contact;
            Reasontext.Text = settings.Reason;
            Citytext.Text = settings.City;
            Countytext.Text = settings.County;
            Countrytext.Text = settings.Country;
            Indextext.Text = settings.Index;

            // digidocpp.conf
            if (!conf.Initialize())
            {
                MessageBox.Show(conf.lastError, Resources.ERROR);
                return;
            }
            if (!conf.Open())
            {
                MessageBox.Show(conf.lastError, Resources.ERROR);
                return;
            }

            // DirectoryX509Store
            OpenCertStore(conf.CertStorePath);
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

            // the following culture based update is needed
            // because MONO 2.4 has issues using some C# classes
            // while changing culture on-the-fly

            switch (comboIndex)
            {
                case 1:
                    culture = new CultureInfo("en-US");
                    linkSettings.Location = new Point(419, 16);
                    linkHelp.Location = new Point(496, 16);
                    break;
                case 2:
                    culture = new CultureInfo("ru-RU");
                    linkSettings.Location = new Point(406, 16);
                    linkHelp.Location = new Point(493, 16);
                    break;
                default:
                    culture = new CultureInfo(string.Empty);
                    linkSettings.Location = new Point(424, 16);
                    linkHelp.Location = new Point(495, 16);
                    break;
            }

            Thread.CurrentThread.CurrentCulture = culture;
            Thread.CurrentThread.CurrentUICulture = culture;
            Resources.Culture = culture;

            settings.Language = culture.Name;
            statusBox.Text = "";

            // Form title
            labelFormName.Text = Resources.UI_FORM_NAME;
            this.Text = Resources.UI_FORM_TITLE;

            // PDF Document box
            label1.Text = Resources.UI_PDF_DOC;
            label2.Text = Resources.UI_INPUT;
            label3.Text = Resources.UI_OUTPUT;
            label4.Text = Resources.UI_STATUS;

            UIInputFile = (this.inputFile.Length > 0) ? this.inputFile : Resources.UI_CHOOSE_INPUT;
            UIOutputFile = (this.outputFile.Length > 0) ? this.outputFile : Resources.UI_CHOOSE_OUTPUT;

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
            label14.Text = Resources.UI_SIGNATURE_RESOLUTION;
            label13.Text = Resources.UI_SIGNATURE_ROLE;
            label15.Text = Resources.UI_SIGNATURE_CITY;
            label16.Text = Resources.UI_SIGNATURE_COUNTY;
            label17.Text = Resources.UI_SIGNATURE_COUNTRY;
            label18.Text = Resources.UI_SIGNATURE_INDEX;
            SigVisible.Text = Resources.UI_SIGNATURE_VISIBLE;

            // Other fields
            signButton.Text = Resources.UI_SIGN_BUTTON;
            linkSettings.Text = Resources.UI_SETTINGS_LINK;
            linkSettings.LinkArea = new LinkArea(0, linkSettings.Text.Length);
            linkHelp.Text = Resources.UI_HELP_LINK;
            linkHelp.LinkArea = new LinkArea(0, linkHelp.Text.Length);
        }

        private void SigVisible_CheckedChanged(object sender, EventArgs e)
        {
            settings.VisibleSignature = this.SigVisible.Checked;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (settings != null)
            {
                settings.Contact = Contacttext.Text;
                settings.Reason = Reasontext.Text;
                settings.City = Citytext.Text;
                settings.County = Countytext.Text;
                settings.Country = Countrytext.Text;
                settings.Index = Indextext.Text;
                settings.Save();
            }
        }

        private void linkSettings_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            FormSettings formSettings = new FormSettings(settings);

            formSettings.ShowDialog();
        }

        private void linkHelp_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            string helpUrl = settings.HelpUrl;

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

        private void Form1_DragDrop(object sender, DragEventArgs e)
        {
            string[] fileList = (string[])e.Data.GetData(DataFormats.FileDrop, false);

            if (fileList != null)
            {
                if (fileList.Length > 1)
                {
                    MessageBox.Show(Resources.UI_TOO_MANY_FILES, Resources.ERROR);
                    return;
                }
                
                if (System.IO.Path.GetExtension(fileList[0]).Equals(".pdf"))
                {
                    InputFile = fileList[0];
                    InitReader();
                }
            }            
        }

        private void Form1_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
                e.Effect = DragDropEffects.All;
            else
                e.Effect = DragDropEffects.None;
        }
    }
}
