//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputMatchItemWrapper
//* .File Name       : $Workfile:   SVInputMatchItemWrapper.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   04 Nov 2014 16:07:30  $
//******************************************************************************
using System;
using System.ComponentModel;

namespace SVRemoteTest
{
	[DisplayName("SVInputMatchItem")]
	public class SVInputMatchItemWrapper
    {
        #region Member Variables
        private String m_matchString = "";
		private Int32 m_transactionId = 0;
        #endregion

        #region Properties
        [Category("Input Parameters")]
        [DisplayName("Match String")]
		[Description("Match String")]
		public String MatchString
		{
			get { return m_matchString; }
			set { m_matchString = value; }
		}

        [Category("Input Parameters")]
		[DisplayName("Transaction ID")]
		[Description("Transaction ID")]
		public Int32 TransactionId
		{
			get { return m_transactionId; }
			set { m_transactionId = value; }
		}
        #endregion
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVInputMatchItemWrapper.cs_v  $
 * 
 *    Rev 1.1   04 Nov 2014 16:07:30   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 19
 * SCR Title: Enchance SVRC Test Application to remember method data
 * Checked in by: sJones; Steve Jones
 * Change Description: Added Category attribute tag of Input Parameters to aid in persistence.
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

