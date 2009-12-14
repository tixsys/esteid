namespace EstEIDSigner
{
    partial class Form1
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

        public partial class DisabledSignButton : System.Windows.Forms.Button
        {
            public DisabledSignButton()
            {                
            }

            protected override void OnPaint(System.Windows.Forms.PaintEventArgs pe)
            {
                if (base.Enabled)
                {
                    base.OnPaint(pe);
                }
                else
                {
                    // Calling the base class OnPaint
                    base.OnPaint(pe);
                    // Fill
                    pe.Graphics.FillRectangle(new System.Drawing.SolidBrush(
                        System.Drawing.Color.FromArgb(base.BackColor.R, base.BackColor.G, base.BackColor.B)),
                        pe.ClipRectangle);
                    // Draw the line around the button
                    pe.Graphics.DrawRectangle(new System.Drawing.Pen(System.Drawing.Color.Black, 1), 0, 0, base.Width - 1, base.Height - 1);
                    // Draw the text in the button in red
                    pe.Graphics.DrawString(base.Text, base.Font,
                        new System.Drawing.SolidBrush(System.Drawing.Color.Silver),
                            (base.Width - pe.Graphics.MeasureString(base.Text, base.Font).Width) / 2, (base.Height / 2) -
                            (pe.Graphics.MeasureString(base.Text, base.Font).Height / 2));
                }
            }
        }


        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.DebugBox = new System.Windows.Forms.TextBox();
            this.linkSettings = new System.Windows.Forms.LinkLabel();
            this.linkHelp = new System.Windows.Forms.LinkLabel();
            this.signButton = new EstEIDSigner.Form1.DisabledSignButton();
            this.labelSeparator1 = new System.Windows.Forms.Label();
            this.linkOutput = new System.Windows.Forms.LinkLabel();
            this.linkInput = new System.Windows.Forms.LinkLabel();
            this.statusBox = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.tablePanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.label1 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.authorBox = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.subjectBox = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.kwBox = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.titleBox = new System.Windows.Forms.Label();
            this.creatorBox = new System.Windows.Forms.Label();
            this.prodBox = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.Citytext = new System.Windows.Forms.TextBox();
            this.Contacttext = new System.Windows.Forms.TextBox();
            this.label14 = new System.Windows.Forms.Label();
            this.Reasontext = new System.Windows.Forms.TextBox();
            this.tablePanel2 = new System.Windows.Forms.TableLayoutPanel();
            this.label5 = new System.Windows.Forms.Label();
            this.tablePanel3 = new System.Windows.Forms.TableLayoutPanel();
            this.Countytext = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.Countrytext = new System.Windows.Forms.TextBox();
            this.label18 = new System.Windows.Forms.Label();
            this.Indextext = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.SigVisible = new System.Windows.Forms.CheckBox();
            this.label12 = new System.Windows.Forms.Label();
            this.comboLanguage = new System.Windows.Forms.ComboBox();
            this.labelFormName = new System.Windows.Forms.Label();
            this.tablePanel1.SuspendLayout();
            this.tablePanel2.SuspendLayout();
            this.tablePanel3.SuspendLayout();
            this.SuspendLayout();
            // 
            // DebugBox
            // 
            resources.ApplyResources(this.DebugBox, "DebugBox");
            this.DebugBox.MaximumSize = new System.Drawing.Size(620, 130);
            this.DebugBox.MinimumSize = new System.Drawing.Size(620, 138);
            this.DebugBox.Name = "DebugBox";
            // 
            // linkSettings
            // 
            resources.ApplyResources(this.linkSettings, "linkSettings");
            this.linkSettings.BackColor = System.Drawing.Color.Transparent;
            this.linkSettings.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(175)))), ((int)(((byte)(198)))), ((int)(((byte)(209)))));
            this.linkSettings.LinkBehavior = System.Windows.Forms.LinkBehavior.NeverUnderline;
            this.linkSettings.LinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(175)))), ((int)(((byte)(198)))), ((int)(((byte)(209)))));
            this.linkSettings.Name = "linkSettings";
            this.linkSettings.TabStop = true;
            this.linkSettings.UseCompatibleTextRendering = true;
            this.linkSettings.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkSettings_LinkClicked);
            // 
            // linkHelp
            // 
            resources.ApplyResources(this.linkHelp, "linkHelp");
            this.linkHelp.BackColor = System.Drawing.Color.Transparent;
            this.linkHelp.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(175)))), ((int)(((byte)(198)))), ((int)(((byte)(209)))));
            this.linkHelp.LinkBehavior = System.Windows.Forms.LinkBehavior.NeverUnderline;
            this.linkHelp.LinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(175)))), ((int)(((byte)(198)))), ((int)(((byte)(209)))));
            this.linkHelp.Name = "linkHelp";
            this.linkHelp.TabStop = true;
            this.linkHelp.UseCompatibleTextRendering = true;
            this.linkHelp.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkHelp_LinkClicked);
            // 
            // signButton
            // 
            resources.ApplyResources(this.signButton, "signButton");
            this.signButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(53)))), ((int)(((byte)(95)))));
            this.signButton.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(53)))), ((int)(((byte)(95)))));
            this.signButton.ForeColor = System.Drawing.SystemColors.ButtonFace;
            this.signButton.Name = "signButton";
            this.signButton.UseVisualStyleBackColor = false;
            this.signButton.Click += new System.EventHandler(this.sign_Click);
            // 
            // labelSeparator1
            // 
            resources.ApplyResources(this.labelSeparator1, "labelSeparator1");
            this.labelSeparator1.BackColor = System.Drawing.Color.Transparent;
            this.labelSeparator1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(175)))), ((int)(((byte)(198)))), ((int)(((byte)(209)))));
            this.labelSeparator1.Name = "labelSeparator1";
            // 
            // linkOutput
            // 
            resources.ApplyResources(this.linkOutput, "linkOutput");
            this.linkOutput.LinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
            this.linkOutput.Name = "linkOutput";
            this.linkOutput.TabStop = true;
            this.linkOutput.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkOutput_LinkClicked);
            // 
            // linkInput
            // 
            resources.ApplyResources(this.linkInput, "linkInput");
            this.linkInput.LinkBehavior = System.Windows.Forms.LinkBehavior.AlwaysUnderline;
            this.linkInput.LinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
            this.linkInput.Name = "linkInput";
            this.linkInput.TabStop = true;
            this.linkInput.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkInput_LinkClicked);
            // 
            // statusBox
            // 
            resources.ApplyResources(this.statusBox, "statusBox");
            this.statusBox.Name = "statusBox";
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.Name = "label4";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // tablePanel1
            // 
            this.tablePanel1.BackColor = System.Drawing.SystemColors.Window;
            resources.ApplyResources(this.tablePanel1, "tablePanel1");
            this.tablePanel1.Controls.Add(this.statusBox, 1, 2);
            this.tablePanel1.Controls.Add(this.label4, 0, 2);
            this.tablePanel1.Controls.Add(this.linkOutput, 1, 1);
            this.tablePanel1.Controls.Add(this.linkInput, 1, 0);
            this.tablePanel1.Controls.Add(this.label2, 0, 0);
            this.tablePanel1.Controls.Add(this.label3, 0, 1);
            this.tablePanel1.Name = "tablePanel1";
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(53)))), ((int)(((byte)(95)))));
            this.label1.Name = "label1";
            // 
            // label11
            // 
            resources.ApplyResources(this.label11, "label11");
            this.label11.Name = "label11";
            // 
            // authorBox
            // 
            resources.ApplyResources(this.authorBox, "authorBox");
            this.authorBox.ForeColor = System.Drawing.SystemColors.ActiveCaption;
            this.authorBox.Name = "authorBox";
            // 
            // label6
            // 
            resources.ApplyResources(this.label6, "label6");
            this.label6.Name = "label6";
            // 
            // label10
            // 
            resources.ApplyResources(this.label10, "label10");
            this.label10.Name = "label10";
            // 
            // label7
            // 
            resources.ApplyResources(this.label7, "label7");
            this.label7.Name = "label7";
            // 
            // subjectBox
            // 
            resources.ApplyResources(this.subjectBox, "subjectBox");
            this.subjectBox.ForeColor = System.Drawing.SystemColors.ActiveCaption;
            this.subjectBox.Name = "subjectBox";
            // 
            // label9
            // 
            resources.ApplyResources(this.label9, "label9");
            this.label9.Name = "label9";
            // 
            // kwBox
            // 
            resources.ApplyResources(this.kwBox, "kwBox");
            this.kwBox.ForeColor = System.Drawing.SystemColors.ActiveCaption;
            this.kwBox.Name = "kwBox";
            // 
            // label8
            // 
            resources.ApplyResources(this.label8, "label8");
            this.label8.Name = "label8";
            // 
            // titleBox
            // 
            resources.ApplyResources(this.titleBox, "titleBox");
            this.titleBox.ForeColor = System.Drawing.SystemColors.ActiveCaption;
            this.titleBox.Name = "titleBox";
            // 
            // creatorBox
            // 
            resources.ApplyResources(this.creatorBox, "creatorBox");
            this.creatorBox.ForeColor = System.Drawing.SystemColors.ActiveCaption;
            this.creatorBox.Name = "creatorBox";
            // 
            // prodBox
            // 
            resources.ApplyResources(this.prodBox, "prodBox");
            this.prodBox.ForeColor = System.Drawing.SystemColors.ActiveCaption;
            this.prodBox.Name = "prodBox";
            // 
            // label15
            // 
            resources.ApplyResources(this.label15, "label15");
            this.label15.Name = "label15";
            // 
            // Citytext
            // 
            resources.ApplyResources(this.Citytext, "Citytext");
            this.Citytext.Name = "Citytext";
            // 
            // Contacttext
            // 
            resources.ApplyResources(this.Contacttext, "Contacttext");
            this.Contacttext.Name = "Contacttext";
            // 
            // label14
            // 
            resources.ApplyResources(this.label14, "label14");
            this.label14.Name = "label14";
            // 
            // Reasontext
            // 
            resources.ApplyResources(this.Reasontext, "Reasontext");
            this.Reasontext.Name = "Reasontext";
            // 
            // tablePanel2
            // 
            this.tablePanel2.BackColor = System.Drawing.SystemColors.Window;
            resources.ApplyResources(this.tablePanel2, "tablePanel2");
            this.tablePanel2.Controls.Add(this.prodBox, 1, 5);
            this.tablePanel2.Controls.Add(this.label11, 0, 5);
            this.tablePanel2.Controls.Add(this.label6, 0, 0);
            this.tablePanel2.Controls.Add(this.creatorBox, 1, 4);
            this.tablePanel2.Controls.Add(this.label10, 0, 4);
            this.tablePanel2.Controls.Add(this.authorBox, 1, 0);
            this.tablePanel2.Controls.Add(this.kwBox, 1, 3);
            this.tablePanel2.Controls.Add(this.label9, 0, 3);
            this.tablePanel2.Controls.Add(this.subjectBox, 1, 2);
            this.tablePanel2.Controls.Add(this.label7, 0, 1);
            this.tablePanel2.Controls.Add(this.titleBox, 1, 1);
            this.tablePanel2.Controls.Add(this.label8, 0, 2);
            this.tablePanel2.Name = "tablePanel2";
            // 
            // label5
            // 
            resources.ApplyResources(this.label5, "label5");
            this.label5.BackColor = System.Drawing.Color.Transparent;
            this.label5.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(53)))), ((int)(((byte)(95)))));
            this.label5.Name = "label5";
            // 
            // tablePanel3
            // 
            this.tablePanel3.BackColor = System.Drawing.SystemColors.Window;
            resources.ApplyResources(this.tablePanel3, "tablePanel3");
            this.tablePanel3.Controls.Add(this.Citytext, 1, 3);
            this.tablePanel3.Controls.Add(this.label15, 0, 3);
            this.tablePanel3.Controls.Add(this.Countytext, 1, 4);
            this.tablePanel3.Controls.Add(this.label16, 0, 4);
            this.tablePanel3.Controls.Add(this.label17, 0, 5);
            this.tablePanel3.Controls.Add(this.Countrytext, 1, 5);
            this.tablePanel3.Controls.Add(this.label18, 0, 6);
            this.tablePanel3.Controls.Add(this.Indextext, 1, 6);
            this.tablePanel3.Controls.Add(this.label13, 0, 0);
            this.tablePanel3.Controls.Add(this.label14, 0, 1);
            this.tablePanel3.Controls.Add(this.Contacttext, 1, 0);
            this.tablePanel3.Controls.Add(this.Reasontext, 1, 1);
            this.tablePanel3.Controls.Add(this.SigVisible, 1, 7);
            this.tablePanel3.Name = "tablePanel3";
            // 
            // Countytext
            // 
            resources.ApplyResources(this.Countytext, "Countytext");
            this.Countytext.Name = "Countytext";
            // 
            // label16
            // 
            resources.ApplyResources(this.label16, "label16");
            this.label16.Name = "label16";
            // 
            // label17
            // 
            resources.ApplyResources(this.label17, "label17");
            this.label17.Name = "label17";
            // 
            // Countrytext
            // 
            resources.ApplyResources(this.Countrytext, "Countrytext");
            this.Countrytext.Name = "Countrytext";
            // 
            // label18
            // 
            resources.ApplyResources(this.label18, "label18");
            this.label18.Name = "label18";
            // 
            // Indextext
            // 
            resources.ApplyResources(this.Indextext, "Indextext");
            this.Indextext.Name = "Indextext";
            // 
            // label13
            // 
            resources.ApplyResources(this.label13, "label13");
            this.label13.Name = "label13";
            // 
            // SigVisible
            // 
            resources.ApplyResources(this.SigVisible, "SigVisible");
            this.SigVisible.Name = "SigVisible";
            this.SigVisible.UseVisualStyleBackColor = true;
            this.SigVisible.CheckedChanged += new System.EventHandler(this.SigVisible_CheckedChanged);
            // 
            // label12
            // 
            resources.ApplyResources(this.label12, "label12");
            this.label12.BackColor = System.Drawing.Color.Transparent;
            this.label12.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(53)))), ((int)(((byte)(95)))));
            this.label12.Name = "label12";
            // 
            // comboLanguage
            // 
            this.comboLanguage.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboLanguage.FormattingEnabled = true;
            this.comboLanguage.Items.AddRange(new object[] {
            resources.GetString("comboLanguage.Items"),
            resources.GetString("comboLanguage.Items1"),
            resources.GetString("comboLanguage.Items2")});
            resources.ApplyResources(this.comboLanguage, "comboLanguage");
            this.comboLanguage.Name = "comboLanguage";
            this.comboLanguage.SelectedIndexChanged += new System.EventHandler(this.comboLanguage_SelectedIndexChanged);
            // 
            // labelFormName
            // 
            resources.ApplyResources(this.labelFormName, "labelFormName");
            this.labelFormName.BackColor = System.Drawing.Color.Transparent;
            this.labelFormName.ForeColor = System.Drawing.SystemColors.Info;
            this.labelFormName.Name = "labelFormName";
            // 
            // Form1
            // 
            this.AllowDrop = true;
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImage = global::EstEIDSigner.Properties.Resources.background;
            this.Controls.Add(this.labelFormName);
            this.Controls.Add(this.comboLanguage);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.tablePanel3);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.tablePanel2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.tablePanel1);
            this.Controls.Add(this.labelSeparator1);
            this.Controls.Add(this.linkHelp);
            this.Controls.Add(this.linkSettings);
            this.Controls.Add(this.signButton);
            this.Controls.Add(this.DebugBox);
            this.DoubleBuffered = true;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.DragDrop += new System.Windows.Forms.DragEventHandler(this.Form1_DragDrop);
            this.DragEnter += new System.Windows.Forms.DragEventHandler(this.Form1_DragEnter);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.tablePanel1.ResumeLayout(false);
            this.tablePanel1.PerformLayout();
            this.tablePanel2.ResumeLayout(false);
            this.tablePanel2.PerformLayout();
            this.tablePanel3.ResumeLayout(false);
            this.tablePanel3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label prodBox;
        private System.Windows.Forms.Label creatorBox;
        private System.Windows.Forms.Label kwBox;
        private System.Windows.Forms.Label subjectBox;        
        private System.Windows.Forms.Label titleBox;        
        private System.Windows.Forms.Label authorBox;        
        private System.Windows.Forms.TextBox DebugBox;      
        private DisabledSignButton signButton;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TextBox Reasontext;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox Citytext;
        private System.Windows.Forms.TextBox Contacttext;
        private System.Windows.Forms.Label statusBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.LinkLabel linkOutput;
        private System.Windows.Forms.LinkLabel linkInput;
        private System.Windows.Forms.LinkLabel linkSettings;
        private System.Windows.Forms.LinkLabel linkHelp;
        private System.Windows.Forms.Label labelSeparator1;
        private System.Windows.Forms.TableLayoutPanel tablePanel1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TableLayoutPanel tablePanel2;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TableLayoutPanel tablePanel3;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.ComboBox comboLanguage;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label labelFormName;
        private System.Windows.Forms.TextBox Countytext;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox Countrytext;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.TextBox Indextext;
        private System.Windows.Forms.CheckBox SigVisible;
    }
}

