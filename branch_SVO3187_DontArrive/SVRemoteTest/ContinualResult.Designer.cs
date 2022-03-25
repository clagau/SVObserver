//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ContinualResultForm
//* .File Name       : $Workfile:   ContinualResult.Designer.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   11 Nov 2014 11:08:30  $
//******************************************************************************
namespace SVRemoteTest
{
    partial class ContinualResultForm
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
			this.panel1 = new System.Windows.Forms.Panel();
			this.labelTimeSpan = new System.Windows.Forms.Label();
			this.timeSpanBox = new System.Windows.Forms.TextBox();
			this.ImageSelectionBox = new System.Windows.Forms.ComboBox();
			this.ErrorLabel = new System.Windows.Forms.Label();
			this.StartStopButton = new System.Windows.Forms.Button();
			this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
			this.tableLayoutPanel_ErrorMsg = new System.Windows.Forms.TableLayoutPanel();
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.panel2 = new System.Windows.Forms.Panel();
			this.RejectButton4 = new System.Windows.Forms.Button();
			this.RejectButton5 = new System.Windows.Forms.Button();
			this.RejectButton3 = new System.Windows.Forms.Button();
			this.RejectButton2 = new System.Windows.Forms.Button();
			this.RejectButton1 = new System.Windows.Forms.Button();
			this.ResetButton = new System.Windows.Forms.Button();
			this.inputPropertyGrid = new SVRemoteTest.SVPropertyGrid();
			this.UseRemoteOutputsCheckBox = new System.Windows.Forms.CheckBox();
			this.LoadAllImagesCheckbox = new System.Windows.Forms.CheckBox();
			this.IsRunLoopCheckbox = new System.Windows.Forms.CheckBox();
			this.NumberOfRunsTextBox = new System.Windows.Forms.TextBox();
			this.panel1.SuspendLayout();
			this.tableLayoutPanel1.SuspendLayout();
			this.tableLayoutPanel_ErrorMsg.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
			this.panel2.SuspendLayout();
			this.SuspendLayout();
			// 
			// panel1
			// 
			this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.panel1.Controls.Add(this.NumberOfRunsTextBox);
			this.panel1.Controls.Add(this.IsRunLoopCheckbox);
			this.panel1.Controls.Add(this.ResetButton);
			this.panel1.Controls.Add(this.LoadAllImagesCheckbox);
			this.panel1.Controls.Add(this.UseRemoteOutputsCheckBox);
			this.panel1.Controls.Add(this.labelTimeSpan);
			this.panel1.Controls.Add(this.timeSpanBox);
			this.panel1.Controls.Add(this.ImageSelectionBox);
			this.panel1.Controls.Add(this.ErrorLabel);
			this.panel1.Controls.Add(this.StartStopButton);
			this.panel1.Location = new System.Drawing.Point(3, 469);
			this.panel1.MinimumSize = new System.Drawing.Size(528, 86);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(528, 129);
			this.panel1.TabIndex = 11;
			// 
			// labelTimeSpan
			// 
			this.labelTimeSpan.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.labelTimeSpan.AutoSize = true;
			this.labelTimeSpan.Location = new System.Drawing.Point(10, 36);
			this.labelTimeSpan.Name = "labelTimeSpan";
			this.labelTimeSpan.Size = new System.Drawing.Size(208, 13);
			this.labelTimeSpan.TabIndex = 15;
			this.labelTimeSpan.Text = "Time span from result to restart in seconds:";
			// 
			// timeSpanBox
			// 
			this.timeSpanBox.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.timeSpanBox.Location = new System.Drawing.Point(224, 33);
			this.timeSpanBox.Name = "timeSpanBox";
			this.timeSpanBox.Size = new System.Drawing.Size(96, 20);
			this.timeSpanBox.TabIndex = 14;
			this.timeSpanBox.WordWrap = false;
			this.timeSpanBox.TextChanged += new System.EventHandler(this.timerBoxValueChanged);
			// 
			// ImageSelectionBox
			// 
			this.ImageSelectionBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.ImageSelectionBox.FormattingEnabled = true;
			this.ImageSelectionBox.Items.AddRange(new object[] {
            "Image 1",
            "Image 2",
            "Image 3",
            "Image 4"});
			this.ImageSelectionBox.Location = new System.Drawing.Point(13, 12);
			this.ImageSelectionBox.Name = "ImageSelectionBox";
			this.ImageSelectionBox.Size = new System.Drawing.Size(506, 21);
			this.ImageSelectionBox.TabIndex = 13;
			this.ImageSelectionBox.Visible = false;
			this.ImageSelectionBox.SelectedIndexChanged += new System.EventHandler(this.ImageSelectionBox_SelectedIndexChanged);
			// 
			// ErrorLabel
			// 
			this.ErrorLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.ErrorLabel.AutoSize = true;
			this.ErrorLabel.Location = new System.Drawing.Point(10, 105);
			this.ErrorLabel.Name = "ErrorLabel";
			this.ErrorLabel.Size = new System.Drawing.Size(102, 13);
			this.ErrorLabel.TabIndex = 10;
			this.ErrorLabel.Text = "Error: 10/100 = 10%";
			// 
			// StartStopButton
			// 
			this.StartStopButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.StartStopButton.Location = new System.Drawing.Point(428, 82);
			this.StartStopButton.Name = "StartStopButton";
			this.StartStopButton.Size = new System.Drawing.Size(91, 36);
			this.StartStopButton.TabIndex = 9;
			this.StartStopButton.Text = "Start";
			this.StartStopButton.UseVisualStyleBackColor = true;
			this.StartStopButton.Click += new System.EventHandler(this.StartStopButton_Click);
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.ColumnCount = 1;
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel1.Controls.Add(this.tableLayoutPanel_ErrorMsg, 0, 0);
			this.tableLayoutPanel1.Controls.Add(this.inputPropertyGrid, 0, 1);
			this.tableLayoutPanel1.Controls.Add(this.panel1, 0, 2);
			this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.RowCount = 3;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 135F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
			this.tableLayoutPanel1.Size = new System.Drawing.Size(534, 601);
			this.tableLayoutPanel1.TabIndex = 10;
			// 
			// tableLayoutPanel_ErrorMsg
			// 
			this.tableLayoutPanel_ErrorMsg.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.tableLayoutPanel_ErrorMsg.ColumnCount = 2;
			this.tableLayoutPanel_ErrorMsg.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel_ErrorMsg.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 151F));
			this.tableLayoutPanel_ErrorMsg.Controls.Add(this.pictureBox1, 0, 0);
			this.tableLayoutPanel_ErrorMsg.Controls.Add(this.panel2, 1, 0);
			this.tableLayoutPanel_ErrorMsg.Location = new System.Drawing.Point(3, 3);
			this.tableLayoutPanel_ErrorMsg.Name = "tableLayoutPanel_ErrorMsg";
			this.tableLayoutPanel_ErrorMsg.RowCount = 1;
			this.tableLayoutPanel_ErrorMsg.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel_ErrorMsg.Size = new System.Drawing.Size(528, 227);
			this.tableLayoutPanel_ErrorMsg.TabIndex = 16;
			// 
			// pictureBox1
			// 
			this.pictureBox1.Anchor = System.Windows.Forms.AnchorStyles.None;
			this.pictureBox1.Image = global::SVRemoteTest.Properties.Resources.Chrysanthemum;
			this.pictureBox1.Location = new System.Drawing.Point(23, 33);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(330, 161);
			this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
			this.pictureBox1.TabIndex = 2;
			this.pictureBox1.TabStop = false;
			// 
			// panel2
			// 
			this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.panel2.Controls.Add(this.RejectButton4);
			this.panel2.Controls.Add(this.RejectButton5);
			this.panel2.Controls.Add(this.RejectButton3);
			this.panel2.Controls.Add(this.RejectButton2);
			this.panel2.Controls.Add(this.RejectButton1);
			this.panel2.Location = new System.Drawing.Point(380, 3);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(145, 221);
			this.panel2.TabIndex = 3;
			// 
			// RejectButton4
			// 
			this.RejectButton4.Location = new System.Drawing.Point(0, 93);
			this.RejectButton4.Name = "RejectButton4";
			this.RejectButton4.Size = new System.Drawing.Size(139, 23);
			this.RejectButton4.TabIndex = 4;
			this.RejectButton4.Text = "1000";
			this.RejectButton4.UseVisualStyleBackColor = true;
			this.RejectButton4.Visible = false;
			this.RejectButton4.Click += new System.EventHandler(this.RejectButton_Click);
			// 
			// RejectButton5
			// 
			this.RejectButton5.Location = new System.Drawing.Point(3, 122);
			this.RejectButton5.Name = "RejectButton5";
			this.RejectButton5.Size = new System.Drawing.Size(136, 23);
			this.RejectButton5.TabIndex = 3;
			this.RejectButton5.Text = "1000";
			this.RejectButton5.UseVisualStyleBackColor = true;
			this.RejectButton5.Visible = false;
			this.RejectButton5.Click += new System.EventHandler(this.RejectButton_Click);
			// 
			// RejectButton3
			// 
			this.RejectButton3.Location = new System.Drawing.Point(3, 64);
			this.RejectButton3.Name = "RejectButton3";
			this.RejectButton3.Size = new System.Drawing.Size(136, 23);
			this.RejectButton3.TabIndex = 2;
			this.RejectButton3.Text = "1000";
			this.RejectButton3.UseVisualStyleBackColor = true;
			this.RejectButton3.Visible = false;
			this.RejectButton3.Click += new System.EventHandler(this.RejectButton_Click);
			// 
			// RejectButton2
			// 
			this.RejectButton2.Location = new System.Drawing.Point(3, 35);
			this.RejectButton2.Name = "RejectButton2";
			this.RejectButton2.Size = new System.Drawing.Size(136, 23);
			this.RejectButton2.TabIndex = 1;
			this.RejectButton2.Text = "1000";
			this.RejectButton2.UseVisualStyleBackColor = true;
			this.RejectButton2.Visible = false;
			this.RejectButton2.Click += new System.EventHandler(this.RejectButton_Click);
			// 
			// RejectButton1
			// 
			this.RejectButton1.Location = new System.Drawing.Point(3, 6);
			this.RejectButton1.Name = "RejectButton1";
			this.RejectButton1.Size = new System.Drawing.Size(136, 23);
			this.RejectButton1.TabIndex = 0;
			this.RejectButton1.Text = "1000";
			this.RejectButton1.UseVisualStyleBackColor = true;
			this.RejectButton1.Visible = false;
			this.RejectButton1.Click += new System.EventHandler(this.RejectButton_Click);
			// 
			// ResetButton
			// 
			this.ResetButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.ResetButton.Location = new System.Drawing.Point(331, 82);
			this.ResetButton.Name = "ResetButton";
			this.ResetButton.Size = new System.Drawing.Size(91, 36);
			this.ResetButton.TabIndex = 19;
			this.ResetButton.Text = "Reset";
			this.ResetButton.UseVisualStyleBackColor = true;
			this.ResetButton.Click += new System.EventHandler(this.ResetButton_Click);
			// 
			// inputPropertyGrid
			// 
			this.inputPropertyGrid.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.inputPropertyGrid.HelpVisible = false;
			this.inputPropertyGrid.Location = new System.Drawing.Point(3, 236);
			this.inputPropertyGrid.Name = "inputPropertyGrid";
			this.inputPropertyGrid.PropertySort = System.Windows.Forms.PropertySort.Categorized;
			this.inputPropertyGrid.Size = new System.Drawing.Size(528, 227);
			this.inputPropertyGrid.TabIndex = 10;
			this.inputPropertyGrid.ToolbarVisible = false;
			// 
			// UseRemoteOutputsCheckBox
			// 
			this.UseRemoteOutputsCheckBox.AutoSize = true;
			this.UseRemoteOutputsCheckBox.Location = new System.Drawing.Point(159, 101);
			this.UseRemoteOutputsCheckBox.Name = "UseRemoteOutputsCheckBox";
			this.UseRemoteOutputsCheckBox.Size = new System.Drawing.Size(125, 17);
			this.UseRemoteOutputsCheckBox.TabIndex = 16;
			this.UseRemoteOutputsCheckBox.Text = "Use Remote Outputs";
			this.UseRemoteOutputsCheckBox.UseVisualStyleBackColor = true;
			// 
			// LoadAllImagesCheckbox
			// 
			this.LoadAllImagesCheckbox.AutoSize = true;
			this.LoadAllImagesCheckbox.Location = new System.Drawing.Point(13, 68);
			this.LoadAllImagesCheckbox.Name = "LoadAllImagesCheckbox";
			this.LoadAllImagesCheckbox.Size = new System.Drawing.Size(99, 17);
			this.LoadAllImagesCheckbox.TabIndex = 17;
			this.LoadAllImagesCheckbox.Text = "Load all images";
			this.LoadAllImagesCheckbox.UseVisualStyleBackColor = true;
			// 
			// IsRunLoopCheckbox
			// 
			this.IsRunLoopCheckbox.AutoSize = true;
			this.IsRunLoopCheckbox.Location = new System.Drawing.Point(331, 36);
			this.IsRunLoopCheckbox.Name = "IsRunLoopCheckbox";
			this.IsRunLoopCheckbox.Size = new System.Drawing.Size(108, 17);
			this.IsRunLoopCheckbox.TabIndex = 20;
			this.IsRunLoopCheckbox.Text = "Number Off Runs";
			this.IsRunLoopCheckbox.UseVisualStyleBackColor = true;
			this.IsRunLoopCheckbox.CheckedChanged += new System.EventHandler(this.IsRunLoopCheckbox_CheckedChanged);
			// 
			// NumberOfRunsTextBox
			// 
			this.NumberOfRunsTextBox.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.NumberOfRunsTextBox.Location = new System.Drawing.Point(436, 36);
			this.NumberOfRunsTextBox.Name = "NumberOfRunsTextBox";
			this.NumberOfRunsTextBox.Size = new System.Drawing.Size(72, 20);
			this.NumberOfRunsTextBox.TabIndex = 21;
			this.NumberOfRunsTextBox.Text = "10";
			this.NumberOfRunsTextBox.WordWrap = false;
			this.NumberOfRunsTextBox.TextChanged += new System.EventHandler(this.NumberOfRunsTextBox_TextChanged);
			// 
			// ContinualResultForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(534, 601);
			this.ControlBox = false;
			this.Controls.Add(this.tableLayoutPanel1);
			this.Name = "ContinualResultForm";
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.tableLayoutPanel1.ResumeLayout(false);
			this.tableLayoutPanel_ErrorMsg.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			this.panel2.ResumeLayout(false);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label labelTimeSpan;
        private System.Windows.Forms.TextBox timeSpanBox;
		private System.Windows.Forms.ComboBox ImageSelectionBox;
        private System.Windows.Forms.Label ErrorLabel;
        private System.Windows.Forms.Button StartStopButton;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel_ErrorMsg;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Button RejectButton4;
        private System.Windows.Forms.Button RejectButton5;
        private System.Windows.Forms.Button RejectButton3;
        private System.Windows.Forms.Button RejectButton2;
        private System.Windows.Forms.Button RejectButton1;
        private SVPropertyGrid inputPropertyGrid;
        private System.Windows.Forms.CheckBox UseRemoteOutputsCheckBox;
		private System.Windows.Forms.CheckBox LoadAllImagesCheckbox;
		private System.Windows.Forms.Button ResetButton;
		private System.Windows.Forms.TextBox NumberOfRunsTextBox;
		private System.Windows.Forms.CheckBox IsRunLoopCheckbox;


    }
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\ContinualResult.Designer.cs_v  $
 * 
 *    Rev 1.4   11 Nov 2014 11:08:30   ryoho
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 17
 * SCR Title: Enhance SVRC Test App for Features for Testing Run/Reject Server
 * Checked in by: sJones; Steve Jones
 * Change Description: changed size and position of the buttons for Rejects
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.3   10 Nov 2014 11:18:24   ryoho
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 18
 * SCR Title: Enhance the Continual Results Page for GetReject in the SVRC Test App
 * Checked in by: sJones; Steve Jones
 * Change Description: changed button size for the rejects
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.2   04 Nov 2014 16:18:22   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised to use SVPropertyGrid.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   08 Sep 2014 18:32:30   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Removed ImageLabel and Lablel1
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   14 Jul 2014 10:29:16   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: initilized check in
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/