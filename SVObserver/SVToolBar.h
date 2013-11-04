//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolBarClass
//* .File Name       : $Workfile:   SVToolBar.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:42:02  $
//******************************************************************************

#if !defined(AFX_SVToolbar_H__3BD1EE4E_079D_11D3_A74F_00106F000C78__INCLUDED_)
#define AFX_SVToolbar_H__3BD1EE4E_079D_11D3_A74F_00106F000C78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SV_TOOLBAR_BORDER_W 4
#define SV_TOOLBAR_BORDER_H	4

/////////////////////////////////////////////////////////////////////////////
// SVToolBarClass window

class SVToolBarClass : public CToolBarCtrl
{
// Construction
public:
	SVToolBarClass();

// Attributes
public:

private:
	int         m_nButtonCount;
	TBBUTTON    *m_pTBButtons;

// Operations
public:

	virtual BOOL Create(	DWORD dwStyle, const RECT& rect, 
							CWnd* pParentWnd, UINT nID, 
							UINT nBaseID, int nCount,const SIZE &size);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVToolBarClass)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SVToolBarClass();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVToolBarClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVToolbar_H__3BD1EE4E_079D_11D3_A74F_00106F000C78__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolBar.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:42:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 16:47:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */