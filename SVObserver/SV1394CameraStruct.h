//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraStruct
//* .File Name       : $Workfile:   SV1394CameraStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:30  $
//******************************************************************************

#ifndef SV1394CAMERASTRUCT_H
#define SV1394CAMERASTRUCT_H

#include "SVCameraStructInterface.h"
#include "SVContainerLibrary/SVVector.h"

/**
@SVObjectName 1394 Camera Description

@SVObjectOverview This object is used to hold the 1394 Camera data.

@SVObjectOperations This operator presents operations to store, clear, and compair 1394 camera data.

*/
struct SV1394CameraStruct : public SVCameraStructInterface
{
	SV1394CameraStruct();
	virtual ~SV1394CameraStruct();
	SV1394CameraStruct(const SV1394CameraStruct &SV1394cs);
	const SV1394CameraStruct& operator= (const SV1394CameraStruct &SV1394cs);
	void SetInfo(const SV1394CameraStruct& rhs);
	void Clear();
	bool operator == (const SV1394CameraStruct &SV1394cs);
	bool operator < (const SV1394CameraStruct &SV1394cs);
	bool operator > (const SV1394CameraStruct &SV1394cs);

	virtual bool HasSerialNumber() const;
	virtual CString GetSerialNumber() const;
	virtual bool HasModelName() const;
	virtual CString GetModelName() const;

	unsigned __int64 m_ui64SerialNumber;
	CString m_csVendorId;
	CString strSerialNum;
	CString strModelName;
	CString strVendorName;
	
	int iPosition;
	unsigned long m_ulHandle;
};

typedef SVVector< SV1394CameraStruct > SV1394CameraStructSet;

#endif

