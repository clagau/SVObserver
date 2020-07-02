//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraStruct
//* .File Name       : $Workfile:   SVGigeCameraStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Jun 2013 15:19:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVGigeCameraStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

SVGigeCameraStruct::SVGigeCameraStruct()
{
	Clear();
}

SVGigeCameraStruct::SVGigeCameraStruct(const SVGigeCameraStruct &SVGigecs)
{
	*this = SVGigecs;
}

SVGigeCameraStruct::~SVGigeCameraStruct()
{
}

const SVGigeCameraStruct& SVGigeCameraStruct::operator = (const SVGigeCameraStruct& rRhs)
{
	if ( this != &rRhs )
	{
		m_SerialNum = rRhs.m_SerialNum;
		m_ModelName = rRhs.m_ModelName;
		m_VendorName = rRhs.m_VendorName;
		m_IPAddress = rRhs.m_IPAddress;
		m_CameraID = rRhs.m_CameraID;
		m_DigitizerID = rRhs.m_DigitizerID;
		m_AcquisitionHandle = rRhs.m_AcquisitionHandle;
	}

	return *this;
}

void SVGigeCameraStruct::Clear()
{
	m_SerialNum.clear();
	m_ModelName.clear();
	m_VendorName.clear();
	m_IPAddress.clear();
	m_CameraID = -1;
	m_DigitizerID = -1;
	m_AcquisitionHandle = 0;
}

bool SVGigeCameraStruct::operator == (const SVGigeCameraStruct& rRhs) const
{
	return m_IPAddress == rRhs.m_IPAddress;
}

bool SVGigeCameraStruct::operator < (const SVGigeCameraStruct& rRhs) const
{
	return m_IPAddress < rRhs.m_IPAddress;
}

bool SVGigeCameraStruct::operator > (const SVGigeCameraStruct& rRhs) const
{
	return m_IPAddress > rRhs.m_IPAddress;
}
