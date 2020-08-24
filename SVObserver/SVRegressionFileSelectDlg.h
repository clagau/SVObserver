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

#pragma region Includes
#include "SVRegressionTestStruct.h"
#pragma endregion Includes

class SVRegressionFileSelectSheet;

/////////////////////////////////////////////////////////////////////////////
// SVRegressionFileSelectDlg dialog

class SVRegressionFileSelectDlg : public CPropertyPage/*, public ISVPropertyPageDialog*/
{
	DECLARE_DYNCREATE(SVRegressionFileSelectDlg)

// Construction
public:
	SVRegressionFileSelectDlg(LPCTSTR lptstrDialogName = nullptr);
	virtual ~SVRegressionFileSelectDlg();

	void SetDlgTitle( LPCTSTR lpszTitle );

	RegressionFileEnum GetFileSelectType();
	CString GetSelectedFile();
	CString GetPageName();
	void SetRegressionData(RegressionTestStruct *p_pDataStruct);



// Dialog Data
	//{{AFX_DATA(SVRegressionFileSelectDlg)
	enum { IDD = IDD_PROPPAGE_REGRESSION_FILESELECT };
	CButton	m_btnBrowseFiles;
	int		m_iSelectFileRadio;
	CString	m_RegTestFiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVRegressionFileSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVRegressionFileSelectDlg)
	afx_msg void OnBtnRegTestBrowseFiles();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnRadioRegUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	/// Show a dialog to select a file and save the result in m_RegTestFiles if press IDOK.
	/// \param bFullAccess [in]
	void ShowSelectFileDlg(bool bFullAccess);
	/// Show a dialog to select a directory and save the result in m_RegTestFiles if press IDOK.
	/// \param bFullAccess [in]
	void ShowSelectDirectoryDlg(bool bFullAccess);

private:
    SVRegressionFileSelectSheet *m_pParent;
	std::string m_DialogName;
	std::string m_RegistryPath;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
