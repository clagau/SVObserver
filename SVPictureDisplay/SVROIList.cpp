//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
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
// CSVROIList

IMPLEMENT_DYNCREATE(CSVROIList, CCmdTarget)
// The coclass uuid
IMPLEMENT_OLECREATE(CSVROIList, "SVROILIST", 0xB861BF63, 0xCD67, 0x4de7, 0xA6, 0x21, 0xCB, 0x46, 0x08, 0x8B, 0x00, 0xC3)

BEGIN_INTERFACE_MAP(CSVROIList, CCmdTarget)
    INTERFACE_PART(CSVROIList, IID_ISVROIList, Dispatch)
    INTERFACE_PART(CSVROIList, IID_IEnumVARIANT, EnumVARIANT)
END_INTERFACE_MAP()

BEGIN_DISPATCH_MAP(CSVROIList, CCmdTarget)
    //{{AFX_DISPATCH_MAP(CSVROIList)
    //}}AFX_DISPATCH_MAP
    DISP_FUNCTION_ID(CSVROIList, "Item", 0, GetItem, VT_DISPATCH, VTS_I4)
    DISP_DEFVALUE_ID(CSVROIList, 0)
    DISP_PROPERTY_EX_ID(CSVROIList, "Count", 1, GetCount, SetNotSupported, VT_I4)
    DISP_FUNCTION_ID(CSVROIList, "Add", 2, Add, VT_I4, VTS_DISPATCH)
    DISP_FUNCTION_ID(CSVROIList, "Remove", 3, Remove, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(CSVROIList, "RemoveAll", 4, RemoveAll, VT_I4, VTS_NONE)
    DISP_PROPERTY_EX_ID(CSVROIList, "_NewEnum", DISPID_NEWENUM, _NewEnum, SetNotSupported, VT_UNKNOWN)
END_DISPATCH_MAP()

BEGIN_MESSAGE_MAP(CSVROIList, CCmdTarget)
    //{{AFX_MSG_MAP(CSVROIList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSVROIList::CSVROIList()
{
    m_arROIObjects.RemoveAll();

    EnableAutomation();
    AfxOleLockApp();
}

CSVROIList::~CSVROIList()
{
    RemoveAll();
    AfxOleUnlockApp();
}

void CSVROIList::OnFinalRelease()
{
    // When the last reference for an automation object is released
    //  OnFinalRelease is called.  This implementation deletes the 
    //  object.  Add additional cleanup required for your object before
    //  deleting it from memory.
    delete this;
}

/*static*/ CSVROIList* CSVROIList::GetCppObjectFromDispatchPointer(LPDISPATCH lpDispatch)
{
    static size_t offset = offsetof(CSVROIList, m_xDispatch);
    return reinterpret_cast <CSVROIList*> ( lpDispatch ? ((BYTE*)lpDispatch - offset) : NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CSVROIList message handlers

long CSVROIList::GetCount()
{
	return (long)m_arROIObjects.GetSize();
}

long CSVROIList::Add(LPDISPATCH pValue)
{
    pValue->AddRef();
	long lIndex = (long)m_arROIObjects.Add( pValue );

    // to appease VB programmers,
    lIndex++;

	return lIndex;
}

HRESULT CSVROIList::Remove(long lIndex)
{
    LPDISPATCH pValue;

    // to appease VB programmers,
    lIndex--;

	if( lIndex < 0 || lIndex >= m_arROIObjects.GetSize() )
	{
        ASSERT(FALSE);
		// Throw some exception here
		return S_FALSE;
	}// end if

	pValue = m_arROIObjects.GetAt( lIndex );
	pValue->Release();

    m_arROIObjects.RemoveAt( lIndex );

    return S_OK;
}

HRESULT CSVROIList::RemoveAll()
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

LPDISPATCH CSVROIList::GetItem(long lIndex)
{
    LPDISPATCH pValue;

    // to appease VB programmers,
    lIndex--;

	if( lIndex < 0 || lIndex >= m_arROIObjects.GetSize() )
	{
        ASSERT(FALSE);
        // Throw some exception here
		return (LPDISPATCH) VT_EMPTY;
	}

 	pValue = m_arROIObjects.GetAt( lIndex );
    pValue->AddRef();

    return pValue;
}
  
LPUNKNOWN CSVROIList::_NewEnum()
{
    CSVROIList* pList = new CSVROIList();

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
    return NULL;
}

//----------------------------------------------------------------------------
// Implementation of the nested class XEnumVARIANT (declared through the
// BEGIN/END_INTERFACE_PART macros in the declaration of CValueObjectList)
//
// This class implements the IEnumVARIANT interface requried by the 
// _NewEnum property of the collection
//
CSVROIList::XEnumVARIANT::XEnumVARIANT()
{
    m_lCurrent = 0;
}

STDMETHODIMP_(ULONG) CSVROIList::XEnumVARIANT::AddRef()
{
    METHOD_PROLOGUE(CSVROIList, EnumVARIANT)
    return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CSVROIList::XEnumVARIANT::Release()
{
    METHOD_PROLOGUE(CSVROIList, EnumVARIANT)
    return pThis->ExternalRelease();
}

STDMETHODIMP CSVROIList::XEnumVARIANT::QueryInterface( REFIID iid, void FAR* FAR* ppvObj )
{
    METHOD_PROLOGUE(CSVROIList, EnumVARIANT)
    return (HRESULT)pThis->ExternalQueryInterface( (void FAR*)&iid, ppvObj);
}

STDMETHODIMP CSVROIList::XEnumVARIANT::Next( ULONG celt, VARIANT FAR* rgvar, ULONG FAR* pceltFetched)
{
    METHOD_PROLOGUE(CSVROIList, EnumVARIANT)

    HRESULT hr = S_OK;
    ULONG l = 0;

    // pceltFetched can legally == 0
    if (pceltFetched != NULL)
	{
        *pceltFetched = 0;
	}
    else if (celt > 1)
    {   
        TRACE(_T("XEnumVARIANT::Next() celt > 1 and pceltFetched == NULL!\r\n"), celt );
        return ResultFromScode( E_INVALIDARG );
    }

    for (l=0; l < celt; l++)
        VariantInit( &rgvar[l] );

    // Retrieve the next celt elements.
    hr = NOERROR;
	long lSize = (long)pThis->m_arROIObjects.GetSize();
    for (l = 0; m_lCurrent >= 0 && m_lCurrent < lSize && celt != 0; l++)
    {   
        LPDISPATCH pItem = NULL;
        pItem = pThis->m_arROIObjects.GetAt( m_lCurrent++ );
        celt--;
        if (pItem)
        {
 	        pItem->AddRef();
            rgvar[l].vt = VT_DISPATCH;
            rgvar[l].pdispVal = pItem;
            if (pceltFetched != NULL)
                (*pceltFetched)++;
        }
        else 
        {
            TRACE(_T("GetNext failed in IEnumVARIANT::Next\r\n") );
            return ResultFromScode( E_UNEXPECTED );
        }
    }
    
    if (celt != 0)
    {
        hr = ResultFromScode( S_FALSE );
    }
    
    return hr;
}

STDMETHODIMP CSVROIList::XEnumVARIANT::Skip(unsigned long celt) 
{
    METHOD_PROLOGUE(CSVROIList, EnumVARIANT)

	long lSize = (long)pThis->m_arROIObjects.GetSize();
	while (m_lCurrent < lSize && celt--) { m_lCurrent++; }

    return (celt == 0 ? NOERROR : ResultFromScode( S_FALSE ));
}

STDMETHODIMP CSVROIList::XEnumVARIANT::Reset()
{
    METHOD_PROLOGUE(CSVROIList, EnumVARIANT)

    m_lCurrent = 0;

    return NOERROR;
}

STDMETHODIMP CSVROIList::XEnumVARIANT::Clone(IEnumVARIANT FAR* FAR* ppenum)
{
    METHOD_PROLOGUE(CSVROIList, EnumVARIANT)

    CSVROIList* pList = new CSVROIList();
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

