//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVPropertyPageCharsAndLabels
//* .File Name       : $Workfile:   SVPropertyPageCharsAndLabels.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:38:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVLVFastOCR.h"
#include "SVOCRFontTraining.h"
#include "SVPropertyPageCharsAndLabels.h"
#include "SVPropertySheetFontTraining.h"
#include "SVDlgCharLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CorEdit corEditHandle;
static int corEditDoneProc(CorEdit handle, Boolean done)
{
  if (done)
    corEditHandle = NULL;
  else
    corEditHandle = handle;

	return 0;
}

static CorDisplay CLcorDisplayHandle;
static int CLcorDisplayDoneProc(CorDisplay handle, Boolean done)
{
  if (done)
    CLcorDisplayHandle = NULL;
  else
    CLcorDisplayHandle = handle;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CSVPropertyPageCharsAndLabels property page

IMPLEMENT_DYNCREATE(CSVPropertyPageCharsAndLabels, CPropertyPage)

CSVPropertyPageCharsAndLabels::CSVPropertyPageCharsAndLabels() : CPropertyPage(CSVPropertyPageCharsAndLabels::IDD)
{
	//{{AFX_DATA_INIT(CSVPropertyPageCharsAndLabels)
	m_csStaticDirections = _T("");
	m_csAddCharLabel = _T("");
	m_radioPenWhite = 0;             // Set for White Pen
	//}}AFX_DATA_INIT

	m_bFirstTime = TRUE;

	m_pFastOCR = NULL;

	m_nBrushSize = 1;   // One pixel
	m_pObjBrushColor = NULL;
	m_pObjEditedCharImage = NULL;

	m_hCorDisplay = NULL;
	m_hCorEdit = NULL;

	m_pCharImage = NULL;
	m_pCharImageVec = NULL;
	m_pCharLabel = NULL;
	m_pCharLabelVec = NULL;

	m_pObjExtractedCharImage = NULL;

	m_pOcrFont = NULL;
}

CSVPropertyPageCharsAndLabels::~CSVPropertyPageCharsAndLabels()
{
	if (m_hCorDisplay)
	{
		cor_displayDestroy(m_hCorDisplay);
	}

	if (m_hCorEdit)
	{
		cor_editDestroy(m_hCorEdit);
	}

	if (m_pObjBrushColor)
	{
		m_pFastOCR->DestroyLVObject(&m_pObjBrushColor);
	}

	if (m_pObjEditedCharImage)
	{
		m_pFastOCR->DestroyLVObject(&m_pObjEditedCharImage);
	}
}

void CSVPropertyPageCharsAndLabels::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVPropertyPageCharsAndLabels)
	DDX_Control(pDX, IDC_STATIC_PEN_SIZE, m_staticPenSize);
	DDX_Text(pDX, IDC_STATIC_DIRECTIONS, m_csStaticDirections);
	DDX_Control(pDX, IDC_LIST_CHAR_LABELS, m_listCharLabels);
	DDX_Text(pDX, IDC_EDIT_ADD, m_csAddCharLabel);
	DDX_Radio(pDX, IDC_RADIO_PEN_WHITE, m_radioPenWhite);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSVPropertyPageCharsAndLabels, CPropertyPage)
	//{{AFX_MSG_MAP(CSVPropertyPageCharsAndLabels)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnButtonModify)
	ON_BN_CLICKED(IDC_CHAR_OK, OnCharOk)
	ON_BN_CLICKED(IDC_CHAR_CANCEL, OnCharCancel)
	ON_LBN_DBLCLK(IDC_LIST_CHAR_LABELS, OnDblclkListCharLabels)
	ON_LBN_SELCHANGE(IDC_LIST_CHAR_LABELS, OnSelchangeListCharLabels)
	ON_BN_CLICKED(IDC_RADIO_PEN_WHITE, OnRadioPenWhite)
	ON_BN_CLICKED(IDC_RADIO_PEN_BLACK, OnRadioPenBlack)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PEN_SIZE, OnDeltaposSpinPenSize)
	ON_BN_CLICKED(IDC_CHAR_EDIT_RESET, OnCharEditReset)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSVPropertyPageCharsAndLabels::OnInitDialog() 
{
	InitializeWitObjects();

	BOOL bOk = CPropertyPage::OnInitDialog();

	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	return bOk;
}

