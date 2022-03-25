//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVOGui/DisplayHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

IMPLEMENT_DYNCREATE(SVROI, CCmdTarget)
// The coclass uuid
IMPLEMENT_OLECREATE(SVROI, "SVROI",  0x2105ADD0, 0x7C8B, 0x4ec7, 0xA8, 0x75, 0x54, 0x90, 0x5D, 0x47, 0x72, 0x5B)

BEGIN_INTERFACE_MAP(SVROI, CCmdTarget)
	INTERFACE_PART(SVROI, IID_ISVROI, Dispatch)
// because we go through IDispatch, we don't implement a regular interface
//    INTERFACE_PART(SVROI, IID_ISVROI, ISVROI)
END_INTERFACE_MAP()

BEGIN_DISPATCH_MAP(SVROI, CCmdTarget)
	//{{AFX_DISPATCH_MAP(SVROI)
	//}}AFX_DISPATCH_MAP
	DISP_PROPERTY_EX_ID(SVROI, "Group", dispidGroup, GetParent, SetNotSupported, VT_I4)
	DISP_FUNCTION_ID(SVROI, "Set Overlay", dispidSetOverlay, SetOverlay, VT_HRESULT, VTS_PVARIANT VTS_PVARIANT VTS_I4)
END_DISPATCH_MAP()

BEGIN_MESSAGE_MAP(SVROI, CCmdTarget)
	//{{AFX_MSG_MAP(SVROI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVROI::SVROI()
: m_Parent(0)
{
	EnableAutomation();
	AfxOleLockApp();
}

SVROI::~SVROI()
{
	AfxOleUnlockApp();
}

void SVROI::OnFinalRelease()
{
	// When the last reference for an automation object is released
	//  OnFinalRelease is called.  This implementation deletes the 
	//  object.  Add additional cleanup required for your object before
	//  deleting it from memory.

	delete this;
}

/*static*/ SVROI* SVROI::GetCppObjectFromDispatchPointer(LPDISPATCH lpDispatch)
{
	static size_t offset = offsetof(SVROI, m_xDispatch);
	return reinterpret_cast <SVROI*> ( lpDispatch ? ((BYTE*)lpDispatch - offset) : nullptr );
}

////////////////////////////////////////////////////////////
// OLE Automation methods/properties
//
long SVROI::GetParent() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_Parent;
}

VariantParamMap SVROI::GetParameterMap() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_ParameterMap;
}

HRESULT SVROI::SetOverlay(VARIANT* pParameterList, VARIANT* pParameterValue, LONG)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_Parent = 0;
	SvOg::DisplayHelper::FillParameterMap(m_ParameterMap, pParameterList, pParameterValue);

	return S_OK;
}

