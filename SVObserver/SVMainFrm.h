//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMFCControls/SVStatusBar.h"
#include "SVOGuiUtility/ZoomHelper.h"
#include "SVOGuiUtility/ZoomToolBar.h"
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
public:
#pragma region Constructor
	SVMainFrame();
	virtual ~SVMainFrame();
#pragma endregion Constructor

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	void OnConfigurationFinishedInitializing();

	BOOL BuildPPQButtons();
	BOOL DestroyPPQButtons();

	void SetStatusInfoText(LPCTSTR SIText);

	void RefreshAllSplitters(); // Redraws the splitter frames

	LRESULT InvalidateStatusBar(WPARAM wParam, LPARAM lParam);
	LRESULT Shutdown(WPARAM wParam, LPARAM lParam);
	LRESULT UpdateIPDocViews(WPARAM wParam, LPARAM lParam);
	LRESULT SetIPDocEditToolID(WPARAM wParam, LPARAM lParam);

	virtual void OnUpdateFrameTitle(BOOL bAddToTitle) override;
	void UpdateFrameTitleForDocument(LPCTSTR lpszDocName);

	void updateStatusBar() { m_wndStatusBar.OnIdleUpdateCmdUI(1, 0); }

	SVRegressionRunDlg* m_pregTestDlg;

	//{{AFX_VIRTUAL(SVMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr) override;

	void UpdateStatusInfo(CCmdUI* pCmdUI);
	afx_msg LRESULT RemoteRegisterMonitorList(WPARAM wParam, LPARAM lParam);

	//}}AFX_VIRTUAL

#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif


protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) override;

	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);

	void LoadOEMIcon();


//******************************************************************************
// Manually Generated Message Map Entries:
//******************************************************************************

	//{{AFX_MSG(SVMainFrame)

private:

#pragma region AFX_MSG Methods

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnAppAbout();
	afx_msg void OnFileNewConfig();
	afx_msg void OnFileCloseConfig();
	afx_msg void OnFileSaveAll();
	afx_msg void OnFileOpenSVC();
	afx_msg void OnFileSaveAsSVC();

	afx_msg void OnEditEnvironment();

	afx_msg void OnFileSaveConfig(bool saveAs = false);
	afx_msg void OnRunMode();
	afx_msg void OnStop();

	// Special for SVRCComm.DLL
	afx_msg void OnRCGoOffline();
	afx_msg void OnRCClose();

	afx_msg void OnEditIOList();
	afx_msg void OnTestMode();
	afx_msg void OnStopTestMode();
	afx_msg void OnModeEdit();
	afx_msg void OnGoOffline();
	afx_msg void OnGoOnline();

	afx_msg void OnFilePrintConfig();

	afx_msg void OnExtrasLogin();
	afx_msg void OnExtrasLogout();
	afx_msg void OnExtrasUtilitiesEdit();
	afx_msg void OnExtrasSecuritySetup();
	afx_msg void OnAuditTrailDefaultFiles();
	afx_msg void OnAuditTrailAdditionalFiles();

	void OnRCOpenCurrentSVX();
	void OnMemLeakDetection();

	afx_msg void OnAppExit();

	afx_msg void OnSVOFilePrintSetup();

	afx_msg void OnUpdateAuditTrailDefaultFiles(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAuditTrailAdditionalFiles(CCmdUI* PCmdUI);
	afx_msg void OnUpdateModeRun(CCmdUI* PCmdUI);
	afx_msg void OnUpdateEditEnvironment(CCmdUI* PCmdUI);
	afx_msg void OnUpdateEditIOList(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileClose(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileNew(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileOpenSVC(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileSaveAll(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileSaveCopyAs(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileSaveImage(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFileUpdate(CCmdUI* PCmdUI);
	afx_msg void OnUpdateEditEditTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateEditEditToolSet(CCmdUI* PCmdUI);
	afx_msg void OnUpdateEditRemoteInputs(CCmdUI* PCmdUI);
	afx_msg void OnUpdateExtrasLightReference(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFilePrintSetup(CCmdUI* PCmdUI);
	afx_msg void OnUpdateExtrasTestOutputs(CCmdUI* PCmdUI);
	afx_msg void OnUpdateHelp(CCmdUI* PCmdUI);
	afx_msg void OnUpdateHelpFinder(CCmdUI* PCmdUI);
	afx_msg void OnUpdateModeTest(CCmdUI* PCmdUI);
	afx_msg void OnUpdateNextPane(CCmdUI* PCmdUI);
	afx_msg void OnUpdatePrevPane(CCmdUI* PCmdUI);
	afx_msg void OnUpdateWindowArrange(CCmdUI* PCmdUI);
	afx_msg void OnUpdateWindowCascade(CCmdUI* PCmdUI);
	afx_msg void OnUpdateWindowNew(CCmdUI* PCmdUI);
	afx_msg void OnUpdateWindowTileHorz(CCmdUI* PCmdUI);
	afx_msg void OnUpdateWindowTileVert(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAppExit(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAppAbout(CCmdUI* PCmdUI);
	afx_msg void OnUpdateExtrasLogin(CCmdUI* PCmdUI);
	afx_msg void OnUpdateGoOffline(CCmdUI* PCmdUI);
	afx_msg void OnUpdateGoOnline(CCmdUI* PCmdUI);
	afx_msg void OnUpdateFilePrintConfig(CCmdUI* PCmdUI);
	afx_msg void OnUpdateExtrasLogout(CCmdUI* PCmdUI);
	afx_msg void OnRunMostRecentMRU();
	afx_msg void OnUpdateExtraUtilities(CCmdUI* PCmdUI);
	afx_msg void OnUpdateResultsPicker(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRegressionTest(CCmdUI* pCmdUI);
	afx_msg void OnEditMonitorList();
	afx_msg void OnUpdateEditAddMonitorList(CCmdUI* pCmdUT);

	// Special catch in Run Mode...
	afx_msg void OnRunUtility(UINT uiId);

	// Special for SVRCComm.DLL
	afx_msg void OnRCGoOnline();
	afx_msg void OnUpdateAllIOViews();
	afx_msg void OnTriggerSettings();
	afx_msg void OnUpdateModeEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTriggerSettings(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExtrasUtilitiesEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExtrasSecuritySetup(CCmdUI* pCmdUI);
	afx_msg void OnRCCloseAndCleanUpDownloadDirectory();

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

	afx_msg void OnUpdateStatusInfo(CCmdUI* pCmdUI);
	afx_msg void OnViewPPQBar();

	afx_msg LRESULT OnUserChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onAddFileToConfig(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onSaveConfig(WPARAM wParam, LPARAM lParam);
	// Toolbar Methods
	afx_msg void OnToolbarZoom();
	afx_msg void OnUpdateToolbarZoom(CCmdUI* pCmdUI);

	//}}AFX_MSG

#pragma endregion AFX_MSG Methods

	DECLARE_MESSAGE_MAP()

	BOOL VerifyBarStates(LPCTSTR lpszProfileName);

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

	void startDesiredState(DWORD desiredState);

	SVStatusBar				m_wndStatusBar;
	CToolBar				m_wndToolBar;
	ZoomToolBar				m_wndToolBarZoom;

	SVPPQBar			m_PPQBar;

	CBrush m_BackgroundBrush;

	HICON m_oemLargeIcon;
	HICON m_oemSmallIcon;

	BOOL m_bSizeChanged;
	UINT m_PosSize;
	HANDLE m_hDisplayChangedEvent; // Set if the display settings have been changed since the Window was created.
};

SVMainFrame* GetSvoMainFrame();
