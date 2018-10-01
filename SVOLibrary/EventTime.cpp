//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file EventTime.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This is a class which contains start end time and state of the event
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EventTime.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma endregion Declarations

#pragma region Constructor
EventTime::EventTime() 
	: m_bIsValid(false)
	, m_bIsComplete(false)
	, m_StartTime(0.0)
	, m_EndTime(0.0)
{

}

EventTime::EventTime(const EventTime& rRHs)
	: m_bIsValid(rRHs.m_bIsValid)
	, m_bIsComplete(rRHs.m_bIsComplete)
	, m_StartTime(rRHs.m_StartTime)
	, m_EndTime(rRHs.m_EndTime)
{
}

const EventTime& EventTime::operator=(const EventTime& rRHs)
{
	m_bIsValid = rRHs.m_bIsValid;
	m_bIsComplete = rRHs.m_bIsComplete;
	m_StartTime = rRHs.m_StartTime;
	m_EndTime = rRHs.m_EndTime;
	return *this;
}
#pragma endregion Constructor

#pragma region Public Methods
void EventTime::reset()
{
	m_bIsValid = false;
	m_bIsComplete = false;

	m_StartTime = SvTl::GetTimeStamp();
	m_EndTime = SvTl::GetTimeStamp();
}
#pragma endregion Public Methods
