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

