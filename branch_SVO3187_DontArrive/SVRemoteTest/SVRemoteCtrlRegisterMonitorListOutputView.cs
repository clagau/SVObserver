//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCtrlRegisterMonitorListOutputView
//* .File Name       : $Workfile:   SVRemoteCtrlRegisterMonitorListOutputView.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Jan 2015 10:42:42  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Design;
using System.Drawing;
using System.Drawing.Design;
using System.Reflection;
using SeidenaderVision;

namespace SVRemoteTest
{
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class SVRemoteCtrlRegisterMonitorListOutputView : ISVComObjectView
    {
        #region Member Variables
        private String m_ppqName;
        private int m_rejectDepth = 10; // Default value is 10
        private List<String> m_productCondList = new List<String>();
        private List<String> m_RejectedList = new List<String>();
        private List<String> m_failStatusList = new List<String>();
        private List<SVError> m_errorFaultItems = new List<SVError>();
        #endregion

        #region Properties
        [Category("Input Parameters")]
        [Description("Name of the Monitor List")]
		public String ListName
		{
            get { return SVRemoteTestHelper.MonitorListName; }
            set { SVRemoteTestHelper.MonitorListName = value; }
		}

        [Category("Input Parameters")]
        [Description("Name of PPQ")]
        public String PPQName
        {
            get { return m_ppqName; }
            set { m_ppqName = value; }
        }

        [Category("Input Parameters")]
        [Description("Reject Depth")]
        public int RejectDepth
        {
            get { return m_rejectDepth; }
            set { m_rejectDepth = value; }
        }

        [Category("Input Parameters")]
        [Editor(@"System.Windows.Forms.Design.StringCollectionEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", typeof(UITypeEditor))]
        [Description("Product Conditional List")]
        public List<String> ProductCondList
        {
            get { return m_productCondList; }
            set { m_productCondList = value; }
        }

        [Category("Input Parameters")]
        [Editor(@"System.Windows.Forms.Design.StringCollectionEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", typeof(UITypeEditor))]
        [Description("Rejected List")]
        public List<String> RejectedList
        {
            get { return m_RejectedList; }
            set { m_RejectedList = value; }
        }

        [Category("Input Parameters")]
        [Editor(@"System.Windows.Forms.Design.StringCollectionEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", typeof(UITypeEditor))]
        [Description("Fail Status List")]
        public List<String> FailStatusList
        {
            get { return m_failStatusList; }
            set { m_failStatusList = value; }
        }

        [Category("Error Results")]
        [DisplayName("Faults")]
        [Description("List of SVErrorObjects")]
        [ReadOnly(true)]
        public List<SVError> Errors
        {
            get { return m_errorFaultItems; }
            set { m_errorFaultItems = value; }
        }
        #endregion

        #region Interface Implementation
        public void GetPropertyValues(List<object> items)
		{
            items.Add(ListName);
            items.Add(m_ppqName);
            items.Add(m_rejectDepth);
            items.Add(m_productCondList.ToArray());
            items.Add(m_RejectedList.ToArray());
            items.Add(m_failStatusList.ToArray());
		}

		public void SetOutput(String name, object value)
		{
		}

		public void SetResult(object value)
		{
            m_errorFaultItems.Clear();
			// Copy from Com to .Net Collection
            SVRemoteControlLib.ISVErrorObjectList errorItems = value as SVRemoteControlLib.ISVErrorObjectList;
            foreach (SVRemoteControlLib.ISVErrorObject errorObj in errorItems)
            {
                SVError e = new SVError();
                e.Name = errorObj.Name;
                e.Status = errorObj.Status;
                // Update Error Collection
                m_errorFaultItems.Add(e);
            }
        }
        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVRemoteCtrlRegisterMonitorListOutputView.cs_v  $
 * 
 *    Rev 1.2   16 Jan 2015 10:42:42   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 21
 * SCR Title: Add Fail Status Streaming
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised the Errors property to be read only
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.1   28 Oct 2014 11:54:26   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Revised ListName property to use referenec the MonitorList name in SVRemoteTestHelper
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   27 Aug 2014 10:30:40   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial Checkin
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
