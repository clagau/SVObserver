//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureDisplayCtrl
//* .File Name       : $Workfile:   SVPictureDisplayCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:28:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "afxdlgs.h"
#include "SVPictureDisplay.h"
#include "SVPictureDisplayCtrl.h"
#include "SVPictureDisplayPropPage.h"
#include "SVOMFCLibrary/Displayhelper.h"
#pragma endregion Includes

using namespace Seidenader::SVOMFCLibrary;

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

IMPLEMENT_DYNCREATE(SVPictureDisplayCtrl, COleControl)

// Message map
BEGIN_MESSAGE_MAP(SVPictureDisplayCtrl, COleControl)
	ON_MESSAGE(OCM_COMMAND, &SVPictureDisplayCtrl::OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(OBJECT_DELETED, &SVPictureDisplayCtrl::OnObjectDeleted)
	ON_MESSAGE(SAVE_BUTTON_PRESSED, &SVPictureDisplayCtrl::OnSaveButton)
	ON_MESSAGE(OBJECT_SELECTED, &SVPictureDisplayCtrl::OnObjectSelected)
	ON_MESSAGE(MOUSE_MOVEDIM,&SVPictureDisplayCtrl::OnMouseMovedIm)
	ON_MESSAGE(TAB_CHANGED,&SVPictureDisplayCtrl::OnTabChanged)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// Dispatch map
BEGIN_DISPATCH_MAP(SVPictureDisplayCtrl, COleControl)
	DISP_STOCKFUNC_REFRESH()
	DISP_PROPERTY_EX_ID(SVPictureDisplayCtrl, "ButtonAppearance", dispidButtonAppearance, GetButtonAppearance, SetButtonAppearance, VT_I4)
	DISP_PROPERTY_EX_ID(SVPictureDisplayCtrl, "ButtonSpacing", dispidButtonSpacing, GetButtonSpacing, SetButtonSpacing, VT_I4)
	DISP_PROPERTY_EX_ID(SVPictureDisplayCtrl, "ButtonJustify", dispidButtonJustify, GetButtonJustify, SetButtonJustify, VT_I4)
	DISP_PROPERTY_EX_ID(SVPictureDisplayCtrl, "ScrollBarWidth", dispidScrollBarWidth, GetScrollBarWidth, SetScrollBarWidth, VT_I4)
	DISP_PROPERTY_EX_ID(SVPictureDisplayCtrl, "IsClearOverlayByNewImage", dispidIsClearOverlayByNewImage, GetIsClearOverlayByNewImage, SetIsClearOverlayByNewImage, VT_BOOL)
	DISP_PROPERTY_EX_ID(SVPictureDisplayCtrl, "SimultaneousZoom", dispidSimultaneousZoom, GetZoomSimultaneous, SetZoomSimultaneous, VT_BOOL)
	DISP_PROPERTY_EX_ID(SVPictureDisplayCtrl, "BoundaryCheck", dispidBoundaryCheck, GetBoundaryCheck, SetBoundaryCheck, VT_BOOL)

	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "AddTab", dispidAddTab, AddTab, VT_ERROR, VTS_BSTR VTS_PI4)
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "RemoveTab", dispidRemoveTab, RemoveTab, VT_ERROR, VTS_I4 )
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "SelectTab", dispidSelectTab, SelectTab, VT_ERROR, VTS_I4 )
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "ShowTab", dispidShowTab, ShowTab, VT_ERROR, VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "RenameTab", dispidRenameTab, RenameTab, VT_ERROR, VTS_I4 VTS_BSTR)

	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "SetZoom", dispidSetZoom, SetZoom, VT_ERROR, VTS_I4 VTS_R4)
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "GetZoom", dispidGetZoom, GetZoom, VT_ERROR, VTS_I4 VTS_PR4)

	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "SetPicture", dispidSetPicture, SetPicture, VT_ERROR, VTS_I4 VTS_PICTURE VTS_COLOR)
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "Set Picture with ROI", dispidSetPictureWithROI, SetPictureWithROI, VT_ERROR, VTS_I4 VTS_DISPATCH VTS_COLOR VTS_DISPATCH)

	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "RemoveOverlay", dispidRemoveOverlay, RemoveOverlay, VT_ERROR, VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "RemoveAllOverlays", dispidRemoveAllOverlays, RemoveAllOverlays, VT_ERROR, VTS_I4)
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "SetEditAllow", dispidSetEditAllow, SetEditAllow, VT_ERROR, VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "SelectObject", dispidSelectObject, SelectObject, VT_ERROR, VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "EnableSaveButton", dispidEnableSaveButton, EnableSaveButton, VT_ERROR, VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "GetViewRect", dispidGetViewRect, GetViewRect, VT_ERROR, VTS_I4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4)
	DISP_DEFVALUE(SVPictureDisplayCtrl, "SimultaneousZoom")
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "GetSelectedTab", dispidGetSelectedTab, GetSelectedTab, VT_ERROR, VTS_PI4)
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "AddOverlay", dispidAddOverlay, AddOverlay, VT_ERROR, VTS_I4 VTS_PVARIANT VTS_PVARIANT  VTS_PI4)
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "AddOverlayToGroup", dispidAddOverlayToGroup, AddOverlayToGroup, VT_ERROR, VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT  VTS_PI4)
	DISP_FUNCTION_ID(SVPictureDisplayCtrl, "EditOverlay", dispidEditOverlay, EditOverlay, VT_ERROR, VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT)


	DISP_PROPERTY_EX_ID(SVPictureDisplayCtrl, "ZoomSlider", dispidZoomSlider, GetZoomSlider, SetZoomSlider, VT_BOOL)
	DISP_PROPERTY_EX_ID(SVPictureDisplayCtrl, "HideScrollBar", dispidHideScrollBar, GetHideScrollBar, SetHideScrollBar, VT_BOOL)
	DISP_PROPERTY_EX_ID(SVPictureDisplayCtrl, "HideTabControl", dispidHideTabControl, GetHideTabControl, SetHideTabControl, VT_BOOL)
