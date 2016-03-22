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

#pragma once

//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <corapi.h>

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

