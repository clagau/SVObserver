//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMainFrame
//* .File Name       : $Workfile:   SVMainFrm.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   12 Dec 2014 13:12:22  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVPPQBar.h"
#include "SVMFCControls\SVStatusBar.h"
#include "ZoomToolBar.h"
#include "ZoomHelperEx.h"
#pragma endregion Includes

#pragma region Declarations
class CSVRegressionRunDlg;
#pragma endregion Declarations

/**
@SVObjectName Main Frame

@SVObjectOverview This object manages the multiple inspection and I/O documents through the Multi-Document Interface (MDI) base object.  This object handles some of the menu options and document sizing issues.

@SVObjectOperations This object presents operation for it to manage the attached documents, menu options, tool bars, and status bar information.

*/
class SVMainFrame : public CMDIFrameWnd
{
//******************************************************************************
// MFC Declarations:
//******************************************************************************
	DECLARE_DYNAMIC(SVMainFrame)

//******************************************************************************
// Manual Generated Message Map Entries:
//******************************************************************************
private:
	//{{AFX_MSG(SVMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnViewToolBar();
	afx_msg void OnUpdateViewPPQBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewImageDisplayUpdate();
	afx_msg void OnUpdateViewImageDisplayUpdate(CCmdUI* pCmdUI);
	afx_msg void OnViewResultDisplayUpdate();
	afx_msg void OnUpdateViewResultDisplayUpdate(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg LRESULT OnDisplayChange(WPARAM p_p1, LPARAM p_p2);
	afx_msg LRESULT OnSetMode( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnLoadConfiguration( WPARAM wParam, LPARAM lParam );

	//************************************
	/// called by the MFC framework when automatic 
	/// configuration backups are enabled by the user
	//************************************
	afx_msg void OnExtrasEnableAutoSave();

	/// called by the MFC framework when file based write filter is enabled or disabled by the user
	afx_msg void OnExtrasEnableFbwf();


	//************************************
	/// called by the MFC framework
	/// \param pCmdUI [in]
	//************************************
	afx_msg void OnUpdateExtrasEnableAutoSave(CCmdUI *pCmdUI);

	/// updates the FBWT menu entry. called by the MFC framework when the corresponding drop-down menu is drawn.
	/// \param pCmdUI [in]
	afx_msg void OnUpdateExtrasEnableFbwf(CCmdUI *pCmdUI);


public:
	afx_msg void OnUpdateStatusInfo(CCmdUI *pCmdUI);
	afx_msg void OnViewPPQBar();
	//}}AFX_MSG

	// DDE Server Connect Confirm Message...
	afx_msg LRESULT OnDDEServerConnect( WPARAM wParam, LPARAM lParam );

	// Load and Run Last Configuration Automatically...
	//afx_msg LRESULT OnAutoRunLastConfiguration( WPARAM wParam, LPARAM lParam );

	// End Object Script Parsing
	afx_msg LRESULT OnEndObjectScriptParse( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnEndObjectCreation( WPARAM wParam, LPARAM lParam );

	afx_msg LRESULT OnUserChanged( WPARAM wParam, LPARAM lParam );

	afx_msg LRESULT OnHideIOViewTab( WPARAM p_wParam, LPARAM p_lParam );

	afx_msg LRESULT RemoteRegisterMonitorList( WPARAM wParam, LPARAM lParam );
	// Toolbar Methods
	afx_msg void OnToolbarZoom();
	afx_msg void OnUpdateToolbarZoom(CCmdUI *pCmdUI);

	DECLARE_MESSAGE_MAP()

public:
#pragma region Constructor
	SVMainFrame();
	virtual ~SVMainFrame();
#pragma endregion Constructor

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	HRESULT ParseToolsetScripts( SVTreeType& p_rTree );

	void OnConfigurationFinishedInitializing();

	BOOL BuildPPQButtons();
	BOOL DestroyPPQButtons();

	void SetStatusInfoText( LPCTSTR SIText );


	//************************************
	// Method:    ShowToolBars
	// Description:  Show or Hide the tool bars.  
	// Parameter: BOOL Show (True is show False is hide)
	// Parameter: BOOL RememberOldState if true remember or restore the old state 
	// Parameter: DWORD dwBars Flag_Standard | Flag_Zoom
	// Returns:   void
	//************************************
	void ShowToolBars(BOOL Show, BOOL RememberOldState , DWORD dwBars );

	//************************************
	// Method:    RefreshAllSplitters
	// Description:  Redraws the splitter frames
	// Returns:   void
	//************************************
	void RefreshAllSplitters();

	LRESULT InvalidateStatusBar( WPARAM wParam, LPARAM lParam );
	LRESULT Shutdown( WPARAM wParam, LPARAM lParam );

	virtual void OnUpdateFrameTitle( BOOL bAddToTitle );
	void UpdateFrameTitleForDocument( LPCTSTR lpszDocName );

	// MFC 1.0 backward compatible CreateClient hook (called by OnCreateClient)
	virtual BOOL CreateClient(LPCREATESTRUCT lpCreateStruct, CMenu* pWindowMenu);


	CSVRegressionRunDlg *m_pregTestDlg;

	//{{AFX_VIRTUAL(SVMainFrame)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);

protected:
	void LoadOEMIcon();

private:
	BOOL VerifyBarStates(LPCTSTR lpszProfileName);

public:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	void SetNotifyCommRC ();

	//************************************
	// Method:    GetZoomToolbarValue
	// Description:  Get the value from the zoom toolbar
	// Returns:   double
	//************************************
	double GetZoomToolbarValue() const;

	//************************************
	// Method:    SetZoomToolbar
	// Description:  Set the zoom toolbar with the the values from Zoomhelper
	// Parameter: ZoomHelperEx & helper
	// Returns:   void
	//************************************
	void SetZoomToolbar(const ZoomHelperEx &helper );

	//************************************
	// Method:    EnableZoomToolbar
	// Description:  Enable the zoom tool bar
	// Parameter: bool enable
	// Returns:   void
	//************************************
	void EnableZoomToolbar(bool enable );	

	//************************************
	// Method:    IsZoomingPossible
	// Description: True if the zoom toolbar is enabled
	// Returns:   bool
	//************************************
	bool IsZoomingPossible();

private: //Arvid 150203: changed access class from protected
	// Bars...
	SVStatusBar				m_wndStatusBar;
	CToolBar				m_wndToolBar;
	ZoomToolBar				m_wndToolBarZoom;

	SVPPQBarClass			m_PPQBar;

	BOOL m_toolbarZoomWasShown;
	BOOL m_toolbarWasShown;
	BOOL m_PPQBarWasShown;

	CBrush m_BackgroundBrush;

	HICON m_oemLargeIcon;
	HICON m_oemSmallIcon;

private:

	BOOL m_notifyCommRC;

	BOOL m_bSizeChanged;
	UINT m_PosSize;
	HANDLE m_hDisplayChangedEvent; // Set if the display settings have been changed since the Window was created.

public:
	const static DWORD Flag_Standard = 0x1;
	const static DWORD Flag_Zoom = 0x2;
};

