//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraStruct
//* .File Name       : $Workfile:   SVGigeCameraStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Jun 2013 15:19:56  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVContainerLibrary/SVVector.h"
#include "ObjectInterfaces/SVGigeCameraStructInterface.h"
#pragma endregion Includes

/**
@SVObjectName Gige Camera Description

@SVObjectOverview This object is used to hold the Gige Camera data.

@SVObjectOperations This operator presents operations to store, clear, and compare Gige camera data.

*/
struct SVGigeCameraStruct : public SvOi::SVGigeCameraStructInterface
{
	SVGigeCameraStruct();
	virtual ~SVGigeCameraStruct();

	SVGigeCameraStruct(const SVGigeCameraStruct &SVGigecs);
	const SVGigeCameraStruct& operator= (const SVGigeCameraStruct &SVGigecs);
	
	void SetInfo(const SVGigeCameraStruct& rhs);
	void Clear();
	
	bool operator == (const SVGigeCameraStruct &SVGigecs) const;
	bool operator < (const SVGigeCameraStruct &SVGigecs) const;
	bool operator > (const SVGigeCameraStruct &SVGigecs) const;

	virtual bool HasSerialNumber() const;
	virtual CString GetSerialNumber() const;
	virtual bool HasModelName() const;
	virtual CString GetModelName() const;
	virtual bool HasVendorName() const;
	virtual CString GetVendorName() const;
	virtual bool HasIPAddress() const;
	virtual CString GetIPAddress() const;

	CString strSerialNum;
	CString strModelName;
	CString strVendorName;
	CString strIPAddress;
	
	int iPosition;
	unsigned long m_ulHandle;
};

typedef SVVector< SVGigeCameraStruct > SVGigeCameraStructSet;


