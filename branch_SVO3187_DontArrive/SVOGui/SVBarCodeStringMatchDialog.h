//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	BOOL GetUseSingle() const { return m_UseSingleMatch; };

	BOOL SetRegExpression( LPCTSTR szRegExpression );
	const CString& GetRegExpression() const { return m_RegExpression; };

	BOOL SetSaveInFile( BOOL bValue );
	BOOL GetSaveInFile() const { return m_SaveInFile; };

	BOOL SetSingleFileName( LPCTSTR szFileName );
	const CString& GetSingleFileName() const { return m_StringFileName; };

	BOOL SetUseMultiple( BOOL bValue );
	BOOL GetUseMultiple() const { return m_UseMultiMatch; };

	BOOL SetMultiFileName( LPCTSTR szFileName );
	const CString& GetMultiFileName() const { return m_MultiMatchFileName; };

	enum { IDD = IDD_BARCODE_STRINGMATCH };


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

private:
	BOOL m_UseSingleMatch;
	CString m_RegExpression;
	BOOL m_SaveInFile;
	CString m_StringFileName;
	BOOL m_UseMultiMatch;
	CString m_MultiMatchFileName;
};