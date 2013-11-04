// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCIAcqStruct
// * .File Name       : $Workfile:   SVCIAcqStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   07 May 2013 19:56:56  $
// ******************************************************************************

#ifndef SVCIACQSTRUCT_H
#define SVCIACQSTRUCT_H

#include <vector>
#include <corapi.h>

#include "SVContainerLibrary/SVRingBuffer.h"
#include "SVOMFCLibrary/SVCallbackListClass.h"
#include "SVTimerLibrary/SVClock.h"

#include "SVCICallbackStruct.h"

class SVAcquisitionBufferInterface;

/*
This class is the data class for each camera available for use.
*/
struct SVCIAcqStruct  
{
public:
	typedef SVRingBuffer< long > SVIndexRingBuffer;

	SVCIAcqStruct();
	SVCIAcqStruct( const SVCIAcqStruct& p_rValue );
	virtual ~SVCIAcqStruct();

	const SVCIAcqStruct& operator=( const SVCIAcqStruct& p_rValue );

	//This attribute contains the index to the associated server for this digitizer.
	long m_lServerIndex;

	//This attribute contains the name of this digitizer.
	CString m_csName;

	//This attribute contains the fully qualified name of this digitizer.
	CString m_csFullName;

	//This attribute contains the Coreco handle to the associated server for this digitizer.
	CORSERVER m_hCorServer;

	//This attribute contains the Coreco handle to the digitizer.
	CORACQ    m_hCorAcq;

	//This attribute contains the Coreco handle to the LUT.
	CORLUT    m_hCorLut;

	//This attribute contains the Coreco handle to the digitizer camera format parameters.
	CORCAM    m_hCorCam;

	//This attribute contains the Coreco handle to the digitizer camera functionality parameters.
	CORVIC    m_hCorVic;

	//This attribute contains the Coreco handle to the transfer system assocoated with this digitizer.
	CORXFER   m_hCorXfer;

	//This attribute holds the current width of the image in pixels.
	unsigned long m_ulWidth;

	//This attribute holds the current height of the image in pixels.
	unsigned long m_ulHeight;

	//This attribute holds the current format of the image.
	unsigned long m_ulFormat;

	//This attribute holds the current type of the image.
	unsigned long m_ulType;

	//This attribute holds the current number of LUT entries.
	unsigned long m_ulLutEntries;

	//This attribute holds the current format of the LUT entries.
	unsigned long m_ulLutFormat;

	//This attribute holds the current cycle transfer mode.
	unsigned long m_ulCycleMode;

	//This attribute holds the whether the cycle mode uses a trash buffer.
	bool m_UsesTrashBuffer;

	//This attribute specifies the active state of the digitizer.
	long m_lIsStarted;

	//This attribute contains the acquisition buffers associated with this digitizer.
	SVCICallbackStructPtrCArray m_caBuffers;

	//This attribute specifies the register state of the digitizer trigger. 
	long m_lIsTriggerRegistered;

	//This attribute contains the callback elements when an image is received.
	SVCallbackListClass m_svTriggerCallbacks;

	// This attribute provides the interface to the buffer interface of the host application.
	SVAcquisitionBufferInterface* m_pBufferInterface;

	// This attribute provides the last start-of-frame was a trash call used for the acquisition process.
	bool m_StartIsTrash;

	// This attribute provides the last start-of-frame index used for the acquisition process.
	long m_StartIndex;

	// This attribute holds the high performance clock tick count used to collate the acquisition buffer to the correct product data.
	SVClock::SVTimeStamp m_StartFrameTimeStamp;

	// This attribute holds the handle to the acquisition processing thread shutdown event.
	HANDLE mhShutdown;

	// This attribute holds the handle to the acquistion processing thread.
	HANDLE mhThread;

	// This container contains the queue of availiable indexes used in the acquisistion process.
	SVIndexRingBuffer m_BufferIndexQueue;

};

/*
This specialization of the CArray template creates a container object to hold the SVCIAcqStruct objects.
*/
typedef std::vector< SVCIAcqStruct > SVCIAcqStructVector;

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCI\SVCIAcqStruct.h_v  $
 * 
 *    Rev 1.1   07 May 2013 19:56:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   29 Apr 2013 12:18:24   jspila
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
 *    Rev 1.7   01 Jun 2011 10:48:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use original functionality for acquisition and dynamic loading of the DLL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   24 May 2011 15:30:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added ring buffer to DLL to remove locks in Sapera callback threads.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   20 May 2011 09:50:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the Dalsa/Coreco libraries directly, change the acquisition mode to CORXFER_VAL_CYCLE_MODE_SYNCHRONOUS_WITH_TRASH, and change the buffer allocation to CORBUFFER_VAL_TYPE_SCATTER_GATHER.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Feb 2010 08:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Dec 2009 11:17:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jul 2009 13:04:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new methodology to aquire a buffer from SVObserver and copy/convert data from the acquisition buffer into the SVObserver buffer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/