/////////////////////////////////////////////////////////////////////////////
// CSVPropertyPageCharsAndLabels message handlers
/////////////////////////////////////////////////////////////////////////////
//
//
//
void CSVPropertyPageCharsAndLabels::InitializeWitObjects()
{
	CSVPropertySheetFontTraining *pSheet = (CSVPropertySheetFontTraining *)GetParent();
	ASSERT_VALID(pSheet);

	m_pFastOCR = pSheet->m_pFastOCR;

	if (m_pObjBrushColor)
	{
		m_pFastOCR->DestroyLVObject(&m_pObjBrushColor);
	}

	m_pObjBrushColor = m_pFastOCR->CreateLVObject(COR_OBJ_UBYTE);
	CorObj_UByte(m_pObjBrushColor) = 255; // White

	m_pOcrFont = (CorOcrFont *)CorObj_OcrFont(pSheet->m_pObjOcrFont);
	m_pCharImageVec = CorObj_svOcrFontImages(m_pOcrFont);
	m_pCharLabelVec = CorObj_svOcrFontLabels(m_pOcrFont);
	m_pObjExtractedCharImage = pSheet->m_pExtractedCharImage;

	//
	// Enable/Disable buttons and set directions text.
	//
	SetForSelectCharFromList();
}

void CSVPropertyPageCharsAndLabels::OnDestroy() 
{
	//
	// just in case ...
	//
	CancelWitEdit();

	CPropertyPage::OnDestroy();
}

void CSVPropertyPageCharsAndLabels::SetForSelectCharFromList()
{
	//
	// Disable the buttons that we will not use for now
	//
	GetDlgItem(IDC_CHAR_OK)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHAR_CANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MODIFY)->EnableWindow(FALSE);

	//
	// Disable character edit controls
	//
	GetDlgItem(IDC_RADIO_PEN_WHITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PEN_BLACK)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_PEN_SIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHAR_EDIT_RESET)->EnableWindow(FALSE);

	//
	// Set the directions control text.
	//
	m_csStaticDirections = "Select Character from list or Get Another Image";
	UpdateData(FALSE);           // to controls
}


/////////////////////////////////////////////////////////////////////////////
//
//
// This method gets control when the 'edit' operator is destroyed in response
// to user clicking "OK" button.
//
void CSVPropertyPageCharsAndLabels::OnPixelEditorCompletes()
{
	CSVPropertySheetFontTraining *pSheet = (CSVPropertySheetFontTraining *)GetParent();
	ASSERT_VALID(pSheet);

	//
	// A single character is completed
	//
	GetDlgItem(IDC_CHAR_OK)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHAR_CANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MODIFY)->EnableWindow(FALSE);

	//
	// Disable character edit controls
	//
	GetDlgItem(IDC_RADIO_PEN_WHITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PEN_BLACK)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_PEN_SIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHAR_EDIT_RESET)->EnableWindow(FALSE);

	//
	// Meanwhile we can check for our button activity.
	//
	switch(m_enumPixelEditorState)
	{
		case ENUM_CHAR_OK:
		{
			break;
		}
		case ENUM_CHAR_CANCEL:
		default:
		{
			//
			// 11 Aug 1999 - frb.
			// Indicate that character list changed...
			//
			pSheet->m_bCharactersAndLabelsChanged = TRUE;

			//
			// Start the char image acquisition all over again on other page.
			//
			SetForSelectCharFromList();
			return;
		}
	}

	//
	// Validate the char label selected for this new character image.
	//
	UpdateData(TRUE);
	BOOL bDone = FALSE;
	while(!bDone)
	{
		if(m_csAddCharLabel.GetLength() == 0)
		{
			SVDlgCharLabel dlg;

			dlg.m_csCharLabel = m_csAddCharLabel;

			if (dlg.DoModal() == IDCANCEL)
			{
				SetForSelectCharFromList();
				return;
			}

			m_csAddCharLabel = dlg.m_csCharLabel;
			continue;           // Test again
		}

		bDone = TRUE;
	}

	//
	// Scan the label string object for non-display characters.
	//
	int nLen = m_csAddCharLabel.GetLength();
	for(int i=nLen-1;i>=0;i--)
	{
		if(m_csAddCharLabel.GetAt(i)>= 0x20)
		{
			break;
		}

		m_csAddCharLabel.SetAt(i,0);
	}

	//
	// Update the font's image vector and label vector.
	// The new image is in m_pObjEditCharImage;
	// The new label is in m_csAddCharLabel.
	//

	//
	// Wrap up the vectors (required by addElem) and create temporary objects for addElem's outputs
	//

	// Wrap up the label string 
	CorObj objLabel = {0};
	m_pFastOCR->InitializeLVStringObject(&objLabel,m_csAddCharLabel);

	//
	// Add the edited label to the vector of labels
	//
	BOOL bOk = m_pFastOCR->AddLVObjectToVector(&objLabel,(CorVector *)m_pCharLabelVec);
	bOk = m_pFastOCR->ReleaseLVObject(&objLabel) && bOk;  // May not need to do this

	//
	// Add the edited image to the vector of images
	//
	bOk = m_pFastOCR->AddLVObjectToVector(m_pObjEditedCharImage,(CorVector *)m_pCharImageVec) && bOk;

	//
	// abb: memory management note: at this point, the parent's 
	// font image vector and label vector have been updated to include the
	// new character image and label. m_pObjEditCharImage has still
	// not been freed (it must be done sooner or later...)
	//

	//
	// Add the character label string to the list box.
	//
	int nResult = m_listCharLabels.AddString((const char *)m_csAddCharLabel);
	ASSERT(nResult != LB_ERR  && nResult != LB_ERRSPACE );

	//
	// We have at least one char/label combination so...
	//
	pSheet->m_bCharactersAndLabelsChanged = TRUE;

	//
	// Reset for next character image to add to list of character images and
	// associated labels.
	//
	SetForSelectCharFromList();
}

