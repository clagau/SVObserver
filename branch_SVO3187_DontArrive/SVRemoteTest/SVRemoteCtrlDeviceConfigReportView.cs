//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCtrlDeviceConfigReportView
//* .File Name       : $Workfile:   SVRemoteCtrlDeviceConfigReportView.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:42:38  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Design;
using System.Drawing.Design;
using System.Reflection;

namespace SVRemoteTest
{
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class SVRemoteCtrlDeviceConfigReportView : ISVComObjectView
    {
        #region Member Variables
        private String m_report;
        #endregion

        #region Properties
        [Category("Results")]
        [Editor(typeof(MultilineStringEditor), typeof(UITypeEditor))]
        [Description("Configuration Report")]
        //[ReadOnly(true)] // Doesn't work with the MultilineStringEditor
        public String Report
        {
            get { return m_report; }
            set { m_report = value; }
        }
        #endregion

        #region Interface Implementation
        public void GetPropertyValues(List<object> items)
		{
		}

		public void SetOutput(String name, object value)
		{
		}

        public void SetResult(object value)
        {
            m_report = value.ToString();
        }
        #endregion
    }
}
