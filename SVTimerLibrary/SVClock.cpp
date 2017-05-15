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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <limits>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include "boost/config.hpp"
//Moved to precompiled header: #include "boost/assign.hpp"
#include "SVClock.h"
#pragma endregion Includes

namespace SvTl
{
	typedef std::map< SvTl::SVConversionEnum, double > SVConversionFactorMap;
	static const SVConversionFactorMap g_Conversions = boost::assign::map_list_of< SvTl::SVConversionEnum, double >
		( SvTl::Hours, ( 1.0 / 3600.0 ) )
		( SvTl::Minutes, ( 1.0 / 60.0 ) )
		( SvTl::Seconds, 1.0 )
		( SvTl::Milliseconds, 1000.0 )
		( SvTl::Microseconds, 1000000.0 )
		;

	SvTl::SVFrequency SvTl::GetFrequency()
	{
		SVFrequency l_Frequency = 1000.0;
		return l_Frequency;
	}

	SvTl::SVTimeStamp SvTl::GetTimeStamp()
	{
		SVTimeStamp l_TimeStamp = 0.0;

		// VMWare ESXI 5.0 has an issue with QueryPerformanceCounter (it's resolution is only 24bit not 64bit)
		// need to use something different here like GetTickCount
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

	SvTl::SVTimeStamp SvTl::GetMaxTimeStamp()
	{
		return std::numeric_limits< double >::max();
	}

	SvTl::SVTimeStamp SvTl::GetMinTimeStamp()
	{
		return std::numeric_limits< double >::min();
	}

	double SvTl::ConvertTo( SVConversionEnum p_Units, const SVTimeStamp& p_rTimeStamp )
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

	SvTl::SVTimeStamp SvTl::ConvertFrom( SVConversionEnum p_Units, double p_Time )
	{
		SvTl::SVTimeStamp l_TimeStamp = 0.0;

		SVConversionFactorMap::const_iterator l_Iter = g_Conversions.find( p_Units );

		if( l_Iter != g_Conversions.end() )
		{
			l_TimeStamp = p_Time;
			l_TimeStamp /= l_Iter->second;
			l_TimeStamp *= GetFrequency();
		}

		return l_TimeStamp;
	}
} //namespace SvTl
