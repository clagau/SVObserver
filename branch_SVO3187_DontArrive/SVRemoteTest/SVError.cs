//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVError
//* .File Name       : $Workfile:   SVError.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 May 2013 17:29:00  $
//******************************************************************************
using System;
using System.ComponentModel;

namespace SVRemoteTest
{
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class SVError
    {
        private String m_name = "";
        private Int32 m_Status;

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
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVError.cs_v  $
 * 
 *    Rev 1.0   14 May 2013 17:29:00   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 4
 * SCR Title: Changes for 2.01 version
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial Check-In
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
