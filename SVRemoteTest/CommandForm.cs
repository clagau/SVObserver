//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CommandForm
//* .File Name       : $Workfile:   CommandForm.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   24 Apr 2014 16:18:36  $
//******************************************************************************
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Reflection;

namespace SVRemoteTest
{
	public partial class CommandForm : Form
	{
        private List<MethodInfo> m_MethodList = new List<MethodInfo>();

		#region Selected Item Event and Delegate
		public delegate void CommandSelectedEventDelegate(object sender, EventArgs e);
		public event CommandSelectedEventDelegate m_CommandEvent;
		#endregion

		#region Constructor
		public CommandForm()
		{
			InitializeComponent();
		}
		#endregion

        private static int CompareMethodInfo(MethodInfo info1, MethodInfo info2)
        {
            int cmp = info1.Name.CompareTo(info2.Name);
            if (cmp == 0)
            {
                // check args and sort acordingly
                cmp = (info1.GetParameters().Length - info2.GetParameters().Length);
            }
            return cmp;
        }

		#region SetCommandList
		public void SetCommandList(Type type)
		{
			MethodInfo[] methods = type.GetMethods(BindingFlags.Instance | BindingFlags.Public | BindingFlags.DeclaredOnly);

			foreach (MethodInfo mf in methods)
			{
				// filter out getter/setters (properties)
				// filter out event (Advisor)
				if (!mf.Name.StartsWith("get_") && !mf.Name.StartsWith("set_") && !mf.Name.StartsWith("add_") && !mf.Name.StartsWith("remove_"))
				{
                    m_MethodList.Add(mf);
                }
			}
            m_MethodList.Sort(CompareMethodInfo);

            commandListBox.DataSource = m_MethodList;
            commandListBox.DisplayMember = "Name";
            commandListBox.ValueMember = "";
            commandListBox.SelectedIndex = -1;
        }
		#endregion

		#region SelectedItem
		public MethodInfo SelectedItem
		{
			get
			{
				MethodInfo selection = null;
				if (commandListBox.SelectedItem != null)
				{
                    int index = commandListBox.SelectedIndex;
                    selection = m_MethodList[index];
				}
				return selection;
			}
		}
		#endregion

		#region commandListBox SelectedIndexChanged handler
		private void commandListBox_SelectedIndexChanged(object sender, EventArgs e)
		{
            if (m_CommandEvent != null)
            {
                m_CommandEvent.Invoke(sender, e);
            }
		}
		#endregion
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\CommandForm.cs_v  $
 * 
 *    Rev 1.2   24 Apr 2014 16:18:36   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Revised SetCommandList to sort the MethodInfos.
 * Added CompareMethodInfo method.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   14 May 2013 17:38:44   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 4
 * SCR Title: Changes for 2.01 version
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: 
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