END_DISPATCH_MAP()

// Event map
BEGIN_EVENT_MAP(SVPictureDisplayCtrl, COleControl)
	EVENT_CUSTOM_ID("ObjectDeleted", eventidObjectDeleted, ObjectDeleted, VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("SavePictureEvent", eventidSavePictureEvent, SavePictureEvent, VTS_PICTURE VTS_I4)
	EVENT_CUSTOM_ID("ObjectSelected", eventidObjectSelected, ObjectSelected, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("MouseMovedIm", eventidMouseMovedIm, MouseMovedIm, VTS_I4 VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("ObjectMovedEx", eventidObjectMovedEx, ObjectMovedEx, VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT)
	EVENT_CUSTOM_ID("ObjectChangedEx", eventidObjectChangedEx, ObjectChangedEx, VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT)
	EVENT_CUSTOM_ID("TabChange", eventidTabChange, TabChange, VTS_I4)
END_EVENT_MAP()

// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(SVPictureDisplayCtrl, 1)
	PROPPAGEID(SVPictureDisplayPropPage::guid)
END_PROPPAGEIDS(SVPictureDisplayCtrl)

// Initialize class factory and guid
IMPLEMENT_OLECREATE_EX(SVPictureDisplayCtrl, "SVPICTUREDISPLAY.SVPictureDisplayCtrl.1",
	0x8174991a, 0xa5f5, 0x4cb3, 0x85, 0xe3, 0x3b, 0x31, 0xfa, 0xef, 0x23, 0x14)

// Type library ID and version
IMPLEMENT_OLETYPELIB(SVPictureDisplayCtrl, _tlid, _wVerMajor, _wVerMinor)

// Interface IDs
const IID BASED_CODE IID_DSVPictureDisplay =
		{ 0xC16C57B1, 0xF3A1, 0x437B, { 0xBB, 0xF7, 0x18, 0x29, 0xD7, 0xC0, 0xA5, 0xDC } };
const IID BASED_CODE IID_DSVPictureDisplayEvents =
		{ 0x292B012C, 0xB2C2, 0x4C43, { 0x8E, 0xE0, 0x7F, 0xA7, 0xC3, 0xDA, 0xBE, 0x73 } };

// Control type information
static const DWORD BASED_CODE _dwSVPictureDisplayOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(SVPictureDisplayCtrl, IDS_SVPICTUREDISPLAY, _dwSVPictureDisplayOleMisc)

// SVPictureDisplayCtrl::SVPictureDisplayCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for SVPictureDisplayCtrl

BOOL SVPictureDisplayCtrl::SVPictureDisplayCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.

	if (bRegister)
	{
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_SVPICTUREDISPLAY,
			IDB_SVPICTUREDISPLAY,
			afxRegInsertable | afxRegApartmentThreading,
			_dwSVPictureDisplayOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	}
	else
	{
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
	}
}

#pragma region Constructor
SVPictureDisplayCtrl::SVPictureDisplayCtrl()
: m_lAppearance( 0L )
, m_lButtonJustify( 0L )
, m_lButtonSpacing( 4L )
, m_lScrollBarWidth( 24L )
, m_isClearOverlayByNewImage( true )
, m_isZoomSimultaneous( false )
, m_isBoundaryCheck( TRUE )
, m_ShowZoomSlider( TRUE )
, m_HideScrollBar( FALSE )
, m_HideTabControl( FALSE )
, m_Handle(1) //first handle should be 1
, m_TabHandle(0) //must be 0 to keep previous behavior
{
	InitializeIIDs(&IID_DSVPictureDisplay, &IID_DSVPictureDisplayEvents);
}

SVPictureDisplayCtrl::~SVPictureDisplayCtrl()
{
	// TODO: Cleanup your control's instance data here.
}

void SVPictureDisplayCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
	{
		return;
	}

	DoSuperclassPaint(pdc, rcBounds);

	if (!IsOptimizedDraw())
	{
		// The container does not support optimized drawing.

		// TODO: if you selected any GDI objects into the device context *pdc,
		//		restore the previously-selected objects here.
	}
}
#pragma endregion Constructor

// SVPictureDisplayCtrl::DoPropExchange - Persistence support
void SVPictureDisplayCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	PX_Long( pPX, _T("Button Appearance"), m_lAppearance );
	PX_Long( pPX, _T("Button Justify"), m_lButtonJustify );
	PX_Long( pPX, _T("Button Spacing"), m_lButtonSpacing );
	PX_Long( pPX, _T("Scroll Bar Width"), m_lScrollBarWidth );
	PX_Bool( pPX, _T("Clear Overlay by new image"), m_isClearOverlayByNewImage );
	PX_Bool( pPX, _T("Zoom image in all tabs simultaneuosly"), m_isZoomSimultaneous, 0 );
	PX_Bool( pPX, _T("Don't allow Overlays outside Boundary"), m_isBoundaryCheck, 1 );
	PX_Bool( pPX, _T("Show ZoomSlider Check"), m_ShowZoomSlider, 1 );
	PX_Bool( pPX, _T("Hide Scroll Bars"), m_HideScrollBar, 0 );
	PX_Bool( pPX, _T("Hide Tab Control"), m_HideTabControl, 0 );
}

