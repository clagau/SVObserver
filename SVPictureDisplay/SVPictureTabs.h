//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureTabs
//* .File Name       : $Workfile:   SVPictureTabs.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:28:56  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comutil.h>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/shared_ptr.hpp>
#include "SVPictureDialog.h"
#include "SVPictureTabInfo.h"
#pragma endregion Includes

class SVPictureTabs : public CDialog
{
public:
#pragma region Constructor
	SVPictureTabs(CWnd* pParent = nullptr);
	virtual ~SVPictureTabs();
#pragma endregion Constructor

#pragma region Declarations
	enum { IDD = IDD_PICTURE_TABS };
#pragma endregion Declarations

public:
#pragma region Public Methods
	virtual BOOL OnInitDialog() override;
	HRESULT SetZoom( long p_lTab, double p_fZoom );
	HRESULT GetZoom( long p_lTab, double& rZoomVal ) const;

	void SetAppearance( long p_lType );
	void SetButtonSpacing( long p_lSpacing );
	void SetButtonJustify( long p_lJustify );
	void SetScrollBarWidth( long p_lWidth );
	void SetIsClearOverlayByNewImage( BOOL isValue );
	void SetZoomSimultaneous( BOOL isZoomSimultaneous );
	BOOL GetZoomSimultaneous() const;
	void SetShowZoomSlider( BOOL showZoomSlider );
	BOOL GetShowZoomSlider() const;
	BOOL GetBoundaryCheck() const;
	void SetBoundaryCheck( BOOL isBoundaryCheck );

	//************************************
	// Method:    CloneZoomSettings
	// Description: Copies the zoom setings from Tab to all other tabs.
	// Access:    public
	// Returns:   HRESULT
	//************************************
	HRESULT CloneZoomSettings(long Tab);

	//************************************
	// Method:   CloneScrollSettings
	// Description: Copies the scroll settings from Tab to all other tabs.
	// Access:    public
	// Returns:   HRESULT
	//************************************
	HRESULT CloneScrollPosition( int orientation, long Tab );

	HRESULT RemoveOverlay( long p_lTab, long p_Handle );
	HRESULT RemoveAllOverlays( long p_lTab );
	HRESULT SetEditAllow( long p_lTab, long p_lHandle, long p_lAllowEdit );
	HRESULT SelectDrawObject( long p_lTab, long p_lHandle );

	//************************************
	// Method:    AddTab
	// Description:  Add a new tab
	// Parameter: LPCTSTR Name of the tab
	// Parameter: long * phandle gets the handle of the tab
	//    We use long* here instead of long& because this method is called from the COM interface.
	// Returns:   HRESULT
	//************************************
	HRESULT AddTab( LPCTSTR p_stFileName, long* phandle );
	HRESULT RemoveTab( long p_lTab );
	HRESULT SelectTab( long p_lTab );

	//************************************
	// Method:    GetSelectedTab
	// Description: Get the tabhandle of the selected Tab
	// Access:    public
	// Returns:   HRESULT
	//************************************
	HRESULT GetSelectedTab(long& Tab) const;

	HRESULT RenameTab( long p_lTabHandle, LPCTSTR p_Name );
	HRESULT ShowTab( long p_lTabHandle, bool p_bShow );

	HRESULT GetObjectCoords( long lTab, long lHandle, long& rlX1, long& rlY1, long& rlX2, long& rlY2 ) const;
	HRESULT GetObjectParameter( long lTab, long lHandle, VariantParamMap& parameterMap ) const;

	HRESULT SetPicture( long p_lTab, IPictureDisp* l_pPicture, COLORREF p_color );
	HRESULT SetPictureWithROI( long p_lTab, IPictureDisp* l_pPicture, COLORREF color, ISVROIList* p_RoiList );

	HRESULT EnableSaveButton( long p_lTab, long p_lEnable );

	HRESULT GetViewRect( long p_lTab, long& pX1, long& pY1, long& pX2, long& pY2) const;

	//************************************
	// Method:    AddOverlay
	// Description:  Adds on object to draw on top of the picture
	// Parameter: long tabhandle
	// Parameter: VariantParamMap& ParameterMap
	// Parameter: long & handle
	// Returns:   HRESULT
	//************************************
	HRESULT AddOverlay(long tabhandle, const VariantParamMap& ParameterMap, long& handle);

	HRESULT AddOverlayToGroup(long group, long tabhandle, const VariantParamMap& ParameterMap, long& handle);
	HRESULT EditOverlay(LONG tab, LONG Handle, const VariantParamMap& ParameterMap);
	void SetHideTabControl(VARIANT_BOOL newVal);
	void SetHideScrollBar(VARIANT_BOOL newVal);
	BOOL IsScrollbarHidden() const;
public:
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	void OnTcnSelChangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
#pragma endregion Protected Methods
	DECLARE_MESSAGE_MAP()

private:
#pragma region Private Methods
	bool SelectActiveTab( int p_iSelect );
	bool IsValidTabIndex( int p_iTab ) const;
	bool IsValidTabHandle( long p_lTab ) const;
	long GetHandleFromTabIndex( int p_lTab ) const;
	int GetTabIndexFromTabHandle( long p_lTabHandle ) const;

	void UpdateAppearance();
	void UpdateButtonSpacing();
	void UpdateButtonJustify();
	void UpdateScrollBarWidth();
	void UpdateTabClient(SVPictureDialogRef p_Dialog);
	void ReIndexHandles();
#pragma endregion Private Methods

private:
#pragma region Member Variables
	CTabCtrl m_TabCtrl; // Tabs to select picture.
	typedef std::map<long, SVPictureTabInfo> TabPictureDialogList;
	typedef std::map<long, SVPictureTabInfo>::iterator TabPictureDialogListIt;

	TabPictureDialogList m_PicDialogs; // PictureDisplays

	BOOL m_isClearOverlayByNewImage;
	BOOL m_isZoomSimultaneous; // all tabs zoom simultaneously
	BOOL m_isBoundaryCheck; // don't allow Overlays to move outside the Boundary
	BOOL m_ShowZoomSlider; // Show Zoom slider
	BOOL m_HideTabCtrl; // Hide Tab control
	BOOL m_HideScrollBar; // Hide Scroll Bar
	long m_lLastTab;
	long m_TabHandle; //the next unique tabHandle

	AppearanceTypes m_lAppearance;
	long m_lButtonJustify;
	long m_lButtonSpacing;
	long m_lScrollBarWidth;
#pragma endregion Member Variables
};

