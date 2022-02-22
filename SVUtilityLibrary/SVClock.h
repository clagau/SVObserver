//******************************************************************************
//* COPYRIGHT (c) 2011 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClock
//* .File Name       : $Workfile:   SVClock.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 May 2013 11:56:20  $
//******************************************************************************

#pragma once

namespace SvUl
{
	static const double c_MicrosecondsPerMillisecond = 1000.0; ///< the number of microseconds per millisecond: one thousand
	static const int c_secondsPerMinute = 60; ///< the number of seconds per minute: sixty

	enum SVConversionEnum
	{
		Unknown = 0,
		Hours,
		Minutes,
		Seconds,
		Milliseconds,
		Microseconds,
	};

	double GetTimeStamp();
	double GetMaxTimeStamp();
	double GetMinTimeStamp();

	double ConvertTo(SVConversionEnum p_Units, const double& p_rTimeStamp);
	double ConvertFrom(SVConversionEnum p_Units, double p_Time);
} //namespace SvUl
