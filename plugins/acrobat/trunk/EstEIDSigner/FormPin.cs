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
        public FormPin(string s, int maxpin)
        {
            InitializeComponent();
            labelExtra.Text = s;
            textBoxPin.MaxLength = maxpin;
        }

        public string Pin
        {
            get { return (this.textBoxPin.Text); }
        }

        private void buttonOk_Click(object sender, EventArgs e)
        {

        }
    }
}
