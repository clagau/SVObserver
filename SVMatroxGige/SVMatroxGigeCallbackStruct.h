//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCallbackStruct
//* .File Name       : $Workfile:   SVMatroxGigeCallbackStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 15:22:46  $
//******************************************************************************

#ifndef SVMATROXGIGECALLBACKSTRUCT_H
#define SVMATROXGIGECALLBACKSTRUCT_H

#include <deque>
#include "SVTimerLibrary/SVClock.h"

struct SVMatroxGigeCallbackStruct
{
public:
	SVMatroxGigeCallbackStruct();
	virtual ~SVMatroxGigeCallbackStruct();

	unsigned long m_ulDigitizerHandle;
	unsigned long m_ulBufferIndex;

	SVClock::SVTimeStamp m_StartFrameTimeStamp;

};

typedef std::deque<SVMatroxGigeCallbackStruct*> SVMatroxGigeCallbackStructPtrArray;

#endif

