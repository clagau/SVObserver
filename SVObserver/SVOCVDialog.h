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

#if !defined(SVOCVDIALOGCLASS_H)
#define SVOCVDIALOGCLASS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVOCVMatchDlg.h"
#include "SVOCVGeneralDlg.h"

class SVToolClass;
class SVIPDoc;
class SVOCVAnalyzeResultClass;

/////////////////////////////////////////////////////////////////////////////
// SVOCVDialogClass dialog

class SVOCVDialogClass : public CPropertySheet
{
// Construction
public:
	SVOCVDialogClass(CWnd* pParent = NULL);   // standard constructor
    virtual ~SVOCVDialogClass();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOCVDialogClass)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:	
	SVToolClass				*pTool;
	SVIPDoc					*pDocument;
	SVOCVAnalyzeResultClass	*pOCVAnalyzerResult;

	SVOCVGeneralDlg			m_GeneralParamsDlg;
	SVOCVMatchDlg			m_MatchStringParamsDlg;

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

#endif // !defined(SVOCVDIALOGCLASS_H)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCVDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:02:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Sep 2003 13:07:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/