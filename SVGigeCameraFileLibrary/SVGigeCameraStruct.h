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
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

/**
@SVObjectName Gige Camera Description

@SVObjectOverview This object is used to hold the Gige Camera data.

@SVObjectOperations This operator presents operations to store, clear, and compare Gige camera data.

*/
struct SVGigeCameraStruct
{
	SVGigeCameraStruct();
	~SVGigeCameraStruct();

	SVGigeCameraStruct(const SVGigeCameraStruct& rRhs);
	const SVGigeCameraStruct& operator= (const SVGigeCameraStruct& rRhs);
	
	void Clear();
	
	bool operator == (const SVGigeCameraStruct& rRhs) const;
	bool operator < (const SVGigeCameraStruct& rRhs) const;
	bool operator > (const SVGigeCameraStruct& rRhs) const;

	bool HasSerialNumber() const;
	SVString GetSerialNumber() const;
	bool HasModelName() const;
	SVString GetModelName() const;
	bool HasVendorName() const;
	SVString GetVendorName() const;
	bool HasIPAddress() const;
	SVString GetIPAddress() const;

	SVString m_SerialNum;
	SVString m_ModelName;
	SVString m_VendorName;
	SVString m_IPAddress;
	
	int m_CameraID;
	int m_DigitizerID;
	unsigned long m_AcquisitionHandle;
};

typedef SVVector<SVGigeCameraStruct> SVGigeCameraStructVector;


