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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVUtilitiesCustomizeDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:57:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   02 Mar 2007 06:27:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  595
 * SCR Title:  Fix issues with Utility Menu when switching products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the utilities to use the ini files instead of the registry
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 17:17:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */