//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCtrlGetFailStatusView
//* .File Name       : $Workfile:   SVRemoteCtrlGetFailStatusView.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   28 Oct 2014 11:54:26  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using SeidenaderVision;

namespace SVRemoteTest
{
	[TypeConverter(typeof(ExpandableObjectConverter))]
    public class SVRemoteCtrlGetFailStatusView : ISVComObjectView
    {
        #region Member Variables
        private SVValueItemsProperties m_returnValue = new SVValueItemsProperties();
        #endregion

        #region Properties
        [Category("Input Parameters")]
		[Description("Name of Monitor List")]
        public String ListName
		{
            get { return SVRemoteTestHelper.MonitorListName; }
            set { SVRemoteTestHelper.MonitorListName = value; }
		}

		[Category("Results")]
		[DisplayName("Return Value")]
		[Description("Result of operation")]
		[TypeConverter(typeof(SVProductItemsTypeConverter))]
		[ReadOnly(true)]
        public SVValueItemsProperties ReturnValue
		{
			get { return m_returnValue; }
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

		public void SetResult(object values)
		{
			// Copy from Com to .Net Collection
            object[] objectList = values as object[];
            m_returnValue = new SVValueItemsProperties();
            List<SVValue> valueList = new List<SVValue>();

            foreach (object value in objectList)
            {
                if (null != value)
                {
                    SVRemoteControlLib.ISVValueObjectList valueObjects = value as SVRemoteControlLib.ISVValueObjectList;
                    
                    foreach (SVRemoteControlLib.ISVValueObject val in valueObjects)
                    {
                        SVValue valueObj = new SVValue();
                        valueObj.Name = val.Name;
                        valueObj.TriggerCount = val.TriggerCount;
                        valueObj.Status = val.Status;

                        foreach (object o in val)
                        {
                            valueObj.Array.Add(o.ToString());
                        }
                        valueList.Add(valueObj);
                    }
                }
            }
            
            m_returnValue.Values = valueList;
        }
        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVRemoteCtrlGetFailStatusView.cs_v  $
 * 
 *    Rev 1.1   28 Oct 2014 11:54:26   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised ListName property to use referenec the MonitorList name in SVRemoteTestHelper
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   27 Aug 2014 10:30:38   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial Checkin
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
*/
