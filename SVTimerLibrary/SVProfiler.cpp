//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProfiler
//* .File Name       : $Workfile:   SVProfiler.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jun 2014 10:26:14  $
//******************************************************************************

#include "stdafx.h"
#include "SVProfiler.h"

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader { namespace SVTimerLibrary
{


#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information

	SVClock::SVTimeStamp ReferenceTime = SVClock::GetTimeStamp();

	double setReferenceTime()
	{
		SVClock::SVTimeStamp now  = SVClock::GetTimeStamp();
		double delta = now - ReferenceTime;
		ReferenceTime= SVClock::GetTimeStamp();

		return delta;
	}


	SVClock::SVTimeStamp  getReferenceTime()
	{
		return ReferenceTime;
	}

	SVClock::SVTimeStamp GetRelTimeStamp()
	{
		SVClock::SVTimeStamp ts = SVClock::GetTimeStamp();
		return ts - ReferenceTime;
	}


#endif




#pragma region Constructor
	SVProfiler::SVProfiler()
	: m_startTime(0)
	, m_endTime(0)
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	void SVProfiler::Start()
	{
		m_startTime = SVClock::GetTimeStamp();
	}

	void SVProfiler::End()
	{
		m_endTime = SVClock::GetTimeStamp();
	}

	// in Milliseconds
	double SVProfiler::ElapsedMilliSeconds()
	{
		double iElapsed = SVClock::ConvertTo(SVClock::Milliseconds, (m_endTime - m_startTime));
		return iElapsed;
	}

	// in Microseconds
	double SVProfiler::ElapsedMicroSeconds()
	{
		double iElapsed = SVClock::ConvertTo(SVClock::Microseconds, (m_endTime - m_startTime));
		return iElapsed;
	}
#pragma endregion Public Methods
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVUtilityLibrary\SVProfiler.cpp_v  $
 * 
 *    Rev 1.0   17 Jun 2014 10:26:14   bwalter
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