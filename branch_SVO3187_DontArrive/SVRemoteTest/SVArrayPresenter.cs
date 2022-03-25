//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVArrayPresenter
//* .File Name       : $Workfile:   SVArrayPresenter.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:42:12  $
//******************************************************************************

using System;
using System.Collections.Generic;
using System.Text;

namespace SVRemoteTest
{
	public class SVArrayPresenter
    {
        #region ToString
        public static String ToString(Array list)
		{
			String str = "";
			foreach (object obj in list)
			{
				if (str.Length > 0)
				{
					str += ",";
				}
				str += obj.ToString();
			}
			return str;
        }
        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVREMOTECONTROL\SVRemoteTest\SVArrayPresenter.cs_v  $
 * 
 *    Rev 1.0   24 Apr 2012 15:42:12   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 1
 * SCR Title: Initial check-in
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial check-in.
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/