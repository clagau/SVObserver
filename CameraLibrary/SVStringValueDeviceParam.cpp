//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringValueDeviceParam
//* .File Name       : $Workfile:   SVStringValueDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:14:16  $
//******************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include "SVStringValueDeviceParam.h"
#pragma endregion Includes

SVStringValueDeviceParam::SVStringValueDeviceParam()
{
	m_eDataType = DeviceDataTypeString;
}

SVStringValueDeviceParam::SVStringValueDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
{
	m_eDataType = DeviceDataTypeString;
}

SVStringValueDeviceParam::SVStringValueDeviceParam( const SVStringValueDeviceParam& rRhs )
: SVDeviceParam( rRhs )
{
	m_eDataType = DeviceDataTypeString;
	strValue = rRhs.strValue;
	info = rRhs.info;
}

SVStringValueDeviceParam::~SVStringValueDeviceParam()
{
}

SVClonable* SVStringValueDeviceParam::CloneImpl() const
{
	return new SVStringValueDeviceParam( *this );
}

SVStringValueDeviceParam::SVStringValueDeviceParam( const SVString& rRhs )
{
	m_eDataType = DeviceDataTypeString;
	strValue = rRhs;
}

SVStringValueDeviceParam& SVStringValueDeviceParam::operator = (const SVStringValueDeviceParam& rRhs)
{
	if ( this != &rRhs )
	{
		strValue = rRhs.strValue;
		info = rRhs.info;
	}
	return *this;
}

SVStringValueDeviceParam& SVStringValueDeviceParam::operator = (const SVString& rValue)
{
	strValue = rValue;
	return *this;
}

HRESULT SVStringValueDeviceParam::GetValue( VARIANT& rv ) const
{
	return ToVariant( strValue, rv );
}

HRESULT SVStringValueDeviceParam::SetValue( const VARIANT& rv )
{
	return FromVariant( strValue, rv );
}

HRESULT SVStringValueDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam )
{
	HRESULT hr = S_FALSE;
	if ( const SVStringValueDeviceParam* pParam = dynamic_cast<const SVStringValueDeviceParam*>(pBaseParam) )
	{
		info = pParam->info;
		hr = S_OK;
	}
	return hr;
}

#if defined(_DEBUG)

SVString& StringValue(SVDeviceParamWrapper& w) 
{
	const SVStringValueDeviceParam* p = w.DerivedValue(p); 
	if (nullptr == p) 
	{
		w = SVStringValueDeviceParam(); 
		p = w.DerivedValue(p);
	} 
	ASSERT(p); 
	return const_cast<SVStringValueDeviceParam*>(p)->strValue;
}

const SVString& StringValue(const SVDeviceParamWrapper& w) 
{
	const SVStringValueDeviceParam* p = w.DerivedValue(p); 
	if (p) 
		return p->strValue; 
	
	ASSERT(FALSE); 
	static SVString s(""); 
	return s;
}

SVDeviceParamWrapper DeviceParam(const SVString& s) 
{
	return SVStringValueDeviceParam(s.c_str());
}

#endif

