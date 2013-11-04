// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVFileEditCtrl
// * .File Name       : $Workfile:   SVFileEditCtrl.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:36:46  $
// ******************************************************************************

#ifndef SVFILEEDITCTRL_H
#define SVFILEEDITCTRL_H

#include "SVFEButton.h"

/*  FileEditCtrl style constants.
	This control uses these optional Edit control style constants:
		One or none of - ES_LEFT (default), ES_CENTER, or ES_RIGHT.
		One or none of - ES_UPPERCASE or ES_LOWERCASE.
		ES_AUTOHSCROLL.

	This control also defines two of its own style constants:
		One or none of - FES_FILE (default) or FES_FOLDER.

	If you specify FES_FILE (the default), then clicking on the
	browse button will bring up the 'File Open' dialog.
	If you specify FES_FOLDER, then clicking on the browse button
	will bring up the SHBrowseForFolder dialog.
*/

#define FES_FILE   0x0000L
#define FES_FOLDER 0x4000L

/////////////////////////////////////////////////////////////////////////////
// SVFileEditCtrl window

class SVFileEditCtrl : public CWnd
{
// Construction
public:
	SVFileEditCtrl(CWnd* pParent,
								CRect& rect,
								DWORD dwStyle,
								UINT nID,
								int nRow, int nCol,
								LPCTSTR sInitText,
								UINT nFirstChar,
								BOOL bExitOnArrows=TRUE);

// Attributes
public:

// Operations
public:
	void EndEdit();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVFileEditCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
		virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SVFileEditCtrl();
	BOOL FindFolder(BOOL bFolder);			// changes the controls functionality
	POSITION GetStartPosition();			// get the position of the first file
	CString GetNextPathName(POSITION &pos);	// get the file at pos and update pos
	CEdit* GetEditControl() const;			// returns a pointer to the internal edit control
	OPENFILENAME* GetOpenFileName() const;	// returns a pointer to the internal OPENFILENAME structure
	BROWSEINFO* GetBrowseInfo();			// returns a pointer to the internal BROWSEINFO structure
	int GetWindowText (LPTSTR, int) const;	// get the text in the control
	void GetWindowText (CString &) const;
	int GetWindowTextLength() const;		// get the text length in the edit control
	void SetWindowText(LPCTSTR text);		// set the text in the control
	void SetFont(CFont *pFont, BOOL bRedraw = TRUE); // set the controls font
	void SetSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE);


	// Generated message map functions
protected:
	BOOL FECOpenFile();
	BOOL FECBrowseForFolder();
	//{{AFX_MSG(SVFileEditCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	afx_msg void OnBrowse();
	afx_msg void OnKillfocusEdit();
	DECLARE_MESSAGE_MAP()

	void AllocateBuffer(int len);

private:
	CString			m_sInitText;
	int					m_nRow;
	int					m_nCol;
	UINT				m_nLastChar;
	BOOL				m_bExitOnArrows;
	CFileDialog	*m_pcfd;
	BROWSEINFO	m_bi;
	CEdit				*m_edit;
	SVFEButton	*m_button;
	BOOL				m_bFindFolder;
	TCHAR				*m_buffer;
};

#endif	// #define AFX_SVFILEEDITCTRL_INCLUDED_

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVFileEditCtrl.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:36:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Oct 2010 08:54:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include issues and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Oct 2010 08:10:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Jun 2007 15:16:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Mar 2001 13:33:22   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Program change: no longer requires SVFEEdit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Dec 2000 11:54:58   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Split existing classes into their own file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/