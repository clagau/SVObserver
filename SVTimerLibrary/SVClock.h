//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClock
//* .File Name       : $Workfile:   SVClock.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 May 2013 11:56:20  $
//******************************************************************************

#ifndef SVCLOCK_H
#define SVCLOCK_H

namespace SVClock
{
	static const double c_MicrosecondsPerMillisecond=1000.0; ///< the number of microseconds per millisecond: one thousand
	static const int c_secondsPerMinute=60; ///< the number of seconds per minute: sixty

	enum SVConversionEnum
	{
		Unknown = 0,
		Hours,
		Minutes,
		Seconds,
		Milliseconds,
		Microseconds,
	};

	typedef double SVFrequency; // SVTimeStamp / SVFrequency = Seconds
	typedef double SVTimeStamp; // In Milliseconds

	SVFrequency GetFrequency(); 

	SVTimeStamp GetTimeStamp();
	SVTimeStamp GetMaxTimeStamp();
	SVTimeStamp GetMinTimeStamp();

	double ConvertTo( SVConversionEnum p_Units, const SVTimeStamp& p_rTimeStamp );
	SVTimeStamp ConvertFrom( SVConversionEnum p_Units, double p_Time );

};

#endif 

