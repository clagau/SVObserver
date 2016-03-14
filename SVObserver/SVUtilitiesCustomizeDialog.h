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

#if !defined(AFX_SVUTILITIESCUSTOMIZEDIALOG_H__0011F4F0_9C37_11D3_A80B_00106F010A93__INCLUDED_)
#define AFX_SVUTILITIESCUSTOMIZEDIALOG_H__0011F4F0_9C37_11D3_A80B_00106F010A93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVUtilities.h"

/////////////////////////////////////////////////////////////////////////////
// SVUtilitiesCustomizeDialogClass dialog

class SVUtilitiesCustomizeDialogClass : public CDialog
{
// Construction
public:
	CString mszUtilityKey;
	SVUtilitiesCustomizeDialogClass(CWnd* pParent = NULL);   // standard constructor

	void SetUtilityClass(SVUtilitiesClass *pUtilClass );

private:
	void SetDeleteState (BOOL bEnabled);
	void SetApplyState (BOOL bEnabled);
  CString mszCommandValueName;
  CString mszArgumentsValueName;
  CString mszWorkingDirectoryValueName;
  CString mszPromptValueName;
  CString mszIdValueName;
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVUtilitiesCustomizeDialogClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelEndOkCustomizeMenuText();
	afx_msg void OnEditChangeCustomizeMenuText();
	afx_msg void OnChangeCustomizeCommand();
	afx_msg void OnChangeCustomizeArguments();
	afx_msg void OnChangeCustomizeWorkingDirectory();
	afx_msg void OnCustomizePromptForArguments();
	afx_msg void OnApply();
	virtual void OnOK();
	afx_msg void OnCustomizeDelete();
	afx_msg void OnCustomizeCommandSelect();
	afx_msg void OnCustomizeDirectorySelect();
	afx_msg void OnKillfocusCustomizeMenutext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVUTILITIESCUSTOMIZEDIALOG_H__0011F4F0_9C37_11D3_A80B_00106F010A93__INCLUDED_)

