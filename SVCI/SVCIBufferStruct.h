// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCIBufferStruct
// * .File Name       : $Workfile:   SVCIBufferStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   07 May 2013 19:56:58  $
// ******************************************************************************

#ifndef SVCIBUFFERSTRUCT_H
#define SVCIBUFFERSTRUCT_H

#include <corapi.h>
#include "SVContainerLibrary/SVVector.h"
#include "SVTimerLibrary/SVClock.h"

/*
This class is responsible to hold the image buffer information and the image buffer itself.
*/
class SVCIBufferStruct  
{
public:
	SVCIBufferStruct();
	SVCIBufferStruct( const SVCIBufferStruct &p_rValue );
	virtual ~SVCIBufferStruct();

	const SVCIBufferStruct &operator=( const SVCIBufferStruct &p_rValue );

	//This attribute is the Coreco server handle to the associated to this image buffer.
	CORSERVER m_hCorServer;

	//This attribute holds the current image buffer width in pixels.
	unsigned long m_ulWidth;
	//This attribute holds the current image buffer height in pixels.
	unsigned long m_ulHeight;
	//This attribute holds the current image buffer format.
	unsigned long m_ulFormat;
	//This attribute holds the current image type.
	unsigned long m_ulType;
	//This attribute holds the current image buffer address.
	unsigned char* m_pAddress;
	//This attribute holds the pitch width of the image in pixels.
	unsigned long m_Pitch;

	//This attribute is the Coreco image buffer handle.
	CORBUFFER m_hCorBuffer;

	// This attribute holds the high performance clock tick of the start-of-frame event.
	SVClock::SVTimeStamp m_StartTimeStamp;
	// This attribute holds the high performance clock tick of the end-of-frame event.
	SVClock::SVTimeStamp m_EndTimeStamp;

};

/*
This specialization of the CArray template creates a container object to hold the SVCIBufferStruct objects.
*/
typedef SVVector< SVCIBufferStruct > SVCIBufferStructCArray;

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCI\SVCIBufferStruct.h_v  $
 * 
 *    Rev 1.1   07 May 2013 19:56:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Apr 2013 12:18:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 17:59:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Nov 2010 15:38:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Feb 2010 08:40:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jul 2009 13:04:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new methodology to aquire a buffer from SVObserver and copy/convert data from the acquisition buffer into the SVObserver buffer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/