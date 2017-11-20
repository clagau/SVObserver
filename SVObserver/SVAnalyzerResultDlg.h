//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzerResultDlg
//* .File Name       : $Workfile:   SVAnalyzerResultDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:32:42  $
//******************************************************************************
#pragma once

#pragma region Inlcudes
#include "SVOCVBlobRecord.h"
#pragma endregion Inlcudes

class SVAnalyzerResultDlg : public CDialog
{
// Construction
public:
	SVAnalyzerResultDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVAnalyzerResultDlg();

	SVOCVCharacterResultPtrVector *m_pOcvCharaterResults{ nullptr };

// Dialog Data
	//{{AFX_DATA(SVAnalyzerResultDlg)
	enum { IDD = IDD_ANALYZE_RESULT };
	CListBox	mclbResultList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVAnalyzerResultDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVAnalyzerResultDlg)
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

