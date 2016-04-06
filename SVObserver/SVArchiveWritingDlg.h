// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVArchiveWritingDlg
// * .File Name       : $Workfile:   SVArchiveWritingDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 09:33:26  $
// ******************************************************************************

#pragma once

/////////////////////////////////////////////////////////////////////////////
// SVArchiveWritingDlg dialog

class SVArchiveWritingDlg : public CDialog
{
// Construction
public:
	SVArchiveWritingDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVArchiveWritingDlg();

// Dialog Data
	//{{AFX_DATA(SVArchiveWritingDlg)
	enum { IDD = IDD_DLG_ARCHIVETOOL_CLOSE_PROGRESS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVArchiveWritingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVArchiveWritingDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