// Flags to customize MFC's implementation of ActiveX controls.
DWORD SVPictureDisplayCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();

	// The control will not be redrawn when making the transition
	// between the active and inactivate state.
	dwFlags |= noFlickerActivate;

	// The control can optimize its OnDraw method, by not restoring
	// the original GDI objects in the device context.
	dwFlags |= canOptimizeDraw;
	return dwFlags;
}

// Reset control to default state
void SVPictureDisplayCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}

// Modify parameters for CreateWindowEx
BOOL SVPictureDisplayCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("STATIC");
	return COleControl::PreCreateWindow(cs);
}

BOOL SVPictureDisplayCtrl::IsSubclassedControl()
{
	return TRUE; // This is a subclassed control
}

// SVPictureDisplayCtrl::OnOcmCommand - Handle command messages

LRESULT SVPictureDisplayCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.

	return 0;
}

// SVPictureDisplayCtrl message handlers

int SVPictureDisplayCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	// TODO:  Add your specialized creation code here
	m_TabDisplay.Create( SVPictureTabs::IDD, this );

	m_TabDisplay.SetAppearance( m_lAppearance );
	m_TabDisplay.SetButtonJustify( m_lButtonJustify );
	m_TabDisplay.SetButtonSpacing( m_lButtonSpacing );
	m_TabDisplay.SetScrollBarWidth( m_lScrollBarWidth );
	m_TabDisplay.SetZoomSimultaneous( m_isZoomSimultaneous );
	m_TabDisplay.SetIsClearOverlayByNewImage( m_isClearOverlayByNewImage );
	m_TabDisplay.SetBoundaryCheck( m_isBoundaryCheck );
	m_TabDisplay.SetShowZoomSlider( m_ShowZoomSlider );
	m_TabDisplay.SetHideTabControl( m_HideTabControl );
	m_TabDisplay.SetHideScrollBar( m_HideScrollBar );
	m_TabDisplay.ShowWindow( SW_SHOW );

	return 0;
}

