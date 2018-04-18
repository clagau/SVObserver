//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Form1.Designer
//* .File Name       : $Workfile:   MainForm.Designer.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Jan 2015 10:46:42  $
//******************************************************************************
namespace SVRemoteTest
{
	partial class MainForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.panel1 = new System.Windows.Forms.Panel();
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.tabControl = new System.Windows.Forms.TabControl();
            this.RemoteTabPage = new System.Windows.Forms.TabPage();
            this.ExecButton = new System.Windows.Forms.Button();
            this.panel3 = new System.Windows.Forms.Panel();
            this.tableLayoutPanelNotify = new System.Windows.Forms.TableLayoutPanel();
            this.statusTextBox = new System.Windows.Forms.TextBox();
            this.statusLabel = new System.Windows.Forms.Label();
            this.Label_LastModified = new System.Windows.Forms.Label();
            this.TextBox_LastModified = new System.Windows.Forms.TextBox();
            this.Mode_Label = new System.Windows.Forms.Label();
            this.axSVRemoteCtrl1 = new AxSVRemoteControlLib.AxSVRemoteCtrl();
            this.DisplayTabControl = new System.Windows.Forms.TabControl();
            this.PropertiesPage = new System.Windows.Forms.TabPage();
            this.Panel_Properties = new System.Windows.Forms.Panel();
            this.ContinualResultsPage = new System.Windows.Forms.TabPage();
            this.Panel_ContinualResults = new System.Windows.Forms.Panel();
            this.tableLayoutPanelErrorNotify = new System.Windows.Forms.TableLayoutPanel();
            this.label_ErrorNr = new System.Windows.Forms.Label();
            this.label_ErrorType = new System.Windows.Forms.Label();
            this.label_ErrorText = new System.Windows.Forms.Label();
            this.panel1.SuspendLayout();
            this.flowLayoutPanel1.SuspendLayout();
            this.tabControl.SuspendLayout();
            this.panel3.SuspendLayout();
            this.tableLayoutPanelNotify.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.axSVRemoteCtrl1)).BeginInit();
            this.DisplayTabControl.SuspendLayout();
            this.PropertiesPage.SuspendLayout();
            this.ContinualResultsPage.SuspendLayout();
            this.tableLayoutPanelErrorNotify.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.flowLayoutPanel1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(206, 581);
            this.panel1.TabIndex = 0;
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.Controls.Add(this.tabControl);
            this.flowLayoutPanel1.Controls.Add(this.ExecButton);
            this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(209, 581);
            this.flowLayoutPanel1.TabIndex = 8;
            // 
            // tabControl
            // 
            this.tabControl.Alignment = System.Windows.Forms.TabAlignment.Bottom;
            this.tabControl.Controls.Add(this.RemoteTabPage);
            this.tabControl.Location = new System.Drawing.Point(3, 3);
            this.tabControl.Name = "tabControl";
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(200, 526);
            this.tabControl.TabIndex = 0;
            this.tabControl.Selected += new System.Windows.Forms.TabControlEventHandler(this.tabControl_Selected);
            // 
            // RemoteTabPage
            // 
            this.RemoteTabPage.Location = new System.Drawing.Point(4, 4);
            this.RemoteTabPage.Name = "RemoteTabPage";
            this.RemoteTabPage.Padding = new System.Windows.Forms.Padding(3);
            this.RemoteTabPage.Size = new System.Drawing.Size(192, 500);
            this.RemoteTabPage.TabIndex = 0;
            this.RemoteTabPage.Text = "Remote";
            this.RemoteTabPage.UseVisualStyleBackColor = true;
        
            //// 
            // ExecButton
            // 
            this.ExecButton.Location = new System.Drawing.Point(60, 542);
            this.ExecButton.Margin = new System.Windows.Forms.Padding(60, 10, 3, 3);
            this.ExecButton.Name = "ExecButton";
            this.ExecButton.Size = new System.Drawing.Size(75, 23);
            this.ExecButton.TabIndex = 3;
            this.ExecButton.Text = "Execute";
            this.ExecButton.UseVisualStyleBackColor = true;
            this.ExecButton.Click += new System.EventHandler(this.ExecButton_Click);
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.tableLayoutPanelNotify);
            this.panel3.Controls.Add(this.axSVRemoteCtrl1);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel3.Location = new System.Drawing.Point(206, 481);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(553, 100);
            this.panel3.TabIndex = 2;
            // 
            // tableLayoutPanelNotify
            // 
            this.tableLayoutPanelNotify.ColumnCount = 4;
            this.tableLayoutPanelNotify.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 58.56482F));
            this.tableLayoutPanelNotify.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 21.99074F));
            this.tableLayoutPanelNotify.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 19.39954F));
            this.tableLayoutPanelNotify.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 134F));
            this.tableLayoutPanelNotify.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 12F));
            this.tableLayoutPanelNotify.Controls.Add(this.statusTextBox, 0, 1);
            this.tableLayoutPanelNotify.Controls.Add(this.statusLabel, 0, 0);
            this.tableLayoutPanelNotify.Controls.Add(this.Label_LastModified, 2, 0);
            this.tableLayoutPanelNotify.Controls.Add(this.TextBox_LastModified, 3, 0);
            this.tableLayoutPanelNotify.Controls.Add(this.Mode_Label, 1, 0);
            this.tableLayoutPanelNotify.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanelNotify.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanelNotify.Name = "tableLayoutPanelNotify";
            this.tableLayoutPanelNotify.RowCount = 2;
            this.tableLayoutPanelNotify.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanelNotify.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 75F));
            this.tableLayoutPanelNotify.Size = new System.Drawing.Size(553, 100);
            this.tableLayoutPanelNotify.TabIndex = 1;
            // 
            // statusTextBox
            // 
            this.tableLayoutPanelNotify.SetColumnSpan(this.statusTextBox, 4);
            this.statusTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.statusTextBox.Location = new System.Drawing.Point(3, 28);
            this.statusTextBox.Multiline = true;
            this.statusTextBox.Name = "statusTextBox";
            this.statusTextBox.ReadOnly = true;
            this.statusTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.statusTextBox.Size = new System.Drawing.Size(547, 69);
            this.statusTextBox.TabIndex = 3;
            // 
            // statusLabel
            // 
            this.statusLabel.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this.statusLabel.AutoSize = true;
            this.statusLabel.Location = new System.Drawing.Point(3, 6);
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(40, 13);
            this.statusLabel.TabIndex = 2;
            this.statusLabel.Text = "Status:";
            this.statusLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Label_LastModified
            // 
            this.Label_LastModified.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this.Label_LastModified.AutoSize = true;
            this.Label_LastModified.Location = new System.Drawing.Point(340, 6);
            this.Label_LastModified.Name = "Label_LastModified";
            this.Label_LastModified.Size = new System.Drawing.Size(73, 13);
            this.Label_LastModified.TabIndex = 4;
            this.Label_LastModified.Text = "Last Modified:";
            this.Label_LastModified.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // TextBox_LastModified
            // 
            this.TextBox_LastModified.Dock = System.Windows.Forms.DockStyle.Fill;
            this.TextBox_LastModified.Location = new System.Drawing.Point(421, 3);
            this.TextBox_LastModified.Name = "TextBox_LastModified";
            this.TextBox_LastModified.ReadOnly = true;
            this.TextBox_LastModified.Size = new System.Drawing.Size(129, 20);
            this.TextBox_LastModified.TabIndex = 5;
            // 
            // Mode_Label
            // 
            this.Mode_Label.AutoSize = true;
            this.Mode_Label.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Mode_Label.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Mode_Label.ForeColor = System.Drawing.Color.MediumBlue;
            this.Mode_Label.Location = new System.Drawing.Point(248, 0);
            this.Mode_Label.Name = "Mode_Label";
            this.Mode_Label.Size = new System.Drawing.Size(86, 25);
            this.Mode_Label.TabIndex = 6;
            this.Mode_Label.Text = "UnknownMode";
            this.Mode_Label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // axSVRemoteCtrl1
            // 
            this.axSVRemoteCtrl1.Enabled = true;
            this.axSVRemoteCtrl1.Location = new System.Drawing.Point(369, 51);
            this.axSVRemoteCtrl1.Name = "axSVRemoteCtrl1";
            this.axSVRemoteCtrl1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axSVRemoteCtrl1.OcxState")));
            this.axSVRemoteCtrl1.Size = new System.Drawing.Size(192, 192);
            this.axSVRemoteCtrl1.TabIndex = 0;
            this.axSVRemoteCtrl1.OnNotify += new AxSVRemoteControlLib._ISVRemoteCtrlEvents_OnNotifyEventHandler(this.axSVRemoteCtrl1_OnNotification);
            // 
            // DisplayTabControl
            // 
            this.DisplayTabControl.Controls.Add(this.PropertiesPage);
            this.DisplayTabControl.Controls.Add(this.ContinualResultsPage);
            this.DisplayTabControl.Location = new System.Drawing.Point(206, 0);
            this.DisplayTabControl.Name = "DisplayTabControl";
            this.DisplayTabControl.SelectedIndex = 0;
            this.DisplayTabControl.Size = new System.Drawing.Size(553, 428);
            this.DisplayTabControl.SizeMode = System.Windows.Forms.TabSizeMode.FillToRight;
            this.DisplayTabControl.TabIndex = 3;
            this.DisplayTabControl.Selected += new System.Windows.Forms.TabControlEventHandler(this.OnSelectedPropertyTab);
            // 
            // PropertiesPage
            // 
            this.PropertiesPage.Controls.Add(this.Panel_Properties);
            this.PropertiesPage.Location = new System.Drawing.Point(4, 22);
            this.PropertiesPage.Name = "PropertiesPage";
            this.PropertiesPage.Padding = new System.Windows.Forms.Padding(3);
            this.PropertiesPage.Size = new System.Drawing.Size(545, 402);
            this.PropertiesPage.TabIndex = 0;
            this.PropertiesPage.Text = "Properties";
            this.PropertiesPage.UseVisualStyleBackColor = true;
            // 
            // Panel_Properties
            // 
            this.Panel_Properties.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Panel_Properties.Location = new System.Drawing.Point(3, 3);
            this.Panel_Properties.Name = "Panel_Properties";
            this.Panel_Properties.Size = new System.Drawing.Size(539, 396);
            this.Panel_Properties.TabIndex = 0;
            // 
            // ContinualResultsPage
            // 
            this.ContinualResultsPage.Controls.Add(this.Panel_ContinualResults);
            this.ContinualResultsPage.Location = new System.Drawing.Point(4, 22);
            this.ContinualResultsPage.Name = "ContinualResultsPage";
            this.ContinualResultsPage.Padding = new System.Windows.Forms.Padding(3);
            this.ContinualResultsPage.Size = new System.Drawing.Size(545, 402);
            this.ContinualResultsPage.TabIndex = 1;
            this.ContinualResultsPage.Text = "Continual Results";
            this.ContinualResultsPage.UseVisualStyleBackColor = true;
            // 
            // Panel_ContinualResults
            // 
            this.Panel_ContinualResults.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Panel_ContinualResults.Location = new System.Drawing.Point(3, 3);
            this.Panel_ContinualResults.Name = "Panel_ContinualResults";
            this.Panel_ContinualResults.Size = new System.Drawing.Size(539, 396);
            this.Panel_ContinualResults.TabIndex = 0;
            // 
            // tableLayoutPanelErrorNotify
            // 
            this.tableLayoutPanelErrorNotify.ColumnCount = 3;
            this.tableLayoutPanelErrorNotify.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 70F));
            this.tableLayoutPanelErrorNotify.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 15F));
            this.tableLayoutPanelErrorNotify.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 15F));
            this.tableLayoutPanelErrorNotify.Controls.Add(this.label_ErrorNr, 1, 0);
            this.tableLayoutPanelErrorNotify.Controls.Add(this.label_ErrorType, 2, 0);
            this.tableLayoutPanelErrorNotify.Controls.Add(this.label_ErrorText, 0, 0);
            this.tableLayoutPanelErrorNotify.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.tableLayoutPanelErrorNotify.Location = new System.Drawing.Point(206, 434);
            this.tableLayoutPanelErrorNotify.Name = "tableLayoutPanelErrorNotify";
            this.tableLayoutPanelErrorNotify.RowCount = 1;
            this.tableLayoutPanelErrorNotify.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelErrorNotify.Size = new System.Drawing.Size(553, 47);
            this.tableLayoutPanelErrorNotify.TabIndex = 4;
            // 
            // label_ErrorNr
            // 
            this.label_ErrorNr.AutoSize = true;
            this.label_ErrorNr.Location = new System.Drawing.Point(390, 0);
            this.label_ErrorNr.Name = "label_ErrorNr";
            this.label_ErrorNr.Size = new System.Drawing.Size(21, 13);
            this.label_ErrorNr.TabIndex = 1;
            this.label_ErrorNr.Text = "XX";
            // 
            // label_ErrorType
            // 
            this.label_ErrorType.AutoSize = true;
            this.label_ErrorType.Location = new System.Drawing.Point(472, 0);
            this.label_ErrorType.Name = "label_ErrorType";
            this.label_ErrorType.Size = new System.Drawing.Size(53, 13);
            this.label_ErrorType.TabIndex = 0;
            this.label_ErrorType.Text = "Unknown";
            // 
            // label_ErrorText
            // 
            this.label_ErrorText.AutoSize = true;
            this.label_ErrorText.Location = new System.Drawing.Point(3, 0);
            this.label_ErrorText.Name = "label_ErrorText";
            this.label_ErrorText.Size = new System.Drawing.Size(31, 13);
            this.label_ErrorText.TabIndex = 2;
            this.label_ErrorText.Text = "????";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(759, 581);
            this.Controls.Add(this.tableLayoutPanelErrorNotify);
            this.Controls.Add(this.DisplayTabControl);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel1);
            this.Name = "MainForm";
            this.Text = "SVRemoteTest";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Shown += new System.EventHandler(this.MainForm_Shown);
            this.panel1.ResumeLayout(false);
            this.flowLayoutPanel1.ResumeLayout(false);
            this.tabControl.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.tableLayoutPanelNotify.ResumeLayout(false);
            this.tableLayoutPanelNotify.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.axSVRemoteCtrl1)).EndInit();
            this.DisplayTabControl.ResumeLayout(false);
            this.PropertiesPage.ResumeLayout(false);
            this.ContinualResultsPage.ResumeLayout(false);
            this.tableLayoutPanelErrorNotify.ResumeLayout(false);
            this.tableLayoutPanelErrorNotify.PerformLayout();
            this.ResumeLayout(false);

		}

		#endregion

        private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Panel panel3;
		private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
		private AxSVRemoteControlLib.AxSVRemoteCtrl axSVRemoteCtrl1;
		private System.Windows.Forms.TableLayoutPanel tableLayoutPanelNotify;
		private System.Windows.Forms.Label statusLabel;
        private System.Windows.Forms.TextBox statusTextBox;
		private System.Windows.Forms.TabControl tabControl;
		private System.Windows.Forms.TabPage RemoteTabPage;
	
		private System.Windows.Forms.Button ExecButton;
        private System.Windows.Forms.Label Label_LastModified;
        private System.Windows.Forms.TextBox TextBox_LastModified;
		public System.Windows.Forms.TabControl DisplayTabControl;
        private System.Windows.Forms.TabPage PropertiesPage;
        private System.Windows.Forms.TabPage ContinualResultsPage;
        private System.Windows.Forms.Panel Panel_Properties;
        private System.Windows.Forms.Panel Panel_ContinualResults;
        private System.Windows.Forms.Label Mode_Label;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelErrorNotify;
        private System.Windows.Forms.Label label_ErrorType;
        private System.Windows.Forms.Label label_ErrorNr;
        private System.Windows.Forms.Label label_ErrorText;
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\MainForm.Designer.cs_v  $
 * 
 *    Rev 1.5   16 Jan 2015 10:46:42   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 21
 * SCR Title: Add Fail Status Streaming
 * Checked in by: sJones; Steve Jones
 * Change Description: Added FailStatusStream tab and form
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.4   05 Sep 2014 15:22:28   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Changed DisplayTabControl to public, so the current tab selection can be determined.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.3   14 Jul 2014 10:51:24   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: add ContinualResultPage and PropertiesPage
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.2   06 Jun 2013 13:34:54   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 5
 * SCR Title: Add Last Modified Notification Support to SVRemoteControl, SVRemoteTest
 * Checked in by: bWalter; Ben Walter
 * Change Description: Added Label_LastModified and TextBox_LastModified controls.
 *   Replaced OnDataArrive with OnNotify.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   02 Jun 2013 11:39:36   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 5
 * SCR Title: Add Last Modified Notification Support to SVRemoteControl, SVRemoteTest
 * Checked in by: bWalter; Ben Walter
 * Change Description: Added Label_LastModified and TextBox_LastModified controls.
 *   Replaced OnDataArrive with OnNotify.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   24 Apr 2012 15:42:10   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/