/////////////////////////////////////////////////////////////////////////////
//
// Action when a char in the list of characters is selected so user can
// modify the individual character image.
//
void CSVPropertyPageCharsAndLabels::OnButtonModify() 
{
	BOOL bResult = TRUE;

	CSVPropertySheetFontTraining *pSheet = (CSVPropertySheetFontTraining *)GetParent();
	ASSERT_VALID(pSheet);

	if(m_listCharLabels.GetCount() == 0)
	{
		return;           // Maybe we should complain???
	}

	int nCurSelection = m_listCharLabels.GetCurSel();

	if(!(nCurSelection >= 0 && nCurSelection < m_listCharLabels.GetCount()))
	{
		AfxMessageBox("Select Character Label to Modify");
		return;
	}

	//
	// Make sure the current wit 'edit' operator has ended. This also
	// abandons any modifications made to the character being edited.
	// Also, free the current pixel editor source image (m_pObjExtractedCharImage)
	//
	CancelWitEdit();

	if (pSheet->m_pExtractedCharImage)
	{
		bResult = m_pFastOCR->DestroyLVObject(&(pSheet->m_pExtractedCharImage));
	}

	if (bResult)
	{
		pSheet->m_pExtractedCharImage = m_pFastOCR->CreateLVObject(COR_OBJ_IMAGE);
		m_pObjExtractedCharImage = pSheet->m_pExtractedCharImage;
	}

	//
	// Extract image corresponding to selected character from vector of images
	//
	// wrap up the font's image vector in a container object (needed by getElem)

	if (bResult)
	{
		bResult = m_pFastOCR->GetLVObjectFromVector(m_pObjExtractedCharImage, 
		                                            (CorVector *)m_pCharImageVec,
																							  nCurSelection);
	}

	// wrap up the font's label vector in a container object (needed by getElem)
	if (bResult)
	{
		CorObj objLabel = {0};

		bResult = m_pFastOCR->GetLVObjectFromVector(&objLabel, 
		                                            (CorVector *)m_pCharLabelVec,
																							  nCurSelection);

		if (bResult)
		{
			// Scan the temporary buffer for non-display characters.
			//
			char *cTemp = CorObj_string(&objLabel);
			int nLen = strlen(cTemp);
			for(int i=nLen-1;i>=0;i--)
			{
				if(cTemp[i] >= 0x20)
				{
					break;
				}

				cTemp[i] = 0;
			}

			//
			//
			// Put label in edit control.
			//
			m_csAddCharLabel = CorObj_string(&objLabel);
			bResult = m_pFastOCR->ReleaseLVObject(&objLabel);

			//
			// Delete string from char list box now that everything seems OK.
			//
			bResult = m_listCharLabels.DeleteString(nCurSelection) != LB_ERR && bResult;
		}
	}

	UpdateData(FALSE);                 // to controls

	//
	// Actual start of pixel editor 
	//
	OnStartPixelEditor();
}

