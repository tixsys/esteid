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
using System.Configuration;
using System.Threading;

using iTextSharp.text.pdf;

namespace EstEIDSigner
{
    public partial class Form1 : Form
    {
        private DirectoryX509CertStore store = null;
        private Configuration config = null;
        private PKCS12Settings credentials = null;
        private PdfReader reader = null;

        public Form1()
        {
            this.Font = new Font("Verdana", (float)8.25);

            InitializeComponent();

            config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);

            LoadConfig();            
        }

        private void debug(string txt)
        {
            DebugBox.AppendText(txt + System.Environment.NewLine);
            Application.DoEvents();
        }

        private void status(string txt)
        {            
            statusBox.Text = txt;
            debug(txt);
        }

        private void InitReader()
        {
            reader = new PdfReader(inputBox.Text);

            MetaData md = new MetaData();
            md.Info = reader.Info;

            authorBox.Text = md.Author;
            titleBox.Text = md.Title;
            subjectBox.Text = md.Subject;
            kwBox.Text = md.Keywords;
            creatorBox.Text = md.Creator;
            prodBox.Text = md.Producer; 
        }

        private void button4_Click(object sender, EventArgs e)
        {
            System.Windows.Forms.OpenFileDialog openFile;
            openFile = new System.Windows.Forms.OpenFileDialog();
            openFile.Filter = "PDF failid *.pdf|*.pdf";
            openFile.Title = "Vali fail";
            if (openFile.ShowDialog() != DialogResult.OK)
                return;

            inputBox.Text = openFile.FileName;

            InitReader();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            System.Windows.Forms.SaveFileDialog saveFile;
            saveFile = new System.Windows.Forms.SaveFileDialog();
            saveFile.Filter = "PDF failid (*.pdf)|*.pdf";
            saveFile.Title = "Salvesta PDF File";
            if (saveFile.ShowDialog() != DialogResult.OK)
                return;

            outputBox.Text = saveFile.FileName;
        }

        private void OpenCertStore()
        {
            try
            {
                if (store == null)
                {
                    string path = System.Configuration.ConfigurationSettings.AppSettings["cert_path"];
                    if (path == null || path.Length == 0)
                        path = Directory.GetCurrentDirectory();
                    store = new DirectoryX509CertStore(path);
                    if (store.Open() == false)
                        debug("Sertifikaatide lugemine ebaõnnestus kataloogist: " + path);
                }
            }
            catch (Exception ex)
            {
                debug(ex.ToString());
                MessageBox.Show(ex.Message);
            }
        }

        private void LoadPKCS12Config()
        {
            if (credentials == null)
            {
                credentials = new PKCS12Settings();

                if (credentials.Load() == false)             
                    throw new Exception("PKCS#12 muutujate lugemine ebaõnnestus: " + credentials.lastError);
            }
        }

        private void sign_Click(object sender, EventArgs e)
        {
            if (!File.Exists(inputBox.Text))
            {
                MessageBox.Show("Allkirjastamiseks on vaja valida olemasolev PDF dokument!", "Hoiatus");
                return;
            }

            if (File.Exists(outputBox.Text))
            {
                if (MessageBox.Show("Kas soovid olemasoleva väljund PDF dokumendi üle kirjutada?", "Hoiatus", 
                    MessageBoxButtons.YesNo) == DialogResult.No)
                    return;
            }

            Timestamp stamp = null;
            if (EnableTSA.Checked)
                stamp = new Timestamp(urlTextBox.Text, nameTextBox.Text, passwordBox.Text);

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
                status("Loen sertifikaate ... ");
                OpenCertStore();

                status("Loen seadeid ... ");
                LoadPKCS12Config();

                // if user copy-pasted filename into PDF input field then force init...
                if (reader == null)
                    InitReader();

                status("Allkirjastan dokumenti ...");

                pdfSigner = new PDFSigner(reader, inputBox.Text, outputBox.Text, stamp, metaData, app, store);
                pdfSigner.StatusHandler = this.status;
                pdfSigner.Sign(credentials);

                status("Dokument allkirjastatud!");
            }
            catch (CancelException)
            {
                // user action was canceled
                status("Allkirjastamine katkestati ...");
            }
            catch (RevocationException ex)
            {
                status("Kehtivussertifikaadi viga!");
                MessageBox.Show(ex.Message);
            }
            catch (AlreadySignedException ex)
            {
                status("Dokument on juba allkirjastatud!!");
                MessageBox.Show(ex.Message);
            }
            catch (Exception ex)
            {
                status("Tekkis viga ...");
                debug(ex.ToString());
                MessageBox.Show(ex.Message);
            }

            // force Garbage Collector to close pkcs11 bridge
            GC.Collect();
        }

        private void LoadConfig()
        {
            string s;
            bool b;

            try
            {
                s = ConfigurationSettings.AppSettings["debug"];
                b = System.Convert.ToBoolean(s);

                if (!b)
                {
                    DebugBox.Hide();
                    ClientSize = new Size(this.Width - 10, groupBox3.Bottom + 15);
                    MaximumSize = new Size(this.Width, groupBox3.Bottom + 50);
                }

                s = ConfigurationSettings.AppSettings["enable_tsa"];
                b = System.Convert.ToBoolean(s);
                this.EnableTSA.Checked = b;

                s = ConfigurationSettings.AppSettings["visible_signature"];
                b = System.Convert.ToBoolean(s);
                this.SigVisible.Checked = b;
            }
            catch (Exception) { }
        }

        private void EnableTSA_CheckedChanged(object sender, EventArgs e)
        {
            if (config.AppSettings.Settings["enable_tsa"] == null)
                config.AppSettings.Settings.Add("enable_tsa", "");

            config.AppSettings.Settings["enable_tsa"].Value = this.EnableTSA.Checked.ToString();
        }

        private void SigVisible_CheckedChanged(object sender, EventArgs e)
        {
            if (config.AppSettings.Settings["visible_signature"] == null)
                config.AppSettings.Settings.Add("visible_signature", "");

            config.AppSettings.Settings["visible_signature"].Value = this.SigVisible.Checked.ToString();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (config != null)
                config.Save();
        }        
    }
}