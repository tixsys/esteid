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
            this.pageTextBox.Text = config.SignaturePage.ToString();
            this.SignatureRender.Checked = config.SignatureRender.Equals("1");
            this.radioButtonSector.Checked = config.SignatureUseSector;
            this.radioButtonCoordinates.Checked = config.SignatureUseCoordinates;
            this.xTextBox.Text = config.SignatureX.ToString();
            this.yTextBox.Text = config.SignatureY.ToString();
            this.wTextBox.Text = config.SignatureW.ToString();
            this.hTextBox.Text = config.SignatureH.ToString();
            
            // timestamp authority
            this.EnableTSA.Checked = config.TsaEnabled;
            this.urlTextBox.Text = config.TsaUrl;
            this.nameTextBox.Text = config.TsaUser;
            this.passwordBox.Text = config.TsaPassword;

            if ((!this.radioButtonSector.Checked) && (!this.radioButtonCoordinates.Checked))
            {
                this.radioButtonSector.Checked = true;
                config.SignatureUseSector = this.radioButtonSector.Checked;
            }

            SignatureSector = config.SignatureSector;
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            // signature location
            config.SignaturePage = this.pageTextBox.Text;
            config.SignatureRender = this.SignatureRender.Checked ? "1" : "0";
            config.SignatureSector = SignatureSector;
            config.SignatureUseSector = this.radioButtonSector.Checked;
            config.SignatureUseCoordinates = this.radioButtonCoordinates.Checked;
            config.SignatureX = uint.Parse(this.xTextBox.Text);
            config.SignatureY = uint.Parse(this.yTextBox.Text);
            config.SignatureW = uint.Parse(this.wTextBox.Text);
            config.SignatureH = uint.Parse(this.hTextBox.Text);
            
            // timestamp authority
            config.TsaEnabled = this.EnableTSA.Checked;
            config.TsaUrl = this.urlTextBox.Text;
            config.TsaUser = this.nameTextBox.Text;
            config.TsaPassword = this.passwordBox.Text;

            Close();
        }

        private void radioButtonSector_CheckedChanged(object sender, EventArgs e)
        {
            tableLayoutPanelSector.Enabled = true;
            tableLayoutPanelCoordinates.Enabled = false;
            SignatureSector = config.SignatureSector;
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
