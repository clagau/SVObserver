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

#pragma once

//Moved to precompiled header: #include <corapi.h>
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

