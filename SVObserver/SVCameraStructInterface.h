//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraStructInterface
//* .File Name       : $Workfile:   SVCameraStructInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:50:12  $
//******************************************************************************

#pragma once

struct SVCameraStructInterface
{
	enum CameraChangeTypeEnum
	{
		SVUndefined,
		SVNoChange,
		SVNewCamera,
		SVReplacedCamera,
		SVCameraRemoved
	};

	SVCameraStructInterface();
	virtual ~SVCameraStructInterface();
	
	virtual bool HasSerialNumber() const=0;
	virtual CString GetSerialNumber() const=0;
	virtual bool HasModelName() const=0;
	virtual CString GetModelName() const=0;

	CameraChangeTypeEnum eChangeType;
};

