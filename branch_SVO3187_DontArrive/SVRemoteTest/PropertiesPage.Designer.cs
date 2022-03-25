//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PropertiesPage
//* .File Name       : $Workfile:   PropertiesPage.Designer.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   04 Nov 2014 16:18:22  $
//******************************************************************************
namespace SVRemoteTest
{
    partial class PropertiesPage
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
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.comObjectPropertyGrid = new System.Windows.Forms.PropertyGrid();
            this.inputPropertyGrid = new SVRemoteTest.SVPropertyGrid();
            this.tableLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 1;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Controls.Add(this.comObjectPropertyGrid, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.inputPropertyGrid, 0, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 180F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(507, 425);
            this.tableLayoutPanel1.TabIndex = 5;
            // 
            // comObjectPropertyGrid
            // 
            this.comObjectPropertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comObjectPropertyGrid.Location = new System.Drawing.Point(3, 248);
            this.comObjectPropertyGrid.Name = "comObjectPropertyGrid";
            this.comObjectPropertyGrid.PropertySort = System.Windows.Forms.PropertySort.Categorized;
            this.comObjectPropertyGrid.Size = new System.Drawing.Size(501, 174);
            this.comObjectPropertyGrid.TabIndex = 5;
            this.comObjectPropertyGrid.ToolbarVisible = false;
            // 
            // inputPropertyGrid
            // 
            this.inputPropertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.inputPropertyGrid.Location = new System.Drawing.Point(3, 3);
            this.inputPropertyGrid.Name = "inputPropertyGrid";
            this.inputPropertyGrid.PropertySort = System.Windows.Forms.PropertySort.Categorized;
            this.inputPropertyGrid.Size = new System.Drawing.Size(501, 239);
            this.inputPropertyGrid.TabIndex = 3;
            this.inputPropertyGrid.ToolbarVisible = false;
            // 
            // PropertiesPage
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.ClientSize = new System.Drawing.Size(507, 425);
            this.ControlBox = false;
            this.Controls.Add(this.tableLayoutPanel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "PropertiesPage";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.tableLayoutPanel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.PropertyGrid comObjectPropertyGrid;
        private SVPropertyGrid inputPropertyGrid;

    }
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\PropertiesPage.Designer.cs_v  $
 * 
 *    Rev 1.1   04 Nov 2014 16:18:22   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised to use SVPropertyGrid.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   14 Jul 2014 10:29:20   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: initilized check in
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/