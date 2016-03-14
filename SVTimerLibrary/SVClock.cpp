//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClock
//* .File Name       : $Workfile:   SVClock.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 May 2013 11:56:18  $
//******************************************************************************

#include "stdafx.h"
#include <limits>
#include <map>
#include "boost/config.hpp"
#include "boost/assign.hpp"
#include "SVClock.h"

typedef std::map< SVClock::SVConversionEnum, double > SVConversionFactorMap;
static const SVConversionFactorMap g_Conversions = boost::assign::map_list_of< SVClock::SVConversionEnum, double >
	( SVClock::Hours, ( 1.0 / 3600.0 ) )
	( SVClock::Minutes, ( 1.0 / 60.0 ) )
	( SVClock::Seconds, 1.0 )
	( SVClock::Milliseconds, 1000.0 )
	( SVClock::Microseconds, 1000000.0 )
	;

SVClock::SVFrequency SVClock::GetFrequency()
{
	SVFrequency l_Frequency = 0.0;

	// VMWare ESXI has an issue with QueryPerformanceCounter (it's resolution is only 24bit not 64bit)
	// need to use something different here
#ifndef COMPILE_FOR_VM
	l_Frequency = 1000.0;
#else
	l_Frequency = 1000.0;
#endif

	return l_Frequency;
}


SVClock::SVTimeStamp SVClock::GetTimeStamp()
{

	SVTimeStamp l_TimeStamp = 0.0;

	// VMWare ESXI has an issue with QueryPerformanceCounter (it's resolution is only 24bit not 64bit)
	// need to use something different here
	#ifndef COMPILE_FOR_VM
		__int64 l_Tick = 0;
		__int64 l_Frequency = 0;

		::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&l_Tick));
		::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&l_Frequency));

		l_TimeStamp = static_cast< double >( l_Tick );
		l_TimeStamp /= static_cast< double >( l_Frequency ) / 1000.0;   // in millisec
	#else
		 l_TimeStamp = static_cast< double >( GetTickCount() );  // in millisec
	#endif

	return l_TimeStamp;
}

SVClock::SVTimeStamp SVClock::GetMaxTimeStamp()
{
	return std::numeric_limits< double >::max();
}

SVClock::SVTimeStamp SVClock::GetMinTimeStamp()
{
	return std::numeric_limits< double >::min();
}

double SVClock::ConvertTo( SVConversionEnum p_Units, const SVTimeStamp& p_rTimeStamp )
{
	double l_Value = 0.0;

	SVConversionFactorMap::const_iterator l_Iter = g_Conversions.find( p_Units );

	if( l_Iter != g_Conversions.end() )
	{
		l_Value = p_rTimeStamp;
		l_Value /= GetFrequency();
		l_Value *= l_Iter->second;
	}

	return l_Value;
}

SVClock::SVTimeStamp SVClock::ConvertFrom( SVConversionEnum p_Units, double p_Time )
{
	SVClock::SVTimeStamp l_TimeStamp = 0.0;

	SVConversionFactorMap::const_iterator l_Iter = g_Conversions.find( p_Units );

	if( l_Iter != g_Conversions.end() )
	{
		l_TimeStamp = p_Time;
		l_TimeStamp /= l_Iter->second;
		l_TimeStamp *= GetFrequency();
	}

	return l_TimeStamp;
}

