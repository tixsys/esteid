namespace EstEIDSigner
{
    partial class FormSettings
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormSettings));
            this.tabControlSettings = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.EnableTSA = new System.Windows.Forms.CheckBox();
            this.passwordBox = new System.Windows.Forms.TextBox();
            this.nameTextBox = new System.Windows.Forms.TextBox();
            this.urlTextBox = new System.Windows.Forms.TextBox();
            this.label3Tab1 = new System.Windows.Forms.Label();
            this.label2Tab1 = new System.Windows.Forms.Label();
            this.label1Tab1 = new System.Windows.Forms.Label();
            this.buttonClose = new System.Windows.Forms.Button();
            this.tabControlSettings.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControlSettings
            // 
            this.tabControlSettings.Controls.Add(this.tabPage1);
            resources.ApplyResources(this.tabControlSettings, "tabControlSettings");
            this.tabControlSettings.Name = "tabControlSettings";
            this.tabControlSettings.SelectedIndex = 0;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.EnableTSA);
            this.tabPage1.Controls.Add(this.passwordBox);
            this.tabPage1.Controls.Add(this.nameTextBox);
            this.tabPage1.Controls.Add(this.urlTextBox);
            this.tabPage1.Controls.Add(this.label3Tab1);
            this.tabPage1.Controls.Add(this.label2Tab1);
            this.tabPage1.Controls.Add(this.label1Tab1);
            resources.ApplyResources(this.tabPage1, "tabPage1");
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // EnableTSA
            // 
            resources.ApplyResources(this.EnableTSA, "EnableTSA");
            this.EnableTSA.Name = "EnableTSA";
            this.EnableTSA.UseVisualStyleBackColor = true;
            this.EnableTSA.CheckedChanged += new System.EventHandler(this.EnableTSA_CheckedChanged);
            // 
            // passwordBox
            // 
            resources.ApplyResources(this.passwordBox, "passwordBox");
            this.passwordBox.Name = "passwordBox";
            // 
            // nameTextBox
            // 
            resources.ApplyResources(this.nameTextBox, "nameTextBox");
            this.nameTextBox.Name = "nameTextBox";
            // 
            // urlTextBox
            // 
            resources.ApplyResources(this.urlTextBox, "urlTextBox");
            this.urlTextBox.Name = "urlTextBox";
            // 
            // label3Tab1
            // 
            resources.ApplyResources(this.label3Tab1, "label3Tab1");
            this.label3Tab1.Name = "label3Tab1";
            // 
            // label2Tab1
            // 
            resources.ApplyResources(this.label2Tab1, "label2Tab1");
            this.label2Tab1.Name = "label2Tab1";
            // 
            // label1Tab1
            // 
            resources.ApplyResources(this.label1Tab1, "label1Tab1");
            this.label1Tab1.Name = "label1Tab1";
            // 
            // buttonClose
            // 
            this.buttonClose.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(53)))), ((int)(((byte)(95)))));
            resources.ApplyResources(this.buttonClose, "buttonClose");
            this.buttonClose.ForeColor = System.Drawing.SystemColors.ButtonHighlight;
            this.buttonClose.Name = "buttonClose";
            this.buttonClose.UseVisualStyleBackColor = false;
            this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
            // 
            // FormSettings
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.buttonClose);
            this.Controls.Add(this.tabControlSettings);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormSettings";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.tabControlSettings.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControlSettings;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.Button buttonClose;
        private System.Windows.Forms.Label label3Tab1;
        private System.Windows.Forms.Label label2Tab1;
        private System.Windows.Forms.Label label1Tab1;
        private System.Windows.Forms.TextBox passwordBox;
        private System.Windows.Forms.TextBox nameTextBox;
        private System.Windows.Forms.TextBox urlTextBox;
        private System.Windows.Forms.CheckBox EnableTSA;
    }
}