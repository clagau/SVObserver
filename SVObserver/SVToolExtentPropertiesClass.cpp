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
#include "SVValueObjectLibrary/SVValueObject.h"
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

HRESULT SVToolExtentPropertiesClass::GetProperties( SVImageExtentClass &p_rsvExtents ) const
{
	HRESULT l_hrOk = S_OK;

	SVToolExtentPropertiesMap::const_iterator iter;

	for ( iter = m_svProperties.begin(); S_OK == l_hrOk && iter != m_svProperties.end(); ++iter )
	{
		SVValueObjectClass* l_psvValue = iter->second.pValueObject;

		if ( nullptr != l_psvValue )
		{
			double l_dValue = 0.0;

			if ( S_OK == ( l_hrOk = l_psvValue->GetValue( l_dValue ) ) )
			{
				l_hrOk = p_rsvExtents.SetExtentProperty( iter->first, l_dValue );
			}
		}
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::GetProperties( unsigned long p_ulIndex, SVImageExtentClass &p_rsvExtents ) const
{
	HRESULT l_hrOk = S_OK;

	SVToolExtentPropertiesMap::const_iterator iter;

	for ( iter = m_svProperties.begin(); S_OK == l_hrOk && iter != m_svProperties.end(); ++iter )
	{
		SVValueObjectClass* l_psvValue = iter->second.pValueObject;

		if ( nullptr != l_psvValue )
		{
			double l_dValue = 0.0;

			if ( S_OK == ( l_hrOk = l_psvValue->GetValue( p_ulIndex, l_dValue ) ) )
			{
				l_hrOk = p_rsvExtents.SetExtentProperty( iter->first, l_dValue );
			}
		}
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::GetExtentObject( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *&p_rpsvValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	SVToolExtentPropertiesMap::const_iterator iter = m_svProperties.find( p_eProperty );

	if ( iter != m_svProperties.end() )
	{
		p_rpsvValue = iter->second.pValueObject;
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::SetExtentObject( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *p_psvValue )
{
	HRESULT l_hrOk = S_OK;

	if ( nullptr == p_psvValue )
	{
		m_svProperties.erase( p_eProperty );
	}
	else
	{
		m_svProperties[ p_eProperty ].pValueObject = p_psvValue;
	}

	return l_hrOk;
}

HRESULT SVToolExtentPropertiesClass::GetPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const
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

HRESULT SVToolExtentPropertiesClass::SetPropertyInfo( SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo )
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

