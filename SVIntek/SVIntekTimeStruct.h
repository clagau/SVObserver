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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIntek\SVIntekTimeStruct.h_v  $
 * 
 *    Rev 1.1   08 May 2013 14:56:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Apr 2013 12:48:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Apr 2013 14:54:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Feb 2010 10:23:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Nov 2009 16:11:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix time conversion issue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jul 2009 10:00:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include the new buffer acquisition methodology and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Nov 2005 13:32:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial code to implement Intek 1394 driver for SVObserver and test applications.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
