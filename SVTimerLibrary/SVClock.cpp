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

#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 161212 this is helpful for debugging the creation of Performance Information

SVClock::SVTimeStamp ReferenceTime = SVClock::GetTimeStamp();

double SVClock::setReferenceTime()
{
	SVClock::SVTimeStamp now  = GetTimeStamp();
	double delta = now - ReferenceTime;
	ReferenceTime= GetTimeStamp();

	return delta;
}


double SVClock::getReferenceTime()
{
	return ReferenceTime;
}

SVClock::SVTimeStamp SVClock::GetRelTimeStamp()
{
	SVClock::SVTimeStamp ts = SVClock::GetTimeStamp();
	return ts - ReferenceTime;
}


#endif

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTimerLibrary\SVClock.cpp_v  $
 * 
 *    Rev 1.2   13 May 2013 11:56:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   07 May 2013 13:13:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to create new SVClock conversions functions, which will clarify timing tolerance checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 15:56:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Apr 2013 13:53:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 18:35:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Jan 2013 15:43:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added now method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Sep 2012 13:36:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the return type of the Get Difference method of the Clock class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Apr 2011 07:59:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow for inspection to for the display to give up when an inspection get queued.  The display is now regulated to a maximum of 10 frames per second.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Mar 2011 07:13:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
