// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemFile.h
// * .File Name       : $Workfile:   PropTreeItemFile.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   18 Sep 2014 13:33:28  $
// ******************************************************************************

// PropertyItemFile.h : header file
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.

#pragma once

#include "PropTreeItem.h"

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyItemFile control
// Flags used by dwFlags parameter of the Create(), SetFlags() and DDX_FileEditCtrl() functions
// also returned by GetFlags()
// Usage :
// SVR_FILE			- the control is used to find files, can not be used with SVR_FOLDER
// SVR_FOLDER		- the control is used to find folders, can not be used with SVR_FILE
// SVR_TRAILINGSLASH - used with SVR_FOLDER, adds a slash to the end of the folder string
// SVR_BUTTONLEFT	- places the button on the left side of the control, only used when control is being initialized
// SVR_VALIDATE   - The Folder or Filename entered must already exist.

#define SVR_FILE			0x00000001
#define SVR_FOLDER			0x00000002
#define SVR_TRAILINGSLASH	0x00000004
#define SVR_BUTTONLEFT		0x00000008
#define SVR_VALIDATE		0x00000010

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyItemFile window

class SVRPropertyItemFile : public CEdit, public SVRPropertyItem
{
// Construction
public:
    SVRPropertyItemFile(bool bFullAccess, DWORD dwFlags, LPCTSTR sFilter = nullptr, LPCTSTR sInitialDir = nullptr, BOOL bSetDir = false);
	virtual ~SVRPropertyItemFile();

// Attributes
public:
	// The attribute area needs drawing
	virtual void DrawAttribute(CDC* pDC, const RECT& rc);

	// Retrieve the item's attribute value
	virtual bool GetItemValue(CString& strVal);
    virtual bool GetItemValue(VARIANT& vtVal);

	// Set the item's attribute value
	virtual bool SetItemValue(LPCTSTR lpszVal);

	// @cmember Called when attribute area has changed size.
	virtual void OnMove();

	// @cmember Called when the item needs to refresh its data.
	virtual void OnRefresh();

	// @cmember Called when the item needs to commit its changes.
	virtual void OnCommit();

	// @cmember Called to activate the item.
	virtual void OnActivate();

// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVRPropertyItemFile)
	//}}AFX_VIRTUAL
	bool SetItemType(DWORD dwFlags, LPCTSTR lpszVal = NULL);

// Implementation
public:
	CString GetPathName(void);

	// Generated message map functions
protected:
	// @cmember Initialize member variables.
	void Initialize(void);
	void ResetControl(void);

	// @cmember Handles a mouse click on the button.
	void ButtonClicked();
	
	// @cmember Draws the button.
	void DrawButton(int nButtonState = 0);
	
	// @cmember Draws the dots on the button.
	void DrawDots(CDC *pDC, COLORREF CR, int nOffset = 0);
	
	// @cmember Starts and handles the returns from the SHBrowseForFolder() shell function.
	bool SVRBrowseForFolder();
	
	// @cmember Starts and handles the returns from the <c FileDialog>.
	bool SVROpenFile();
	
	// @cmember Checks if the given point is in the browse button.
	BOOL ScreenPointInButtonRect(CPoint point);

	//{{AFX_MSG(SVRPropertyItemFile)
	afx_msg BOOL OnChange();
	afx_msg void OnDestroy();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	// @cmember Filter for CFileDialog control.
	CString	m_sFilter;
	// @cmember Browse button on left side of control?
	bool m_bButtonLeft;
	// @cmember TRUE while control is being created, FALSE otherwise.
	bool m_bCreatingControl;
	// @cmember Browse for files or folders?
	bool m_bFindFolder;
	// @cmember Button has captured the mouse?
	bool m_bMouseCaptured;
	// @cmember Window text has changed since last time FillBuffers() was called.
	bool m_bTextChanged;
	// @cmember Add a trailing slash to folders?
	bool m_bTrailingSlash;
	// @cmember Current button state (up, down, or disabled).
	int m_nButtonState;
	// @cmember Window coordinates of the button.
	CRect m_rcButtonRect;

    BOOL m_bInitialDirSet;
    CString m_sInitialDir;
	bool m_bFullAccess;
protected:
	CString m_sAttribute;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

