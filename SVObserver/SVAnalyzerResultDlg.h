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

#if !defined(SVANALYZERRESULTDLG_H)
#define SVANALYZERRESULTDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVOCVBlobRecord.h"
/////////////////////////////////////////////////////////////////////////////
// SVAnalyzerResultDlg dialog

class SVOCRBlobRecordArray;

class SVAnalyzerResultDlg : public CDialog
{
// Construction
public:
	SVAnalyzerResultDlg(CWnd* pParent = NULL);   // standard constructor

	SVOCRBlobRecordArray *svocrbraResults;
	SVOCVCharacterResultArray *psvocvResultArray;

// Dialog Data
	//{{AFX_DATA(SVAnalyzerResultDlg)
	enum { IDD = IDD_ANALYZE_RESULT };
	CListBox	mclbResultList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVAnalyzerResultDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVAnalyzerResultDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(SVANALYZERRESULTDLG_H)

