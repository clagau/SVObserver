//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCtrlActivateMonitorList
//* .File Name       : $Workfile:   SVRemoteCtrlActivateMonitorList.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   28 Oct 2014 11:41:56  $
//******************************************************************************
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using SeidenaderVision;

namespace SVRemoteTest
{
    public class SVRemoteCtrlActivateMonitorList : ISVComObjectView
    {
        #region Properties
        [Category("Input Parameters")]
        [Description("Name of Monitor List")]
        public String ListName
        {
            get { return SVRemoteTestHelper.MonitorListName; }
            set { SVRemoteTestHelper.MonitorListName = value; }
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

        public void SetResult(object values)
        {
        }

        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVRemoteCtrlActivateMonitorList.cs_v  $
 * 
 *    Rev 1.0   28 Oct 2014 11:41:56   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Initial Checkin
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
