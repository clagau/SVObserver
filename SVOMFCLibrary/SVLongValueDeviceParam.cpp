//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLongValueDeviceParam
//* .File Name       : $Workfile:   SVLongValueDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   03 Dec 2014 19:41:32  $
//******************************************************************************
#include "Stdafx.h"
//Moved to precompiled header: #include <cmath>
#include "SVLongValueDeviceParam.h"

SVLongValueDeviceParam::SVLongValueDeviceParam()
: lValue(0) 
{
	m_eDataType = DeviceDataTypeLong;
	init();
}

SVLongValueDeviceParam::SVLongValueDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
, lValue(0) 
{
	m_eDataType = DeviceDataTypeLong;
	init();
}

SVLongValueDeviceParam::SVLongValueDeviceParam(const SVLongValueDeviceParam& rhs) 
: SVDeviceParam( rhs )
{
	m_eDataType = DeviceDataTypeLong;
	lValue = rhs.lValue;
	info = rhs.info;
}

SVClonable* SVLongValueDeviceParam::CloneImpl() const
{
	return new SVLongValueDeviceParam(*this);
}

SVLongValueDeviceParam::SVLongValueDeviceParam(long rhs)	// not const long& so that we get automatic conversion from other integral types for free
{
	m_eDataType = DeviceDataTypeLong;
	lValue = rhs;
	init();
}

SVLongValueDeviceParam::operator long() const
{
	return lValue;
}

void SVLongValueDeviceParam::init()
{
	m_eDataType = DeviceDataTypeLong;
	info.min = 0;
	info.max = LONG_MAX;
	info.multiplier = 1.0;
	info.offset = 0;
	info.sUnits = "";
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

#if defined(_DEBUG) || defined(_MINDEBUG)

long& LongValue(SVDeviceParamWrapper& w) 
{
	const SVLongValueDeviceParam* p = w.DerivedValue(p); 
	if (p == NULL) 
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
	
	ASSERT(FALSE); 
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

