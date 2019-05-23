//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVScrollWnd1Ctl
//* .File Name       : $Workfile:   SVScrollWnd1Ctl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Apr 2013 14:09:08  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVScrollWnd1Ctl.h"
#include "SVMaskEditor.h"
#include "SVScrollWnd1Ppg.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVScrollWnd1Ctrl, COleControl)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(SVScrollWnd1Ctrl, COleControl)
	//{{AFX_MSG_MAP(SVScrollWnd1Ctrl)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(SVScrollWnd1Ctrl, COleControl)
	//{{AFX_DISPATCH_MAP(SVScrollWnd1Ctrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(SVScrollWnd1Ctrl, COleControl)
	//{{AFX_EVENT_MAP(SVScrollWnd1Ctrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(SVScrollWnd1Ctrl, 1)
	PROPPAGEID(SVScrollWnd1PropPage::guid)
END_PROPPAGEIDS(SVScrollWnd1Ctrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(SVScrollWnd1Ctrl, "SVSCROLLWND.SVScrollWnd1Ctrl.1",
	0x6df99a35, 0xebb8, 0x11d2, 0xa7, 0x59, 0, 0x10, 0x6f, 0, 0xc, 0x7a)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(SVScrollWnd1Ctrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DSVScrollWnd1 =
		{ 0x6df99a33, 0xebb8, 0x11d2, { 0xa7, 0x59, 0, 0x10, 0x6f, 0, 0xc, 0x7a } };
const IID BASED_CODE IID_DSVScrollWnd1Events =
		{ 0x6df99a34, 0xebb8, 0x11d2, { 0xa7, 0x59, 0, 0x10, 0x6f, 0, 0xc, 0x7a } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

constexpr DWORD BASED_CODE _dwSVScrollWnd1OleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(SVScrollWnd1Ctrl, IDS_SVSCROLLWND1, _dwSVScrollWnd1OleMisc)


/////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1Ctrl::SVScrollWnd1CtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for SVScrollWnd1Ctrl

BOOL SVScrollWnd1Ctrl::SVScrollWnd1CtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
	{
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_SVSCROLLWND1,
			IDB_SVSCROLLWND1,
			afxRegApartmentThreading,
			_dwSVScrollWnd1OleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	}
	return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

/////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1Ctrl::SVScrollWnd1Ctrl - Constructor

SVScrollWnd1Ctrl::SVScrollWnd1Ctrl()
{
	InitializeIIDs(&IID_DSVScrollWnd1, &IID_DSVScrollWnd1Events);

	// TODO: Initialize your control's instance data here.
}

/////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1Ctrl::~SVScrollWnd1Ctrl - Destructor

SVScrollWnd1Ctrl::~SVScrollWnd1Ctrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1Ctrl::OnDraw - Drawing function

void SVScrollWnd1Ctrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	DoSuperclassPaint(pdc, rcBounds);
}

/////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1Ctrl::DoPropExchange - Persistence support

void SVScrollWnd1Ctrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}

/////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1Ctrl::OnResetState - Reset control to default state

void SVScrollWnd1Ctrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}

/////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1Ctrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL SVScrollWnd1Ctrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("STATIC");
	return COleControl::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1Ctrl::IsSubclassedControl - This is a subclassed control

BOOL SVScrollWnd1Ctrl::IsSubclassedControl()
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1Ctrl::OnOcmCommand - Handle command messages

LRESULT SVScrollWnd1Ctrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.

	return 0;
}

