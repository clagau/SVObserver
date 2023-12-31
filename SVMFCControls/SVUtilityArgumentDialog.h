//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilityArgumentDialog
//* .File Name       : $Workfile:   SVUtilityArgumentDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:58:04  $
//******************************************************************************

#pragma once

class SVUtilityArgumentDialog : public CDialog
{
// Construction
public:
	SVUtilityArgumentDialog(CWnd* pParent = nullptr) : CDialog(IDD, pParent) {};
	virtual ~SVUtilityArgumentDialog() = default;

// Dialog Data
	//{{AFX_DATA(SVUtilityArgumentDialog)
	enum { IDD = IDD_ARGUMENT_PROMPT };
	CString	mszArguments{};
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVUtilityArgumentDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVUtilityArgumentDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
