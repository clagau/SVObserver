// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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

HRESULT SVIODigitalLoadLibraryClass::Open(HMODULE libraryHandle)
{
	HRESULT result {nullptr != libraryHandle ? S_OK : E_FAIL};

	if (S_OK == result && libraryHandle != m_Handle)
	{
		m_Handle = libraryHandle;
		if ( nullptr != m_Handle )
		{
			m_pCreate = (SVCreatePtr)::GetProcAddress( m_Handle, "SVCreate" );
			m_pDestroy = (SVDestroyPtr)::GetProcAddress( m_Handle, "SVDestroy" );
			m_pGetInputCount = (SVInputGetCountPtr)::GetProcAddress( m_Handle, "SVInputGetCount" );
			m_pGetInputValue = (SVInputGetValuePtr)::GetProcAddress( m_Handle, "SVInputGetValue" );
			m_pGetInputValues = (SVInputGetValuesPtr)::GetProcAddress( m_Handle, "SVInputGetValues" );
			m_pGetOutputCount = (SVOutputGetCountPtr)::GetProcAddress( m_Handle, "SVOutputGetCount" );
			m_pSetOutputValue = (SVOutputSetValuePtr)::GetProcAddress( m_Handle, "SVOutputSetValue" );
			m_pGetOutputPortCount = (SVOutputGetPortCountPtr)::GetProcAddress( m_Handle, "SVOutputGetPortCount" );
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
			     nullptr != m_pGetInputValues &&
			     nullptr != m_pGetOutputCount &&
			     nullptr != m_pSetOutputValue &&
			     nullptr != m_pGetOutputPortCount &&
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
		m_Handle = nullptr;
	}

	m_pCreate = nullptr;
	m_pDestroy = nullptr;
	m_pGetInputCount = nullptr;
	m_pGetInputValue = nullptr;
	m_pGetInputValues = nullptr;
	m_pGetOutputCount = nullptr;
	m_pSetOutputValue = nullptr;
	m_pGetOutputPortCount = nullptr;
	m_pSetOutputPortValue = nullptr;
	m_pSetOutputData = nullptr;
	m_pSetParameterValue = nullptr;

	return result;
}

unsigned long SVIODigitalLoadLibraryClass::GetInputCount() const
{
	unsigned long result{ 0UL };

	if ( nullptr != m_pGetInputCount )
	{
		result = m_pGetInputCount();
	}
	return result;
}

bool SVIODigitalLoadLibraryClass::GetInputValue( unsigned long channel) const
{
	bool result{ false };

	if ( nullptr != m_pGetInputValue )
	{
		result = m_pGetInputValue(channel);
	}
	return result;
}

unsigned long SVIODigitalLoadLibraryClass::GetInputValues() const
{
	unsigned long result {0UL};

	if ( nullptr != m_pGetInputValues )
	{
		result = m_pGetInputValues();
	}
	return result;
}

unsigned long SVIODigitalLoadLibraryClass::GetOutputCount() const
{
	unsigned long result{ 0UL };

	if ( nullptr != m_pGetOutputCount )
	{
		result = m_pGetOutputCount();
	}
	return result;
}

HRESULT SVIODigitalLoadLibraryClass::SetOutputValue(unsigned long channel, bool value)
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pSetOutputValue )
	{
		result = m_pSetOutputValue( channel, value );
	}

	return result;
}

// cppcheck-suppress unusedFunction
unsigned long SVIODigitalLoadLibraryClass::GetOutputPortCount() const
{
	unsigned long result{ 0UL };

	if ( nullptr != m_pGetOutputPortCount )
	{
		result = m_pGetOutputPortCount();
	}
	return result;
}

HRESULT SVIODigitalLoadLibraryClass::SetOutputPortValue(unsigned long port, unsigned long value)
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pSetOutputPortValue )
	{
		result = m_pSetOutputPortValue( port, value );
	}
	return result;
}

HRESULT SVIODigitalLoadLibraryClass::SetOutputData(unsigned long channel, const TriggerData& rData)
{
	HRESULT result {E_FAIL};

	if (nullptr != m_pSetOutputData)
	{
		result = m_pSetOutputData(channel, rData);
	}
	return result;
}

unsigned long SVIODigitalLoadLibraryClass::GetParameterCount() const
{
	unsigned long result{ 0UL };

	if ( nullptr != m_pGetParameterCount )
	{
		result = m_pGetParameterCount();
	}
	return result;
}

_variant_t SVIODigitalLoadLibraryClass::GetParameterName(unsigned long index) const
{
	_variant_t result;

	if ( nullptr != m_pGetParameterName )
	{
		result = m_pGetParameterName(index);
	}
	return result;
}

_variant_t SVIODigitalLoadLibraryClass::GetParameterValue( unsigned long index) const
{
	_variant_t result;

	if ( nullptr != m_pGetParameterValue )
	{
		result = m_pGetParameterValue( index);
	}
	return result;
}

HRESULT SVIODigitalLoadLibraryClass::SetParameterValue(unsigned long index, const _variant_t& rValue)
{
	HRESULT result {E_FAIL};

	if ( nullptr != m_pSetParameterValue )
	{
		result = m_pSetParameterValue(index, rValue);
	}
	return result;
}
