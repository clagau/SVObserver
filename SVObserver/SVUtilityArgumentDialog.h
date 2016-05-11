//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilityArgumentDialogClass
//* .File Name       : $Workfile:   SVUtilityArgumentDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:58:04  $
//******************************************************************************

#pragma once

class SVUtilityArgumentDialogClass : public CDialog
{
// Construction
public:
	SVUtilityArgumentDialogClass(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVUtilityArgumentDialogClass();

// Dialog Data
	//{{AFX_DATA(SVUtilityArgumentDialogClass)
	enum { IDD = IDD_ARGUMENT_PROMPT };
	CString	mszArguments;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVUtilityArgumentDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVUtilityArgumentDialogClass)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
