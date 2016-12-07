//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVROIList
//* .File Name       : $Workfile:   SVROIList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:28:58  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVROI.h"
#pragma endregion Includes
/////////////////////////////////////////////////////////////////////////////
// CSVROIList command target

// The dispinterface uuid
const IID BASED_CODE IID_ISVROIList =
	{0x058A6CA5, 0x044C, 0x46b1, {0xBB, 0x65, 0xCB, 0xEA, 0x5A, 0x39, 0xB8, 0xBC}};

class CSVROIList : public CCmdTarget
{
public:
#pragma region Constructor
    CSVROIList();
    virtual ~CSVROIList();
    virtual void OnFinalRelease() override;
#pragma endregion Constructor

    static CSVROIList* GetCppObjectFromDispatchPointer(LPDISPATCH lpDispatch);
    
// Attributes
protected:
	CArray <LPDISPATCH, LPDISPATCH> m_arROIObjects;

// Implementation
public:
    LPDISPATCH GetItem(long lIndex);
    long GetCount();
    long Add(LPDISPATCH pValue);
    HRESULT Remove(long lIndex);
    HRESULT RemoveAll();
    LPUNKNOWN _NewEnum();

    DECLARE_MESSAGE_MAP()
    DECLARE_DISPATCH_MAP()
    DECLARE_INTERFACE_MAP()

    DECLARE_DYNCREATE(CSVROIList)
    DECLARE_OLECREATE(CSVROIList)

	BEGIN_INTERFACE_PART(EnumVARIANT, IEnumVARIANT)
        XEnumVARIANT();				// constructor to set m_lCurrent
        long m_lCurrent;			// Next() requires that we keep track of our current item

        STDMETHOD(Next)(THIS_ unsigned long celt, VARIANT FAR* rgvar, unsigned long FAR* pceltFetched);
        STDMETHOD(Skip)(THIS_ unsigned long celt);
        STDMETHOD(Reset)(THIS);
        STDMETHOD(Clone)(THIS_ IEnumVARIANT FAR* FAR* ppenum);
    END_INTERFACE_PART(EnumVARIANT)

    // Generated message map functions
    //{{AFX_MSG(CSVROIList)
    //}}AFX_MSG

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CSVROIList)
    //}}AFX_DISPATCH
};

