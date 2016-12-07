//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVDialog
//* .File Name       : $Workfile:   SVOCVDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:02:48  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOCVMatchDlg.h"
#include "SVOCVGeneralDlg.h"
#pragma endregion Includes

class SVToolClass;
class SVIPDoc;
class SVOCVAnalyzeResultClass;

class SVOCVDialogClass : public CPropertySheet
{
public:
	SVOCVDialogClass(SVIPDoc* pDoc, SVOCVAnalyzeResultClass* pResult, CWnd* pParent = nullptr);   // standard constructor
    virtual ~SVOCVDialogClass();

	SVOCVAnalyzeResultClass* GetAnalyzerResult() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOCVDialogClass)
	public:
	virtual BOOL OnInitDialog() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

private:	
	SVIPDoc* m_pDocument;
	SVOCVAnalyzeResultClass* m_pOCVAnalyzerResult;

	SVOCVGeneralDlg	m_GeneralParamsDlg;
	SVOCVMatchDlg	m_MatchStringParamsDlg;

private:
	void AddPageDialogs();

protected:
	// Generated message map functions
	//{{AFX_MSG(SVOCVDialogClass)
	//}}AFX_MSG
	afx_msg void OnOK();
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

