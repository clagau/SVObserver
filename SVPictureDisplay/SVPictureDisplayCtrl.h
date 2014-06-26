//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureDisplayCtrl
//* .File Name       : $Workfile:   SVPictureDisplayCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:28:08  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVPictureDisplayIdl.h"
#include "SVPictureTabs.h"
#pragma endregion Includes

class SVPictureDisplayCtrl : public COleControl
{
	DECLARE_DYNCREATE(SVPictureDisplayCtrl)

public:
#pragma region Constructor
	SVPictureDisplayCtrl();
protected:
	~SVPictureDisplayCtrl();
#pragma endregion Constructor
public:

	// Dispatch and event IDs
#pragma region Declarations
	enum
	{
		eventidTabChange = 9L,
		dispidHideTabControl = 43L,
		dispidHideScrollBar = 42L,
		dispidZoomSlider = 41L,
		dispidBoundaryCheck = 40L,
		dispidAddOverlay = 39L,
		dispidEditOverlay = 38L,
		dispidAddOverlayToGroup = 37L,
		dispidSimultaneousZoom = 35L,
		dispidGetSelectedTab = 34L,
		dispidIsClearOverlayByNewImage = 33L,
		dispidGetViewRect = 32L,
		eventidObjectChangedEx = 8L,
		eventidObjectMovedEx = 7L,
		eventidMouseMovedIm = 6L,
		eventidObjectSelected = 5L,
		//eventidObjectMoved = 4L,
		eventidSavePictureEvent = 3L,
		eventidObjectDeleted = 2L,
		eventidRectangleChanged = 1L,

		dispidEnableSaveButton = 31L,
		dispidSelectObject = 30L,
		dispidSetEditAllow = 29L,

		dispidRemoveAllOverlays = 28L,
		dispidRemoveOverlay = 27L,

		dispidSetPictureWithROI = 13L,
		dispidSetPicture = 12L,

		dispidGetZoom = 11L,
		dispidSetZoom = 10L,

		dispidRenameTab = 9L,
		dispidShowTab = 8L,
		dispidSelectTab = 7L,
		dispidRemoveTab = 6L,
		dispidAddTab = 5L,