HRESULT SVPictureDisplayCtrl::GetZoom( LONG p_lTab, FLOAT* p_pfZoom )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	double value = 1.0f;
	HRESULT hr = m_TabDisplay.GetZoom( p_lTab, value );
	if (S_OK == hr)
	{
		*p_pfZoom = static_cast<float>(value);
	}
	return hr;
}

HRESULT SVPictureDisplayCtrl::SetZoom( LONG p_lTab, FLOAT p_fZoom )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT l_hr = m_TabDisplay.SetZoom( p_lTab, p_fZoom );
	return l_hr;
}

HRESULT SVPictureDisplayCtrl::SetPicture( LONG p_lTab, IPictureDisp* p_Picture, COLORREF color)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Put the hbitmap into the appropriate tab.
	HRESULT l_hr = m_TabDisplay.SetPicture( p_lTab, p_Picture, color );
	return l_hr;
}

HRESULT SVPictureDisplayCtrl::SetPictureWithROI( LONG p_lTab, IPictureDisp* p_Picture, COLORREF color, ISVROIList* p_RoiList)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Put the hbitmap into the appropriate tab.
	HRESULT l_hr = m_TabDisplay.SetPictureWithROI( p_lTab, p_Picture, color, p_RoiList );
	return l_hr;
}

void SVPictureDisplayCtrl::OnSize(UINT nType, int cx, int cy)
{
	COleControl::OnSize(nType, cx, cy);

	m_TabDisplay.SetWindowPos( &wndTop, 0, 0, cx, cy, SWP_SHOWWINDOW );
	m_TabDisplay.OnSize( nType, cx, cy );
}

LONG SVPictureDisplayCtrl::GetButtonAppearance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_lAppearance;
}

void SVPictureDisplayCtrl::SetButtonAppearance( LONG newVal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_lAppearance = newVal;

	m_TabDisplay.SetAppearance( newVal );
}

LONG SVPictureDisplayCtrl::GetButtonSpacing()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return this->m_lButtonSpacing;
}

void SVPictureDisplayCtrl::SetButtonSpacing(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_lButtonSpacing = newVal;
	m_TabDisplay.SetButtonSpacing( newVal );
}

LONG SVPictureDisplayCtrl::GetButtonJustify()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_lButtonJustify;
}

void SVPictureDisplayCtrl::SetButtonJustify(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_lButtonJustify = newVal;

	m_TabDisplay.SetButtonJustify( newVal );
}

HRESULT SVPictureDisplayCtrl::AddTab(LPCTSTR text, LONG* handle )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT l_hr = m_TabDisplay.AddTab( text, handle );
	return l_hr;
}

HRESULT SVPictureDisplayCtrl::RemoveTab(LONG p_lTab)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT l_hr = m_TabDisplay.RemoveTab(p_lTab);
	return l_hr;
}

HRESULT SVPictureDisplayCtrl::SelectTab(LONG p_lTab)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT l_hr = m_TabDisplay.SelectTab(p_lTab);
	return l_hr;
}

SCODE SVPictureDisplayCtrl::GetSelectedTab(LONG* pTab)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT l_hr = m_TabDisplay.GetSelectedTab(*pTab);
	return l_hr;
}

HRESULT SVPictureDisplayCtrl::RemoveOverlay(LONG p_lTab, LONG Handle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	HRESULT l_hr = m_TabDisplay.RemoveOverlay( p_lTab, Handle );
	return l_hr;
}

HRESULT SVPictureDisplayCtrl::RemoveAllOverlays(LONG p_lTab)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	HRESULT l_hr = m_TabDisplay.RemoveAllOverlays(p_lTab);
	return l_hr;
}

LONG SVPictureDisplayCtrl::GetScrollBarWidth()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_lScrollBarWidth;
}

void SVPictureDisplayCtrl::SetScrollBarWidth(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_lScrollBarWidth = newVal;
	m_TabDisplay.SetScrollBarWidth( newVal );
}

HRESULT SVPictureDisplayCtrl::SetEditAllow(LONG p_lTab, LONG p_lHandle, LONG p_lAllowEdit)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT l_hr = m_TabDisplay.SetEditAllow( p_lTab, p_lHandle, p_lAllowEdit );
	return l_hr;
}

