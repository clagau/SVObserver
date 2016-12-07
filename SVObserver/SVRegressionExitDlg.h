// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionExitDlg
// * .File Name       : $Workfile:   SVRegressionExitDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 14:41:08  $
// ******************************************************************************

#pragma once

class SVRegressionExitDlg : public CDialog
{
// Construction
public:
	SVRegressionExitDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVRegressionExitDlg();

// Dialog Data
	//{{AFX_DATA(SVRegressionExitDlg)
	enum { IDD = IDD_DLG_EXIT_REGRESSION };
	CComboBox	m_ctlExitModes;
	CString	m_csExitModes;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVRegressionExitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVRegressionExitDlg)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int m_iExitMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
