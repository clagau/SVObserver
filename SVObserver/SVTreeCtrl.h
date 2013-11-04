//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTreeCtrl
//* .File Name       : $Workfile:   SVTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:27:42  $
//******************************************************************************

#if !defined(AFX_SVTreeCtrl_H__A4C49D6C_24DA_11d4_A8F2_00106F030D16__INCLUDED_)
#define AFX_SVTreeCtrl_H__A4C49D6C_24DA_11d4_A8F2_00106F030D16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// SVTreeCtrlClass window

class SVTreeCtrlClass : public CTreeCtrl
{
	DECLARE_DYNCREATE(SVTreeCtrlClass)

// Construction
public:
	SVTreeCtrlClass();
	BOOL InitTreeCtrl();

// Attributes

protected:
	CImageList StateImageList;

// Operations
public:
	virtual BOOL GetCheckState(	HTREEITEM hItem	);

	virtual BOOL SetCheckState(	HTREEITEM hItem, BOOL fCheck );
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVTreeCtrlClass)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SVTreeCtrlClass();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVTreeCtrlClass)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVTreeCtrl_H__A4C49D6C_24DA_11d4_A8F2_00106F030D16__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTreeCtrl.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 12:27:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 17:07:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Nov 2002 14:47:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed OLE Automation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 May 2000 09:32:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Implements a generic tree control for SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