void CSVPropertyPageCharsAndLabels::RebuildCharacterList()
{
	char *label;

	//
	// We can only use the input vector of chars/label one time
	// abb: (comment) -- it can be done more than once, but it only
	// makes sense to do it once on entry
	//
	if(!m_bFirstTime)
	{
		return;
	}
	m_bFirstTime = FALSE;

	//
	// Clear the list box of characters.
	//
	m_listCharLabels.ResetContent();

	// abb
	// determine length (number of characters) of font 
	// (note: the assertion is just idiot-proofing during initial development)
	//
	ASSERT(CorObj_size(m_pCharImageVec) == CorObj_size(m_pCharLabelVec));
	int fontLength = CorObj_size(m_pCharImageVec);
	m_pCharLabel = CorObj_mdData(m_pCharLabelVec);
	//
	// build the label list from the vector of label strings in the OcrFont
	//
	for(int i = 0; i < fontLength; i++)
	{
		label = *m_pCharLabel++;
		//
		// Scan the temporary buffer for non-display characters.
		//
		int nLen = strlen(label);
		for(int i=nLen-1;i>=0;i--)
		{
			if(label[i] >= 0x20)
			{
				break;
			}

			label[i] = 0;
		}

		//abb: I don't know what the above filtering is for: there should
		// not be a problem of non-printing characters in the labels. 
		// (Unless the MFC dialog control is misbehaving).
		m_listCharLabels.AddString(label);
	}

	m_csAddCharLabel.Empty();
	UpdateData(FALSE);             // to controls.
}

BOOL CSVPropertyPageCharsAndLabels::OnKillActive() 
{
	//
	// If the Wit 'edit' operator is running - cancel it, and abandon any
	// changes made to current image.
	//
	if(m_enumPixelEditorState == ENUM_EDIT_RUNNING)
	{
		CancelWitEdit();
	}

	return CPropertyPage::OnKillActive();
}

