//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayDialog
//* .File Name       : $Workfile:   SVOCRGrayDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:56:36  $
//******************************************************************************

#if !defined(AFX_SVOCRGRAYDIALOG_H__C66B4E52_DCB0_11D2_842D_000000000000__INCLUDED_)
#define AFX_SVOCRGRAYDIALOG_H__C66B4E52_DCB0_11D2_842D_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVOCRGrayMatchThresholdDlg.h"
#include "SVOCRGrayMatchDlg.h"
#include "SVOCRGrayCandidateDlg.h"
#include "SVOCRGrayGeneralDlg.h"
#include "SVOCRGrayParamStruct.h"

#include "SVTaskObjectInterfaceClass.h"

class SVToolClass;
class SVIPDoc;

/////////////////////////////////////////////////////////////////////////////
// SVOCRGrayDialogClass dialog

class SVOCRGrayDialogClass : public CPropertySheet, public SVTaskObjectInterfaceClass
{
// Construction
public:
	SVOCRGrayDialogClass(CWnd* pParent = NULL);   // standard constructor
  virtual ~SVOCRGrayDialogClass();

	virtual HRESULT SetInspectionData();
		
	void AddButtons();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOCRGrayDialogClass)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:	
    //
	// Save a copy of the parameters for possible CANCEL.
	//
	SVOCRGrayParamStruct	OCRParametersSaved;
	long lBlobSortDirectionSaved;
	long lPixelsForASpaceSaved;

	SVToolClass*		pTool;

	SVIPDoc*			pDocument;
	SVOCRGrayAnalyzeResultClass	*pOCRAnalyzerResult;


	SVOCRGrayGeneralDlg			m_GeneralParamsDlg;
	SVOCRGrayMatchDlg			m_MatchStringParamsDlg;
	SVOCRGrayCandidateDlg		m_CandidateParamsDlg;
	SVOCRGrayMatchThresholdDlg	m_MatchThresholdParamsDlg;

	//CButton				m_testButton;
	CButton				m_editFontButton;
	CFont				m_font;

private:

	void				AddPageDialogs ();

protected:

	// Generated message map functions
	//{{AFX_MSG(SVOCRGrayDialogClass)
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
    afx_msg void OnFontTraining();
	//afx_msg void OnTestButton();
	afx_msg void OnOK();
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOCRGRAYDIALOG_H__C66B4E52_DCB0_11D2_842D_000000000000__INCLUDED_)

