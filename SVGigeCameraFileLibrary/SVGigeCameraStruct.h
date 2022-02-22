//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
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
//Moved to precompiled header: #include <vector>
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

	std::string m_SerialNum;
	std::string m_ModelName;
	std::string m_VendorName;
	std::string m_IPAddress;
	
	int m_CameraID;
	int m_DigitizerID;
	unsigned long m_AcquisitionHandle;
};

typedef std::vector<SVGigeCameraStruct> SVGigeCameraStructVector;


