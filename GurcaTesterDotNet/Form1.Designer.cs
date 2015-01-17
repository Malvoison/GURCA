namespace GurcaTesterDotNet
{
    partial class frmGurcaTester
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
            this.webBrowser1 = new System.Windows.Forms.WebBrowser();
            this.pbInitialize = new System.Windows.Forms.Button();
            this.pbSendMessage = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // webBrowser1
            // 
            this.webBrowser1.Dock = System.Windows.Forms.DockStyle.Top;
            this.webBrowser1.Location = new System.Drawing.Point(0, 0);
            this.webBrowser1.MinimumSize = new System.Drawing.Size(20, 20);
            this.webBrowser1.Name = "webBrowser1";
            this.webBrowser1.Size = new System.Drawing.Size(980, 608);
            this.webBrowser1.TabIndex = 0;            
            this.webBrowser1.WebBrowserShortcutsEnabled = false;
            // 
            // pbInitialize
            // 
            this.pbInitialize.Location = new System.Drawing.Point(801, 642);
            this.pbInitialize.Name = "pbInitialize";
            this.pbInitialize.Size = new System.Drawing.Size(75, 23);
            this.pbInitialize.TabIndex = 1;
            this.pbInitialize.Text = "Initialize";
            this.pbInitialize.UseVisualStyleBackColor = true;
            this.pbInitialize.Click += new System.EventHandler(this.pbInitialize_Click);
            // 
            // pbSendMessage
            // 
            this.pbSendMessage.Location = new System.Drawing.Point(893, 642);
            this.pbSendMessage.Name = "pbSendMessage";
            this.pbSendMessage.Size = new System.Drawing.Size(75, 23);
            this.pbSendMessage.TabIndex = 2;
            this.pbSendMessage.Text = "Send";
            this.pbSendMessage.UseVisualStyleBackColor = true;
            this.pbSendMessage.Click += new System.EventHandler(this.pbSendMessage_Click);
            // 
            // frmGurcaTester
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(980, 677);
            this.Controls.Add(this.pbSendMessage);
            this.Controls.Add(this.pbInitialize);
            this.Controls.Add(this.webBrowser1);
            this.Name = "frmGurcaTester";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "GURCA Tester";
            this.Load += new System.EventHandler(this.frmGurcaTester_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.WebBrowser webBrowser1;
        private System.Windows.Forms.Button pbInitialize;
        private System.Windows.Forms.Button pbSendMessage;
    }
}

