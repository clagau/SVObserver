//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputObject
//* .File Name       : $Workfile:   SVRemoteInputObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Jun 2013 15:20:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRemoteInputObject.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#pragma endregion Includes

SVRemoteInputObject::SVRemoteInputObject( LPCSTR strObjectName )
: SVInputObject( strObjectName )
{
	LocalInitialize();
}

SVRemoteInputObject::SVRemoteInputObject( SVObjectClass* POwner, int StringResourceID )
: SVInputObject( POwner, StringResourceID )
{
	LocalInitialize();
}

SVRemoteInputObject::~SVRemoteInputObject()
{
	if( m_isCreated )
	{
		Destroy();
	}
}

bool SVRemoteInputObject::Create()
{
	if( !m_isCreated )
	{
		m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVIoObjectType;
		m_outObjectInfo.m_ObjectTypeInfo.SubType = SVRemoteInputObjectType;

		m_isCreated = true;

		::InitializeCriticalSection( &m_hCriticalSection );
	}

	return true;
}// end Create

bool SVRemoteInputObject::Destroy()
{
	m_vtRemoteCache.Clear();

	::DeleteCriticalSection( &m_hCriticalSection );

	m_isCreated = false;

	return true;
}// end Destroy

void SVRemoteInputObject::Lock()
{
	::EnterCriticalSection( &m_hCriticalSection );
}// end Lock

void SVRemoteInputObject::Unlock()
{
	::LeaveCriticalSection( &m_hCriticalSection );
}// end Unlock

HRESULT SVRemoteInputObject::Read( _variant_t& p_rValue )
{
	HRESULT l_Status = GetCache( p_rValue );

	return l_Status;
}// end Read

HRESULT SVRemoteInputObject::WriteCache( const _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;

	_variant_t l_Temp = 0.0;

	if( p_rValue.vt & VT_ARRAY )
	{
		SVSAFEARRAY l_SafeArray( p_rValue );

		if ( 1 == l_SafeArray.size() )
		{
			l_Status = l_SafeArray.GetElement( 0, l_Temp );
		}
		else
		{
			l_Status = E_INVALIDARG;
		}
	}
	else
	{
		l_Temp = p_rValue;
	}

	if( S_OK == l_Status )
	{
		::EnterCriticalSection( &m_hCriticalSection );

		m_vtRemoteCache = l_Temp;

		::LeaveCriticalSection( &m_hCriticalSection );
	}

	return l_Status;
}// end ReadCache

HRESULT SVRemoteInputObject::GetCache( _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;

	p_rValue.Clear();

	::EnterCriticalSection( &m_hCriticalSection );

	p_rValue = m_vtRemoteCache;

	::LeaveCriticalSection( &m_hCriticalSection );

	return l_Status;
}

void SVRemoteInputObject::updateGuid(int position)
{
	if (0 <= position && 0x100 > position)
	{
		SVGUID ParameterUID(RemoteInputUidGuid);
		ParameterUID.ToGUID().Data1 += position;
		SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, ParameterUID);
	}
}

void SVRemoteInputObject::LocalInitialize()
{
	m_isCreated = false;
	m_lIndex = -1;
	m_vtRemoteCache = 0.0;
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVIoObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVRemoteInputObjectType;
}