HRESULT SVPictureDisplayCtrl::SelectObject(LONG p_lTab, LONG p_lHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT l_hr = m_TabDisplay.SelectDrawObject( p_lTab, p_lHandle );
	return l_hr;
}

LRESULT SVPictureDisplayCtrl::OnObjectSelected( WPARAM wParam, LPARAM lParam )
{
	long l_lTab = static_cast<long>( wParam );
	long l_lHandle = static_cast<long>( lParam );
	long l_lX1;
	long l_lY1;
	long l_lX2;
	long l_lY2;
	if( l_lHandle > 0 )
	{
		if( S_OK == m_TabDisplay.GetObjectCoords( l_lTab, l_lHandle, l_lX1, l_lY1, l_lX2, l_lY2 ) )
		{
			ObjectSelected( l_lTab, l_lHandle, l_lX1, l_lY1, l_lX2, l_lY2 );
		}
	}
	else
	{
		//Fire Event with Handle == -1 to show deselection
		ObjectSelected( l_lTab, -1, 0, 0, 0, 0);
	}
	return 0L;
}

LRESULT SVPictureDisplayCtrl::OnMouseMovedIm( WPARAM x, LPARAM y )
{
	long Tab = -1;
	long xParam = static_cast< long >( x ); // MouseMovedIm takes LONG because it's a COM method, but WPARAM is LONG_PTR
	long yParam = static_cast< long >( y ); // MouseMovedIm takes LONG because it's a COM method, but LPARAM is LONG_PTR
	HRESULT l_hr = m_TabDisplay.GetSelectedTab( Tab );
	MouseMovedIm( Tab, xParam, yParam );
	return 0L;
}

LRESULT SVPictureDisplayCtrl::OnObjectDeleted( WPARAM tabIndex, LPARAM handle )
{
	long l_lTab = static_cast<long>(tabIndex); // ObjectDeleted takes LONG, but WPARAM is LONG_PTR
	long l_lHandle = static_cast<long>(handle); // ObjectDeleted takes LONG, but WPARAM is LONG_PTR
	ObjectDeleted( l_lTab, l_lHandle ); // Maybe ObjectDeleted should take LONG_PTR
	return 0L;
}

LRESULT SVPictureDisplayCtrl::OnTabChanged( WPARAM tabIndex, LPARAM lParam )
{
	TabChange( static_cast<long>(tabIndex) );
	return 0L;
}

LRESULT SVPictureDisplayCtrl::OnSaveButton( WPARAM wParam, LPARAM lParam )
{
	long l_lTab = static_cast<long>(wParam);
	IPictureDisp* l_pPicture = reinterpret_cast<IPictureDisp*>(lParam);
	SavePictureEvent( l_pPicture, l_lTab );
	return 0L;
}

void SVPictureDisplayCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_TabDisplay.OnKeyDown(nChar, nRepCnt, nFlags);
}

void SVPictureDisplayCtrl::ObjectMovedEx( LONG tab, LONG handle )
{
	VariantParamMap ParameterMap;
	HRESULT hres = m_TabDisplay.GetObjectParameter(tab, handle, ParameterMap);

	size_t size = ParameterMap.size();
	if(S_OK == hres && 0 < size)
	{
		COleSafeArray saPar, saVal;
		Seidenader::SVOMFCLibrary::DisplayHelper::CreateSafeArrayFromMap(ParameterMap, saPar, saVal);

		ObjectMovedEx(tab, handle, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ) );
	}
}

void SVPictureDisplayCtrl::ObjectChangedEx(LONG tab, LONG handle)
{
	VariantParamMap ParameterMap;
	HRESULT hres = m_TabDisplay.GetObjectParameter(tab, handle, ParameterMap);

	size_t size = ParameterMap.size();
	if(S_OK == hres && 0 < size)
	{
		COleSafeArray saPar, saVal;
		Seidenader::SVOMFCLibrary::DisplayHelper::CreateSafeArrayFromMap(ParameterMap, saPar, saVal);

		ObjectChangedEx( tab, handle, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ) );
	}
}

HRESULT SVPictureDisplayCtrl::EnableSaveButton(LONG p_lTab, LONG p_lEnable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT l_hr = m_TabDisplay.EnableSaveButton( p_lTab, p_lEnable );
	return l_hr;
}

