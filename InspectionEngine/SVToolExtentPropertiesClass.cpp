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
	HRESULT l_hrOk = S_OK;

	SVToolExtentPropertiesMap::const_iterator iter;

	for ( iter = m_svProperties.begin(); S_OK == l_hrOk && iter != m_svProperties.end(); ++iter )
	{
		if ( nullptr != iter->second.pValueObject )
		{
			_variant_t Value;

			if ( S_OK == ( l_hrOk = iter->second.pValueObject->getValue( Value ) ) )
			{
				l_hrOk = rExtents.SetExtentProperty( iter->first, Value.dblVal );
			}
		}
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::GetExtentObject( SvDef::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject*& rpValueObject ) const
{
	HRESULT l_hrOk = S_FALSE;

	SVToolExtentPropertiesMap::const_iterator iter = m_svProperties.find( p_eProperty );

	if ( iter != m_svProperties.end() )
	{
		rpValueObject = iter->second.pValueObject;
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::SetExtentObject( SvDef::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject )
{
	HRESULT l_hrOk = S_OK;

	if ( nullptr == pValueObject )
	{
		m_svProperties.erase( p_eProperty );
	}
	else
	{
		m_svProperties[ p_eProperty ].pValueObject = pValueObject;
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::GetPropertyInfo( SvDef::SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const
{
	HRESULT l_hrOk = S_FALSE;

	SVToolExtentPropertiesMap::const_iterator iter = m_svProperties.find( p_eProperty );

	if ( iter != m_svProperties.end() )
	{
		p_rInfo = iter->second.info;
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::SetPropertyInfo( SvDef::SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo )
{
	HRESULT l_hrOk = S_FALSE;

	SVToolExtentPropertiesMap::iterator iter = m_svProperties.find( p_eProperty );

	if ( iter != m_svProperties.end() )
	{
		iter->second.info = p_rInfo;
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

