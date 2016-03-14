//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferInterface
//* .File Name       : $Workfile:   SVImageBufferInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 20:14:46  $
//******************************************************************************

#ifndef SVIMAGEBUFFERINTERFACE_H
#define SVIMAGEBUFFERINTERFACE_H

#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVImageBufferHandleInterface.h"

struct SVImageBufferInterface  
{
	SVImageBufferInterface();
	SVImageBufferInterface( const SVImageBufferInterface& p_rObject );
	SVImageBufferInterface( SVSmartHandlePointer p_ImageHandle, const SVDataManagerHandle& p_rIndexHandle );
	virtual ~SVImageBufferInterface();

	const SVImageBufferInterface& operator=( const SVImageBufferInterface& p_rObject );

	virtual void clear();

	virtual HRESULT Assign( SVSmartHandlePointer p_ImageHandle, const SVDataManagerHandle& p_rIndexHandle );

	virtual unsigned char* GetBufferAddress() const;

	virtual const SVClock::SVTimeStamp& GetStartFrameTimeStamp() const;
	virtual void SetStartFrameTimeStamp();
	virtual void SetStartFrameTimeStamp( const SVClock::SVTimeStamp& p_rTimeStamp );

	virtual const SVClock::SVTimeStamp& GetEndFrameTimeStamp() const;
	virtual void SetEndFrameTimeStamp();
	virtual void SetEndFrameTimeStamp( const SVClock::SVTimeStamp& p_rTimeStamp );

	SVSmartHandlePointer m_ImageHandle;
	SVDataManagerHandle m_IndexHandle;

	SVClock::SVTimeStamp m_StartTimeStamp;
	SVClock::SVTimeStamp m_EndTimeStamp;

};

typedef SVSharedPtr< SVImageBufferInterface > SVImageBufferPointer;

#endif

