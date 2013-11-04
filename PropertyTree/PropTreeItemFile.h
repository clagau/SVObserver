// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemFile.h
// * .File Name       : $Workfile:   PropTreeItemFile.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 16:43:18  $
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

#ifndef PROPERTYITEMFILE_H
#define PROPERTYITEMFILE_H

#include <shlobj.h>
#include <shlwapi.h>

#include "PropTreeItem.h"

#pragma comment(lib, "shlwapi")

/////////////////////////////////////////////////////////////////////////////
// @doc INTERNAL
// @class SVRFileDialog
// @base public | CFileDialog
// The sole purpose for this class is to change the
// text of the "OPEN" button to "OK".

class SVRFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(SVRFileDialog)
		
public:
	SVRFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
						LPCTSTR lpszDefExt = NULL,
						LPCTSTR lpszFileName = NULL,
						DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						LPCTSTR lpszFilter = NULL,
						CWnd* pParentWnd = NULL);


protected:
	//{{AFX_MSG(SVRFileDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	virtual void OnInitDone();
	DECLARE_MESSAGE_MAP()
};


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

#define SVR_FILE				0x00000001
#define SVR_FOLDER			0x00000002
#define SVR_TRAILINGSLASH	0x00000004
#define SVR_BUTTONLEFT		0x00000008
#define SVR_VALIDATE			0x00000010


/////////////////////////////////////////////////////////////////////////////
// SVRPropertyItemFile window

class SVRPropertyItemFile : public CEdit, public SVRPropertyItem
{
// Construction
public:
	SVRPropertyItemFile();
	SVRPropertyItemFile(DWORD dwFlags, LPCTSTR sFilter = NULL);
    SVRPropertyItemFile(DWORD dwFlags, LPCTSTR sFilter = NULL, LPCTSTR sInitialDir = NULL,
                        BOOL bSetDir = FALSE);
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
	// @cmember Returns a pointer to the internal BROWSEINFO structure.
	BROWSEINFO* GetBrowseInfo() const;
	CString GetPathName(void);
	// @cmember Returns a pointer to the internal OPENFILENAME structure.
	OPENFILENAME* GetOpenFileName() const;
	// @cmember Get the starting position for GetNextPathName().

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
	
	// @cmember Starts and handles the returns from the <c CFileDialog>.
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
	CString				m_sFilter;
	// @cmember Browse button on left side of control?
	bool					m_bButtonLeft;
	// @cmember TRUE while control is being created, FALSE otherwise.
	bool					m_bCreatingControl;
	// @cmember Browse for files or folders?
	bool					m_bFindFolder;
	// @cmember Button has captured the mouse?
	bool					m_bMouseCaptured;
	// @cmember Window text has changed since last time FillBuffers() was called.
	bool					m_bTextChanged;
	// @cmember Add a trailing slash to folders?
	bool					m_bTrailingSlash;
	// @cmember Current button state (up, down, or disabled).
	int					m_nButtonState;
	// @cmember Only active when m_bFindFolder is TRUE.
	BROWSEINFO*			m_pBROWSEINFO;
	// @cmember Only active when m_bFindFolder is FALSE.
	SVRFileDialog*	m_pCFileDialog;
	// @cmember Window coordinates of the button.
	CRect					m_rcButtonRect;

    BOOL m_bInitialDirSet;
    CString m_sInitialDir;
protected:
	CString			m_sAttribute;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYITEMFILE_H__642536B1_1162_4F99_B09D_9B1BD2CF88B6__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\PropertyTree\PropTreeItemFile.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 16:43:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Oct 2010 07:53:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Jun 2007 08:42:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Jun 2007 12:20:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 15:42:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/