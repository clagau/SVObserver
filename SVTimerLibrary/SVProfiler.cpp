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

#pragma region Includes
#include "stdafx.h"
#include "SVProfiler.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvTl
{
	#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information

	SvTl::SVTimeStamp ReferenceTime = SvTl::GetTimeStamp();

	double setReferenceTime()
	{
		SvTl::SVTimeStamp now  = SvTl::GetTimeStamp();
		double delta = now - ReferenceTime;
		ReferenceTime= SvTl::GetTimeStamp();

		return delta;
	}


	SvTl::SVTimeStamp  getReferenceTime()
	{
		return ReferenceTime;
	}

	SvTl::SVTimeStamp GetRelTimeStamp()
	{
		SvTl::SVTimeStamp ts = SvTl::GetTimeStamp();
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
		m_startTime = SvTl::GetTimeStamp();
	}

	void SVProfiler::End()
	{
		m_endTime = SvTl::GetTimeStamp();
	}

	// in Milliseconds
	double SVProfiler::ElapsedMilliSeconds()
	{
		double iElapsed = SvTl::ConvertTo(SvTl::Milliseconds, (m_endTime - m_startTime));
		return iElapsed;
	}

	// in Microseconds
	double SVProfiler::ElapsedMicroSeconds()
	{
		double iElapsed = SvTl::ConvertTo(SvTl::Microseconds, (m_endTime - m_startTime));
		return iElapsed;
	}
	#pragma endregion Public Methods
} //namespace SvTl
