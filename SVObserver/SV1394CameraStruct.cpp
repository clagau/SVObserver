//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraStruct
//* .File Name       : $Workfile:   SV1394CameraStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:22  $
//******************************************************************************

#include "Stdafx.h"
#include "SV1394CameraStruct.h"

SV1394CameraStruct::SV1394CameraStruct()
{
	Clear();

	iPosition = 0;
}

SV1394CameraStruct::SV1394CameraStruct(const SV1394CameraStruct &SV1394cs)
{
	*this = SV1394cs;
}

SV1394CameraStruct::~SV1394CameraStruct()
{
}

const SV1394CameraStruct& SV1394CameraStruct::operator = (const SV1394CameraStruct &SV1394cs)
{
	if ( this != &SV1394cs )
	{
		eChangeType = SV1394cs.eChangeType;
		iPosition = SV1394cs.iPosition;
		m_ui64SerialNumber = SV1394cs.m_ui64SerialNumber;
		m_csVendorId = SV1394cs.m_csVendorId;
		strSerialNum = SV1394cs.strSerialNum;
		strModelName = SV1394cs.strModelName;
		strVendorName = SV1394cs.strVendorName;

		m_ulHandle = SV1394cs.m_ulHandle;
	}

	return *this;
}

void SV1394CameraStruct::SetInfo(const SV1394CameraStruct& rhs)
{
	if ( this != &rhs )
	{
		m_ui64SerialNumber = rhs.m_ui64SerialNumber;
		m_csVendorId = rhs.m_csVendorId;
		strSerialNum = rhs.strSerialNum;
		strModelName = rhs.strModelName;
		strVendorName = rhs.strVendorName;

		m_ulHandle = rhs.m_ulHandle;

	}
}

void SV1394CameraStruct::Clear()
{
	m_ui64SerialNumber = 0;
	m_csVendorId.Empty();
	strSerialNum.Empty();
	strModelName.Empty();
	strVendorName.Empty();
	eChangeType = SVNoChange;
	
	m_ulHandle = NULL;
}

bool SV1394CameraStruct::operator == (const SV1394CameraStruct &SV1394cs)
{
	return strSerialNum == SV1394cs.strSerialNum;
}

bool SV1394CameraStruct::operator < (const SV1394CameraStruct &SV1394cs)
{
	return strSerialNum < SV1394cs.strSerialNum;
}

bool SV1394CameraStruct::operator > (const SV1394CameraStruct &SV1394cs)
{
	return strSerialNum > SV1394cs.strSerialNum;
}

bool SV1394CameraStruct::HasSerialNumber() const
{
	return !strSerialNum.IsEmpty();
}

CString SV1394CameraStruct::GetSerialNumber() const
{
	return strSerialNum;
}

bool SV1394CameraStruct::HasModelName() const
{
	return !strModelName.IsEmpty();
}

CString SV1394CameraStruct::GetModelName() const
{
	return strModelName;
}

