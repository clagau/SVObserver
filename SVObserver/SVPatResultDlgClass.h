//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatResultDlgClass
//* .File Name       : $Workfile:   SVPatResultDlgClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:19:48  $
//******************************************************************************

#pragma once

/////////////////////////////////////////////////////////////////////////////
// SVPatResultDlgClass dialog

class SVPatResultDlgClass : public CDialog
{
// Construction
public:
	SVPatResultDlgClass(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVPatResultDlgClass();

// Dialog Data
	//{{AFX_DATA(SVPatResultDlgClass)
	enum { IDD = IDD_PAT_RESULT_DIALOG };
	CString	m_strAngle;
	CString	m_strScore;
	CString	m_strXPos;
	CString	m_strYPos;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPatResultDlgClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVPatResultDlgClass)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
