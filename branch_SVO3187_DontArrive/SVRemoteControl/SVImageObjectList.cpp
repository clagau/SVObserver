//*****************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// COM Collection object for Images
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageObjectList.h"
#pragma endregion Includes

// Random Access method
// if index is a String then it's a keyed access
// else it's an ordinal
STDMETHODIMP SVImageObjectList::get_Item(VARIANT index, VARIANT* pVal)
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
				SVImageObjectListCollection::ContainerType::const_iterator it =  m_coll.begin();
				std::advance(it, index.lVal);
				if (it != m_coll.end())
				{
					SVImageObjectListCollection::EnumeratorCopyType::copy(pVal, &*it);
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
STDMETHODIMP SVImageObjectList::Remove(LONG index)
{
	HRESULT hr = S_OK;

	SVImageObjectListCollection::ContainerType::iterator it = m_coll.begin();

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
STDMETHODIMP SVImageObjectList::Clear(void)
{
	m_coll.clear();
	
	return S_OK;
}

STDMETHODIMP SVImageObjectList::Add(ISVImageObject* p_Item)
{
	m_coll.push_back(CComPtr<ISVImageObject>(p_Item));

	return S_OK;
}
