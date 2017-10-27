//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : StreamingDataForm
//* .File Name       : $Workfile:   StreamingDataForm.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Jan 2015 10:46:06  $
//******************************************************************************
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SVRemoteTest
{
	public partial class StreamingDataForm : Form
	{
		private Int32 m_count = 0;
        private static readonly String failStatusGroupLabel = "Fail Status";
        private static readonly String outputBrokerGroupLabel = "Output Broker";
        private ListViewGroup failStatusGroup = new ListViewGroup(failStatusGroupLabel, HorizontalAlignment.Left);
        private ListViewGroup outputBrokerGroup = new ListViewGroup(outputBrokerGroupLabel, HorizontalAlignment.Left);
        private SortedSet<string> masterFailStatusList = new SortedSet<string>();

        public String FailStatusGroupTag
        {
            get { return failStatusGroupLabel; }
        }

        public String OutputBrokerGroupTag
        {
            get { return outputBrokerGroupLabel; }
        }

		public StreamingDataForm()
		{
			InitializeComponent();
            // Add Groups
            DataListView.Groups.Add(failStatusGroup);
            DataListView.Groups.Add(outputBrokerGroup);
		}

        private ListViewItem FindExactName(string name, ListViewGroup group)
        {
            foreach(ListViewItem item in DataListView.Items)
            {
                if (item.Text == name && item.Group == group)
                {
                    return item;
                }
            }
            return null;
        }

        // Call this when a FailStatus stream is started
        public void AddFailStatusList(List<string> list)
        {
            lock (masterFailStatusList)
            {
                foreach (string s in list)
                {
                    if (!masterFailStatusList.Contains(s))
                    {
                        masterFailStatusList.Add(s);
                    }
                }
            }
        }
        
        // Call this when a FailStatus stream is started
        public void RemoveFailStatusList(List<string> list)
        {
            lock (masterFailStatusList)
            {
                foreach (string s in list)
                {
                    if (masterFailStatusList.Contains(s))
                    {
                        masterFailStatusList.Remove(s);
                    }
                }
            }
        }

        #region AddToList
        /// <summary>
        /// Add to Master List
        /// </summary>
        /// <param name="name"></param>
        /// <param name="group"></param>
        private void AddToList(string name, ListViewGroup group)
        {
            if (group == failStatusGroup)
            {
                lock (masterFailStatusList)
                {
                    if (!masterFailStatusList.Contains(name))
                    {
                        masterFailStatusList.Add(name);
                    }
                }
            }
        }
        #endregion

        private delegate void UpdateStreamingDataDel(List<SVValue> values, String label); 
		public void UpdateStreamingData(List<SVValue> values, String label)
		{
			if (InvokeRequired)
			{
				UpdateStreamingDataDel del = new UpdateStreamingDataDel(UpdateStreamingData);
				Invoke(del, new object[] { values, label });
			}
			else
			{
				String title = "Streaming Data - Packets Recv'd " + ++m_count;
				this.Text = title;

                ListViewGroup group = outputBrokerGroup;
                if (label == failStatusGroupLabel)
                {
                    group = failStatusGroup;
                }
				foreach (SVValue v in values)
				{
					ListViewItem item = FindExactName(v.Name, group);

					if (null == item)
					{
                        ListViewItem newItem = new ListViewItem();
                        newItem.Text = v.Name;
                        newItem.SubItems.Add(SVArrayPresenter.ToString(v.Array.ToArray()));
                        newItem.SubItems.Add(v.TriggerCount.ToString());
                        newItem.SubItems.Add(v.Status.ToString());
                        newItem.Group = group;
                        DataListView.Items.Add(newItem);
                        AddToList(v.Name, group);
					}
					else
					{
                        item.SubItems[1].Text = SVArrayPresenter.ToString(v.Array.ToArray());
                        item.SubItems[2].Text = v.TriggerCount.ToString();
                        item.SubItems[3].Text = v.Status.ToString();
                        DataListView.Update();
					}
				}
                if (label == failStatusGroupLabel)
                {
                    lock (masterFailStatusList)
                    {
                        // Remove those items not in Master FailStatus list
                        // leave the output broker items...
                        foreach (ListViewItem item in DataListView.Items)
                        {
                            if (!masterFailStatusList.Contains(item.Text) && item.Group == failStatusGroup)
                            {
                                // remove item 
                                DataListView.Items.Remove(item);
                            }
                        }
                    }
                }
                else
                {
                    // Remove those items in the DataListView not in values
                    // leave the fail status items...
                    foreach (ListViewItem item in DataListView.Items)
                    {
                        if (values.Find(delegate(SVValue v) { return v.Name == item.Text; }) == null)
                        {
                            if (item.Group == outputBrokerGroup)
                            {
                                // remove item 
                                DataListView.Items.Remove(item);
                            }
                        }
                    }
                }
            }
            values.Clear();
		}

		private void StreamingDataForm_FormClosing(object sender, FormClosingEventArgs e)
		{
			if (e.CloseReason == CloseReason.UserClosing)
			{
				e.Cancel = true;
			}
		}

		private void contextMenuStrip_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
		{
			if (e.ClickedItem == clearMenuItem)
			{
				DataListView.Items.Clear();
			}
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\StreamingDataForm.cs_v  $
 * 
 *    Rev 1.2   16 Jan 2015 10:46:06   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 21
 * SCR Title: Add Fail Status Streaming
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised to use List Groups
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   17 Oct 2014 14:18:50   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 17
 * SCR Title: Enhance SVRC Test App for Features for Testing Run/Reject Server
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised UpdateStreamingData to prune the list to be dispalyed of items no longer present.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   24 Apr 2012 15:42:12   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/