//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectListCtrl
//* .File Name       : $Workfile:   SVTaskObjectListCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:35:02  $
//******************************************************************************

#if !defined(AFX_SVTASKOBJECTLISTCTRL_H__436D5ACA_C8F6_11D2_ACC1_00C04FC38F76__INCLUDED_)
#define AFX_SVTASKOBJECTLISTCTRL_H__436D5ACA_C8F6_11D2_ACC1_00C04FC38F76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SVTaskObjectListClass;

class SVTaskObjectListCtrlClass : public CListCtrl
{
	DECLARE_DYNCREATE(SVTaskObjectListCtrlClass)

// Construction
public:
	SVTaskObjectListCtrlClass();

// Attributes
public:
protected:
	SVTaskObjectListClass* pOwnerTaskList;

// Operations
public:
	
	BOOL SetTaskObjectList( SVTaskObjectListClass* POwnerTaskList );
	int GetCurrentSelection( int StartAt = -1 /* start searching at top */ );
	BOOL IsValidSelection( int index );

	virtual void Rebuild();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVTaskObjectListCtrlClass)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SVTaskObjectListCtrlClass();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVTaskObjectListCtrlClass)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnWindowPosChanged( WINDOWPOS* lpwndpos );
	afx_msg void OnBeginTrack(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVTASKOBJECTLISTCTRL_H__436D5ACA_C8F6_11D2_ACC1_00C04FC38F76__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTaskObjectListCtrl.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:35:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   16 Aug 2005 15:21:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  506
 * SCR Title:  Update Tool Set List Tab to include an indicator when a tool is invalid
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated header to make Rebuild method virtual.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 16:36:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   20 Nov 2002 13:52:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed old Automation, ole dispatch map
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   02 Dec 1999 11:14:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  66
 * SCR Title:  ToolSet TabView Sizing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use OnWindowPosChanged handler instead of OnSize.
 * Added handler for HDN_BEGINTRACK to restrict sizing of 
 * the column heading by the user.
 * Revised the column heading style to not be a button that can be clicked.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
