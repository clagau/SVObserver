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
#include "SVClock.h"
#pragma endregion Includes

//@TODO This is not a profiler! Class and files should be renamed to something like "TimeDelta"

namespace Seidenader { namespace SVTimerLibrary
{
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
	double setReferenceTime(); ///< sets the current time as reference TimeStamp
	double getReferenceTime(); ///< returns the reference TimeStamp
	SVClock::SVTimeStamp GetRelTimeStamp();
#endif

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
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */

namespace SvTl = Seidenader::SVTimerLibrary;

