//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCtrlNoInputVariantStringArrayOutputView
//* .File Name       : $Workfile:   SVRemoteCtrlNoInputVariantStringArrayOutputView.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Jul 2014 10:29:20  $
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
	public class SVRemoteCtrlNoInputVariantStringArrayOutputView : ISVComObjectView
    {
        #region Member Variables
        private List<String> m_returnValue = new List<String>();
        #endregion

        #region Properties
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVRemoteCtrlNoInputVariantStringArrayOutputView.cs_v  $
 * 
 *    Rev 1.0   14 Jul 2014 10:29:20   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: initilized check in
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
