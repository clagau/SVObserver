// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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

namespace SvTrig
{
SVIOTriggerLoadLibraryClass::~SVIOTriggerLoadLibraryClass()
{
	Close();
}

HRESULT SVIOTriggerLoadLibraryClass::Open(LPCTSTR libraryPath)
{
	HRESULT result {S_OK};

	if (nullptr == m_Handle)
	{
		m_Handle = ::LoadLibrary(libraryPath);
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);

		if (nullptr != m_Handle)
		{
			m_pCreate = (SVCreatePtr)::GetProcAddress(m_Handle, "SVCreate");
			m_pDestroy = (SVDestroyPtr)::GetProcAddress(m_Handle, "SVDestroy");
			m_pGetCount = (SVTriggerGetCountPtr)::GetProcAddress(m_Handle, "SVTriggerGetCount");
			m_pGetHandle = (SVTriggerGetHandlePtr)::GetProcAddress(m_Handle, "SVTriggerGetHandle");
			m_pGetName = (SVTriggerGetNamePtr)::GetProcAddress(m_Handle, "SVTriggerGetName");
			m_pRegister = (SVTriggerRegisterPtr)::GetProcAddress(m_Handle, "SVTriggerRegister");
			m_pUnregister = (SVTriggerUnregisterPtr)::GetProcAddress(m_Handle, "SVTriggerUnregister");
			m_pStart = (SVTriggerStartPtr)::GetProcAddress(m_Handle, "SVTriggerStart");
			m_pStop = (SVTriggerStopPtr)::GetProcAddress(m_Handle, "SVTriggerStop");
			m_pGetParameterCount = (SVTriggerGetParameterCountPtr)::GetProcAddress(m_Handle, "SVTriggerGetParameterCount");
			m_pGetParameterName = (SVTriggerGetParameterNamePtr)::GetProcAddress(m_Handle, "SVTriggerGetParameterName");
			m_pGetParameterValue = (SVTriggerGetParameterValuePtr)::GetProcAddress(m_Handle, "SVTriggerGetParameterValue");
			m_pSetParameterValue = (SVTriggerSetParameterValuePtr)::GetProcAddress(m_Handle, "SVTriggerSetParameterValue");

			if (nullptr != m_pCreate &&
				 nullptr != m_pDestroy &&
				 nullptr != m_pGetCount &&
				 nullptr != m_pGetHandle &&
				 nullptr != m_pGetName &&
				 nullptr != m_pRegister &&
				 nullptr != m_pUnregister &&
				 nullptr != m_pStart &&
				 nullptr != m_pStop)
			{
				result = m_pCreate();
			}
			else
			{
				result = S_FALSE;
			}

		}
	}

	if (S_OK != result)
	{
		Close();
	}

	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::Close()
{
	HRESULT result {S_OK};

	if (nullptr != m_Handle)
	{
		if (nullptr != m_pDestroy)
		{
			result = m_pDestroy();
		}

		if (::FreeLibrary(m_Handle))
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

unsigned long SVIOTriggerLoadLibraryClass::GetCount() const
{
	unsigned long result {0UL};

	if (nullptr != m_pGetCount)
	{
		result = m_pGetCount();
	}
	return result;
}

unsigned long SVIOTriggerLoadLibraryClass::GetHandle(unsigned long index) const
{
	unsigned long result {0UL};

	if (nullptr != m_pGetHandle)
	{
		result = m_pGetHandle(index);
	}
	return result;
}

_variant_t SVIOTriggerLoadLibraryClass::GetName(unsigned long triggerIndex) const
{
	_variant_t result;

	if (nullptr != m_pGetName)
	{
		result = m_pGetName(triggerIndex);
	}
	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::Register(unsigned long triggerIndex, SvTrig::TriggerCallBack pTriggerCallback)
{
	HRESULT result {E_FAIL};

	if (nullptr != m_pRegister)
	{
		result = m_pRegister(triggerIndex, pTriggerCallback);
	}
	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::Unregister(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (nullptr != m_pUnregister)
	{
		result = m_pUnregister(triggerIndex);
	}
	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::Start(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (nullptr != m_pStart)
	{
		result = m_pStart(triggerIndex);
	}
	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::Stop(unsigned long triggerIndex)
{
	HRESULT result {E_FAIL};

	if (nullptr != m_pStop)
	{
		result = m_pStop(triggerIndex);
	}
	return result;
}

unsigned long SVIOTriggerLoadLibraryClass::GetParameterCount(unsigned long triggerIndex) const
{
	unsigned long result {0UL};

	if (nullptr != m_pGetParameterCount)
	{
		result = m_pGetParameterCount(triggerIndex);
	}
	return result;
}

_variant_t SVIOTriggerLoadLibraryClass::GetParameterName(unsigned long triggerIndex, unsigned long index) const
{
	_variant_t result;

	if (nullptr != m_pGetParameterName)
	{
		result = m_pGetParameterName(triggerIndex, index);
	}
	return result;
}

_variant_t SVIOTriggerLoadLibraryClass::GetParameterValue(unsigned long triggerIndex, unsigned long index) const
{
	_variant_t result;

	if (nullptr != m_pGetParameterValue)
	{
		result = m_pGetParameterValue(triggerIndex, index);
	}
	return result;
}

HRESULT SVIOTriggerLoadLibraryClass::SetParameterValue(unsigned long triggerIndex, unsigned long index, const _variant_t& rValue)
{
	HRESULT result {E_FAIL};

	if (nullptr != m_pSetParameterValue)
	{
		result = m_pSetParameterValue(triggerIndex, index, rValue);
	}

	return result;
}
}