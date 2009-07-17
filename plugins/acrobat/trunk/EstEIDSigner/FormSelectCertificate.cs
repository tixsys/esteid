using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace EstEIDSigner
{
    public partial class FormSelectCertificate : Form
    {
        private int selected = -1;

        public FormSelectCertificate(string title, string message)
        {
            InitializeComponent();

            this.Text = title;
            this.Message.Text = message;

            ColumnHeader[] head = new ColumnHeader[3];

            head[0] = new ColumnHeader();
            head[0].Text = "Väljastatud";
            head[0].Width = (int)(this.listCertificates.Width * 0.49);
            
            head[1] = new ColumnHeader();
            head[1].Text = "Väljastaja";
            head[1].Width = (int)(this.listCertificates.Width * 0.25);

            head[2] = new ColumnHeader();
            head[2].Text = "Kehtivusaeg";
            head[2].Width = (int)(this.listCertificates.Width * 0.25);
            
            listCertificates.Columns.AddRange(head);

            /*listCertificates.Columns.Add("Väljastatud", -2, HorizontalAlignment.Center);
            listCertificates.Columns.Add("Väljastaja", -2, HorizontalAlignment.Center);
            listCertificates.Columns.Add("Kehtivusaeg", -2, HorizontalAlignment.Center);*/
        }

        public int Add(string issuedTo, string issudeBy, string validUntil)
        {
            ListViewItem item = new ListViewItem();

            item.Text = issuedTo;
            item.SubItems.Add(issudeBy);
            item.SubItems.Add(validUntil);

            item = listCertificates.Items.Add(item);

            return item.Index;
        }

        private void Ok_Click(object sender, EventArgs e)
        {
            foreach (int index in listCertificates.SelectedIndices)
            {
                ListViewItem item = listCertificates.Items[index];
                selected = item.Index;
                break; // we use 1-row selection
            }
        }

        public int Selected
        {
            get { return selected; }
        }
    }
}
