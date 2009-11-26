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
    public partial class FormPin : Form
    {
        private int minLength = 0;

        public FormPin(string s, int minpin, int maxpin)
        {
            this.Font = new Font("Verdana", (float)8.25);

            InitializeComponent();
            labelExtra.Text = s;
            textBoxPin.MaxLength = maxpin;
            minLength = minpin;
            buttonOk.Enabled = false;
        }

        public string Pin
        {
            get { return (this.textBoxPin.Text); }
        }

        private void textBoxPin_TextChanged(object sender, EventArgs e)
        {
            buttonOk.Enabled = (textBoxPin.Text.Length >= minLength);
        }
    }
}
