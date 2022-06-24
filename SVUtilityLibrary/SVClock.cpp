//******************************************************************************
//* COPYRIGHT (c) 2011 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVClock.h"
#pragma endregion Includes

namespace SvUl
{
	constexpr double cNanoToMilli = 1000000.0;

	constexpr std::array<std::pair<SvUl::SVConversionEnum, double>, 5> cConversions
	{{
		{SvUl::Hours, (1.0 / 3600000.0)},
		{SvUl::Minutes, (1.0 / 60000.0)},
		{SvUl::Seconds, 1.0 / 1000.0},
		{SvUl::Milliseconds, 1.0},
		{SvUl::Microseconds, 1000.0}
	}};


	double GetTimeStamp()
	{
		double result {static_cast<double> (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count())};
		return  result / cNanoToMilli;
	}

	double GetMaxTimeStamp()
	{
		return std::numeric_limits< double >::max();
	}

	double GetMinTimeStamp()
	{
		return std::numeric_limits< double >::min();
	}

	double ConvertTo( SVConversionEnum units, const double& rTimeStamp )
	{
		double result {0.0};

		const auto iter = std::find_if(cConversions.cbegin(), cConversions.cend(), [units](const auto& rEntry) {return rEntry.first == units; });

		if( iter != cConversions.end() )
		{
			result = rTimeStamp;
			result *= iter->second;
		}

		return result;
	}

	double SvUl::ConvertFrom( SVConversionEnum units, const double& rTimeStamp)
	{
		double result {0.0};

		const auto iter = std::find_if(cConversions.cbegin(), cConversions.cend(), [&units](const auto& rEntry) {return rEntry.first == units; });

		if( iter != cConversions.end() )
		{
			result = rTimeStamp;
			result /= iter->second;
		}

		return result;
	}
} //namespace SvUl
