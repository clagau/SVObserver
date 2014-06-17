//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProfiler
//* .File Name       : $Workfile:   SVProfiler.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jun 2014 10:26:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

// This does not use the guideline namespace format because it is legacy code from the SVObserverWeb project.
namespace SeidenaderVision
{
	class SVProfiler
	{
	public:
#pragma region Constructor
		SVProfiler();
#pragma endregion Constructor

#pragma region Public Methods
		void Start();
		void End();
		double ElapsedMilliSeconds();
		double ElapsedMicroSeconds();
#pragma endregion Public Methods

#pragma region Member Variables
	private:
		SVClock::SVTimeStamp m_startTime;
		SVClock::SVTimeStamp m_endTime;
#pragma endregion Member Variables
	};
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVUtilityLibrary\SVProfiler.h_v  $
 * 
 *    Rev 1.0   17 Jun 2014 10:26:12   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:39:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/