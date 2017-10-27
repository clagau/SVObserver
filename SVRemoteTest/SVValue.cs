//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValue
//* .File Name       : $Workfile:   SVValue.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   04 Nov 2014 16:07:30  $
//******************************************************************************
using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Drawing.Design;

namespace SVRemoteTest
{
	[TypeConverter(typeof(ExpandableObjectConverter))]
	public class SVValue
	{
		private String m_name = "";
		private Int32 m_Status;
		private Int32 m_TriggerCount;
		private List<String> m_Array = new List<String>();

        [Category("Input Parameters")]
		public String Name
		{
			get { return m_name; }
			set { m_name = value; }
		}

		public Int32 Status
		{
			get { return m_Status; }
			set { m_Status = value; }
		}

		public Int32 TriggerCount
		{
			get { return m_TriggerCount; }
			set { m_TriggerCount = value; }
		}
/*
		public Int32 ArraySize
		{
			get { return m_Array.Count; }
		}
*/
        [Category("Input Parameters")]
		[Editor(@"System.Windows.Forms.Design.StringCollectionEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", typeof(UITypeEditor))]
		public List<String> Array
		{
			get { return m_Array; }
			set { m_Array = value; }
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVValue.cs_v  $
 * 
 *    Rev 1.1   04 Nov 2014 16:07:30   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Added Category attribute tag of Input Parameters to aid in persistence.
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

