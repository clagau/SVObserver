//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObjectList
//* .File Name       : $Workfile:   SVValueObjectList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:40:58  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVValueObjectList.h"
#pragma endregion Includes

SVValueObjectList::SVValueObjectList()
{
}

HRESULT SVValueObjectList::FinalConstruct()
{
	return S_OK;
}

void SVValueObjectList::FinalRelease()
{
	m_coll.clear();
}

// Random Access method
// if index is a String then it's a keyed access
// else it's an ordinal
STDMETHODIMP SVValueObjectList::get_Item(VARIANT index, VARIANT* pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	HRESULT hr = S_OK;
	CComVariant var;

	if (index.vt != VT_BSTR)
	{
		// If the index isn't a string, but can be converted to a long value, we use it as an ordinal
		hr = var.ChangeType(VT_I4, &index);
		if (SUCCEEDED(hr))
		{
			if( 0 <= index.lVal && index.lVal < static_cast< long >( m_coll.size() ) )
			{
				SVValueObjectListCollection::ContainerType::const_iterator it =  m_coll.begin();
				std::advance(it, index.lVal);
				if (it != m_coll.end())
				{
					SVValueObjectListCollection::EnumeratorCopyType::copy(pVal, &*it);
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
STDMETHODIMP SVValueObjectList::Remove(LONG index)
{
	HRESULT hr = S_OK;

	SVValueObjectListCollection::ContainerType::iterator it = m_coll.begin();

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
STDMETHODIMP SVValueObjectList::Clear(void)
{
	m_coll.clear();
	
	return S_OK;
}

STDMETHODIMP SVValueObjectList::Add(ISVValueObject* p_Item)
{
	m_coll.push_back(CComPtr<ISVValueObject>(p_Item));

	return S_OK;
}
