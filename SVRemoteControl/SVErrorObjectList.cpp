//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVErrorObjectList
//* .File Name       : $Workfile:   SVErrorObjectList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 May 2013 14:36:10  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVErrorObjectList.h"
#pragma endregion Includes

SVErrorObjectList::SVErrorObjectList()
{
}

HRESULT SVErrorObjectList::FinalConstruct()
{
	return S_OK;
}

void SVErrorObjectList::FinalRelease()
{
	m_coll.clear();
}

// Random Access method
// if index is a String then it's a keyed access
// else it's an ordinal
STDMETHODIMP SVErrorObjectList::get_Item(VARIANT index, VARIANT* pVal)
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
				SVErrorObjectListCollection::ContainerType::const_iterator it =  m_coll.begin();
				std::advance(it, index.lVal);
				if (it != m_coll.end())
				{
					SVErrorObjectListCollection::EnumeratorCopyType::copy(pVal, &*it);
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
STDMETHODIMP SVErrorObjectList::Remove(LONG index)
{
	HRESULT hr = S_OK;

	SVErrorObjectListCollection::ContainerType::iterator it = m_coll.begin();

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
STDMETHODIMP SVErrorObjectList::Clear(void)
{
	m_coll.clear();
	
	return S_OK;
}

STDMETHODIMP SVErrorObjectList::Add(ISVErrorObject* p_Item)
{
	m_coll.push_back(CComPtr<ISVErrorObject>(p_Item));

	return S_OK;
}
