// ******************************************************************************
// * COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
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

using strMatrix = std::vector<std::vector<std::string>>;

/////////////////////////////////////////////////////////////////////////////
// SVRegressionFileSelectDlg dialog

class SVRegressionFileSelectDlg : public CPropertyPage/*, public ISVPropertyPageDialog*/
{
	DECLARE_DYNCREATE(SVRegressionFileSelectDlg)

	// Construction
public:
	SVRegressionFileSelectDlg(LPCTSTR lptstrDialogName = nullptr, bool isCamera = true, uint32_t toolId = SvDef::InvalidObjectId);
	virtual ~SVRegressionFileSelectDlg();
	void SetDlgTitle(LPCTSTR lpszTitle);
	void ShowControls();
	void OnSelectAll();
	RegressionFileEnum GetFileSelectType();
	CString GetSelectedFile();
	CString GetPageName();
	uint32_t getToolId() const { return m_toolId; };
	void SetRegressionData(RegressionTestStruct* p_pDataStruct);
	bool isCamera() const { return m_isCamera; };
	const std::vector<std::string>& GetFolders() const { return m_folders; };

	// Dialog Data
		//{{AFX_DATA(SVRegressionFileSelectDlg)
	enum { IDD = IDD_PROPPAGE_REGRESSION_FILESELECT };
	CButton	m_btnBrowseFiles;
	int		m_iSelectFileRadio;
	CString	m_RegTestFiles;
	std::vector<std::string> m_folders;
	CListBox	m_SelectFolderList;
	CEdit m_EditRegTestFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVRegressionFileSelectDlg)
protected:
	virtual BOOL PreTranslateMessage(MSG*);
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVRegressionFileSelectDlg)
	afx_msg void OnBtnRegTestBrowseFiles();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnRemovePage();
	afx_msg void OnClickRadioMultDir();
	afx_msg void OnClickRadioList();
	afx_msg void OnClickRadioNone();
	afx_msg void OnClickRadioSingle();
	afx_msg void OnClickRadioDirectory();
	afx_msg void OnClickRadioSubDirectory();
	afx_msg void OnDeleteFolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void OnClickRadio(int select);
	/// Show a dialog to select a file and save the result in m_RegTestFiles if press IDOK.
	/// \param bFullAccess [in]
	void ShowSelectFileDlg(bool bFullAccess);
	/// Show a dialog to select a directory and save the result in m_RegTestFiles if press IDOK.
	/// \param bFullAccess [in]
	void ShowSelectDirectoryDlg(bool bFullAccess);
	void ShowSelectMultDirectoryDlg(bool bFullAccess);
	void UpdateFolderList();
	void UpdateEditCtrlSelect();
	void UpdateRadioButton();

private:
	static std::vector<int> m_PreviousSelctedRadius;
	static strMatrix m_PreviousFolderSettings;
	SVRegressionFileSelectSheet* m_pParent;
	std::string m_DialogName;
	std::string m_RegistryPath;
	bool m_isCamera = true;
	uint32_t m_toolId = SvDef::InvalidObjectId;
	int m_CameraNumber = 0;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
