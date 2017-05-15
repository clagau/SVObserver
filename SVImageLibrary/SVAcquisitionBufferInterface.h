//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionBufferInterface
//* .File Name       : $Workfile:   SVAcquisitionBufferInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 07:16:32  $
//******************************************************************************

#pragma once

#include "SVTimerLibrary/SVClock.h"
#include "SVImageBufferInterface.h"

class SVAcquisitionBufferInterface  
{
public:
	virtual ~SVAcquisitionBufferInterface();

	virtual SvTl::SVTimeStamp GetTimeStamp() const = 0;
	
	virtual unsigned long GetBufferWidth() const = 0;
	virtual unsigned long GetBufferHeight() const = 0;
	virtual int GetBufferFormat() const = 0;

	virtual HRESULT GetNextBuffer( SVImageBufferInterface& p_rBuffer ) = 0;
	virtual HRESULT UpdateWithCompletedBuffer( const SVImageBufferInterface& p_rBuffer ) = 0;

};

