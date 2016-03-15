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

#if !defined(AFX_SVREGRESSIONFILESELECTDLG_H__126EEBB2_34C5_4542_B002_B1A875D6CF1D__INCLUDED_)
#define AFX_SVREGRESSIONFILESELECTDLG_H__126EEBB2_34C5_4542_B002_B1A875D6CF1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVRegressionFileSelectDlg.h : header file
//

#include "SVRegressionTestStruct.h"
#include "SVRegressionFileSelectSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CSVRegressionFileSelectDlg dialog

class CSVRegressionFileSelectDlg : public CPropertyPage/*, public ISVPropertyPageDialog*/
{
	DECLARE_DYNCREATE(CSVRegressionFileSelectDlg)

// Construction
public:
	CSVRegressionFileSelectDlg(LPCTSTR lptstrDialogName = NULL);
	~CSVRegressionFileSelectDlg();

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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSVRegressionFileSelectDlg)
	afx_msg void OnBtnRegTestBrowseFiles();
	virtual BOOL OnInitDialog();
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

#endif // !defined(AFX_SVREGRESSIONFILESELECTDLG_H__126EEBB2_34C5_4542_B002_B1A875D6CF1D__INCLUDED_)

