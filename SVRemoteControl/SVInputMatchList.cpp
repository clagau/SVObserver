//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, In.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputMatchList
//* .File Name       : $Workfile:   SVInputMatchList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:39:02  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVInputMatchList.h"
#pragma endregion Includes

SVInputMatchList::SVInputMatchList()
{
}

HRESULT SVInputMatchList::FinalConstruct()
{
	return S_OK;
}

void SVInputMatchList::FinalRelease()
{
	m_coll.clear();
}

// Random Access method
// if index is a String then it's a keyed access
// else it's an ordinal
STDMETHODIMP SVInputMatchList::get_Item(VARIANT index, VARIANT* pVal)
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
				SVInputMatchItemCollection::ContainerType::const_iterator it =  m_coll.begin();
				std::advance(it, index.lVal);
				if (it != m_coll.end())
				{
					SVInputMatchItemCollection::EnumeratorCopyType::copy(pVal, &*it);
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
STDMETHODIMP SVInputMatchList::Remove(LONG index)
{
	HRESULT hr = S_OK;

	SVInputMatchItemCollection::ContainerType::iterator it = m_coll.begin();

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
STDMETHODIMP SVInputMatchList::Clear(void)
{
	m_coll.clear();
	
	return S_OK;
}

// Add Item
STDMETHODIMP SVInputMatchList::Add(ISVInputMatchItem* Item)
{
	CComPtr<ISVInputMatchItem> l_MatchItemPtr(Item);

	m_coll.push_back(l_MatchItemPtr);

	return S_OK;
}

// Add Item
STDMETHODIMP SVInputMatchList::AddItem(BSTR matchString, long txn_Identifier)
{
	CComPtr<ISVInputMatchItem> l_MatchItemPtr;

	HRESULT l_Status = l_MatchItemPtr.CoCreateInstance(__uuidof(SVInputMatchItem));

	if( l_Status == S_OK && l_MatchItemPtr.p != NULL )
	{
		l_Status = l_MatchItemPtr->put_MatchString( matchString );

		if( l_Status == S_OK )
		{
			l_Status = l_MatchItemPtr->put_TxnIdentifier( txn_Identifier );
		}

		if( l_Status == S_OK )
		{
			m_coll.push_back(l_MatchItemPtr);
		}
	}
	else
	{
		if( l_Status == S_OK )
		{
			l_Status = E_NOINTERFACE;
		}
	}

	return l_Status;
}
