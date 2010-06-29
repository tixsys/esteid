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

namespace EstEIDSigner
{
    partial class FormSelectCertificate
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormSelectCertificate));
            this.buttonSelect = new System.Windows.Forms.Button();
            this.buttonCancel = new System.Windows.Forms.Button();
            this.Message = new System.Windows.Forms.Label();
            this.listCertificates = new System.Windows.Forms.ListView();
            this.SuspendLayout();
            // 
            // buttonSelect
            // 
            this.buttonSelect.AccessibleDescription = null;
            this.buttonSelect.AccessibleName = null;
            resources.ApplyResources(this.buttonSelect, "buttonSelect");
            this.buttonSelect.BackgroundImage = null;
            this.buttonSelect.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.buttonSelect.Name = "buttonSelect";
            this.buttonSelect.UseVisualStyleBackColor = true;
            this.buttonSelect.Click += new System.EventHandler(this.Ok_Click);
            // 
            // buttonCancel
            // 
            this.buttonCancel.AccessibleDescription = null;
            this.buttonCancel.AccessibleName = null;
            resources.ApplyResources(this.buttonCancel, "buttonCancel");
            this.buttonCancel.BackgroundImage = null;
            this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.UseVisualStyleBackColor = true;
            // 
            // Message
            // 
            this.Message.AccessibleDescription = null;
            this.Message.AccessibleName = null;
            resources.ApplyResources(this.Message, "Message");
            this.Message.Name = "Message";
            // 
            // listCertificates
            // 
            this.listCertificates.AccessibleDescription = null;
            this.listCertificates.AccessibleName = null;
            resources.ApplyResources(this.listCertificates, "listCertificates");
            this.listCertificates.BackgroundImage = null;
            this.listCertificates.MultiSelect = false;
            this.listCertificates.Name = "listCertificates";
            this.listCertificates.UseCompatibleStateImageBehavior = false;
            this.listCertificates.View = System.Windows.Forms.View.Details;
            this.listCertificates.SelectedIndexChanged += new System.EventHandler(this.listCertificates_SelectedIndexChanged);
            // 
            // FormSelectCertificate
            // 
            this.AcceptButton = this.buttonSelect;
            this.AccessibleDescription = null;
            this.AccessibleName = null;
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImage = null;
            this.CancelButton = this.buttonCancel;
            this.Controls.Add(this.listCertificates);
            this.Controls.Add(this.Message);
            this.Controls.Add(this.buttonCancel);
            this.Controls.Add(this.buttonSelect);            
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormSelectCertificate";
            this.ResumeLayout(false);
            this.PerformLayout();
        }

        #endregion

        private System.Windows.Forms.Button buttonSelect;
        private System.Windows.Forms.Button buttonCancel;
        private System.Windows.Forms.Label Message;
        private System.Windows.Forms.ListView listCertificates;
    }
}