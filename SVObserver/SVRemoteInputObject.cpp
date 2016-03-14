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

#include "stdafx.h"
#include "SVRemoteInputObject.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"

SVRemoteInputObject::SVRemoteInputObject( LPCSTR strObjectName )
: SVInputObject( strObjectName )
{
	m_bCreated = FALSE;
	m_lIndex   = -1;
	m_vtRemoteCache = 0.0;
}

SVRemoteInputObject::SVRemoteInputObject( SVObjectClass* POwner, int StringResourceID )
: SVInputObject( POwner, StringResourceID )
{
	m_bCreated = FALSE;
	m_lIndex   = -1;
	m_vtRemoteCache = 0.0;
}

SVRemoteInputObject::~SVRemoteInputObject()
{
	if( m_bCreated )
		Destroy();
}

BOOL SVRemoteInputObject::Create()
{
	m_bCreated = TRUE;

  ::InitializeCriticalSection( &m_hCriticalSection );

	return TRUE;
}// end Create

BOOL SVRemoteInputObject::Destroy()
{
	m_vtRemoteCache.Clear();

	::DeleteCriticalSection( &m_hCriticalSection );

	m_bCreated = FALSE;

	return TRUE;
}// end Destroy

BOOL SVRemoteInputObject::IsCreated()
{
	return m_bCreated;
}// end IsCreated

BOOL SVRemoteInputObject::Lock()
{
	::EnterCriticalSection( &m_hCriticalSection );

	return TRUE;
}// end Lock

BOOL SVRemoteInputObject::Unlock()
{
	::LeaveCriticalSection( &m_hCriticalSection );

	return TRUE;
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

		if ( l_SafeArray.size() == 1 )
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

	if( l_Status == S_OK )
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

