//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataDefObject
//* .File Name       : $Workfile:   SVDataDefObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Nov 2013 18:57:56  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVDataDefObject.h"
#pragma endregion Includes

SVDataDefObject::SVDataDefObject()
{
}
	
HRESULT SVDataDefObject::FinalConstruct()
{
	return S_OK;
}

STDMETHODIMP SVDataDefObject::get_Name( BSTR* pVal )
{
	ATL::CComBSTR tmp;
	tmp.Attach( *pVal );
	tmp = m_Name;
	*pVal = tmp.Detach();
	return S_OK;
}

STDMETHODIMP SVDataDefObject::put_Name( BSTR newVal )
{
	m_Name = newVal;
	return S_OK;
}

STDMETHODIMP SVDataDefObject::get_Writable( VARIANT_BOOL* pVal )
{
	*pVal = m_Writable;
	return S_OK;
}

STDMETHODIMP SVDataDefObject::put_Writable( VARIANT_BOOL newVal )
{
	m_Writable = newVal;
	return S_OK;
}

STDMETHODIMP SVDataDefObject::get_Published( VARIANT_BOOL* pVal )
{
	*pVal = m_Published;
	return S_OK;
}

STDMETHODIMP SVDataDefObject::put_Published( VARIANT_BOOL newVal )
{
	m_Published = newVal;
	return S_OK;
}

STDMETHODIMP SVDataDefObject::get_DataType( BSTR* pVal )
{
	ATL::CComBSTR tmp;
	tmp.Attach( *pVal );
	tmp = m_DataType;
	*pVal = tmp.Detach();
	return S_OK;
}

STDMETHODIMP SVDataDefObject::put_DataType( BSTR newVal )
{
	m_DataType = newVal;
	return S_OK;
}

// Random Access method
// if index is a String then it's a keyed access
// else it's an ordinal
STDMETHODIMP SVDataDefObject::get_Item( VARIANT index, VARIANT* pVal )
{
	if ( pVal == NULL )
		return E_POINTER;

	HRESULT hr = S_OK;
	CComVariant var;

	if ( index.vt != VT_BSTR )
	{
		// If the index isn't a string, but can be converted to a long value, we use it as an ordinal
		hr = var.ChangeType( VT_I4, &index );
		if ( SUCCEEDED( hr ) )
		{
			if( 0 <= index.lVal && index.lVal < static_cast< long >( m_coll.size() ) )
			{
				SVDataDefObjectCollection::ContainerType::const_iterator it =  m_coll.begin();
				std::advance( it, index.lVal );
				if ( it != m_coll.end() )
				{
					SVDataDefObjectCollection::EnumeratorCopyType::copy( pVal, &*it );
				}
				else 	// We got to the end without finding it
				{
					::VariantClear( pVal );
					hr = E_FAIL;
				}
			}
			else
			{
				::VariantClear( pVal );
				hr = E_FAIL;
			}
		}
	}
	else
	{
		// Otherwise, we assume index is a key into the collection
		// need to convert string to long for our usage
		::VariantClear( pVal );
		hr = E_FAIL;
	}
	return hr;
}

// Remove Access Point - index is an ordinal
STDMETHODIMP SVDataDefObject::Remove( LONG index )
{
	HRESULT hr = S_OK;

	SVDataDefObjectCollection::ContainerType::iterator it = m_coll.begin();

	// index versus key, hmmm...
	std::advance( it, index );

	if ( it == m_coll.end() )	// We got to the end without finding it
	{
		hr = E_FAIL;
	}
	else
	{
		m_coll.erase( it );
	}
	return hr;
}

// Clear the internal collection
STDMETHODIMP SVDataDefObject::Clear()
{
	m_coll.clear();
	
	return S_OK;
}

STDMETHODIMP SVDataDefObject::Add( VARIANT p_Item )
{
	m_coll.push_back( CComVariant( p_Item ) );

	return S_OK;
}
