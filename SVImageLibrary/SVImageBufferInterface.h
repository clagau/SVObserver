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

#pragma once

#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVImageBufferHandleInterface.h"

struct SVImageBufferInterface  
{
	SVImageBufferInterface();
	SVImageBufferInterface( const SVImageBufferInterface& p_rObject );
	SVImageBufferInterface( SVSmartHandlePointer p_ImageHandle, const SVDataManagerHandle& p_rIndexHandle );
	~SVImageBufferInterface();

	const SVImageBufferInterface& operator=( const SVImageBufferInterface& p_rObject );

	void clear();

	HRESULT Assign( SVSmartHandlePointer p_ImageHandle, const SVDataManagerHandle& p_rIndexHandle );

	unsigned char* GetBufferAddress() const;

	const SvTl::SVTimeStamp& GetStartFrameTimeStamp() const;
	void SetStartFrameTimeStamp();
	void SetStartFrameTimeStamp( const SvTl::SVTimeStamp& p_rTimeStamp );

	const SvTl::SVTimeStamp& GetEndFrameTimeStamp() const;
	void SetEndFrameTimeStamp();
	void SetEndFrameTimeStamp( const SvTl::SVTimeStamp& p_rTimeStamp );

	SVSmartHandlePointer m_ImageHandle;
	SVDataManagerHandle m_IndexHandle;

	SvTl::SVTimeStamp m_StartTimeStamp;
	SvTl::SVTimeStamp m_EndTimeStamp;

};

typedef SVSharedPtr< SVImageBufferInterface > SVImageBufferPointer;

