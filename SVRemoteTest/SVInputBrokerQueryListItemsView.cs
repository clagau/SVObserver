//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputBrokerQueryListItemsView
//* .File Name       : $Workfile:   SVInputBrokerQueryListItemsView.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   28 Oct 2014 11:52:40  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Design;
using SeidenaderVision;

namespace SVRemoteTest
{
	[TypeConverter(typeof(ExpandableObjectConverter))]
	public class SVInputBrokerQueryListItemsView : ISVComObjectView
    {
        #region Member Variables
        private String m_listName = "";
		private List<SVInputMatchItemWrapper> m_items = new List<SVInputMatchItemWrapper>();
        #endregion

        #region Properties
        [Category("Input Parameters")]
		[DisplayName("List Name")]
		[Description("name of the List")]
		public String ListName
		{
			get { return m_listName; }
			set { m_listName = value; }
		}

		[Category("Results")]
		[DisplayName("Items")]
		[Description("List of SVInputMatchItem")]
		[ReadOnly(true)]
		public List<SVInputMatchItemWrapper> Items
		{
			get { return m_items; }
			set { m_items = value; }
        }
        #endregion

        #region Interface Implementation
        public void GetPropertyValues(List<object> items)
		{
			items.Add(ListName);
		}

		public void SetOutput(String name, object value)
		{
		}

		public void SetResult(object value)
		{
			SVRemoteControlLib.ISVInputMatchList list = value as SVRemoteControlLib.ISVInputMatchList;
			m_items = new List<SVInputMatchItemWrapper>();
			foreach (SVRemoteControlLib.ISVInputMatchItem item in list)
			{
				SVInputMatchItemWrapper v = new SVInputMatchItemWrapper();
				v.MatchString = item.MatchString;
				v.TransactionId = item.TxnIdentifier;
				m_items.Add(v);
			}
        }
        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVInputBrokerQueryListItemsView.cs_v  $
 * 
 *    Rev 1.1   28 Oct 2014 11:52:40   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised Category name for Input Parameters
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   24 Apr 2012 15:42:38   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

