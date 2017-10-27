//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataDefinition
//* .File Name       : $Workfile:   SVDataDefinition.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Nov 2013 19:22:46  $
//******************************************************************************

using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Drawing.Design;

namespace SVRemoteTest
{
	[TypeConverter(typeof(ExpandableObjectConverter))]
    public class SVDataDefinition
	{
		private String  m_Name = "";
        private Boolean m_Writable;
        private Boolean m_Published;
        private String  m_Type = "";
        private List<String> m_AdditionalInfo = new List<String>();
		
		public String Name
		{
			get { return m_Name; }
			set { m_Name = value; }
		}

        public Boolean Writable
        {
            get { return m_Writable; }
            set { m_Writable = value; }
        }

        public Boolean Published
        {
            get { return m_Published; }
            set { m_Published = value; }
        }

        public String Type
        {
            get { return m_Type; }
            set { m_Type = value; }
        }
        
        [Editor(@"System.Windows.Forms.Design.StringCollectionEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", typeof(UITypeEditor))]
		public List<String> AdditionalInfo
		{
            get { return m_AdditionalInfo; }
            set { m_AdditionalInfo = value; }
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVDataDefinition.cs_v  $
 * 
 *    Rev 1.0   07 Nov 2013 19:22:46   bwalter
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 9
 * SCR Title: Add GetDataDefinitionList Command to SVRemoteControl
 * Checked in by: bWalter; Ben Walter
 * Change Description: Initial check in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
