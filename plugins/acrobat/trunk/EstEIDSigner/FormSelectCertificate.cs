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
