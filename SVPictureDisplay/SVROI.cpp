//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVROI
//* .File Name       : $Workfile:   SVROI.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:28:56  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVROI.h"
#include "SVOMFCLibrary\DisplayHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

IMPLEMENT_DYNCREATE(CSVROI, CCmdTarget)
// The coclass uuid
IMPLEMENT_OLECREATE(CSVROI, "SVROI",  0x2105ADD0, 0x7C8B, 0x4ec7, 0xA8, 0x75, 0x54, 0x90, 0x5D, 0x47, 0x72, 0x5B)

BEGIN_INTERFACE_MAP(CSVROI, CCmdTarget)
	INTERFACE_PART(CSVROI, IID_ISVROI, Dispatch)
// because we go through IDispatch, we don't implement a regular interface
//    INTERFACE_PART(CSVROI, IID_ISVROI, ISVROI)
END_INTERFACE_MAP()

BEGIN_DISPATCH_MAP(CSVROI, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CSVROI)
	//}}AFX_DISPATCH_MAP
	DISP_PROPERTY_EX_ID(CSVROI, "Group", dispidGroup, GetParent, SetNotSupported, VT_I4)
	DISP_FUNCTION_ID(CSVROI, "Set Overlay", dispidSetOverlay, SetOverlay, VT_HRESULT, VTS_PVARIANT VTS_PVARIANT VTS_I4)
END_DISPATCH_MAP()

BEGIN_MESSAGE_MAP(CSVROI, CCmdTarget)
	//{{AFX_MSG_MAP(CSVROI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSVROI::CSVROI()
: m_Parent(0)
{
	EnableAutomation();
	AfxOleLockApp();
}

CSVROI::~CSVROI()
{
	AfxOleUnlockApp();
}

void CSVROI::OnFinalRelease()
{
	// When the last reference for an automation object is released
	//  OnFinalRelease is called.  This implementation deletes the 
	//  object.  Add additional cleanup required for your object before
	//  deleting it from memory.

	delete this;
}

/*static*/ CSVROI* CSVROI::GetCppObjectFromDispatchPointer(LPDISPATCH lpDispatch)
{
	static size_t offset = offsetof(CSVROI, m_xDispatch);
	return reinterpret_cast <CSVROI*> ( lpDispatch ? ((BYTE*)lpDispatch - offset) : nullptr );
}

////////////////////////////////////////////////////////////
// OLE Automation methods/properties
//
long CSVROI::GetParent() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_Parent;
}

VariantParamMap CSVROI::GetParameterMap() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_ParameterMap;
}

HRESULT CSVROI::SetOverlay(VARIANT* pParameterList, VARIANT* pParameterValue, LONG bAllowEdit)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_Parent = 0;
	Seidenader::SVOMFCLibrary::DisplayHelper::FillParameterMap(m_ParameterMap, pParameterList, pParameterValue);

	return S_OK;
}

