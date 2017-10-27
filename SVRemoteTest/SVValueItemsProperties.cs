//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueItemsProperties
//* .File Name       : $Workfile:   SVValueItemsProperties.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   27 Aug 2014 10:30:36  $
//******************************************************************************
using System;
using System.ComponentModel;
using System.Collections;
using System.Collections.Generic;

namespace SVRemoteTest
{
	[TypeConverter(typeof(SVProductItemsTypeConverter))]
	public class SVValueItemsProperties
	{
		#region Member Variables
        private List<SVValue> m_values = new List<SVValue>();
		#endregion Member Variables		
		
		#region Properties
        public List<SVValue> Values
		{
			get { return m_values; }
			set { m_values = value; }
		}
		#endregion Properties
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVValueItemsProperties.cs_v  $
 * 
 *    Rev 1.0   27 Aug 2014 10:30:36   mziegler
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 10
 * SCR Title: Add reject/run page server connection and commands
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial Checkin
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
*/

