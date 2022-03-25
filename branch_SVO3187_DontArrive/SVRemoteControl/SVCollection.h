//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCollection
//* .File Name       : $Workfile:   SVCollection.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:37:38  $
//******************************************************************************
#pragma once

#pragma region Includes
#include <algorithm>
#include <atlcom.h>
#pragma endregion Includes

namespace SVCollection
{
	struct _CopyBSTRAdapterToVariant
	{
		typedef CAdapt< CComBSTR > Adapter;

		static HRESULT copy( VARIANT* pTo, const Adapter* pFrom )
		{
			return CComVariant( pFrom->m_T ).Detach( pTo );
		}
		static void init(VARIANT* p)
		{
			_Copy<VARIANT>::init(p);
		}
		static void destroy(VARIANT* p)
		{
			_Copy<VARIANT>::destroy(p);
		}
	};

	struct _CopyBSTRAdapterToBSTR
	{
		typedef CAdapt< CComBSTR > Adapter;

		static HRESULT copy( BSTR* pTo, const Adapter* pFrom )
		{
			*pTo = pFrom->m_T.Copy();

			return S_OK;
		}
		static void init(BSTR* p)
		{
			_Copy<BSTR>::init(p);
		}
		static void destroy(BSTR* p)
		{
			_Copy<BSTR>::destroy(p);
		}
	};

	template<typename T>
	struct _CopyCollectionEntryToVariant
	{
		typedef CComPtr<T> ComType;
		typedef CAdapt<ComType> Adapter;

		static HRESULT copy(VARIANT* pCopy, const Adapter* pIt)
		{
			HRESULT hr = pIt->m_T->QueryInterface(IID_IDispatch, (void **)&pCopy->pdispVal);
			if (hr == S_OK)
			{
				pCopy->vt = VT_DISPATCH;
			}
			else
			{
				hr = pIt->m_T->QueryInterface(IID_IUnknown, (void **)&pCopy->punkVal);
				if (hr == S_OK)
				{
					pCopy->vt = VT_UNKNOWN;
				}
			}
			return hr;
		}
		static void init(VARIANT* p) { VariantInit(p); }
		static void destroy(VARIANT* p) { VariantClear(p); }
	};

	typedef VARIANT ItemType;
	typedef IEnumVARIANT EnumeratorInterface;
	
	template<typename Iterator, typename Compare, typename Type>
	Iterator find(Iterator b, Iterator e, Type value)
	{
		return std::find_if(b, e, Compare(value));
	}

	//
	template<typename T>
	struct _CopyItFromAdapter
	{
		typedef CComPtr<T> ComType;
		typedef CAdapt<ComType> Adapter;

		static HRESULT copy(T** p1, const Adapter* p2)
		{
			HRESULT hr = E_POINTER;
			if (nullptr != (*p1 = p2->m_T))
			{
				hr = (*p1)->AddRef();
			}
			return hr;
		}
		static void init(T** p) {}
		static void destroy(T** p) 
		{
			if (*p)
			{
				(*p)->Release();
			}
		}
	};
}
