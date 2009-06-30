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
            openFile.Filter = "PDF files *.pdf|*.pdf";
            openFile.Title = "Select a file";
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
            saveFile.Filter = "PDF files (*.pdf)|*.pdf";
            saveFile.Title = "Save PDF File";
            if (saveFile.ShowDialog() != DialogResult.OK)
                return;
            outputBox.Text = saveFile.FileName;

        }     

        private void button1_Click(object sender, EventArgs e)
        {
            debug("Alustan ...");
            
            Timestamp stamp = new Timestamp(urlTextBox.Text, passwordBox.Text);

            debug("uus MetaData ... ");

            //Adding Meta Datas
            MetaData metaData = new MetaData();
            metaData.Author = authorBox.Text;
            metaData.Title = titleBox.Text;
            metaData.Subject = subjectBox.Text;
            metaData.Keywords = kwBox.Text;
            metaData.Creator = creatorBox.Text;
            metaData.Producer = prodBox.Text;

            debug("uus Appearance ... ");
            Appearance app = new Appearance();
            app.Contact = Contacttext.Text;
            app.Reason = Reasontext.Text;
            app.Location = Locationtext.Text;
            app.Visible = SigVisible.Checked;
            
            debug("Allkirjastan dokumenti ... ");

            try
            {
                PDFSigner pdfs = new PDFSigner(inputBox.Text, outputBox.Text, stamp, metaData, app);
                pdfs.Sign();
            }
            catch (Exception ex)
            {                
                debug("Exception : " + ex.ToString());
                return;
            }

            debug("Valmis :)");
        }
    }
}