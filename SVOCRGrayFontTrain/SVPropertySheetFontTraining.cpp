//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVPropertySheetFontTraining
//* .File Name       : $Workfile:   SVPropertySheetFontTraining.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 10:32:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVLVFastOCR.h"	// Added by ClassView
#include "SVPropertySheetFontTraining.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSVPropertySheetFontTraining

IMPLEMENT_DYNAMIC(CSVPropertySheetFontTraining, CPropertySheet)

CSVPropertySheetFontTraining::CSVPropertySheetFontTraining(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	InitPropertySheetParameters();
}

CSVPropertySheetFontTraining::CSVPropertySheetFontTraining(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	InitPropertySheetParameters();
}

void CSVPropertySheetFontTraining::InitPropertySheetParameters()
{
	m_pFastOCR = NULL;

	m_pExtractedCharImage = NULL;
	m_pObjOcrFont = NULL;
	m_pOcrFont = NULL;

	m_enumProgramState = PROGRAM_STATE_CHAR_IMAGE_NOT_SELECTED;

	m_bCharactersAndLabelsChanged = FALSE;   // until first char/label created.

	m_bFirstVisitToCharsAndLabels = TRUE;

	//
	// Turn off the APPLY button.
	//
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	AddPage(&m_Page1);
	AddPage(&m_Page2);
}

CSVPropertySheetFontTraining::~CSVPropertySheetFontTraining()
{
	if (m_pExtractedCharImage)
	{
		m_pFastOCR->DestroyLVObject(&m_pExtractedCharImage);
	}

	if (m_pObjOcrFont)
	{
		m_pFastOCR->DestroyLVObject(&m_pObjOcrFont);
	}
}

BEGIN_MESSAGE_MAP(CSVPropertySheetFontTraining, CPropertySheet)
	//{{AFX_MSG_MAP(CSVPropertySheetFontTraining)
	ON_WM_SYSCOMMAND()
	ON_COMMAND(IDOK,OnIDOK)
	ON_COMMAND(IDCANCEL,OnIDCANCEL)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVPropertySheetFontTraining message handlers

BOOL CSVPropertySheetFontTraining::OnInitDialog() 
{
	BOOL bResult = FALSE;

	if (m_pObjOcrFont)
	{
		m_pFastOCR->DestroyLVObject(&m_pObjOcrFont);  // Should never be called
	}

	m_pObjOcrFont = m_pFastOCR->CreateLVObject("CorOcrFont");
	if (m_pObjOcrFont)
	{
		CorOcrFont *pOcrFont = (CorOcrFont *)CorObj_OcrFont(m_pObjOcrFont);
		CorImageVector *pImageVec = CorObj_svOcrFontImages(pOcrFont);
		CorStringVector *pStringVec = CorObj_svOcrFontLabels(pOcrFont);

		CorObj_type(pImageVec) = COR_OBJ_IMAGE;
		CorObj_type(pStringVec) = COR_OBJ_STRING;

		m_Page1.mbSizeSet = m_pFastOCR->ReadLVObjectFromFile(m_pObjOcrFont,m_csPathFileFontParameters);
		if (m_Page1.mbSizeSet)
		{
			CorOcrFont *pFont = (CorOcrFont *)CorObj_OcrFont(m_pObjOcrFont);
			CorImageVector *pImVec = &pFont->images;
			CorImage *pImage = (CorImage *)CorObj_mdData(pImVec);

			m_Page1.mfHeight = (float) CorObj_height(pImage);
			m_Page1.mfWidth =	(float) CorObj_width(pImage);
		}

		bResult = CorObj_type(m_pObjOcrFont) == CorObjGetId("CorOcrFont");

		bResult = CPropertySheet::OnInitDialog();
	}

	return bResult;
}

void CSVPropertySheetFontTraining::OnIDOK()
{
	int nResult;

	m_enumProgramState = PROGRAM_STATE_EXIT_OK_PROCESSED;

	//
	// Make sure the Wit 'getData' ROI is stopped.
	//
	m_Page1.CleanUpWitGetData();    // 11 Aug 1999 - frb.

	//
	// Make sure the WIT font editor is stopped.
	//
	m_Page2.CleanUpWitEdit();

	//
	// Save the font char images and labels if necessary.
	//
	CleanUpAndSaveWitObjects();

	nResult = AfxMessageBox("Confirm Exit from Font Training",MB_YESNO);
	if(nResult == IDYES)
	{
		EndDialog(IDOK);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//
// returns IDYES or IDNO
//
int CSVPropertySheetFontTraining::ConfirmUserAbort()
{
	int nResult = AfxMessageBox("Confirm Exit and Abort Any Font Changes",MB_YESNO);
	return nResult;
}

void CSVPropertySheetFontTraining::OnIDCANCEL()
{
	if(ConfirmUserAbort() == IDYES)
	{
		CleanUpAndExitCancel();
	}
}

void CSVPropertySheetFontTraining::CleanUpAndExitCancel()
{
	m_Page1.CleanUpWitGetData();    // 11 Aug 1999 - frb.
	m_Page2.CleanUpWitEdit();       // 11 Aug 1999 - frb.

	EndDialog(IDCANCEL); // used to terminate a modal dialog
}

/////////////////////////////////////////////////////////////////////////////
//
//
// Copy the array of font images and associated labels to an output file.
//
//
void CSVPropertySheetFontTraining::CleanUpAndSaveWitObjects()
{
	if(m_bCharactersAndLabelsChanged)
	{
		int nResult = AfxMessageBox("Confirm Saving to Font Model File",MB_YESNO);
		if(nResult == IDYES)
		{
			if (m_pFastOCR->WriteLVObjectToFile(m_pObjOcrFont,m_csPathFileFontParameters))
			{
				m_bCharactersAndLabelsChanged = FALSE;
			}
		}
	}
}

void CSVPropertySheetFontTraining::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if(nID == SC_CLOSE)
	{
		if(ConfirmUserAbort() == IDYES)
		{
			CleanUpAndExitCancel();
		}
		else
		{
			return;
		}
	}

	CPropertySheet::OnSysCommand(nID, lParam);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOCRGrayFontTrain\SVPropertySheetFontTraining.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 10:32:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Oct 2000 13:59:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  189
 * SCR Title:  Fix Object Initialization Problem with FastOCR Library
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This change fixes the problem with the creation of the CorOcrFont object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 15:08:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/