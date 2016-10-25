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

	SVGigeCameraStruct(const SVGigeCameraStruct& rRhs);
	const SVGigeCameraStruct& operator= (const SVGigeCameraStruct& rRhs);
	
	void Clear();
	
	bool operator == (const SVGigeCameraStruct& rRhs) const;
	bool operator < (const SVGigeCameraStruct& rRhs) const;
	bool operator > (const SVGigeCameraStruct& rRhs) const;

	virtual bool HasSerialNumber() const;
	virtual SVString GetSerialNumber() const;
	virtual bool HasModelName() const;
	virtual SVString GetModelName() const;
	virtual bool HasVendorName() const;
	virtual SVString GetVendorName() const;
	virtual bool HasIPAddress() const;
	virtual SVString GetIPAddress() const;

	SVString m_SerialNum;
	SVString m_ModelName;
	SVString m_VendorName;
	SVString m_IPAddress;
	
	int m_CameraID;
	int m_DigitizerID;
	unsigned long m_AcquisitionHandle;
};

typedef SVVector< SVGigeCameraStruct > SVGigeCameraStructVector;


