// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionFileSelectDlg
// * .File Name       : $Workfile:   SVRegressionFileSelectDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 14:41:32  $
// ******************************************************************************

#pragma once

#include "SVRegressionTestStruct.h"
#include "SVRegressionFileSelectSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CSVRegressionFileSelectDlg dialog

class CSVRegressionFileSelectDlg : public CPropertyPage/*, public ISVPropertyPageDialog*/
{
	DECLARE_DYNCREATE(CSVRegressionFileSelectDlg)

// Construction
public:
	CSVRegressionFileSelectDlg(LPCTSTR lptstrDialogName = nullptr);
	virtual ~CSVRegressionFileSelectDlg();

	void SetDlgTitle( LPCTSTR lpszTitle );

	RegressionFileEnum GetFileSelectType();
	CString GetSelectedFile();
	CString GetFilePath();
	CString GetPageName();
	void SetRegressionData(RegressionTestStruct *p_pDataStruct);



// Dialog Data
	//{{AFX_DATA(CSVRegressionFileSelectDlg)
	enum { IDD = IDD_PROPPAGE_REGRESSION_FILESELECT };
	CButton	m_btnBrowseFiles;
	int		m_iSelectFileRadio;
	CString	m_sRegTestFiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSVRegressionFileSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSVRegressionFileSelectDlg)
	afx_msg void OnBtnRegTestBrowseFiles();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnRadioRegList();
	afx_msg void OnRadioRegNone();
	afx_msg void OnRadioRegSingle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_sDialogName;
	CString m_sFilePath;
    CSVRegressionFileSelectSheet *m_pParent;
	CString m_sRegistryPath;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
