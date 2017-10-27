//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputBrokerAddItemView
//* .File Name       : $Workfile:   SVInputBrokerAddItemView.cs  $
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
	public class SVInputBrokerAddItemView : ISVComObjectView
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

		[Category("Input Parameters")]
		[DisplayName("Items")]
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
			SVRemoteControlLib.SVInputMatchListClass itemList = new SVRemoteControlLib.SVInputMatchListClass();
			foreach (SVInputMatchItemWrapper itemW in m_items)
			{
				SVRemoteControlLib.SVInputMatchItemClass item = new SVRemoteControlLib.SVInputMatchItemClass();
				item.MatchString = itemW.MatchString;
				item.TxnIdentifier = itemW.TransactionId;
				itemList.Add(item);
			}
			items.Add(itemList);
		}

		public void SetOutput(String name, object value)
		{
		}

		public void SetResult(object value)
		{
        }
        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVInputBrokerAddItemView.cs_v  $
 * 
 *    Rev 1.1   28 Oct 2014 11:52:40   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised Category name for Input Parameters
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   24 Apr 2012 15:42:36   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

