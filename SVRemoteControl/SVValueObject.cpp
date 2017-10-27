//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObject
//* .File Name       : $Workfile:   SVValueObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:40:56  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVValueObject.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#pragma endregion Includes

const wchar_t * quote	= L"`";
const wchar_t * sep		= L", ";

SVValueObject::SVValueObject()
: m_Name("")
, m_TriggerCount(-1)
, m_Status(S_FALSE)
{
}

HRESULT SVValueObject::FinalConstruct()
{
	return S_OK;
}

void SVValueObject::FinalRelease()
{
	m_coll.clear();
}

STDMETHODIMP SVValueObject::get_Name(BSTR* pVal)
{
	if (nullptr == pVal)
	{
		return E_POINTER;
	}

	if (*pVal)
	{
		::SysFreeString(*pVal);
	}

	*pVal = m_Name.copy();	// assign copy
	return S_OK;
}

STDMETHODIMP SVValueObject::put_Name(BSTR newVal)
{
	m_Name = newVal;
	return S_OK;
}

STDMETHODIMP SVValueObject::get_TriggerCount(LONG* pVal)
{
	if (nullptr == pVal)
	{
		return E_POINTER;
	}

	*pVal = m_TriggerCount;
	return S_OK;
}

STDMETHODIMP SVValueObject::put_TriggerCount(LONG newVal)
{
	m_TriggerCount = newVal;
	return S_OK;
}

STDMETHODIMP SVValueObject::get_Status(LONG* pVal)
{
	if (nullptr == pVal)
	{
		return E_POINTER;
	}

	*pVal = m_Status;
	return S_OK;
}

STDMETHODIMP SVValueObject::put_Status(LONG newVal)
{
	m_Status = newVal;
	return S_OK;
}

// Random Access method
// if index is a String then it's a keyed access
// else it's an ordinal
STDMETHODIMP SVValueObject::get_Item(VARIANT index, VARIANT* pVal)
{
	if (nullptr == pVal)
	{
		return E_POINTER;
	}

	HRESULT hr = S_OK;
	CComVariant var;

	if (index.vt != VT_BSTR)
	{
		// If the index isn't a string, but can be converted to a long value, we use it as an ordinal
		hr = var.ChangeType(VT_I4, &index);
		if (SUCCEEDED(hr))
		{
			if (0 <= index.lVal && index.lVal < static_cast<long>(m_coll.size()))
			{
				SVValueObjectCollection::ContainerType::const_iterator it =  m_coll.begin();
				std::advance(it, index.lVal);
				if (it != m_coll.end())
				{
					SVValueObjectCollection::EnumeratorCopyType::copy(pVal, &*it);
				}
				else 	// We got to the end without finding it
				{
					::VariantClear(pVal);
					hr = E_FAIL;
				}
			}
			else
			{
				::VariantClear(pVal);
				hr = E_FAIL;
			}
		}
	}
	else
	{
		// Otherwise, we assume index is a key into the collection
		// need to convert string to long for our usage
		::VariantClear(pVal);
		hr = E_FAIL;
	}
	return hr;
}

// Remove Access Point - index is an ordinal
STDMETHODIMP SVValueObject::Remove(LONG index)
{
	HRESULT hr = S_OK;

	SVValueObjectCollection::ContainerType::iterator it = m_coll.begin();

	// index versus key, hmmm...
	std::advance(it, index);

	if (it == m_coll.end())	// We got to the end without finding it
	{
		hr = E_FAIL;
	}
	else
	{
//		Reset();
		m_coll.erase(it);
	}
	return hr;
}

// Clear the internal collection
STDMETHODIMP SVValueObject::Clear(void)
{
	m_coll.clear();
	
	return S_OK;
}

STDMETHODIMP SVValueObject::Add(VARIANT p_Item)
{
	m_coll.push_back(CComVariant(p_Item));

	return S_OK;
}
