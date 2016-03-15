// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekTimeStruct
// * .File Name       : $Workfile:   SVIntekTimeStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 14:56:02  $
// ******************************************************************************

#ifndef SVINTEKTIMESTRUCT_H
#define SVINTEKTIMESTRUCT_H

/*
This object is used to work with time values received from the IEEE 1394 clock.
*/
struct SVIntekTimeStruct
{
	//This constructor calls the initialize method.
	SVIntekTimeStruct()
	{
		Initialize();
	};

	//This method initializes all local attributes to desired defaults.
	SVIntekTimeStruct( unsigned long p_ulTime )
	{
		SetTime( p_ulTime );
	};

	//This copy constructor uses the operator= method to copy the attributes from the provided object into the current one.
	SVIntekTimeStruct( const SVIntekTimeStruct &p_rsvTime )
	{
		*this = p_rsvTime;
	};

	//This operator copies all of the internal attributes from the provided object into the current one.
	const SVIntekTimeStruct &operator=( const SVIntekTimeStruct &p_rsvTime )
	{
		m_lSecondCount = p_rsvTime.m_lSecondCount;
		m_lCycleCount = p_rsvTime.m_lCycleCount;
		m_lCycleOffset = p_rsvTime.m_lCycleOffset;
	};

	//This method initializes all local attributes to desired defaults.
	void Initialize()
	{
		m_lSecondCount = -1;
		m_lCycleCount = -1;
		m_lCycleOffset = -1;
	};

	//This method sets all local attributes to provided values.
	void SetTime( unsigned long p_ulTime )
	{
		m_lSecondCount = ( 0xFE000000 & p_ulTime ) >> 25;
		m_lCycleCount =  ( 0x01FFF000 & p_ulTime ) >> 12;
		m_lCycleOffset = 0x00000FFF & p_ulTime;
	};

	//This method is used to find the difference between two Intek time objects.
	long GetCycleCountDifference( const SVIntekTimeStruct &p_rsvStartTime ) const
	{
		long l_lStartCount = p_rsvStartTime.m_lSecondCount * 8000 + p_rsvStartTime.m_lCycleCount;
		long l_lCycleCount = -1;

		if ( m_lSecondCount < p_rsvStartTime.m_lSecondCount )
		{
			l_lCycleCount = ( m_lSecondCount + 128 ) * 8000 + m_lCycleCount;
		}
		else
		{
			l_lCycleCount = m_lSecondCount * 8000 + m_lCycleCount;
		}

		l_lCycleCount -= l_lStartCount;

		return l_lCycleCount;
	};

	//This method is used to get a timestamp based on the IEEE1394 clock tick.
	static SVClock::SVTimeStamp GetSystemTime( const SVIntekTimeStruct &p_StartEndTime, const SVIntekTimeStruct &p_CurrentTime, const SVClock::SVTimeStamp& p_SystemCurrentTime )
	{
		double l_TimeDifference = p_CurrentTime.GetCycleCountDifference( p_StartEndTime );

		l_TimeDifference /= 8000.0; //convert to seconds
		l_TimeDifference *= 1000.0; //convert to milliseconds

		SVClock::SVTimeStamp l_Result = p_SystemCurrentTime - l_TimeDifference;

		return l_Result;
	};

	//This attribute holds the counter for seconds. Max Count=0128 [0...0127]
	long m_lSecondCount;
	//This attribute holds the counter for the cycles per second. Max Count=8000 [0...7999]
	long m_lCycleCount;
	//This attribute holds the counter for the cycles offset per second. Max Count=3072 [0...3071]
	long m_lCycleOffset;

};

#endif

