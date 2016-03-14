//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVPropertySheetFontTraining
//* .File Name       : $Workfile:   SVPropertySheetFontTraining.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:39:26  $
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

		m_pFastOCR->ReadLVObjectFromFile(m_pObjOcrFont,m_csPathFileFontParameters);

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

