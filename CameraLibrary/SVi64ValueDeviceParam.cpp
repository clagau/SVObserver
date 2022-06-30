//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVi64ValueDeviceParam
//* .File Name       : $Workfile:   SVi64ValueDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:06:46  $
//******************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include "SVi64ValueDeviceParam.h"
#pragma endregion Includes

SVi64ValueDeviceParam::SVi64ValueDeviceParam()
{
	m_eDataType = DeviceDataTypei64;
}

SVi64ValueDeviceParam::SVi64ValueDeviceParam(SVDeviceParamEnum typeEnum) : SVDeviceParam(typeEnum)
{
	m_eDataType = DeviceDataTypei64;
}

SVi64ValueDeviceParam::SVi64ValueDeviceParam(const SVi64ValueDeviceParam& rhs) : SVDeviceParam(rhs)
,iValue{rhs.iValue}
,info{rhs.info}
{
}

SVi64ValueDeviceParam::~SVi64ValueDeviceParam()
{
}

SVClonable* SVi64ValueDeviceParam::CloneImpl() const
{
	return new SVi64ValueDeviceParam(*this);
}

SVi64ValueDeviceParam::SVi64ValueDeviceParam(const __int64& rhs)
	: iValue{rhs}
{
	m_eDataType = DeviceDataTypei64;
}

SVi64ValueDeviceParam::SVi64ValueDeviceParam(const LARGE_INTEGER& rhs)
	: iValue {rhs.QuadPart}
{
	m_eDataType = DeviceDataTypei64;
}

SVi64ValueDeviceParam::operator __int64() const
{
	return iValue;
}

SVi64ValueDeviceParam& SVi64ValueDeviceParam::operator=(const SVi64ValueDeviceParam& rhs)
{
	if (this != &rhs)
	{
		iValue = rhs.iValue;
		info = rhs.info;
	}
	return *this;
}

HRESULT SVi64ValueDeviceParam::GetValue(VARIANT& rv) const
{
	return ToVariant(iValue, rv);
}

HRESULT SVi64ValueDeviceParam::SetValue(const VARIANT& rv)
{
	return FromVariant(iValue, rv);
}

HRESULT SVi64ValueDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam)
{
	HRESULT hr = S_FALSE;
	if (const SVi64ValueDeviceParam* pParam = dynamic_cast<const SVi64ValueDeviceParam*>(pBaseParam))
	{
		info = pParam->info;
		hr = S_OK;
	}
	return hr;
}
