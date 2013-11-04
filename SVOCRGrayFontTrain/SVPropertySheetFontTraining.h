//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVPropertySheetFontTraining
//* .File Name       : $Workfile:   SVPropertySheetFontTraining.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 10:32:48  $
//******************************************************************************

#if !defined(AFX_PROPERTYSHEETFONTTRAINING_H__68C04C5C_ECEF_11D2_A7A0_00106F000C7D__INCLUDED_)
#define AFX_PROPERTYSHEETFONTTRAINING_H__68C04C5C_ECEF_11D2_A7A0_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "SVOCRGrayFontTrain.h"
#include "SVPropertyPageSelectCharImages.h"
#include "SVPropertyPageCharsAndLabels.h"

#define __WIT_DEBUG_DISPLAY      // Use Wit popup windows for debug display
//#define __USE_WIT_INITIALIZE     // 06 May 1999 - frb.

enum ENUM_PROGRAM_STATE
{
   PROGRAM_STATE_NOT_INITIALIZED = -1,
   PROGRAM_STATE_CHAR_IMAGE_SELECTED = 0,
   //PROGRAM_STATE_CHAR_IMAGE_SELECTED_FIRST_ONE = 1,
   PROGRAM_STATE_CHAR_IMAGE_NOT_SELECTED = 2,

   PROGRAM_STATE_EXIT_OK_PROCESSED,
   PROGRAM_STATE_EXIT_OK_AND_SAVE,
   PROGRAM_STATE_EXIT_OK_AND_NOT_SAVE,

   PROGRAM_STATE_EXIT_CANCEL_PROCESSED
};

class SVLVFastOCR;

/////////////////////////////////////////////////////////////////////////////
// CSVPropertySheetFontTraining

class CSVPropertySheetFontTraining : public CPropertySheet
{
	DECLARE_DYNAMIC(CSVPropertySheetFontTraining)

public:
	enum ENUM_PROGRAM_STATE m_enumProgramState;

	CSVPropertySheetFontTraining(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSVPropertySheetFontTraining(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CSVPropertySheetFontTraining();

	BOOL m_bCharactersAndLabelsChanged;
	BOOL m_bFirstVisitToCharsAndLabels;

	CString  m_csPathFileFontParameters;    // Output from font 'training'.
	CString  m_csPathFileInputImage;

	SVLVFastOCR *m_pFastOCR;

	CorOcrFont *m_pOcrFont;  // pointer to the font object contained in m_objOcrFont
	CorObj *m_pObjOcrFont; // container object that wraps up the font
	CorObj *m_pExtractedCharImage;

	CSVPropertyPageSelectCharImages m_Page1;
	CSVPropertyPageCharsAndLabels   m_Page2;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVPropertySheetFontTraining)
public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

protected:
	void InitPropertySheetParameters();
	void CleanUpAndSaveWitObjects();
	void CleanUpAndExitCancel();
	int ConfirmUserAbort();

	//{{AFX_MSG(CSVPropertySheetFontTraining)
	afx_msg void OnIDOK();
	afx_msg void OnIDCANCEL();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYSHEETFONTTRAINING_H__68C04C5C_ECEF_11D2_A7A0_00106F000C7D__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOCRGrayFontTrain\SVPropertySheetFontTraining.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 10:32:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 15:08:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 15:24:26   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 26 1999 14:24:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Port code from v2.42 and bug fixes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/