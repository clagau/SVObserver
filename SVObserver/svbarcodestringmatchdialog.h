//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeStringMatchDialog
//* .File Name       : $Workfile:   svbarcodestringmatchdialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:43:10  $
//******************************************************************************

#if !defined(AFX_SVBARCODESTRINGMATCH_H__AF5FDD6A_7BE0_11D3_A7D0_00106F010A93__INCLUDED_)
#define AFX_SVBARCODESTRINGMATCH_H__AF5FDD6A_7BE0_11D3_A7D0_00106F010A93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SVBarCodeStringMatchDialog : public CPropertyPage
{
// Construction
public:
	SVBarCodeStringMatchDialog(CWnd* pParent = NULL);   // standard constructor

	BOOL EnableFileSelect (BOOL bEnable);
	void EnableDisableControls(BOOL bSingle = TRUE);

	BOOL SetUseSingle( BOOL bValue );
	BOOL GetUseSingle( BOOL &rbValue );

	BOOL SetRegExpression( LPCTSTR szRegExpression );
	BOOL GetRegExpression( CString &rcsRegExpression );

	BOOL SetSaveInFile( BOOL bValue );
	BOOL GetSaveInFile( BOOL &rbValue );

	BOOL SetSingleFileName( LPCTSTR szFileName );
	BOOL GetSingleFileName( CString &rcsFileName );

	BOOL SetUseMultiple( BOOL bValue );
	BOOL GetUseMultiple( BOOL &rbValue );

	BOOL SetMultiFileName( LPCTSTR szFileName );
	BOOL GetMultiFileName( CString &rcsFileName );

// Dialog Data
	//{{AFX_DATA(SVBarCodeStringMatchDialog)
	enum { IDD = IDD_BARCODE_STRINGMATCH };
	BOOL	mbUseSingleMatch;
	CString	m_szRegExpression;
	BOOL	m_bSaveInFile;
	CString	m_szStringFileName;
	BOOL	mbUseMutiMatch;
	CString	mcsMultiMatchFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVBarCodeStringMatchDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVBarCodeStringMatchDialog)
	afx_msg void OnBarcodeSaveInFile();
	afx_msg void OnBarCodeSelectFile();
	virtual BOOL OnInitDialog();
	afx_msg void OnBcUseSingle();
	afx_msg void OnBcUseMultiple();
	afx_msg void OnBcMultiSelectfile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVBARCODESTRINGMATCH_H__AF5FDD6A_7BE0_11D3_A7D0_00106F010A93__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\svbarcodestringmatchdialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:43:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   17 Apr 2003 16:54:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/