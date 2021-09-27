namespace WindowsFormsApp1
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.textTxIP = new System.Windows.Forms.TextBox();
            this.setIPBtn = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // textTxIP
            // 
            this.textTxIP.Location = new System.Drawing.Point(53, 17);
            this.textTxIP.Name = "textTxIP";
            this.textTxIP.Size = new System.Drawing.Size(178, 22);
            this.textTxIP.TabIndex = 0;
            // 
            // setIPBtn
            // 
            this.setIPBtn.Location = new System.Drawing.Point(251, 11);
            this.setIPBtn.Name = "setIPBtn";
            this.setIPBtn.Size = new System.Drawing.Size(93, 35);
            this.setIPBtn.TabIndex = 1;
            this.setIPBtn.Text = "Set IP";
            this.setIPBtn.UseVisualStyleBackColor = true;
            this.setIPBtn.Click += new System.EventHandler(this.SetIPBtn_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1216, 691);
            this.Controls.Add(this.setIPBtn);
            this.Controls.Add(this.textTxIP);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.Form1_Paint);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Form1_MouseDown);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form1_MouseMove);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Form1_MouseUp);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textTxIP;
        private System.Windows.Forms.Button setIPBtn;
    }
}

