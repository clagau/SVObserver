//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPSplitterFrame
//* .File Name       : $Workfile:   SVIPChildFrm.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 Aug 2014 16:03:20  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "FramedSplitterWnd.h"
#pragma endregion Includes

#pragma region Declarations
class SVIPDoc;
#pragma endregion Declarations

class SVIPSplitterFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE( SVIPSplitterFrame )
protected:
	// App Wizard Generated
	//{{AFX_MSG(SVIPSplitterFrame)
	afx_msg LRESULT OnUpdateAllData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateNextView(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	SVIPSplitterFrame();	// constructor
	virtual ~SVIPSplitterFrame();

	virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* PContext );

public:
	void SetDefaultPaneSizes( RECT &BoundingRect );

	void GetToolWidthAndResultHeight( int& nWidthToolSetView, int& nHeightResultView );

	//{{AFX_VIRTUAL(SVIPSplitterFrame)
	public:
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
	//}}AFX_VIRTUAL

	void SetViewSize( CWnd *l_pView, CSize &p_rViewSize );

	void RefreshAllSplitters();

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// See comments below for a picture of how these splitters look
	FramedSplitterWnd m_oWndSplitter1;
	FramedSplitterWnd m_oWndSplitter2;
	FramedSplitterWnd m_oWndSplitter3;
	FramedSplitterWnd m_oWndSplitter4;
	FramedSplitterWnd m_oWndSplitter5;
	FramedSplitterWnd m_oWndSplitter6;

private:
	SVIPDoc* m_pIPDoc;
};

//  Below is the picture of how the splitters connect
//  The numbers indicate which splitter goes where
//  by default only the left pane of 4 will be maximized
//
//  *****************************************************************************
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       2                               4                                   *
//  *       23333333333333333333333333333333333333333333333333333333333333333333*
//  *       2                  5                      5                         *
//  *       2                  5                      5                         *
//  *       2                  5                      5                         *
//  *       2                  5                      5                         *
//  *       23333333333333333333333333333333333333333333333333333333333333333333*
//  *       2                  6                      6                         *
//  *       2                  6                      6                         *
//  *       2                  6                      6                         *
//  *       2                  6                      6                         *
//  *111111111111111111111111111111111111111111111111111111111111111111111111111*
//  *                                                                           *
//  *                                                                           *
//  *                                                                           *
//  *                                                                           *
//  *****************************************************************************
//

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVOBSERVER_SRC\SVOBSERVER\SVIPChildFrm.h_v  $
 * 
 *    Rev 1.1   14 Aug 2014 16:03:20   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   blue frame around active panel: new functions 
 * RefreshallSplitter, OnMDIActivate
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:16:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.7   07 Oct 2011 14:22:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated MDI Child Window and IP Document to fix threading problems by moving the data through a lockless queue and notifying the IP Document via the MDI Child Window with a windows message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.6   17 Feb 2005 14:35:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.5   22 Apr 2003 10:31:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.4   19 Nov 2002 14:39:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 *            Added splitter wnds
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.3   16 Feb 2000 16:49:30   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added multi-image display to the IPD
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.2   08 Feb 2000 11:03:04   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changes to initialization (SVImageViewScroll is now the
 * child view).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.1   Feb 01 2000 10:20:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  92
 * SCR Title:  Remove Obsolete SVChildFrame class
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed obsolete class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