void CSVPropertyPageCharsAndLabels::CleanUpWitEdit() 
{
	//
	// If the Wit 'edit' operator is running - destroy it, and abandon any
	// mods to current input image.
	//
	if(m_enumPixelEditorState == ENUM_EDIT_RUNNING)
	{
		CancelWitEdit();
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//
// Action necessary when user "double clicks" on a character label in list
// control.
//
void CSVPropertyPageCharsAndLabels::OnDblclkListCharLabels() 
{
	if(m_enumPixelEditorState == ENUM_EDIT_RUNNING)
	{
		AfxMessageBox("Pixel Editor has active character");
		return;
	}

	OnButtonModify();
}

void CSVPropertyPageCharsAndLabels::OnSelchangeListCharLabels() 
{
	int nCurSel = m_listCharLabels.GetCurSel();
	if(nCurSel != LB_ERR )
	{
		//
		// Ignore user attempts to modify an exisiting char/label
		// in font training list if pixel editor is busy...
		//
		if( m_enumPixelEditorState == ENUM_EDIT_RUNNING )
		{
			return;
		}

		//
		// Enable button so user can move char into pixel editor..
		//
		GetDlgItem(IDC_BUTTON_MODIFY)->EnableWindow(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//
// Action when the char image is OK and the label has been assigned by the
// user.
//
void CSVPropertyPageCharsAndLabels::OnCharOk() 
{
	UpdateData(TRUE);            // From controls to member variables;

	if(m_enumPixelEditorState == ENUM_EDIT_RUNNING)
	{
		BOOL bResult = FALSE;

		if (m_hCorEdit)
		{
			bResult = cor_editDestroy(m_hCorEdit);
			m_hCorEdit = NULL;
		}

		if (m_hCorDisplay)
		{
			bResult = cor_displayDestroy(m_hCorDisplay);
			m_hCorDisplay = NULL;

			m_pFastOCR->CopyLVObject(m_pObjExtractedCharImage,m_pObjEditedCharImage);
		}

		m_enumPixelEditorState = ENUM_CHAR_OK;
		OnPixelEditorCompletes();
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//
// Action taken when tha user abandons the char currently selected into the
// character image pixel edit window.
//
void CSVPropertyPageCharsAndLabels::OnCharCancel() 
{
	//
	// Check to see if the pixel editor is running.
	//
	if(m_enumPixelEditorState == ENUM_EDIT_RUNNING)
	{
		int nResult = AfxMessageBox("Confirm Abort of Current Character",
														    MB_YESNO, MB_ICONQUESTION);

		if(nResult != IDYES)
		{
			return;
		}
		CancelWitEdit();
		SetForSelectCharFromList();
	}

	m_enumPixelEditorState = ENUM_CHAR_CANCEL;
}

void CSVPropertyPageCharsAndLabels::CancelWitEdit()
{
	//
	// Check to see if the WIT pixel editor is running.
	//
	if(m_enumPixelEditorState == ENUM_EDIT_RUNNING)
	{
		if (m_hCorEdit)
		{
			BOOL bResult = cor_editDestroy(m_hCorEdit);
			m_hCorEdit = NULL;
		}

		if (m_hCorDisplay)
		{
			BOOL bResult = cor_displayDestroy(m_hCorDisplay);
			m_hCorDisplay = NULL;
		}

		m_enumPixelEditorState = ENUM_CHAR_NOT_DEFINED;
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//
// This method is called when the training cycle for a new character is 
// to be started, i.e. has been selected by the user from the main image.
//
//
BOOL CSVPropertyPageCharsAndLabels::OnSetActive() 
{
	CSVPropertySheetFontTraining * pSheet = (CSVPropertySheetFontTraining *)GetParent();
	ASSERT_VALID(pSheet);

	//
	// Disable the buttons that we will not use for now
	//
	GetDlgItem(IDC_CHAR_OK)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHAR_CANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MODIFY)->EnableWindow(FALSE);

	//
	// Check for the first visit to this property page..
	//
	if(pSheet->m_bFirstVisitToCharsAndLabels == TRUE)
	{
		RebuildCharacterList();
		pSheet->m_bCharactersAndLabelsChanged = FALSE;
	}

	//
	// Check the current program state concerning char ROI.
	//
	switch (pSheet->m_enumProgramState)
	{
		case PROGRAM_STATE_CHAR_IMAGE_SELECTED:
		{
			//
			// Clear the label field
			//
			m_pObjExtractedCharImage = pSheet->m_pExtractedCharImage; //abb: where does this belong??
			m_csAddCharLabel.Empty();
			UpdateData(FALSE);            // To controls.

			OnStartPixelEditor();
		}
		default:
		{
			break;
		}
	}

	//    DisplayPenSize(); //abb: I've taken this out because it appears to be redundant

	return CPropertyPage::OnSetActive();
}

void CSVPropertyPageCharsAndLabels::OnStartPixelEditor() 
{
	CSVPropertySheetFontTraining * pSheet = (CSVPropertySheetFontTraining *)GetParent();
	ASSERT_VALID(pSheet);

	//
	// Enable some additional buttons.
	//
	// A single character has been selected - show it and allow editing.
	//
	GetDlgItem(IDC_CHAR_OK)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHAR_CANCEL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_MODIFY)->EnableWindow(FALSE);

	//
	// Enable character edit controls
	//
	GetDlgItem(IDC_RADIO_PEN_WHITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_PEN_BLACK)->EnableWindow(TRUE);
	GetDlgItem(IDC_SPIN_PEN_SIZE)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHAR_EDIT_RESET)->EnableWindow(TRUE);

	m_csStaticDirections = "Edit Pixel Image of Character";
	UpdateData(FALSE);             // to controls

	m_enumPixelEditorState = ENUM_EDIT_RUNNING;

	/*
	CorObj objWitImTmp = {0};
	CorObj_type(&objWitImTmp) = COR_OBJ_IMAGE;
	*(CorObj_image(&objWitImTmp)) = witIm;
	cor_wrObj(m_pObjExtractedCharImage, _T("c:\\temp\\svIntoEditCreate.wit"), 0);
	*/
	/*
	CorObj objOCRFontModel = {0};
	CorObj_type(&objOCRFontModel) = pfn_getObjId("FontModel");
	CorObj_user(&objOCRFontModel) = pOCRFontModel;
	pfn_wrObj(&objOCRFontModel, _T("c:\\temp\\svFontModeltoOcrExecute.wit"), 0);
	*/

	//
	// Use the pixel editor to edit the extracted image of the character to
	// "train". (Don't block)
	//
	if (m_pObjEditedCharImage)
	{
		m_pFastOCR->DestroyLVObject(&m_pObjEditedCharImage);
	}

	m_pObjEditedCharImage = m_pFastOCR->CreateLVObject(COR_OBJ_IMAGE);

  HWND hWnd = GetDlgItem(IDC_PICTURE_CHAR)->GetSafeHwnd();
	m_hCorEdit = cor_editCreate(hWnd,
															COR_DISPLAY_VALUE, m_pObjExtractedCharImage,
															COR_EDIT_OUTPUT, m_pObjEditedCharImage,
															COR_EDIT_MAGNIFICATION, COR_EDIT_MAG_2,
															COR_DISPLAY_SUPPRESS_DIALOG, 1,
															COR_DISPLAY_SUPPRESS_MENU, 1, 
															COR_DISPLAY_SYNC, 0, // don't block
															COR_DISPLAY_DONE_PROC, corEditDoneProc,
															0);

/*
	m_hCorDisplay = cor_displayCreate(GetDlgItem(IDC_PICTURE_CHAR)->GetSafeHwnd(),
																		COR_DISPLAY_VALUE, m_pObjExtractedCharImage,
																		//COR_DISPLAY_SUPPRESS_DIALOG, 1,
																		COR_DISPLAY_SUPPRESS_MENU, 1, 
																		COR_DISPLAY_SYNC, 0, // don't block
																		COR_DISPLAY_DONE_PROC, corEditDoneProc,
																		0);
*/
	//
	// Set starting Pen size and color (pixel value) for edit operator
	//
	SendPenColorToEdit();
	DisplayPenSize();
}

void CSVPropertyPageCharsAndLabels::OnRadioPenWhite() 
{
	m_radioPenWhite = 0;          // WHITE
	UpdateData(FALSE);            // To Controls
	SendPenColorToEdit();
}

void CSVPropertyPageCharsAndLabels::OnRadioPenBlack() 
{
	m_radioPenWhite = 1;          // BLACK
	UpdateData(FALSE);            // To controls
	SendPenColorToEdit();	
}

void CSVPropertyPageCharsAndLabels::SendPenColorToEdit()
{
	//
	// Check to see if the WIT pixel editor is running.
	//
	if(m_enumPixelEditorState == ENUM_EDIT_RUNNING)
	{
		UpdateData(TRUE);         // Controls to data members

		// abb
		switch(m_radioPenWhite)
		{
			case 0:  
				CorObj_UByte(m_pObjBrushColor) = 255; // White, as long as only usigned 8 bit images are supported
				break;
			case 1:           
				CorObj_UByte(m_pObjBrushColor) = 0;   // Black
				break;
			default:
				break;
		}

		if (m_hCorEdit)
		{
			BOOL bResult = cor_editSet(m_hCorEdit,
																 COR_EDIT_BRUSH_COLOR, COR_OBJ_UBYTE, CorObj_UByte(m_pObjBrushColor),
																 0);
			ASSERT(bResult);
		}
	}
}

void CSVPropertyPageCharsAndLabels::OnDeltaposSpinPenSize(
   NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	//
	// The UP button returns a negative iDelta.
	// the DN button returns a positive iDelta.
	//
	m_nBrushSize -= pNMUpDown->iDelta;

	if(m_nBrushSize < 1)
	{
		m_nBrushSize = 1;
	}
	else
	{
		if(m_nBrushSize > 20)
		{
			m_nBrushSize = 20;
		}
	}

	DisplayPenSize();

	*pResult = 0;
}

void CSVPropertyPageCharsAndLabels::DisplayPenSize()
{
	CString s;
	s.Format("%d",m_nBrushSize);
	m_staticPenSize.SetWindowText(s);

	//
	// Check to see if the WIT pixel editor is running.
	//
	if(m_enumPixelEditorState == ENUM_EDIT_RUNNING)
	{
		//
		// set edit operator's pen brush size
		//
		if (m_hCorEdit)
		{
			BOOL bResult = cor_editSet(m_hCorEdit,
																 COR_EDIT_BRUSH_SIZE, m_nBrushSize,
																 0);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//
// WIT can reset the pixel image to pre edit condition.
//
void CSVPropertyPageCharsAndLabels::OnCharEditReset() 
{
	//
	// Check to see if the WIT pixel editor is running.
	//
	if(m_enumPixelEditorState == ENUM_EDIT_RUNNING)
	{
		int nReply = AfxMessageBox("Confirm Abandon of Character Edit Changes",MB_YESNO);
		if(nReply == IDYES)
		{
			if (m_hCorEdit)
			{
				BOOL bResult = cor_editSet(m_hCorEdit,
																	 COR_EDIT_RESET,
																	 0);
			}
		}
	}
}

