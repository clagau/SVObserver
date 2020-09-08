// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOTriggerLoadLibraryClass
// * .File Name       : $Workfile:   SVIOTriggerLoadLibraryClass.cpp
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:48:20  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVIOTriggerLoadLibraryClass.h"
#pragma endregion Includes

SVIOTriggerLoadLibraryClass::~SVIOTriggerLoadLibraryClass()
{
	Close();
}

HRESULT SVIOTriggerLoadLibraryClass::Open(LPCTSTR libraryPath)
{
	HRESULT result {S_OK};

	if ( nullptr == m_Handle )
	{
		m_Handle = ::LoadLibrary( libraryPath );
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);

		if ( nullptr != m_Handle )
		{
			m_pCreate = (SVCreatePtr)::GetProcAddress( m_Handle, "SVCreate" );
			m_pDestroy = (SVDestroyPtr)::GetProcAddress( m_Handle, "SVDestroy" );
			m_pGetCount = (SVTriggerGetCountPtr)::GetProcAddress( m_Handle, "SVTriggerGetCount" );
			m_pGetHandle = (SVTriggerGetHandlePtr)::GetProcAddress( m_Handle, "SVTriggerGetHandle" );
			m_pGetName = (SVTriggerGetNamePtr)::GetProcAddress( m_Handle, "SVTriggerGetName" );
			m_pRegister = (SVTriggerRegisterPtr)::GetProcAddress( m_Handle, "SVTriggerRegister" );
			m_pUnregister = (SVTriggerUnregisterPtr)::GetProcAddress( m_Handle, "SVTriggerUnregister" );
			m_pStart = (SVTriggerStartPtr)::GetProcAddress( m_Handle, "SVTriggerStart" );
			m_pStop = (SVTriggerStopPtr)::GetProcAddress( m_Handle, "SVTriggerStop" );
			m_pGetParameterCount = (SVTriggerGetParameterCountPtr)::GetProcAddress( m_Handle, "SVTriggerGetParameterCount" );
			m_pGetParameterName = (SVTriggerGetParameterNamePtr)::GetProcAddress( m_Handle, "SVTriggerGetParameterName" );
			m_pGetParameterValue = (SVTriggerGetParameterValuePtr)::GetProcAddress( m_Handle, "SVTriggerGetParameterValue" );
			m_pSetParameterValue = (SVTriggerSetParameterValuePtr)::GetProcAddress( m_Handle, "SVTriggerSetParameterValue" );

			if ( nullptr != m_pCreate &&
			     nullptr != m_pDestroy &&
			     nullptr != m_pGetCount &&
			     nullptr != m_pGetHandle &&
			     nullptr != m_pGetName &&
			     nullptr != m_pRegister &&
			     nullptr != m_pUnregister &&
			     nullptr != m_pStart &&
			     nullptr != m_pStop )
			{
				result = m_pCreate();
			}
			else
			{
				result = S_FALSE;
			}

		}
	}

	if ( S_OK != result )
	{
		Close();
	}

	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::Close()
{
	HRESULT result {S_OK};

	if ( nullptr != m_Handle )
	{
		if ( nullptr != m_pDestroy )
		{
			result = m_pDestroy();
		}

		if ( ::FreeLibrary( m_Handle ) )
		{
			// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
			Sleep(0);
			m_Handle = nullptr;
		}
		else
		{
			result = S_FALSE;
		}
	}

	m_pCreate = nullptr;
	m_pDestroy = nullptr;
	m_pGetCount = nullptr;
	m_pGetHandle = nullptr;
	m_pGetName = nullptr;
	m_pRegister = nullptr;
	m_pUnregister = nullptr;
	m_pStart = nullptr;
	m_pStop = nullptr;
	m_pGetParameterCount = nullptr;
	m_pGetParameterName = nullptr;
	m_pGetParameterValue = nullptr;
	m_pSetParameterValue = nullptr;

	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::GetCount( unsigned long* pCount )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetCount )
	{
		result = m_pGetCount( pCount );
	}

	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::GetHandle( unsigned long* pTriggerChannel, unsigned long index )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetHandle )
	{
		result = m_pGetHandle( pTriggerChannel, index );
	}

	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::GetName( unsigned long triggerChannel, BSTR* pName )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetName )
	{
		result = m_pGetName( triggerChannel, pName );
	}

	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::Register( unsigned long triggerChannel, SvTi::TriggerCallBack pTriggerCallback)
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pRegister )
	{
		result = m_pRegister( triggerChannel, pTriggerCallback);
	}

	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::Unregister( unsigned long triggerChannel)
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pUnregister )
	{
		result = m_pUnregister( triggerChannel);
	}

	return result;
}
	
HRESULT SVIOTriggerLoadLibraryClass::Start( unsigned long triggerChannel )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pStart )
	{
		result = m_pStart( triggerChannel );
	}

	return result;
}
	
HRESULT SVIOTriggerLoadLibraryClass::Stop( unsigned long triggerChannel )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pStop )
	{
		result = m_pStop( triggerChannel );
	}

	return result;
}
	
HRESULT SVIOTriggerLoadLibraryClass::GetParameterCount( unsigned long triggerChannel, unsigned long* pCount )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetParameterCount )
	{
		result = m_pGetParameterCount( triggerChannel, pCount );
	}

	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::GetParameterName( unsigned long triggerChannel, unsigned long p_ulIndex, BSTR* pName )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetParameterName )
	{
		result = m_pGetParameterName( triggerChannel, p_ulIndex, pName );
	}

	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::GetParameterValue( unsigned long triggerChannel, unsigned long index, VARIANT* pValue )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetParameterValue )
	{
		result = m_pGetParameterValue( triggerChannel, index, pValue );
	}

	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::SetParameterValue( unsigned long triggerChannel, unsigned long index, VARIANT* pValue )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pSetParameterValue )
	{
		result = m_pSetParameterValue( triggerChannel, index, pValue );
	}

	return result;
}

