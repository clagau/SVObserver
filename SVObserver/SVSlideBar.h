//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSlideBarClass
//* .File Name       : $Workfile:   SVSlideBar.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:07:54  $
//******************************************************************************

#if !defined(AFX_SVSLIDEBAR_H__0ED68ACB_42C2_11D3_A881_00106F000C74__INCLUDED_)
#define AFX_SVSLIDEBAR_H__0ED68ACB_42C2_11D3_A881_00106F000C74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVSlideBar.h : header file
//


#define SV_SLIDEBAR_WIDTH		5
#define SV_SLIDEBAR_HEIGHT		5


/////////////////////////////////////////////////////////////////////////////
// SVSlideBarClass window

class SVSlideBarClass : public CWnd
{
// Construction
public:
	SVSlideBarClass();
	
	virtual ~SVSlideBarClass();


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVSlideBarClass)
	//}}AFX_VIRTUAL

// Implementation
protected:
	void invertTracker( const CRect& rect );



	// Generated message map functions
protected:
	//{{AFX_MSG(SVSlideBarClass)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:

enum
{
	SV_SLIDEBAR_HORZ	= 10,
	SV_SLIDEBAR_VERT
};

	BOOL	bTracking;
	DWORD	dwSlideBarType;
	CRect   trackRect;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVSLIDEBAR_H__0ED68ACB_42C2_11D3_A881_00106F000C74__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVSlideBar.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:07:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 15:34:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */