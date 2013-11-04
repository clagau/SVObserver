//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskEditorWnd
//* .File Name       : $Workfile:   SVMaskEditorWnd.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Apr 2013 14:08:52  $
//******************************************************************************

#if !defined(AFX_SVMASKEDITORWND_H__326BF442_EBC4_11D2_A75C_00106F000C7A__INCLUDED_)
#define AFX_SVMASKEDITORWND_H__326BF442_EBC4_11D2_A75C_00106F000C7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVMaskEditorWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorWndClass window

class SVMaskEditorDialogClass;

class SVMaskEditorWndClass : public CWnd
{
// Construction
public:
	SVMaskEditorWndClass();

// Attributes
public:

    SVMaskEditorDialogClass* PMother;

protected:

	BOOL bPerformDblClk;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVMaskEditorWndClass)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL
    void OnPaintBckGrnd();

// Implementation
public:
	virtual ~SVMaskEditorWndClass();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVMaskEditorWndClass)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(SVMaskEditorWndClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVMASKEDITORWND_H__326BF442_EBC4_11D2_A75C_00106F000C7A__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVMaskEditor\SVMaskEditorWnd.h_v  $
 * 
 *    Rev 1.0   26 Apr 2013 14:08:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