HRESULT SVPictureDisplayCtrl::ShowTab(LONG p_lTab, LONG bShow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT l_hr = m_TabDisplay.ShowTab( p_lTab, bShow == 0 ? false : true );
	return l_hr;
}

HRESULT SVPictureDisplayCtrl::RenameTab(LONG p_lTab, LPCTSTR Name)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT l_hr = m_TabDisplay.RenameTab( p_lTab, Name );
	return l_hr;
}

SCODE SVPictureDisplayCtrl::AddOverlay(LONG lTab, VARIANT* ParameterList, VARIANT* ParameterValue, LONG* pHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VariantParamMap ParameterMap;
	int size = DisplayHelper::FillParameterMap(ParameterMap, ParameterList, ParameterValue);
	if( size <= 0)
	{
		return E_FAIL;
	}
	long handle(0);
	HRESULT hr = m_TabDisplay.AddOverlay( lTab, ParameterMap, handle );
	if (S_OK == hr)
	{
		*pHandle = handle;
	}
	return hr;
}

SCODE SVPictureDisplayCtrl::AddOverlayToGroup(LONG lGroup, LONG lTab, VARIANT* ParameterList, VARIANT* ParameterValue, LONG* pHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VariantParamMap ParameterMap;
	int size = DisplayHelper::FillParameterMap(ParameterMap, ParameterList, ParameterValue);
	if( size <= 0)
	{
		return E_FAIL;
	}
	long handle(0);
	HRESULT hr = m_TabDisplay.AddOverlayToGroup( lGroup, lTab, ParameterMap, handle );
	if (S_OK == hr)
	{
		*pHandle = handle;
	}
	return hr;
}

SCODE SVPictureDisplayCtrl::EditOverlay(LONG Tab, LONG Handle, VARIANT* pParameterList, VARIANT* pParameterValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	VariantParamMap ParameterMap;
	int size = DisplayHelper::FillParameterMap(ParameterMap, pParameterList, pParameterValue);
	if( size <= 0)
	{
		return E_FAIL;
	}

	HRESULT hr = m_TabDisplay.EditOverlay( Tab, Handle, ParameterMap );
	return hr;
}

SCODE SVPictureDisplayCtrl::GetViewRect(LONG lTab, LONG* X1, LONG* Y1, LONG* X2, LONG* Y2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_TabDisplay.GetViewRect( lTab, *X1, *Y1, *X2, *Y2);
}

VARIANT_BOOL SVPictureDisplayCtrl::GetIsClearOverlayByNewImage()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_isClearOverlayByNewImage;
}

void SVPictureDisplayCtrl::SetIsClearOverlayByNewImage(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_isClearOverlayByNewImage = newVal;
	m_TabDisplay.SetIsClearOverlayByNewImage( newVal );
}

VARIANT_BOOL SVPictureDisplayCtrl::GetZoomSimultaneous()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_isZoomSimultaneous;
}

void SVPictureDisplayCtrl::SetZoomSimultaneous(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_isZoomSimultaneous = newVal;

	m_TabDisplay.SetZoomSimultaneous(m_isZoomSimultaneous);
}

VARIANT_BOOL SVPictureDisplayCtrl::GetZoomSlider()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_ShowZoomSlider;
}

void SVPictureDisplayCtrl::SetZoomSlider(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_ShowZoomSlider = newVal;
	m_TabDisplay.SetShowZoomSlider( m_ShowZoomSlider );
}

VARIANT_BOOL SVPictureDisplayCtrl::GetBoundaryCheck()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_isBoundaryCheck;
}

void SVPictureDisplayCtrl::SetBoundaryCheck(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_isBoundaryCheck = newVal;

	m_TabDisplay.SetBoundaryCheck( m_isBoundaryCheck );
}

VARIANT_BOOL SVPictureDisplayCtrl::GetHideScrollBar()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_HideScrollBar;
}

void SVPictureDisplayCtrl::SetHideScrollBar(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_HideScrollBar = newVal;
	m_TabDisplay.SetHideScrollBar(newVal);
}

VARIANT_BOOL SVPictureDisplayCtrl::GetHideTabControl()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_HideTabControl;
}

void SVPictureDisplayCtrl::SetHideTabControl(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_HideTabControl = newVal;
	m_TabDisplay.SetHideTabControl(newVal);
}

long SVPictureDisplayCtrl::CalculateNextHandle()
{
	return m_Handle++;
}

