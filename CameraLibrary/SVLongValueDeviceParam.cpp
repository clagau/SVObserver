//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLongValueDeviceParam
//* .File Name       : $Workfile:   SVLongValueDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   03 Dec 2014 19:41:32  $
//******************************************************************************

#pragma region Includes
#include "Stdafx.h"
//Moved to precompiled header: #include <cmath>
#include "SVLongValueDeviceParam.h"
#pragma endregion Includes

SVLongValueDeviceParam::SVLongValueDeviceParam()
{
	m_eDataType = DeviceDataTypeLong;
}

SVLongValueDeviceParam::SVLongValueDeviceParam(SVDeviceParamEnum typeEnum) : SVDeviceParam(typeEnum)
{
	m_eDataType = DeviceDataTypeLong;
}

SVLongValueDeviceParam::SVLongValueDeviceParam(const SVLongValueDeviceParam& rhs)  : SVDeviceParam(rhs)
, lValue{rhs.lValue}
, info{rhs.info}
{
}

SVLongValueDeviceParam::~SVLongValueDeviceParam()
{
}

SVClonable* SVLongValueDeviceParam::CloneImpl() const
{
	return new SVLongValueDeviceParam(*this);
}

SVLongValueDeviceParam::SVLongValueDeviceParam(long rhs)	// not const long& so that we get automatic conversion from other integral types for free
{
	m_eDataType = DeviceDataTypeLong;
	lValue = rhs;
}

SVLongValueDeviceParam::operator long() const
{
	return lValue;
}

SVLongValueDeviceParam& SVLongValueDeviceParam::operator=(const SVLongValueDeviceParam& rhs)
{
	if ( this != &rhs )
	{
		lValue = rhs.lValue;
		info = rhs.info;
	}
	return *this;
}

SVLongValueDeviceParam& SVLongValueDeviceParam::operator=(long lVal)
{
	lValue = lVal;
	return *this;
}

HRESULT SVLongValueDeviceParam::GetValue(VARIANT& rv) const
{
	return ToVariant(lValue, rv);
}

HRESULT SVLongValueDeviceParam::SetValue(const VARIANT& rv)
{
	return FromVariant(lValue, rv);
}

long SVLongValueDeviceParam::GetScaledValue() const
{
	return static_cast<long>(ceil( lValue * info.multiplier ));
}

void SVLongValueDeviceParam::SetScaledValue(long lNewValue)
{
	if(info.multiplier != 0)
	{
		lValue = static_cast<long>(lNewValue / info.multiplier);
	}
	else
	{
		lValue = lNewValue;
	}
}

long SVLongValueDeviceParam::GetScaledMin() const
{
	return static_cast<long>(ceil(info.min * info.multiplier));
}

long SVLongValueDeviceParam::GetScaledMax() const
{
	return static_cast<long>(ceil(info.max * info.multiplier));
}

_variant_t SVLongValueDeviceParam::GetNormalizedValue() const
{
	_variant_t l_oValue;
	if (info.unit_divisor > 1.0)
	{
		double val = static_cast<double>(lValue) / info.unit_divisor;
		val = floor((val * info.unit_divisor) + 0.5) / info.unit_divisor; // round to X decimal places
		l_oValue = val;
	}
	else
	{
		l_oValue = lValue;
	}
	return l_oValue;
}

HRESULT SVLongValueDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam)
{
	HRESULT hr = S_FALSE;
	if (const SVLongValueDeviceParam* pParam = dynamic_cast<const SVLongValueDeviceParam*>(pBaseParam))
	{
		info = pParam->info;
		hr = S_OK;
	}
	return hr;
}

#if defined(_DEBUG)

long& LongValue(SVDeviceParamWrapper& w) 
{
	const SVLongValueDeviceParam* p = w.DerivedValue(p); 
	if (nullptr == p) 
	{
		w = SVLongValueDeviceParam();
		p = w.DerivedValue(p);
	} 
	return const_cast<SVLongValueDeviceParam*>(p)->lValue;
}

long LongValue(const SVDeviceParamWrapper& w) 
{
	const SVLongValueDeviceParam* p = w.DerivedValue(p); 
	if (p) 
		return p->lValue; 
	
	assert(false);
	return 0; 
}

SVDeviceParamWrapper DeviceParam(long l) 
{
	return SVLongValueDeviceParam(l);
}

SVDeviceParamWrapper DeviceParam(int i) 
{
	return SVLongValueDeviceParam(i);
}

SVDeviceParamWrapper DeviceParam(short i) 
{
	return SVLongValueDeviceParam(i);
}

#endif

