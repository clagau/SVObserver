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
#define new DEBUG_NEW
#endif

SVGigeCameraStruct::SVGigeCameraStruct()
{
	Clear();
	iPosition = 0;
}

SVGigeCameraStruct::SVGigeCameraStruct(const SVGigeCameraStruct &SVGigecs)
{
	*this = SVGigecs;
}

SVGigeCameraStruct::~SVGigeCameraStruct()
{
}

const SVGigeCameraStruct& SVGigeCameraStruct::operator = (const SVGigeCameraStruct &SVGigecs)
{
	if ( this != &SVGigecs )
	{
		eChangeType = SVGigecs.eChangeType;
		iPosition = SVGigecs.iPosition;

		strSerialNum = SVGigecs.strSerialNum;
		strModelName = SVGigecs.strModelName;
		strVendorName = SVGigecs.strVendorName;
		strIPAddress = SVGigecs.strIPAddress;

		m_ulHandle = SVGigecs.m_ulHandle;
	}

	return *this;
}

void SVGigeCameraStruct::SetInfo(const SVGigeCameraStruct& rhs)
{
	if ( this != &rhs )
	{
		strSerialNum = rhs.strSerialNum;
		strModelName = rhs.strModelName;
		strVendorName = rhs.strVendorName;
		strIPAddress = rhs.strIPAddress;
		m_ulHandle = rhs.m_ulHandle;
	}
}

void SVGigeCameraStruct::Clear()
{
	strSerialNum.Empty();
	strModelName.Empty();
	strVendorName.Empty();
	strIPAddress.Empty();
	eChangeType = SVNoChange;
	
	m_ulHandle = 0;
}

bool SVGigeCameraStruct::operator == (const SVGigeCameraStruct &SVGigecs) const
{
	return strIPAddress == SVGigecs.strIPAddress;
}

bool SVGigeCameraStruct::operator < (const SVGigeCameraStruct &SVGigecs) const
{
	return strIPAddress < SVGigecs.strIPAddress;
}

bool SVGigeCameraStruct::operator > (const SVGigeCameraStruct &SVGigecs) const
{
	return strIPAddress > SVGigecs.strIPAddress;
}

bool SVGigeCameraStruct::HasSerialNumber() const
{
	return !strSerialNum.IsEmpty();
}

CString SVGigeCameraStruct::GetSerialNumber() const
{
	return strSerialNum;
}

bool SVGigeCameraStruct::HasModelName() const
{
	return !strModelName.IsEmpty();
}

CString SVGigeCameraStruct::GetModelName() const
{
	return strModelName;
}

bool SVGigeCameraStruct::HasVendorName() const
{
	return !strVendorName.IsEmpty();
}

CString SVGigeCameraStruct::GetVendorName() const
{
	return strVendorName;
}

bool SVGigeCameraStruct::HasIPAddress() const
{
	return !strIPAddress.IsEmpty();
}

CString SVGigeCameraStruct::GetIPAddress() const
{
	return strIPAddress;
}

