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

	m_svProperties.clear();

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::GetProperties( SVImageExtentClass& rExtents ) const
{
	HRESULT result = S_OK;

	SVToolExtentPropertiesMap::const_iterator iter;

	for ( iter = m_svProperties.begin(); S_OK == result && iter != m_svProperties.end(); ++iter )
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

HRESULT SVToolExtentPropertiesClass::GetExtentObject( SvDef::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject*& rpValueObject ) const
{
	HRESULT result{E_FAIL};

	SVToolExtentPropertiesMap::const_iterator iter = m_svProperties.find( p_eProperty );

	if (m_svProperties.end() != iter)
	{
		rpValueObject = iter->second.pValueObject;
		result = S_OK;
	}

	return result;
}

void SVToolExtentPropertiesClass::SetExtentObject( SvDef::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject )
{
	if ( nullptr == pValueObject )
	{
		m_svProperties.erase( p_eProperty );
	}
	else
	{
		m_svProperties[ p_eProperty ].pValueObject = pValueObject;
	}
}

HRESULT SVToolExtentPropertiesClass::GetPropertyInfo( SvDef::SVExtentPropertyEnum eProperty, SvIe::SVExtentPropertyInfoStruct& rInfo ) const
{
	HRESULT result{E_FAIL};

	SVToolExtentPropertiesMap::const_iterator iter = m_svProperties.find( eProperty );

	if ( iter != m_svProperties.end() )
	{
		rInfo = iter->second.info;
		result = S_OK;
	}

	return result;
}

HRESULT SVToolExtentPropertiesClass::SetPropertyInfo( SvDef::SVExtentPropertyEnum eProperty, const SvIe::SVExtentPropertyInfoStruct& rInfo )
{
	HRESULT result{E_FAIL};

	SVToolExtentPropertiesMap::iterator iter = m_svProperties.find( eProperty );

	if ( iter != m_svProperties.end() )
	{
		iter->second.info = rInfo;
		result = S_OK;
	}

	return result;
}

} //namespace SvIe