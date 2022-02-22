//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilitiesCustomizeDialog
//* .File Name       : $Workfile:   SVUtilitiesCustomizeDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:57:38  $
//******************************************************************************

#pragma once

#pragma region Includes
#pragma region Includes

class SVUtilities;

class SVUtilitiesCustomizeDialog : public CDialog
{
// Construction
public:
	SVUtilitiesCustomizeDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVUtilitiesCustomizeDialog();

private:
	void SetDeleteState (BOOL bEnabled);
	void SetApplyState (BOOL bEnabled);
	SVUtilities *m_pUtilityClass;

// Dialog Data
	//{{AFX_DATA(SVUtilitiesCustomizeDialog)
	enum { IDD = IDD_EXTRAS_UTILITIES_CUSTOMIZE };
	CString	m_Arguments;
	CString	m_Command;
	CString	m_MenuText;
	CString	m_WorkingDirectory;
	BOOL	mbPromptForArguments;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVUtilitiesCustomizeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVUtilitiesCustomizeDialog)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelEndOkCustomizeMenuText();
	afx_msg void OnEditChangeCustomizeMenuText();
	afx_msg void OnChangeCustomizeCommand();
	afx_msg void OnChangeCustomizeArguments();
	afx_msg void OnChangeCustomizeWorkingDirectory();
	afx_msg void OnCustomizePromptForArguments();
	afx_msg void OnApply();
	virtual void OnOK() override;
	afx_msg void OnCustomizeDelete();
	afx_msg void OnCustomizeCommandSelect();
	afx_msg void OnCustomizeDirectorySelect();
	afx_msg void OnKillfocusCustomizeMenutext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
