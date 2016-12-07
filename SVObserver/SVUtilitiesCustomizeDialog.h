//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilitiesCustomizeDialogClass
//* .File Name       : $Workfile:   SVUtilitiesCustomizeDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:57:38  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilities.h"
#pragma region Includes

/////////////////////////////////////////////////////////////////////////////
// SVUtilitiesCustomizeDialogClass dialog

class SVUtilitiesCustomizeDialogClass : public CDialog
{
// Construction
public:
	SVUtilitiesCustomizeDialogClass(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVUtilitiesCustomizeDialogClass();

private:
	void SetDeleteState (BOOL bEnabled);
	void SetApplyState (BOOL bEnabled);
	UINT    muiId;
	SVUtilitiesClass *m_pUtilityClass;

// Dialog Data
	//{{AFX_DATA(SVUtilitiesCustomizeDialogClass)
	enum { IDD = IDD_EXTRAS_UTILITIES_CUSTOMIZE };
	CString	mszArguments;
	CString	mszCommand;
	CString	mszMenuText;
	BOOL	mbPromptForArguments;
	CString	mszWorkingDirectory;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVUtilitiesCustomizeDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVUtilitiesCustomizeDialogClass)
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
