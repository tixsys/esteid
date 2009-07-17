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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using iTextSharp.text.pdf;

namespace EstEIDSigner
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        
        private void debug(string txt)
        {
            DebugBox.AppendText(txt + System.Environment.NewLine);
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


            PdfReader reader = new PdfReader(inputBox.Text);

            MetaData md = new MetaData();
            md.Info = reader.Info;

            authorBox.Text = md.Author;
            titleBox.Text = md.Title;
            subjectBox.Text = md.Subject;
            kwBox.Text = md.Keywords;
            creatorBox.Text = md.Creator;
            prodBox.Text = md.Producer;

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

        private void button1_Click(object sender, EventArgs e)
        {
            bool failed = false;
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
            
            debug("Allkirjastan dokumenti ... ");

            try
            {
                pdfSigner = new PDFSigner(inputBox.Text, outputBox.Text, stamp, metaData, app);
                pdfSigner.Sign();
            }
            catch (Exception ex)
            {                
                debug("Exception: " + ex.ToString());
                MessageBox.Show(ex.Message);
                failed = true;
            }

            // force Garbage Collector to close pkcs11 bridge
            GC.Collect();            

            if (failed == false)
                debug("Valmis :)");
        }
    }
}