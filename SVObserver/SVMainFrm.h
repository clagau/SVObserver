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
#include "SVOGui/ZoomHelper.h"
#pragma endregion Includes

#pragma region Declarations
class SVRegressionRunDlg;
#pragma endregion Declarations

/**
@SVObjectName Main Frame

@SVObjectOverview This object manages the multiple inspection and I/O documents through the Multi-Document Interface (MDI) base object.  This object handles some of the menu options and document sizing issues.

@SVObjectOperations This object presents operation for it to manage the attached documents, menu options, tool bars, and status bar information.

*/
class SVMainFrame : public CMDIFrameWnd
{
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
	afx_msg LRESULT OnLoadPackedConfiguration( WPARAM wParam, LPARAM lParam );
	afx_msg void OnExtrasStartLastConfiguration();
	afx_msg void OnUpdateExtrasStartLastConfiguration(CCmdUI* PCmdUI);
	afx_msg void OnExtrasEnableAutoSave();
	afx_msg void OnUpdateExtrasEnableAutoSave(CCmdUI *pCmdUI);
	afx_msg void OnExtrasEnableFbwf();
	afx_msg void OnUpdateExtrasEnableFbwf(CCmdUI *pCmdUI);


public:
	afx_msg void OnUpdateStatusInfo(CCmdUI *pCmdUI);
	afx_msg void OnViewPPQBar();
	//}}AFX_MSG

	afx_msg LRESULT OnUserChanged( WPARAM wParam, LPARAM lParam );

	afx_msg LRESULT RemoteRegisterMonitorList(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT onAddFileToConfig(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onSaveConfig(WPARAM wParam, LPARAM lParam);
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
	// Method:    RefreshAllSplitters
	// Description:  Redraws the splitter frames
	// Returns:   void
	//************************************
	void RefreshAllSplitters();

	LRESULT InvalidateStatusBar( WPARAM wParam, LPARAM lParam );
	LRESULT Shutdown(WPARAM wParam, LPARAM lParam);
	LRESULT UpdateIPDocViews(WPARAM wParam, LPARAM lParam);

	virtual void OnUpdateFrameTitle( BOOL bAddToTitle ) override;
	void UpdateFrameTitleForDocument( LPCTSTR lpszDocName );

	// MFC 1.0 backward compatible CreateClient hook (called by OnCreateClient)
	virtual BOOL CreateClient(LPCREATESTRUCT lpCreateStruct, CMenu* pWindowMenu) override;


	SVRegressionRunDlg *m_pregTestDlg;

	//{{AFX_VIRTUAL(SVMainFrame)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr) override;
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) override;
	//}}AFX_VIRTUAL

	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);

protected:
	void LoadOEMIcon();

private:
	BOOL VerifyBarStates(LPCTSTR lpszProfileName);

public:

#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	//************************************
	// Method:    GetZoomToolbarValue
	// Description:  Get the value from the zoom toolbar
	// Returns:   double
	//************************************
	double GetZoomToolbarValue() const;

	//************************************
	// Method:    SetZoomToolbar
	// Description:  Set the zoom toolbar with the the values from Zoomhelper
	// Parameter: ZoomHelper& rZoomHelper
	// Returns:   void
	//************************************
	void SetZoomToolbar(const ZoomHelper &rZoomHelper );

	//************************************
	// Method:    EnableZoomToolbar
	// Description:  Enable the zoom tool bar
	// Parameter: bool enable
	// Returns:   void
	//************************************
	void EnableZoomToolbar(bool enable);

	//************************************
	// Method:    IsZoomingPossible
	// Description: True if the zoom toolbar is enabled
	// Returns:   bool
	//************************************
	bool IsZoomingPossible();

	void InitToolBars();
private:

	SVStatusBar				m_wndStatusBar;
	CToolBar				m_wndToolBar;
	ZoomToolBar				m_wndToolBarZoom;

	SVPPQBar			m_PPQBar;

	CBrush m_BackgroundBrush;

	HICON m_oemLargeIcon;
	HICON m_oemSmallIcon;

private:

	BOOL m_bSizeChanged;
	UINT m_PosSize;
	HANDLE m_hDisplayChangedEvent; // Set if the display settings have been changed since the Window was created.

};

