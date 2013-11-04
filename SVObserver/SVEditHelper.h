//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEditHelper
//* .File Name       : $Workfile:   SVEditHelper.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:23:00  $
//******************************************************************************

#pragma once

#define		WM_APP_ED_EDIT_FINISHED			( WM_APP + 04101 )		


class SVEditHelper : public CEdit
{
// Construction
public:
	SVEditHelper();

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~SVEditHelper();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVEditHelper)
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVEditHelper.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:23:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Aug 2012 12:46:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in for editable list box used in remote output groups.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
