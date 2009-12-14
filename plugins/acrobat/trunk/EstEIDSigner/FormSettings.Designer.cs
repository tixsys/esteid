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
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanelGeneral = new System.Windows.Forms.TableLayoutPanel();
            this.label1 = new System.Windows.Forms.Label();
            this.SignatureRender = new System.Windows.Forms.CheckBox();
            this.pageTextBox = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.radioButtonCoordinates = new System.Windows.Forms.RadioButton();
            this.radioButtonSector = new System.Windows.Forms.RadioButton();
            this.tableLayoutPanelSector = new System.Windows.Forms.TableLayoutPanel();
            this.radioSector9 = new System.Windows.Forms.RadioButton();
            this.radioSector8 = new System.Windows.Forms.RadioButton();
            this.radioSector7 = new System.Windows.Forms.RadioButton();
            this.radioSector6 = new System.Windows.Forms.RadioButton();
            this.radioSector5 = new System.Windows.Forms.RadioButton();
            this.radioSector4 = new System.Windows.Forms.RadioButton();
            this.radioSector3 = new System.Windows.Forms.RadioButton();
            this.radioSector2 = new System.Windows.Forms.RadioButton();
            this.radioSector1 = new System.Windows.Forms.RadioButton();
            this.tableLayoutPanelCoordinates = new System.Windows.Forms.TableLayoutPanel();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.xTextBox = new System.Windows.Forms.TextBox();
            this.yTextBox = new System.Windows.Forms.TextBox();
            this.wTextBox = new System.Windows.Forms.TextBox();
            this.hTextBox = new System.Windows.Forms.TextBox();
            this.tabPage2 = new System.Windows.Forms.TabPage();
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
            this.groupBox2.SuspendLayout();
            this.tableLayoutPanelGeneral.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tableLayoutPanelSector.SuspendLayout();
            this.tableLayoutPanelCoordinates.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControlSettings
            // 
            this.tabControlSettings.Controls.Add(this.tabPage1);
            this.tabControlSettings.Controls.Add(this.tabPage2);
            resources.ApplyResources(this.tabControlSettings, "tabControlSettings");
            this.tabControlSettings.Name = "tabControlSettings";
            this.tabControlSettings.SelectedIndex = 0;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.groupBox2);
            this.tabPage1.Controls.Add(this.groupBox1);
            resources.ApplyResources(this.tabPage1, "tabPage1");
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.tableLayoutPanelGeneral);
            resources.ApplyResources(this.groupBox2, "groupBox2");
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.TabStop = false;
            // 
            // tableLayoutPanelGeneral
            // 
            resources.ApplyResources(this.tableLayoutPanelGeneral, "tableLayoutPanelGeneral");
            this.tableLayoutPanelGeneral.Controls.Add(this.label1, 0, 0);
            this.tableLayoutPanelGeneral.Controls.Add(this.SignatureRender, 1, 1);
            this.tableLayoutPanelGeneral.Controls.Add(this.pageTextBox, 1, 0);
            this.tableLayoutPanelGeneral.Name = "tableLayoutPanelGeneral";
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // SignatureRender
            // 
            resources.ApplyResources(this.SignatureRender, "SignatureRender");
            this.SignatureRender.Name = "SignatureRender";
            this.SignatureRender.UseVisualStyleBackColor = true;
            // 
            // pageTextBox
            // 
            resources.ApplyResources(this.pageTextBox, "pageTextBox");
            this.pageTextBox.Name = "pageTextBox";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.radioButtonCoordinates);
            this.groupBox1.Controls.Add(this.radioButtonSector);
            this.groupBox1.Controls.Add(this.tableLayoutPanelSector);
            this.groupBox1.Controls.Add(this.tableLayoutPanelCoordinates);
            resources.ApplyResources(this.groupBox1, "groupBox1");
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.TabStop = false;
            // 
            // radioButtonCoordinates
            // 
            resources.ApplyResources(this.radioButtonCoordinates, "radioButtonCoordinates");
            this.radioButtonCoordinates.Name = "radioButtonCoordinates";
            this.radioButtonCoordinates.TabStop = true;
            this.radioButtonCoordinates.UseVisualStyleBackColor = true;
            this.radioButtonCoordinates.CheckedChanged += new System.EventHandler(this.radioButtonCoordinates_CheckedChanged);
            // 
            // radioButtonSector
            // 
            resources.ApplyResources(this.radioButtonSector, "radioButtonSector");
            this.radioButtonSector.Name = "radioButtonSector";
            this.radioButtonSector.TabStop = true;
            this.radioButtonSector.UseVisualStyleBackColor = true;
            this.radioButtonSector.CheckedChanged += new System.EventHandler(this.radioButtonSector_CheckedChanged);
            // 
            // tableLayoutPanelSector
            // 
            resources.ApplyResources(this.tableLayoutPanelSector, "tableLayoutPanelSector");
            this.tableLayoutPanelSector.Controls.Add(this.radioSector9, 2, 2);
            this.tableLayoutPanelSector.Controls.Add(this.radioSector8, 1, 2);
            this.tableLayoutPanelSector.Controls.Add(this.radioSector7, 0, 2);
            this.tableLayoutPanelSector.Controls.Add(this.radioSector6, 2, 1);
            this.tableLayoutPanelSector.Controls.Add(this.radioSector5, 1, 1);
            this.tableLayoutPanelSector.Controls.Add(this.radioSector4, 0, 1);
            this.tableLayoutPanelSector.Controls.Add(this.radioSector3, 2, 0);
            this.tableLayoutPanelSector.Controls.Add(this.radioSector2, 1, 0);
            this.tableLayoutPanelSector.Controls.Add(this.radioSector1, 0, 0);
            this.tableLayoutPanelSector.Name = "tableLayoutPanelSector";
            // 
            // radioSector9
            // 
            resources.ApplyResources(this.radioSector9, "radioSector9");
            this.radioSector9.Name = "radioSector9";
            this.radioSector9.TabStop = true;
            this.radioSector9.UseVisualStyleBackColor = true;
            // 
            // radioSector8
            // 
            resources.ApplyResources(this.radioSector8, "radioSector8");
            this.radioSector8.Name = "radioSector8";
            this.radioSector8.TabStop = true;
            this.radioSector8.UseVisualStyleBackColor = true;
            // 
            // radioSector7
            // 
            resources.ApplyResources(this.radioSector7, "radioSector7");
            this.radioSector7.Name = "radioSector7";
            this.radioSector7.TabStop = true;
            this.radioSector7.UseVisualStyleBackColor = true;
            // 
            // radioSector6
            // 
            resources.ApplyResources(this.radioSector6, "radioSector6");
            this.radioSector6.Name = "radioSector6";
            this.radioSector6.TabStop = true;
            this.radioSector6.UseVisualStyleBackColor = true;
            // 
            // radioSector5
            // 
            resources.ApplyResources(this.radioSector5, "radioSector5");
            this.radioSector5.Name = "radioSector5";
            this.radioSector5.TabStop = true;
            this.radioSector5.UseVisualStyleBackColor = true;
            // 
            // radioSector4
            // 
            resources.ApplyResources(this.radioSector4, "radioSector4");
            this.radioSector4.Name = "radioSector4";
            this.radioSector4.TabStop = true;
            this.radioSector4.UseVisualStyleBackColor = true;
            // 
            // radioSector3
            // 
            resources.ApplyResources(this.radioSector3, "radioSector3");
            this.radioSector3.Name = "radioSector3";
            this.radioSector3.TabStop = true;
            this.radioSector3.UseVisualStyleBackColor = true;
            // 
            // radioSector2
            // 
            resources.ApplyResources(this.radioSector2, "radioSector2");
            this.radioSector2.Name = "radioSector2";
            this.radioSector2.TabStop = true;
            this.radioSector2.UseVisualStyleBackColor = true;
            // 
            // radioSector1
            // 
            resources.ApplyResources(this.radioSector1, "radioSector1");
            this.radioSector1.Name = "radioSector1";
            this.radioSector1.TabStop = true;
            this.radioSector1.UseVisualStyleBackColor = true;
            // 
            // tableLayoutPanelCoordinates
            // 
            resources.ApplyResources(this.tableLayoutPanelCoordinates, "tableLayoutPanelCoordinates");
            this.tableLayoutPanelCoordinates.Controls.Add(this.label2, 0, 0);
            this.tableLayoutPanelCoordinates.Controls.Add(this.label3, 0, 1);
            this.tableLayoutPanelCoordinates.Controls.Add(this.label4, 0, 2);
            this.tableLayoutPanelCoordinates.Controls.Add(this.label5, 0, 3);
            this.tableLayoutPanelCoordinates.Controls.Add(this.xTextBox, 1, 0);
            this.tableLayoutPanelCoordinates.Controls.Add(this.yTextBox, 1, 1);
            this.tableLayoutPanelCoordinates.Controls.Add(this.wTextBox, 1, 2);
            this.tableLayoutPanelCoordinates.Controls.Add(this.hTextBox, 1, 3);
            this.tableLayoutPanelCoordinates.Name = "tableLayoutPanelCoordinates";
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.Name = "label4";
            // 
            // label5
            // 
            resources.ApplyResources(this.label5, "label5");
            this.label5.Name = "label5";
            // 
            // xTextBox
            // 
            resources.ApplyResources(this.xTextBox, "xTextBox");
            this.xTextBox.Name = "xTextBox";
            // 
            // yTextBox
            // 
            resources.ApplyResources(this.yTextBox, "yTextBox");
            this.yTextBox.Name = "yTextBox";
            // 
            // wTextBox
            // 
            resources.ApplyResources(this.wTextBox, "wTextBox");
            this.wTextBox.Name = "wTextBox";
            // 
            // hTextBox
            // 
            resources.ApplyResources(this.hTextBox, "hTextBox");
            this.hTextBox.Name = "hTextBox";
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.EnableTSA);
            this.tabPage2.Controls.Add(this.passwordBox);
            this.tabPage2.Controls.Add(this.nameTextBox);
            this.tabPage2.Controls.Add(this.urlTextBox);
            this.tabPage2.Controls.Add(this.label3Tab1);
            this.tabPage2.Controls.Add(this.label2Tab1);
            this.tabPage2.Controls.Add(this.label1Tab1);
            resources.ApplyResources(this.tabPage2, "tabPage2");
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // EnableTSA
            // 
            resources.ApplyResources(this.EnableTSA, "EnableTSA");
            this.EnableTSA.Name = "EnableTSA";
            this.EnableTSA.UseVisualStyleBackColor = true;
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
            this.groupBox2.ResumeLayout(false);
            this.tableLayoutPanelGeneral.ResumeLayout(false);
            this.tableLayoutPanelGeneral.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tableLayoutPanelSector.ResumeLayout(false);
            this.tableLayoutPanelSector.PerformLayout();
            this.tableLayoutPanelCoordinates.ResumeLayout(false);
            this.tableLayoutPanelCoordinates.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControlSettings;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Button buttonClose;
        private System.Windows.Forms.Label label3Tab1;
        private System.Windows.Forms.Label label2Tab1;
        private System.Windows.Forms.Label label1Tab1;
        private System.Windows.Forms.TextBox passwordBox;
        private System.Windows.Forms.TextBox nameTextBox;
        private System.Windows.Forms.TextBox urlTextBox;
        private System.Windows.Forms.CheckBox EnableTSA;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelCoordinates;
        private System.Windows.Forms.TextBox pageTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox xTextBox;
        private System.Windows.Forms.TextBox yTextBox;
        private System.Windows.Forms.TextBox wTextBox;
        private System.Windows.Forms.TextBox hTextBox;
        private System.Windows.Forms.CheckBox SignatureRender;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelGeneral;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelSector;
        private System.Windows.Forms.RadioButton radioButtonCoordinates;
        private System.Windows.Forms.RadioButton radioButtonSector;
        private System.Windows.Forms.RadioButton radioSector9;
        private System.Windows.Forms.RadioButton radioSector8;
        private System.Windows.Forms.RadioButton radioSector7;
        private System.Windows.Forms.RadioButton radioSector6;
        private System.Windows.Forms.RadioButton radioSector5;
        private System.Windows.Forms.RadioButton radioSector4;
        private System.Windows.Forms.RadioButton radioSector3;
        private System.Windows.Forms.RadioButton radioSector2;
        private System.Windows.Forms.RadioButton radioSector1;
    }
}