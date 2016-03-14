//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRDialog
//* .File Name       : $Workfile:   SVOCRDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:55:10  $
//******************************************************************************

#if !defined(AFX_SVOCRDIALOG_H__C66B4E52_DCB0_11D2_842D_000000000000__INCLUDED_)
#define AFX_SVOCRDIALOG_H__C66B4E52_DCB0_11D2_842D_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVOCRParamStruct.h"
#include "SV_OMD.h"
#include "sv_match.h"
#include "sv_ogd.h"
#include "sv_ocmd.h"

#include "SVTaskObjectInterfaceClass.h"

/////////////////////////////////////////////////////////////////////////////
// SVOCRDialogClass dialog

class SVToolClass;
class SVIPDoc;
class SVOCRAnalyzeResultClass;

class SVOCRDialogClass : public CPropertySheet, public SVTaskObjectInterfaceClass
{
	// Construction
public:
	SVOCRDialogClass(CWnd* pParent = NULL);   // standard constructor
  virtual ~SVOCRDialogClass();

	virtual HRESULT SetInspectionData();

	void AddButtons();
	
	//
	// Save a copy of the parameters for possible CANCEL.
	//
	SVOCRParamStruct	OCRParametersSaved;
	long				lBlobSortDirectionSaved;
	long				lPixelsForASpaceSaved;
	
	SVToolClass				*pTool;
	SVIPDoc					*pDocument;
	SVOCRAnalyzeResultClass	*pOCRAnalyzerResult;
	
	SVOCRMiscDlg		m_miscParamsDlg;
	SVOCRMatchDlg		m_matchParamsDlg;
	SVOCRGeometryDlg	m_geometryParamsDlg;
	SVOCRCharMatchDlg	m_charMatchParamsDlg;
	
	CButton				m_editFontButton;
	CFont				m_font;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOCRDialogClass)
	public:
		virtual BOOL OnInitDialog();
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
protected:
	
	// Generated message map functions
	//{{AFX_MSG(SVOCRDialogClass)
	afx_msg void OnFontTraining();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

private:
	
	void AddPageDialogs ();
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOCRDIALOG_H__C66B4E52_DCB0_11D2_842D_000000000000__INCLUDED_)

