// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIODigitalLoadLibraryClass
// * .File Name       : $Workfile:   SVIODigitalLoadLibraryClass.cpp
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:46:44  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVIODigitalLoadLibraryClass.h"
#pragma endregion Includes

SVIODigitalLoadLibraryClass::~SVIODigitalLoadLibraryClass()
{
	Close();
}

HRESULT SVIODigitalLoadLibraryClass::Open(LPCTSTR libraryPath)
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
			m_pGetInputCount = (SVInputGetCountPtr)::GetProcAddress( m_Handle, "SVInputGetCount" );
			m_pGetInputValue = (SVInputGetValuePtr)::GetProcAddress( m_Handle, "SVInputGetValue" );
			m_pSetInputValue = (SVInputSetValuePtr)::GetProcAddress( m_Handle, "SVInputSetValue" );
			m_pGetInputPortCount = (SVInputGetPortCountPtr)::GetProcAddress( m_Handle, "SVInputGetPortCount" );
			m_pGetInputPortValue = (SVInputGetPortValuePtr)::GetProcAddress( m_Handle, "SVInputGetPortValue" );
			m_pSetInputPortValue = (SVInputSetPortValuePtr)::GetProcAddress( m_Handle, "SVInputSetPortValue" );
			m_pGetOutputCount = (SVOutputGetCountPtr)::GetProcAddress( m_Handle, "SVOutputGetCount" );
			m_pGetOutputValue = (SVOutputGetValuePtr)::GetProcAddress( m_Handle, "SVOutputGetValue" );
			m_pSetOutputValue = (SVOutputSetValuePtr)::GetProcAddress( m_Handle, "SVOutputSetValue" );
			m_pGetOutputPortCount = (SVOutputGetPortCountPtr)::GetProcAddress( m_Handle, "SVOutputGetPortCount" );
			m_pGetOutputPortValue = (SVOutputGetPortValuePtr)::GetProcAddress( m_Handle, "SVOutputGetPortValue" );
			m_pSetOutputPortValue = (SVOutputSetPortValuePtr)::GetProcAddress(m_Handle, "SVOutputSetPortValue");
			m_pSetOutputData = (SVOutputSetDataPtr)::GetProcAddress(m_Handle, "SVOutputSetData");

			// Proc Addresses for Parameters.  These are only implemented in the SVLptIO DLL.
			m_pSetParameterValue = (SVSetParameterValuePtr)::GetProcAddress( m_Handle, "SVSetParameterValue");
			m_pGetParameterValue = (SVGetParameterValuePtr)::GetProcAddress( m_Handle, "SVGetParameterValue" );
			m_pGetParameterName = (SVGetParameterNamePtr)::GetProcAddress( m_Handle, "SVGetParameterName" );
			m_pGetParameterCount = (SVGetParameterCountPtr)::GetProcAddress( m_Handle, "SVGetParameterCount" );


			//We do not check m_pSetOutputData that its not a nullptr because this is an optional function for PLC only
			if ( nullptr != m_pCreate &&
			     nullptr != m_pDestroy &&
			     nullptr != m_pGetInputCount &&
			     nullptr != m_pGetInputValue &&
			     nullptr != m_pSetInputValue &&
			     nullptr != m_pGetInputPortCount &&
			     nullptr != m_pGetInputPortValue &&
			     nullptr != m_pSetInputPortValue &&
			     nullptr != m_pGetOutputCount &&
			     nullptr != m_pGetOutputValue &&
			     nullptr != m_pSetOutputValue &&
			     nullptr != m_pGetOutputPortCount &&
			     nullptr != m_pGetOutputPortValue &&
			     nullptr != m_pSetOutputPortValue )
			{
				result = m_pCreate();
			}
			else
			{
				result = E_FAIL;
			}
		}
	}

	if ( S_OK != result )
	{
		Close();
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::Close()
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
			result = E_FAIL;
		}
	}

	m_pCreate = nullptr;
	m_pDestroy = nullptr;
	m_pGetInputCount = nullptr;
	m_pGetInputValue = nullptr;
	m_pSetInputValue = nullptr;
	m_pGetInputPortCount = nullptr;
	m_pGetInputPortValue = nullptr;
	m_pSetInputPortValue = nullptr;
	m_pGetOutputCount = nullptr;
	m_pGetOutputValue = nullptr;
	m_pSetOutputValue = nullptr;
	m_pGetOutputPortCount = nullptr;
	m_pGetOutputPortValue = nullptr;
	m_pSetOutputPortValue = nullptr;
	m_pSetOutputData = nullptr;
	m_pSetParameterValue = nullptr;

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::GetInputCount( unsigned long* pCount )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetInputCount )
	{
		result = m_pGetInputCount( pCount );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::GetInputValue( unsigned long channel, bool* pValue )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetInputValue )
	{
		result = m_pGetInputValue( channel, pValue );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::SetInputValue( unsigned long channel, bool value )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pSetInputValue )
	{
		result = m_pSetInputValue( channel, value );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::GetInputPortCount( unsigned long* pCount )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetInputPortCount )
	{
		result = m_pGetInputPortCount( pCount );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::GetInputPortValue( unsigned long port, unsigned long* pValue )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetInputPortValue )
	{
		result = m_pGetInputPortValue( port, pValue );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::SetInputPortValue( unsigned long port, unsigned long value )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pSetInputPortValue )
	{
		result = m_pSetInputPortValue( port, value );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::GetOutputCount( unsigned long* pCount )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetOutputCount )
	{
		result = m_pGetOutputCount( pCount );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::GetOutputValue( unsigned long channel, bool* pValue )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetOutputValue )
	{
		result = m_pGetOutputValue( channel, pValue );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::SetOutputValue( unsigned long channel, bool value )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pSetOutputValue )
	{
		result = m_pSetOutputValue( channel, value );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::GetOutputPortCount( unsigned long* pCount )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetOutputPortCount )
	{
		result = m_pGetOutputPortCount( pCount );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::GetOutputPortValue( unsigned long port, unsigned long* pValue )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetOutputPortValue )
	{
		result = m_pGetOutputPortValue( port, pValue );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::SetOutputPortValue( unsigned long port, unsigned long value )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pSetOutputPortValue )
	{
		result = m_pSetOutputPortValue( port, value );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::SetOutputData(unsigned long channel, const IntVariantMap& rData)
{
	HRESULT result {E_FAIL};

	if (nullptr != m_pSetOutputData)
	{
		result = m_pSetOutputData(channel, rData);
	}

	return result;
}

// Parameter Functions Follow.
HRESULT SVIODigitalLoadLibraryClass::GetParameterCount( unsigned long* pCount )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetParameterCount )
	{
		result = m_pGetParameterCount( pCount );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::GetParameterName( unsigned long index, BSTR* pName )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetParameterName )
	{
		result = m_pGetParameterName( index, pName );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::GetParameterValue( unsigned long index, VARIANT* pValue )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pGetParameterValue )
	{
		result = m_pGetParameterValue( index, pValue );
	}

	return result;
}

HRESULT SVIODigitalLoadLibraryClass::SetParameterValue( unsigned long index, VARIANT* pValue )
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pSetParameterValue )
	{
		result = m_pSetParameterValue( index, pValue );
	}

	return result;
}