		dispidScrollBarWidth = 4L,
		dispidButtonJustify = 3L,
		dispidButtonSpacing = 2L,
		dispidButtonAppearance = 1L,
	};
#pragma endregion Declarations

#pragma region Public Methods
	// Overrides
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid) override;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual void DoPropExchange(CPropExchange* pPX) override;
	virtual void OnResetState() override;
	virtual DWORD GetControlFlags() override;

	DECLARE_OLECREATE_EX(SVPictureDisplayCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(SVPictureDisplayCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(SVPictureDisplayCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(SVPictureDisplayCtrl)      // Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

	// Message maps
	DECLARE_MESSAGE_MAP()

	// Dispatch maps
	DECLARE_DISPATCH_MAP()

	// Event maps
	DECLARE_EVENT_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnObjectDeleted( WPARAM wParam, LPARAM lParam );
	LRESULT OnTabChanged( WPARAM wParam, LPARAM lParam );
	LRESULT OnSaveButton( WPARAM wParam, LPARAM lParam );
	LRESULT OnObjectSelected( WPARAM wParam, LPARAM lParam );
	LRESULT OnMouseMovedIm( WPARAM wParam, LPARAM lParam );
	HRESULT SelectObject(LONG p_lTab, LONG p_lHandle);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void ObjectMovedEx(LONG tab, LONG handle );
	void ObjectChangedEx(LONG tab, LONG handle );

	//************************************
	// Method:    CalculateNextHandle
	// Description:  returns an unambiguous long value
	// Parameter: void
	// Returns:   long
	//************************************
	long CalculateNextHandle();
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	LONG GetButtonAppearance();
	void SetButtonAppearance( LONG newVal );

	LONG GetButtonSpacing();
	void SetButtonSpacing( LONG newVal );

	LONG GetButtonJustify();
	void SetButtonJustify( LONG newVal );

	LONG GetScrollBarWidth();
	void SetScrollBarWidth( LONG newVal );

	VARIANT_BOOL GetIsClearOverlayByNewImage();
	void SetIsClearOverlayByNewImage(VARIANT_BOOL newVal);

	VARIANT_BOOL GetZoomSimultaneous();
	void SetZoomSimultaneous(VARIANT_BOOL newVal);

	VARIANT_BOOL GetZoomSlider();
	void SetZoomSlider(VARIANT_BOOL newVal);

	VARIANT_BOOL GetBoundaryCheck();
	void SetBoundaryCheck(VARIANT_BOOL newVal);

	//************************************
	// Method:    AddTab
	// Description:  Add a new Tab 
	// Parameter: LPCTSTR text
	// Parameter: LONG * pTabHandle the tabhandle of the new tab
	// Returns:   HRESULT
	//************************************
	HRESULT AddTab( LPCTSTR text, LONG* pTabHandle );
	HRESULT RemoveTab( LONG p_lTab );
	HRESULT SelectTab( LONG p_lTab );

	//************************************
	// Method:    GetSelectedTab
	// Description: Get the tabhandle of the selected Tab
	// Access:    protected
	// Returns:   HRESULT
	//************************************
	SCODE GetSelectedTab(LONG* pTab);

	HRESULT ShowTab(LONG p_lTab, LONG bShow);
	HRESULT RenameTab(LONG p_lTab, LPCTSTR Name);

	HRESULT SetZoom( LONG p_lTab, FLOAT p_fZoom );
	HRESULT GetZoom( LONG p_lTab, FLOAT* p_pfZoom );

	HRESULT SetPicture( LONG p_lTab, IPictureDisp* p_Picture, COLORREF color );
	HRESULT SetPictureWithROI( LONG p_lTab, IPictureDisp* p_Picture, COLORREF color, ISVROIList* p_RoiList );

	//************************************
	// Method:    AddOverlay
	// Description:  PLEASE ENTER A DESCRIPTION
	// Parameter: LONG lTab
	// Parameter: VARIANT * pParameterList
	// Parameter: VARIANT * pParameterValues
	// Parameter: LONG * pHandle
	// Returns:   SCODE
	//************************************
	SCODE AddOverlay(LONG lTab, VARIANT* pParameterList, VARIANT* pParameterValues, LONG* pHandle);

	//************************************
	// Method:    AddOverlayToGroup
	// Description:  PLEASE ENTER A DESCRIPTION
	// Parameter: LONG lGroup
	// Parameter: LONG lTab
	// Parameter: VARIANT* pParameterlist
	// Parameter: VARIANT* pParametervalue
	// Parameter: LONG* pHandle
	// Returns:   SCODE
	//************************************
	SCODE AddOverlayToGroup(LONG lGroup, LONG lTab, VARIANT* pParameterlist, VARIANT* pParametervalue,  LONG* pHandle);

	SCODE EditOverlay(LONG lTab, LONG lHandle, VARIANT* pParameterList, VARIANT* pParameterValue);

	HRESULT RemoveOverlay( LONG p_lTab, LONG p_lHandle );
	HRESULT RemoveAllOverlays( LONG p_lTab );

	HRESULT SetEditAllow(LONG p_lTab, LONG p_lHandle, LONG p_lAllowEdit);

	HRESULT EnableSaveButton(LONG p_lTab, LONG p_lEnable);

	// Events....
	void ObjectDeleted(LONG p_lTab, LONG p_lHandle)
	{
		FireEvent(eventidObjectDeleted, EVENT_PARAM(VTS_I4 VTS_I4), p_lTab, p_lHandle);
	}

	void SavePictureEvent(IPictureDisp* Picture, LONG Tab)
	{
		FireEvent(eventidSavePictureEvent, EVENT_PARAM(VTS_PICTURE VTS_I4), Picture, Tab);
	}

	void ObjectSelected(LONG lTab, LONG lHandle, LONG lX1, LONG lY1, LONG lX2, LONG lY2)
	{
		FireEvent(eventidObjectSelected, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4), lTab, lHandle, lX1, lY1, lX2, lY2);
	}

	SCODE GetViewRect(LONG lTab, LONG* X1, LONG* Y1, LONG* X2, LONG* Y2);

	//************************************
	// Method:    MouseMovedIm
	// Description: Fire Event. When Mouse moved.
	// X Y are Pixel coordinate from the image tab is tabhandle of the image 
	// Access:    protected
	// Returns:   void
	//************************************
	void MouseMovedIm(LONG tab, LONG X, LONG Y)
	{
		FireEvent(eventidMouseMovedIm, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4), tab, X, Y);
	}

	//************************************
	// Method:    ObjectMovedEx
	// Description:  PLEASE ENTER A DESCRIPTION
	// Parameter: LONG tab
	// Parameter: LONG handle
	// Parameter: VARIANT * ParameterList
	// Parameter: VARIANT * ParameterValue
	// Returns:   void
	//************************************
	void ObjectMovedEx(LONG tab, LONG handle, VARIANT* ParameterList, VARIANT* ParameterValue)
	{
		FireEvent(eventidObjectMovedEx, EVENT_PARAM(VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT), tab, handle, ParameterList, ParameterValue);
	}

	void ObjectChangedEx(LONG tab, LONG handle, VARIANT* Parameterlist, VARIANT* ParameterValue)
	{
		FireEvent(eventidObjectChangedEx, EVENT_PARAM(VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT), tab, handle, Parameterlist, ParameterValue);
	}

	void TabChange(LONG tabHandle)
	{
		FireEvent(eventidTabChange, EVENT_PARAM(VTS_I4), tabHandle);
	}
	VARIANT_BOOL GetHideScrollBar();
	void SetHideScrollBar(VARIANT_BOOL newVal);
	VARIANT_BOOL GetHideTabControl();
	void SetHideTabControl(VARIANT_BOOL newVal);
#pragma endregion Protected Methods
private:
#pragma region Member variables
	long m_lAppearance;
	long m_lButtonJustify;
	long m_lButtonSpacing;
	long m_lScrollBarWidth;
	BOOL m_isClearOverlayByNewImage;
	BOOL m_isZoomSimultaneous;
	BOOL m_isBoundaryCheck;
	BOOL m_ShowZoomSlider;
	BOOL m_HideScrollBar;
	BOOL m_HideTabControl;
	long m_Handle;
	long m_TabHandle;

	SVPictureTabs m_TabDisplay;
#pragma endregion Member variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\SVPictureDisplayCtrl.h_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:28:08   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Sep 2010 11:19:46   tbair
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  33
 * SCR Title:  Font Wizard Improvements
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added GetViewRect function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Mar 2010 10:55:38   tbair
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  3
 * SCR Title:  Develop Picture Display Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Objected Selected Event
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Mar 2010 13:25:26   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/