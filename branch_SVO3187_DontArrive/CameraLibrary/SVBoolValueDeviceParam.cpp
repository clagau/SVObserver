//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBoolValueDeviceParam
//* .File Name       : $Workfile:   SVBoolValueDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 12:59:10  $
//******************************************************************************
#include "Stdafx.h"
#include "SVBoolValueDeviceParam.h"

SVBoolValueDeviceParam::SVBoolValueDeviceParam()
: bValue(false) 
{
	m_eDataType = DeviceDataTypeBool;
}

SVBoolValueDeviceParam::SVBoolValueDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
, bValue(false) 
{
	m_eDataType = DeviceDataTypeBool;
}

SVBoolValueDeviceParam::SVBoolValueDeviceParam(const SVBoolValueDeviceParam& rhs) 
: SVDeviceParam(rhs)
{
	m_eDataType = DeviceDataTypeBool;
	bValue = rhs.bValue;
	info = rhs.info;
}

SVClonable* SVBoolValueDeviceParam::CloneImpl() const
{
	return new SVBoolValueDeviceParam(*this);
}

SVBoolValueDeviceParam::SVBoolValueDeviceParam(bool rhs)	// not const bool& so that we get automatic conversion from other integral types for free
: SVDeviceParam(DeviceParamInvalid)
{
	m_eDataType = DeviceDataTypeBool;
	bValue = rhs;
}

SVBoolValueDeviceParam::operator bool() const
{
	return bValue;
}

SVBoolValueDeviceParam& SVBoolValueDeviceParam::operator=(const SVBoolValueDeviceParam& rhs)
{
	if ( this != &rhs )
	{
		bValue = rhs.bValue;
		info = rhs.info;
	}
	return *this;
}

SVBoolValueDeviceParam& SVBoolValueDeviceParam::operator=(bool bVal)
{
	bValue = bVal;
	return *this;
}

HRESULT SVBoolValueDeviceParam::GetValue(VARIANT& rv) const
{
	return ToVariant(bValue, rv);
}

HRESULT SVBoolValueDeviceParam::SetValue(const VARIANT& rv)
{
	return FromVariant(bValue, rv);
}

HRESULT SVBoolValueDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam)
{
	HRESULT hr = S_FALSE;
	if (const SVBoolValueDeviceParam* pParam = dynamic_cast<const SVBoolValueDeviceParam*>(pBaseParam))
	{
		info = pParam->info;
		hr = S_OK;
	}
	return hr;
}

#if defined(_DEBUG)

bool& BoolValue(SVDeviceParamWrapper& w) 
{
	const SVBoolValueDeviceParam* p = w.DerivedValue(p); 
	if (nullptr == p) 
	{
		w = SVBoolValueDeviceParam(); 
		p = w.DerivedValue(p);
	} 
	assert(p); 
	return const_cast<SVBoolValueDeviceParam*>(p)->bValue;
}

bool BoolValue(const SVDeviceParamWrapper& w) 
{
	const SVBoolValueDeviceParam* p = w.DerivedValue(p); 
	if (p) 
		return p->bValue; 

	assert(FALSE);
	return 0;
}

// cppcheck-suppress unusedFunction
SVDeviceParamWrapper DeviceParam(bool b) 
{
	return SVBoolValueDeviceParam(b);
}

#endif

