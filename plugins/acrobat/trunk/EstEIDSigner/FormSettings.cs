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

            // signature settings            
            this.pageTextBox.Text = config.ToString("signature_page", EstEIDSettings.SignaturePage);
            this.SignatureRender.Checked = config.ToString("signature_render").Equals("1");
            this.radioButtonSector.Checked = config.ToBoolean("signature_use_sector");
            this.radioButtonCoordinates.Checked = config.ToBoolean("signature_use_coords");
            this.xTextBox.Text = config.ToString("signature_x", EstEIDSettings.SignatureX);
            this.yTextBox.Text = config.ToString("signature_y", EstEIDSettings.SignatureY);
            this.wTextBox.Text = config.ToString("signature_w", EstEIDSettings.SignatureW);
            this.hTextBox.Text = config.ToString("signature_h", EstEIDSettings.SignatureH);
            
            // timestamp authority
            this.EnableTSA.Checked = config.ToBoolean("enable_tsa");
            this.urlTextBox.Text = config.ToString("tsa_url");
            this.nameTextBox.Text = config.ToString("tsa_user");
            this.passwordBox.Text = config.ToString("tsa_password");

            if ((!this.radioButtonSector.Checked) && (!this.radioButtonCoordinates.Checked))
            {
                this.radioButtonSector.Checked = true;
                config.AddOrReplace("signature_use_sector", this.radioButtonSector.Checked.ToString());
            }

            SignatureSector = config.ToString("signature_sector", "9");
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            // signature location
            config.AddOrReplace("signature_page", this.pageTextBox.Text);
            config.AddOrReplace("signature_render", this.SignatureRender.Checked ? "1" : "0");
            config.AddOrReplace("signature_sector", SignatureSector);
            config.AddOrReplace("signature_use_sector", this.radioButtonSector.Checked.ToString());
            config.AddOrReplace("signature_use_coords", this.radioButtonCoordinates.Checked.ToString());
            config.AddOrReplace("signature_x", this.xTextBox.Text);
            config.AddOrReplace("signature_y", this.yTextBox.Text);
            config.AddOrReplace("signature_w", this.wTextBox.Text);
            config.AddOrReplace("signature_h", this.hTextBox.Text);
            
            // timestamp authority
            config.AddOrReplace("enable_tsa", this.EnableTSA.Checked.ToString());
            config.AddOrReplace("tsa_url", this.urlTextBox.Text);
            config.AddOrReplace("tsa_user", this.nameTextBox.Text);
            config.AddOrReplace("tsa_password", this.passwordBox.Text);

            Close();
        }

        private void radioButtonSector_CheckedChanged(object sender, EventArgs e)
        {
            tableLayoutPanelSector.Enabled = true;
            tableLayoutPanelCoordinates.Enabled = false;
            SignatureSector = config.ToString("signature_sector", "9");
        }

        private void radioButtonCoordinates_CheckedChanged(object sender, EventArgs e)
        {
            tableLayoutPanelSector.Enabled = false;
            tableLayoutPanelCoordinates.Enabled = true;

        }

        private string SignatureSector
        {
            set
            {
                uint index = System.Convert.ToUInt32(value);
                switch (index)
                {
                    case 2:
                        radioSector2.Checked = true;
                        break;
                    case 3:
                        radioSector3.Checked = true;
                        break;
                    case 4:
                        radioSector4.Checked = true;
                        break;
                    case 5:
                        radioSector5.Checked = true;
                        break;
                    case 6:
                        radioSector6.Checked = true;
                        break;
                    case 7:
                        radioSector7.Checked = true;
                        break;
                    case 8:
                        radioSector8.Checked = true;
                        break;
                    case 9:
                        radioSector9.Checked = true;
                        break;
                    default:
                        radioSector1.Checked = true;
                        break;
                } 
            }
            get
            {
                if (radioSector2.Checked) return "2";
                if (radioSector3.Checked) return "3";
                if (radioSector4.Checked) return "4";
                if (radioSector5.Checked) return "5";
                if (radioSector6.Checked) return "6";
                if (radioSector7.Checked) return "7";
                if (radioSector8.Checked) return "8";
                if (radioSector9.Checked) return "9";

                return "1";
            }
        }
    }
}
