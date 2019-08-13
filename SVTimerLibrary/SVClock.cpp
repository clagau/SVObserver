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
	constexpr double c_Frequency = 1000.0;
	typedef std::map< SvTl::SVConversionEnum, double > SVConversionFactorMap;
	static const SVConversionFactorMap cConversions
	{
		{SvTl::Hours, ( 1.0 / 3600.0 )},
		{SvTl::Minutes, ( 1.0 / 60.0 )},
		{SvTl::Seconds, 1.0},
		{SvTl::Milliseconds, 1000.0},
		{SvTl::Microseconds, 1000000.0}
	};


	double GetTimeStamp()
	{
		// VMWare ESXI 5.0 has an issue with QueryPerformanceCounter (it's resolution is only 24bit not 64bit)
		// need to use something different here like GetTickCount
		#ifndef COMPILE_FOR_VM
			__int64 l_Tick = 0;
			__int64 l_Frequency = 0;

			::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&l_Tick));
			::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&l_Frequency));

			double l_TimeStamp = static_cast< double >( l_Tick );
			l_TimeStamp /= static_cast< double >( l_Frequency ) / 1000.0;   // in millisec
		#else
			double l_TimeStamp = static_cast< double >( GetTickCount() );  // in millisec
		#endif

		return l_TimeStamp;
	}

	double GetMaxTimeStamp()
	{
		return std::numeric_limits< double >::max();
	}

	double GetMinTimeStamp()
	{
		return std::numeric_limits< double >::min();
	}

	double ConvertTo( SVConversionEnum p_Units, const double& p_rTimeStamp )
	{
		double l_Value = 0.0;

		SVConversionFactorMap::const_iterator l_Iter = cConversions.find( p_Units );

		if( l_Iter != cConversions.end() )
		{
			l_Value = p_rTimeStamp;
			l_Value /= c_Frequency;
			l_Value *= l_Iter->second;
		}

		return l_Value;
	}

	double SvTl::ConvertFrom( SVConversionEnum p_Units, double p_Time )
	{
		double result{0.0};

		SVConversionFactorMap::const_iterator l_Iter = cConversions.find( p_Units );

		if( l_Iter != cConversions.end() )
		{
			result = p_Time;
			result /= l_Iter->second;
			result *= c_Frequency;
		}

		return result;
	}
} //namespace SvTl
