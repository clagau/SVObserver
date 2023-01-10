//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVROIList
//* .File Name       : $Workfile:   SVROIList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:28:58  $
//******************************************************************************

#include "stdafx.h"
#include "SVROIList.h"

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

/////////////////////////////////////////////////////////////////////////////
// SVROIList

IMPLEMENT_DYNCREATE(SVROIList, CCmdTarget)
// The coclass uuid
IMPLEMENT_OLECREATE(SVROIList, "SVROILIST", 0xB861BF63, 0xCD67, 0x4de7, 0xA6, 0x21, 0xCB, 0x46, 0x08, 0x8B, 0x00, 0xC3)

BEGIN_INTERFACE_MAP(SVROIList, CCmdTarget)
    INTERFACE_PART(SVROIList, IID_ISVROIList, Dispatch)
    INTERFACE_PART(SVROIList, IID_IEnumVARIANT, EnumVARIANT)
END_INTERFACE_MAP()

BEGIN_DISPATCH_MAP(SVROIList, CCmdTarget)
    //{{AFX_DISPATCH_MAP(SVROIList)
    //}}AFX_DISPATCH_MAP
    DISP_FUNCTION_ID(SVROIList, "Item", 0, GetItem, VT_DISPATCH, VTS_I4)
    DISP_DEFVALUE_ID(SVROIList, 0)
    DISP_PROPERTY_EX_ID(SVROIList, "Count", 1, GetCount, SetNotSupported, VT_I4)
    DISP_FUNCTION_ID(SVROIList, "Add", 2, Add, VT_I4, VTS_DISPATCH)
    DISP_FUNCTION_ID(SVROIList, "Remove", 3, Remove, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(SVROIList, "RemoveAll", 4, RemoveAll, VT_I4, VTS_NONE)
    DISP_PROPERTY_EX_ID(SVROIList, "_NewEnum", DISPID_NEWENUM, _NewEnum, SetNotSupported, VT_UNKNOWN)
END_DISPATCH_MAP()

BEGIN_MESSAGE_MAP(SVROIList, CCmdTarget)
    //{{AFX_MSG_MAP(SVROIList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVROIList::SVROIList()
{
    m_arROIObjects.RemoveAll();

    EnableAutomation();
    AfxOleLockApp();
}

SVROIList::~SVROIList()
{
    RemoveAll();
    AfxOleUnlockApp();
}

void SVROIList::OnFinalRelease()
{
    // When the last reference for an automation object is released
    //  OnFinalRelease is called.  This implementation deletes the 
    //  object.  Add additional cleanup required for your object before
    //  deleting it from memory.
    delete this;
}

/*static*/ SVROIList* SVROIList::GetCppObjectFromDispatchPointer(LPDISPATCH lpDispatch)
{
    static size_t offset = offsetof(SVROIList, m_xDispatch);
    return reinterpret_cast <SVROIList*> ( lpDispatch ? ((BYTE*)lpDispatch - offset) : nullptr );
}

/////////////////////////////////////////////////////////////////////////////
// SVROIList message handlers

long SVROIList::GetCount()
{
	return (long)m_arROIObjects.GetSize();
}

long SVROIList::Add(LPDISPATCH pValue)
{
    pValue->AddRef();
	long lIndex = (long)m_arROIObjects.Add( pValue );

    // to appease VB programmers,
    lIndex++;

	return lIndex;
}

HRESULT SVROIList::Remove(long lIndex)
{
    LPDISPATCH pValue;

    // to appease VB programmers,
    lIndex--;

	if( lIndex < 0 || lIndex >= m_arROIObjects.GetSize() )
	{
        Log_Assert(FALSE);
		// Throw some exception here
		return S_FALSE;
	}// end if

	pValue = m_arROIObjects.GetAt( lIndex );
	pValue->Release();

    m_arROIObjects.RemoveAt( lIndex );

    return S_OK;
}

HRESULT SVROIList::RemoveAll()
{
    LPDISPATCH pValue;

	long lSize = (long)m_arROIObjects.GetSize();
	for(long l = 0; l < lSize; l++ )
	{
		pValue = m_arROIObjects.GetAt( l );
		pValue->Release();
	}

    m_arROIObjects.RemoveAll();

    return S_OK;
}

LPDISPATCH SVROIList::GetItem(long lIndex)
{
    LPDISPATCH pValue;

    // to appease VB programmers,
    lIndex--;

	if( lIndex < 0 || lIndex >= m_arROIObjects.GetSize() )
	{
        Log_Assert(FALSE);
        // Throw some exception here
		return (LPDISPATCH) VT_EMPTY;
	}

 	pValue = m_arROIObjects.GetAt( lIndex );
    pValue->AddRef();

    return pValue;
}
  
LPUNKNOWN SVROIList::_NewEnum()
{
    SVROIList* pList = new SVROIList();

    if (pList)
    {
		// Copy all the items over to the new array
		long lSize = (long)m_arROIObjects.GetSize();
		for(long l = 0; l < lSize; l++ )
		{
			LPDISPATCH pValue = m_arROIObjects.GetAt( l );
			pList->Add( pValue );
		}

        pList->m_xEnumVARIANT.Reset();
        return &pList->m_xEnumVARIANT;
    }
    return nullptr;
}

//----------------------------------------------------------------------------
// Implementation of the nested class XEnumVARIANT (declared through the
// BEGIN/END_INTERFACE_PART macros in the declaration of CValueObjectList)
//
// This class implements the IEnumVARIANT interface requried by the 
// _NewEnum property of the collection
//
SVROIList::XEnumVARIANT::XEnumVARIANT()
{
    m_lCurrent = 0;
}

STDMETHODIMP_(ULONG) SVROIList::XEnumVARIANT::AddRef()
{
    METHOD_PROLOGUE(SVROIList, EnumVARIANT)
    return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) SVROIList::XEnumVARIANT::Release()
{
    METHOD_PROLOGUE(SVROIList, EnumVARIANT)
    return pThis->ExternalRelease();
}

STDMETHODIMP SVROIList::XEnumVARIANT::QueryInterface( REFIID iid, void FAR* FAR* ppvObj )
{
    METHOD_PROLOGUE(SVROIList, EnumVARIANT)
    return (HRESULT)pThis->ExternalQueryInterface( (void FAR*)&iid, ppvObj);
}

STDMETHODIMP SVROIList::XEnumVARIANT::Next( ULONG celt, VARIANT FAR* rgvar, ULONG FAR* pceltFetched)
{
    METHOD_PROLOGUE(SVROIList, EnumVARIANT)

    HRESULT hr = S_OK;
    ULONG l = 0;

    // pceltFetched can legally == 0
    if (nullptr != pceltFetched)
	{
        *pceltFetched = 0;
	}
    else if (celt > 1)
    {   
#if defined (TRACE_THEM_ALL) || defined (TRACE_OTHER)
        TRACE(_T("XEnumVARIANT::Next() celt > 1 and nullptr == pceltFetched !\r\n"), celt );
#endif
        return ResultFromScode( E_INVALIDARG );
    }

    for (l=0; l < celt; l++)
        VariantInit( &rgvar[l] );

    // Retrieve the next celt elements.
    hr = NOERROR;
	long lSize = (long)pThis->m_arROIObjects.GetSize();
    for (l = 0; m_lCurrent >= 0 && m_lCurrent < lSize && celt != 0; l++)
    {   
        LPDISPATCH pItem = nullptr;
        pItem = pThis->m_arROIObjects.GetAt( m_lCurrent++ );
        celt--;
        if (pItem)
        {
 	        pItem->AddRef();
            rgvar[l].vt = VT_DISPATCH;
            rgvar[l].pdispVal = pItem;
            if (nullptr != pceltFetched )
			{
                (*pceltFetched)++;
			}
        }
        else 
        {
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
            TRACE(_T("GetNext failed in IEnumVARIANT::Next\r\n") );
#endif
            return ResultFromScode( E_UNEXPECTED );
        }
    }
    
    if (celt != 0)
    {
        hr = ResultFromScode( S_FALSE );
    }
    
    return hr;
}

STDMETHODIMP SVROIList::XEnumVARIANT::Skip(unsigned long celt) 
{
    METHOD_PROLOGUE(SVROIList, EnumVARIANT)

	long lSize = (long)pThis->m_arROIObjects.GetSize();
	while (m_lCurrent < lSize && celt--) { m_lCurrent++; }

    return (celt == 0 ? NOERROR : ResultFromScode( S_FALSE ));
}

STDMETHODIMP SVROIList::XEnumVARIANT::Reset()
{
    METHOD_PROLOGUE(SVROIList, EnumVARIANT)

    m_lCurrent = 0;

    return NOERROR;
}

STDMETHODIMP SVROIList::XEnumVARIANT::Clone(IEnumVARIANT FAR* FAR*)
{
    METHOD_PROLOGUE(SVROIList, EnumVARIANT)

    SVROIList* pList = new SVROIList();
    if (pList)
    {
		// Copy all the items over to the new array
		long lSize = (long)pThis->m_arROIObjects.GetSize();
		for(long l = 0; l < lSize; l++ )
		{
			LPDISPATCH pValue = pThis->m_arROIObjects.GetAt( l );
			pList->Add( pValue );
		}// end for

		pList->m_xEnumVARIANT.m_lCurrent = m_lCurrent;
        return NOERROR;
    }
	return ResultFromScode( E_OUTOFMEMORY );
}

