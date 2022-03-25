//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProductItemsProperties
//* .File Name       : $Workfile:   SVProductItemsProperties.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:37:06  $
//******************************************************************************
using System;
using System.ComponentModel;
using System.Collections;
using System.Collections.Generic;

namespace SVRemoteTest
{
	[TypeConverter(typeof(SVProductItemsTypeConverter))]
	public class SVProductItemsProperties
	{
		private List<SVValue> m_values = new List<SVValue>();
        private List<SVImage> m_images = new List<SVImage>();
        private List<SVError> m_errors = new List<SVError>();
		
		public List<SVValue> Values
		{
			get { return m_values; }
			set { m_values = value; }
		}

        public List<SVImage> Images
        {
            get { return m_images; }
            set { m_images = value; }
        }

        public List<SVError> Errors
        {
            get { return m_errors; }
            set { m_errors = value; }
        }
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVProductItemsProperties.cs_v  $
 * 
 *    Rev 1.1   14 May 2013 17:37:06   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 4
 * SCR Title: Changes for 2.01 version
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: 
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   24 Apr 2012 15:42:38   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

