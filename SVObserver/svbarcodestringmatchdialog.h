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

#pragma once

class SVBarCodeStringMatchDialog : public CPropertyPage
{
// Construction
public:
	SVBarCodeStringMatchDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVBarCodeStringMatchDialog();

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
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVBarCodeStringMatchDialog)
	afx_msg void OnBarcodeSaveInFile();
	afx_msg void OnBarCodeSelectFile();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBcUseSingle();
	afx_msg void OnBcUseMultiple();
	afx_msg void OnBcMultiSelectfile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
