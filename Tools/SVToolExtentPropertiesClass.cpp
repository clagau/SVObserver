//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolExtentPropertiesClass
//* .File Name       : $Workfile:   SVToolExtentPropertiesClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:44:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolExtentPropertiesClass.h"
#include "ObjectInterfaces/IValueObject.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVToolExtentPropertiesClass::SVToolExtentPropertiesClass()
{
	Initialize();
}

SVToolExtentPropertiesClass::~SVToolExtentPropertiesClass()
{
	Initialize();
}

HRESULT SVToolExtentPropertiesClass::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_extentPropertyInfo.clear();

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::GetProperties( SVImageExtentClass& rExtents ) const
{
	HRESULT result = S_OK;

	SVToolExtentPropertiesMap::const_iterator iter;

	for ( iter = m_extentPropertyInfo.begin(); S_OK == result && iter != m_extentPropertyInfo.end(); ++iter )
	{
		if ( nullptr != iter->second.pValueObject )
		{
			_variant_t Value;

			if ( S_OK == ( result = iter->second.pValueObject->getValue( Value ) ) )
			{
				result = (VT_R8 == Value.vt) ? rExtents.SetExtentProperty( iter->first, Value.dblVal ) : E_INVALIDARG;
			}
		}
	}

	return result;
}

HRESULT SVToolExtentPropertiesClass::GetExtentObject( SvPb::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject*& rpValueObject ) const
{
	HRESULT result{E_FAIL};

	SVToolExtentPropertiesMap::const_iterator iter = m_extentPropertyInfo.find( p_eProperty );

	if (m_extentPropertyInfo.end() != iter)
	{
		rpValueObject = iter->second.pValueObject;
		result = S_OK;
	}

	return result;
}

void SVToolExtentPropertiesClass::SetExtentObject( SvPb::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject )
{
	if ( nullptr == pValueObject )
	{
		m_extentPropertyInfo.erase( p_eProperty );
	}
	else
	{
		m_extentPropertyInfo[ p_eProperty ].pValueObject = pValueObject;
	}
}

HRESULT SVToolExtentPropertiesClass::GetPropertyInfo( SvPb::SVExtentPropertyEnum eProperty, SvIe::SVExtentPropertyInfoStruct& rInfo ) const
{
	HRESULT result{E_FAIL};

	SVToolExtentPropertiesMap::const_iterator iter = m_extentPropertyInfo.find( eProperty );

	if ( iter != m_extentPropertyInfo.end() )
	{
		rInfo = iter->second.info;
		result = S_OK;
	}

	return result;
}

HRESULT SVToolExtentPropertiesClass::SetPropertyInfo( SvPb::SVExtentPropertyEnum eProperty, const SvIe::SVExtentPropertyInfoStruct& rInfo )
{
	HRESULT result{E_FAIL};

	SVToolExtentPropertiesMap::iterator iter = m_extentPropertyInfo.find( eProperty );

	if ( iter != m_extentPropertyInfo.end() )
	{
		iter->second.info = rInfo;
		result = S_OK;
	}

	return result;
}

} //namespace SvIe
