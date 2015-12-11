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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMainFrm.h_v  $
 * 
 *    Rev 1.9   12 Dec 2014 13:12:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  918
 * SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   added new message handler for SV_REGISTER_MONITOR_LIST RemoteRegisterMonitorList.,
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   09 Dec 2014 10:12:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Remove edit move state and supporting functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   02 Sep 2014 12:16:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Added method RefreshAllSplitters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   14 Aug 2014 16:09:36   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   error handling in checkbarstate
 * New zoom toolbar 
 * new entry in ViewMenu 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 Jun 2014 07:08:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  905
 * SCR Title:  Implement Shutdown Command thru Remote Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add Shutdown message handler function that will start the shutdown executable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jun 2014 09:54:04   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added ID_VIEW_IMAGE_DISPLAY_UPDATE and ID_VIEW_RESULT_DISPLAY_UPDATE command and update command handlers.
 * Removed the ID_VIEW_ONLINE_DISPLAY command and update command handlers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Aug 2013 09:46:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Fix merge error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Aug 2013 08:40:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 023.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Aug 2013 06:17:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added refresh message to update the status bar for mode changes from SVRemoteControl.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:26:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.33   16 Apr 2013 15:35:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.32   25 Mar 2013 13:36:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed OnViewInspectionToolBar, OnUpdateViewInspectionToolBar
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.31   11 Feb 2013 14:19:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.30   10 Jan 2013 15:06:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  792
 * SCR Title:  Change SVObserver Background Color to Körber Grey
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added missing log entries.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.29   04 Oct 2012 11:25:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.28   01 Oct 2012 16:18:02   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  792
 * SCR Title:  Change SVObserver Background Color to Körber Grey
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed MDI Client Window background to grey.
 *   Changed m_BlueBrush to m_BackgroundBrush.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.27   25 Sep 2012 15:21:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.26.1.0   08 Nov 2012 15:25:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  792
 * SCR Title:  Change SVObserver Background Color to Körber Grey
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   changed backgroud color to grey
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.26   03 Aug 2012 11:48:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.25   28 Jun 2012 18:00:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  768
 * SCR Title:  Add the display time-out and throttling options for 5.50
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Reverted back to before changes were made for this change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.24   14 May 2012 14:22:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Implement freeze display
 * Implement display update rate options
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.23   29 Apr 2011 08:06:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow for inspection to for the display to give up when an inspection get queued.  The display is now regulated to a maximum of 10 frames per second.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.22   08 Dec 2010 13:05:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.21   23 Jul 2010 09:35:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated status bar functionality to better size the data so that it does not scroll off the screen.  Updated version number to 5.32 Beta 026.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.20   21 Sep 2009 13:32:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  662
 * SCR Title:  Add Display Change Support to SVImageView
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnDisplayChange handler.
 * Added OnActivateApp handler.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.19   14 Nov 2008 14:22:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added IOView Tabs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.18   21 Jun 2007 14:16:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.17   05 Aug 2005 09:07:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Implement Mode on StatusBar and Add color indicators.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.16   02 Aug 2005 13:51:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed Status Option from View Menu
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.15   21 Jul 2005 14:41:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with regression test.  moved the run dialog to SVMainFrame, instead of having it owned by IPDoc. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.14   17 Feb 2005 14:48:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.13   22 Apr 2003 11:07:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.12   27 Feb 2003 15:02:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  323
 * SCR Title:  Bar State problem when switching versions
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added method - VerifyBarStates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.11   19 Nov 2002 15:31:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed Application Bar
 *             Added Tool Bar
 *             Added Status Bar
 *             Added View Online Display
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.10   09 Aug 2001 07:53:26   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  210
 * SCR Title:  Fix of shared data, multiple IPD's, same digitizer
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added declaration of "OnEndObjectCreation" function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.9   15 Jun 2000 08:49:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  163
 * SCR Title:  OEM Splash Screen, Help About Logo and Application Icon
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added LoadOEMIcon method.
 * Added m_oemLargeIcon and m_oemSmallIcon.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.8   05 May 2000 13:54:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  134
 * SCR Title:  Restricted Operator Security level
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnUserChanged handler.
 * Revised OnUpdateViewApplicationBar to check privileges.
 * Revised OnUpdateViewPPQBar to check privileges.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.7   Feb 01 2000 10:09:36   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  90
 * SCR Title:  Save  Operator Tool Move Priviledge in Configuration
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added definition for new message handler for operator tool move feature.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.6   Jan 03 2000 14:27:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  77
 * SCR Title:  Automatic configuration load and go online fails.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes so an automatically loaded configuration will 'go online'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.5   01 Dec 1999 17:21:48   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  68
 * SCR Title:  Defer "Go Online" processing till after configuration load is complete
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified OnRCLoadSEC to notify SVMainFrm that RCCommandComplete must be called when a configuration load is complete. This required adding SetNotifyComplete function to the SVMainFrmClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.4   Nov 30 1999 16:39:48   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  65
 * SCR Title:  Remove obsolute class SVToolSetViewClass
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed references to SVToolSetViewClass and added headers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
