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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRGrayDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:56:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Aug 2005 07:35:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 12:53:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Nov 2002 10:26:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:44:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:23:16   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Nov 04 1999 16:23:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed 'Test' button and added check for existance of .Wit Font  training file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Oct 20 1999 07:21:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes for character x and y axis sort work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Oct 12 1999 16:59:30   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Repair Test and Edit Font button sizes and locations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 31 1999 19:21:46   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   OCR converted to 3.0 architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/