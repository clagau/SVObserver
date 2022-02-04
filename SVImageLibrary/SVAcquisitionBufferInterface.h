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

#include "ObjectInterfaces/ITRCImage.h"

struct CameraInfo;

class SVAcquisitionBufferInterface  
{
public:
	virtual ~SVAcquisitionBufferInterface() = default;

	virtual unsigned long GetBufferWidth() const = 0;
	virtual unsigned long GetBufferHeight() const = 0;
	virtual int GetBufferFormat() const = 0;

	virtual SvOi::ITRCImagePtr GetNextBuffer() = 0;
	virtual HRESULT UpdateWithCompletedBuffer(CameraInfo&& cameraInfo) = 0;
	virtual void setNeededBuffers(int neededBuffers) = 0;
};

