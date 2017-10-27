//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCtrlStringInputVariantStringArrayOutputView
//* .File Name       : $Workfile:   SVRemoteCtrlStringInputVariantStringArrayOutputView.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   28 Oct 2014 11:54:26  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Design;
using System.Drawing;
using System.Drawing.Design;
using SeidenaderVision;

namespace SVRemoteTest
{
    [TypeConverter(typeof(ExpandableObjectConverter))]
	public class SVRemoteCtrlStringInputVariantStringArrayOutputView : ISVComObjectView
    {
        #region Member Variables
        private List<String> m_returnValue = new List<String>();
        #endregion

        #region Properties
        [Category("Input Parameters")]
		[Description("Name of the Monitor List")]
		public String ListName
		{
			get { return SVRemoteTestHelper.MonitorListName; }
            set { SVRemoteTestHelper.MonitorListName = value; }
		}

		[Category("Results")]
        [Editor(@"System.Windows.Forms.Design.StringCollectionEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", typeof(UITypeEditor))]
		[DisplayName("Return Value")]
		[Description("Result of operation")]
		[ReadOnly(true)]
		public List<String> ReturnValue
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

		public void SetResult(object value)
		{
            m_returnValue.Clear();
			// Copy from Com to .Net Collection
            String[] items = value as String[];
            if (items != null)
            {
                foreach (String name in items)
                {
                    m_returnValue.Add(name);
                }
            }
        }
        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVRemoteCtrlStringInputVariantStringArrayOutputView.cs_v  $
 * 
 *    Rev 1.1   28 Oct 2014 11:54:26   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised ListName property to use referenec the MonitorList name in SVRemoteTestHelper
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   14 Jul 2014 10:30:56   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: rename from SVRemoteCtrlVariantStringArrayView.cs
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   24 Apr 2014 13:53:42   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial checkin.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
