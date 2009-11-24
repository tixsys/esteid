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
    public partial class FormSettings : Form
    {
        private EstEIDSettings config;

        public FormSettings(EstEIDSettings config)
        {
            InitializeComponent();

            this.config = config;

            this.EnableTSA.Checked = config.ToBoolean("enable_tsa");
            this.urlTextBox.Text = config.ToString("tsa_url");
            this.nameTextBox.Text = config.ToString("tsa_user");
            this.passwordBox.Text = config.ToString("tsa_password");
        }

        private void EnableTSA_CheckedChanged(object sender, EventArgs e)
        {            
            config.AddOrReplace("enable_tsa", this.EnableTSA.Checked.ToString());
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            config.AddOrReplace("tsa_url", this.urlTextBox.Text);
            config.AddOrReplace("tsa_user", this.nameTextBox.Text);
            config.AddOrReplace("tsa_password", this.passwordBox.Text);

            Close();
        }
    }
}
