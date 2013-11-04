// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekBufferStruct
// * .File Name       : $Workfile:   SVIntekBufferStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 14:55:54  $
// ******************************************************************************

#ifndef SVINTEKBUFFERSTRUCT_H
#define SVINTEKBUFFERSTRUCT_H

#include "SVContainerLibrary/SVVector.h"
#include "SVTimerLibrary/SVClock.h"

/*
This class is used to hold the image information and data for an acquisition device.
*/
class SVIntekBufferStruct  
{
public:
	SVIntekBufferStruct();
	SVIntekBufferStruct( const SVIntekBufferStruct &p_rValue );
	virtual ~SVIntekBufferStruct();

	const SVIntekBufferStruct &operator=( const SVIntekBufferStruct &p_rValue );

	//This attribute holds the current width of the image in pixels.
	unsigned long m_ulWidth;
	//This attribute holds the current height of the image in pixels.
	unsigned long m_ulHeight;
	//This attribute holds the current format of the image.
	int m_iFormat;

	//This attribute holds the pointer to the bitmap information.
  BITMAPINFO *m_pBitmapInfo;
	//This attribute holds the pointer to the actual image data.
  unsigned char *m_pucImage;
	//This attribute holds the handle to the Intek image buffer.
	unsigned long m_hBuffer;

	//This attribute holds the lock state of this buffer.
	bool m_bIsLocked;

	//This attribute is the index into the camera array.
	long m_CameraIndex;

	// This attribute holds the time stamp of the start-of-frame event.
	SVClock::SVTimeStamp m_StartTimeStamp;
	// This attribute holds the time stamp of the end-of-frame event.
	SVClock::SVTimeStamp m_EndTimeStamp;

};

/*
This specialization of the CArray template creates a container class that holds a group of SVIntekBufferStructs.
*/
typedef SVVector< SVIntekBufferStruct > SVIntekBufferStructCArray;

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIntek\SVIntekBufferStruct.h_v  $
 * 
 *    Rev 1.1   08 May 2013 14:55:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Apr 2013 12:49:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Apr 2013 14:51:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Nov 2010 15:44:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Feb 2010 10:23:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
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
 *    Rev 1.0   11 Nov 2005 12:29:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial code to implement Intek 1394 driver for SVObserver and test applications.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
