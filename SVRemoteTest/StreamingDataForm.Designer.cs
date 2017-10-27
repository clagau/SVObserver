//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : StreamingDataForm
//* .File Name       : $Workfile:   StreamingDataForm.Designer.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:42:12  $
//******************************************************************************
namespace SVRemoteTest
{
	partial class StreamingDataForm
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
			this.components = new System.ComponentModel.Container();
			this.DataListView = new System.Windows.Forms.ListView();
			this.NameColumnHeader = new System.Windows.Forms.ColumnHeader();
			this.ValueColumnHeader = new System.Windows.Forms.ColumnHeader();
			this.TriggerCountColumnHeader = new System.Windows.Forms.ColumnHeader();
			this.StatusColumnHeader = new System.Windows.Forms.ColumnHeader();
			this.contextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.clearMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.contextMenuStrip.SuspendLayout();
			this.SuspendLayout();
			// 
			// DataListView
			// 
			this.DataListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.NameColumnHeader,
            this.ValueColumnHeader,
            this.TriggerCountColumnHeader,
            this.StatusColumnHeader});
			this.DataListView.ContextMenuStrip = this.contextMenuStrip;
			this.DataListView.Dock = System.Windows.Forms.DockStyle.Fill;
			this.DataListView.FullRowSelect = true;
			this.DataListView.GridLines = true;
			this.DataListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
			this.DataListView.Location = new System.Drawing.Point(0, 0);
			this.DataListView.Name = "DataListView";
			this.DataListView.Size = new System.Drawing.Size(769, 480);
			this.DataListView.TabIndex = 0;
			this.DataListView.UseCompatibleStateImageBehavior = false;
			this.DataListView.View = System.Windows.Forms.View.Details;
			// 
			// NameColumnHeader
			// 
			this.NameColumnHeader.Text = "Name";
			this.NameColumnHeader.Width = 398;
			// 
			// ValueColumnHeader
			// 
			this.ValueColumnHeader.Text = "Value";
			this.ValueColumnHeader.Width = 205;
			// 
			// TriggerCountColumnHeader
			// 
			this.TriggerCountColumnHeader.Text = "Trigger Count";
			this.TriggerCountColumnHeader.Width = 101;
			// 
			// StatusColumnHeader
			// 
			this.StatusColumnHeader.Text = "Status";
			// 
			// contextMenuStrip
			// 
			this.contextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.clearMenuItem});
			this.contextMenuStrip.Name = "contextMenuStrip";
			this.contextMenuStrip.Size = new System.Drawing.Size(111, 26);
			this.contextMenuStrip.ItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.contextMenuStrip_ItemClicked);
			// 
			// clearMenuItem
			// 
			this.clearMenuItem.Name = "clearMenuItem";
			this.clearMenuItem.Size = new System.Drawing.Size(110, 22);
			this.clearMenuItem.Text = "Clear";
			// 
			// StreamingDataForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(769, 480);
			this.Controls.Add(this.DataListView);
			this.Name = "StreamingDataForm";
			this.ShowIcon = false;
			this.Text = "Streaming Data";
			this.WindowState = System.Windows.Forms.FormWindowState.Minimized;
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.StreamingDataForm_FormClosing);
			this.contextMenuStrip.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.ListView DataListView;
		private System.Windows.Forms.ColumnHeader NameColumnHeader;
		private System.Windows.Forms.ColumnHeader ValueColumnHeader;
		private System.Windows.Forms.ContextMenuStrip contextMenuStrip;
		private System.Windows.Forms.ToolStripMenuItem clearMenuItem;
		private System.Windows.Forms.ColumnHeader TriggerCountColumnHeader;
		private System.Windows.Forms.ColumnHeader StatusColumnHeader;
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVREMOTECONTROL\SVRemoteTest\StreamingDataForm.Designer.cs_v  $
 * 
 *    Rev 1.0   24 Apr 2012 15:42:12   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/