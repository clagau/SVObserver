//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVPropertyPageSelectCharImages
//* .File Name       : $Workfile:   SVPropertyPageSelectCharImages.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 10:32:02  $
//******************************************************************************

#include "stdafx.h"
#include "SVLVFastOCR.h"
#include "SVOCRGrayFontTrain.h"
#include "SVPropertySheetFontTraining.h"
#include "SVPropertyPageSelectCharImages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// abb
// callback functions for WiT interactive functions
//
// These functions are called when the corresponding interactive
// function is created or destroyed.
//

extern int *gplvLastOpStatus;

static CorDisplay corDisplayHandle;
static int corDisplayDoneProc(CorDisplay handle, Boolean done)
{
  if (done)
    corDisplayHandle = NULL;
  else
    corDisplayHandle = handle;

	return 0;
}

static CorGetData corGetDataHandle;
static int corGetDataDoneProc(CorGetData handle, Boolean done)
{
  if (done)
    corGetDataHandle = NULL;
  else
    corGetDataHandle = handle;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CSVPropertyPageSelectCharImages property page

IMPLEMENT_DYNCREATE(CSVPropertyPageSelectCharImages, CPropertyPage)

CSVPropertyPageSelectCharImages::CSVPropertyPageSelectCharImages() : CPropertyPage(CSVPropertyPageSelectCharImages::IDD)
{
	//{{AFX_DATA_INIT(CSVPropertyPageSelectCharImages)
	//}}AFX_DATA_INIT
	m_enumROIGetDataResult = ENUM_ROI_CANCEL;

	m_fScaleFactor = 1.00;
	m_bFirstRoiSelected = TRUE;

	m_pFastOCR = NULL;

	m_hCorDisplay = NULL;
	m_hCorGetData = NULL;

	m_pObjRoi = NULL;
	m_pObjStartImage = NULL;
	m_pStartImage = NULL;

	mbSizeSet = FALSE;
}

CSVPropertyPageSelectCharImages::~CSVPropertyPageSelectCharImages()
{
	if (m_hCorDisplay)
	{
		if (m_hCorDisplay->dummy > 0)
		{
			cor_displayDestroy(m_hCorDisplay);
		}
	}

	if (m_hCorGetData)
	{
		if (m_hCorGetData->dummy > 0)
		{
			cor_getDataDestroy(m_hCorGetData);
		}
	}

	if (m_pObjRoi)
	{
		m_pFastOCR->DestroyLVObject(&m_pObjRoi);
	}

	if (m_pObjStartImage)
	{
		m_pFastOCR->DestroyLVObject(&m_pObjStartImage);
	}
}

void CSVPropertyPageSelectCharImages::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVPropertyPageSelectCharImages)
	DDX_Control(pDX, IDC_SCALE_FACTOR, m_staticScaleFactor);
	DDX_Control(pDX, IDC_SPIN1, m_spinCtrlScaleFactor);
	DDX_Text(pDX, IDC_STATIC_DIRECTIONS, m_csStaticDirections);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSVPropertyPageSelectCharImages, CPropertyPage)
	//{{AFX_MSG_MAP(CSVPropertyPageSelectCharImages)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonStartNewCharacter)
	ON_BN_CLICKED(IDC_ROI_OK, OnRoiOk)
	ON_BN_CLICKED(IDC_ROI_CANCEL, OnRoiCancel)
	ON_BN_CLICKED(IDC_ROI_RESET, OnRoiReset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVPropertyPageSelectCharImages message handlers

void CSVPropertyPageSelectCharImages::InitializeWitObjects()
{
	BOOL bOk = TRUE;

	CSVPropertySheetFontTraining * pSheet = (CSVPropertySheetFontTraining *)GetParent();

	m_pFastOCR = pSheet->m_pFastOCR;

	//
	// Read the start image from file
	//
	if (m_pObjStartImage)
	{
		bOk = m_pFastOCR->DestroyLVObject(&m_pObjStartImage);
		m_pStartImage = NULL;
	}

	if (bOk)
	{
		m_pObjStartImage = m_pFastOCR->CreateLVObject(COR_OBJ_IMAGE);

		if (m_pObjStartImage)
		{
			bOk = m_pFastOCR->ReadLVImageFromFile(m_pObjStartImage,(char *) (LPCTSTR) pSheet->m_csPathFileInputImage);
			m_pStartImage = CorObj_image(m_pObjStartImage);
		}
	}
}

void CSVPropertyPageSelectCharImages::OnDestroy() 
{
	CPropertyPage::OnDestroy();
}

BOOL CSVPropertyPageSelectCharImages::OnSetActive() 
{
	SetDisplay();
	SetButtons();

	return CPropertyPage::OnSetActive();
}

BOOL CSVPropertyPageSelectCharImages::OnKillActive() 
{
	//
	// Make sure that Wit 'getData' operator is not running.
	//
	if(m_enumROIGetDataResult == ENUM_ROI_ACTIVE)
	{
		OnRoiCancel();
	}

	//
	// Enable buttons in parent sheet
	//
	GetParent()->GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetParent()->GetDlgItem(IDCANCEL)->EnableWindow(TRUE);

	if (m_hCorGetData)
	{
		cor_getDataDestroy(m_hCorGetData);
		m_hCorGetData = NULL;
	}

	if (m_hCorDisplay)
	{
		cor_displayDestroy(m_hCorDisplay);
		m_hCorDisplay = NULL;
	}

	return CPropertyPage::OnKillActive();
}

void CSVPropertyPageSelectCharImages::CleanUpWitGetData() //11 Aug 1999 - frb.
{
	if(m_enumROIGetDataResult == ENUM_ROI_ACTIVE)
	{
		OnRoiCancel();
	}
}

BOOL CSVPropertyPageSelectCharImages::OnInitDialog() 
{
	BOOL bOk = CPropertyPage::OnInitDialog();

	//
	// Initialize wit objects and map dialog control windows to wit display
	// objects.
	//
	InitializeWitObjects();

	//
	// Set up Scale Factor static display.
	//
	DisplayScaleFactor();

	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	return bOk;
}

void CSVPropertyPageSelectCharImages::DisplayScaleFactor()
{
	CString s;
	s.Format("%.2f",m_fScaleFactor);
	m_staticScaleFactor.SetWindowText(s);
}

void CSVPropertyPageSelectCharImages::SetDisplay()
{
	if (m_hCorGetData)
	{
		cor_getDataDestroy(m_hCorGetData);
		m_hCorGetData = NULL;
	}

	if (m_hCorDisplay)
	{
		cor_displayDestroy(m_hCorDisplay);
		m_hCorDisplay = NULL;
	}

	//
	// display the image that contains the characters to be trained.
	//
	m_hCorDisplay = cor_displayCreate(GetDlgItem(IDC_PICTURE_MAIN)->GetSafeHwnd(),
														        COR_DISPLAY_VALUE, m_pObjStartImage,
																	  COR_DISPLAY_SCALE, m_fScaleFactor, m_fScaleFactor,
																	  COR_DISPLAY_SUPPRESS_MENU, 1,
		 															  //COR_DISPLAY_SUPPRESS_DIALOG, 1,
																	  COR_DISPLAY_SUPPRESS_CROSSHAIRS, 1,
																	  COR_DISPLAY_DONE_PROC, corDisplayDoneProc,
														        0);

	UpdateData(FALSE);
}

void CSVPropertyPageSelectCharImages::SetGetData()
{
	if (m_hCorDisplay)
	{
		cor_displayDestroy(m_hCorDisplay);
		m_hCorDisplay = NULL;
	}

	if (m_hCorGetData)
	{
		cor_getDataDestroy(m_hCorGetData);
		m_hCorGetData = NULL;
	}

	m_hCorGetData = cor_getDataCreate(GetDlgItem(IDC_PICTURE_MAIN)->GetSafeHwnd(),
																	  COR_DISPLAY_VALUE, m_pObjStartImage,
																	  COR_GETDATA_OUTPUT, m_pObjRoi,
																	  COR_DISPLAY_SCALE, m_fScaleFactor, m_fScaleFactor,
																	  COR_DISPLAY_SUPPRESS_MENU, 1,
																	  COR_DISPLAY_SUPPRESS_DIALOG, 1,
																	  COR_DISPLAY_DONE_PROC, corGetDataDoneProc,
																	  COR_GETDATA_TYPE, COR_GETDATA_TYPE_RECTANGLE,
																	  COR_DISPLAY_SYNC, 1, // block until done
																	  0);

	// since the above getData is created in blocking mode, the function
	// returns only when the getData has been destroyed

	//
	// Execution continues here when CorGetDataCreate() returns after
	// user action on one of the three buttons.
	//

	// 
	// abb: general comment. I don't want to change the gui logic
	// of the existing code unless really necessary, so I'm leaving
	// this pretty much alone. Note however that it is unnecessary and
	// inefficent to restart over and over. The above CorDisplayDestroy
	// and CorGetDataCreate should be done once outside the loop. Inside
	// the loop, (in this switch, it is only necessary to call CorGetDataSet
	// to reset or cancel the roi, whichever is appropriate.
	//
}

/////////////////////////////////////////////////////////////////////////////
//
//
// This method is called when the training cycle for a new character is 
// to be started.
//
void CSVPropertyPageSelectCharImages::OnButtonStartNewCharacter() 
{
	while(TRUE)
	{
		BOOL bResult = TRUE;

		CSVPropertySheetFontTraining * pSheet = (CSVPropertySheetFontTraining *)GetParent();

		m_pFastOCR = pSheet->m_pFastOCR;
		//
		// Disable the buttons that we will not use for now
		//
		GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_ROI_OK)->EnableWindow(TRUE);
		GetDlgItem(IDC_ROI_CANCEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_ROI_RESET)->EnableWindow(TRUE);
		m_spinCtrlScaleFactor.EnableWindow(FALSE);

		//
		// Disable buttons in parent sheet
		//
		GetParent()->GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetParent()->GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

		m_csStaticDirections = "Use cursor to select char ROI in Image";
		UpdateData(FALSE);           // to controls

		// must kill the CorDisplay before creating the CorGetData

		if (bResult)
		{
			//
			// Get ROI from user
			//
			// This CorGetDataCreate call does not return until user presses the
			// 'ROIOk', 'ROICancel' or 'ROIReset' button
			if (m_pObjRoi)
			{
				bResult = m_pFastOCR->DestroyLVObject(&m_pObjRoi);
			}

			if (bResult)
			{
				m_pObjRoi = m_pFastOCR->CreateLVObject(COR_OBJ_GENERIC);
				bResult = m_pObjRoi != NULL;
			}
		}

		// Set an indicator that getData is active...
		m_enumROIGetDataResult = ENUM_ROI_ACTIVE;  // 10 Aug 1999 - frb.

		if (bResult)
		{
			SetGetData();
		}

		switch(m_enumROIGetDataResult)
		{
			case ENUM_ROI_OK:
			{
				break;               // Continue processing.
			}
			case ENUM_ROI_ERROR:
			case ENUM_ROI_CANCEL:
			{
				//
				// Reset and require user to press 'start' button.
				//
				SetDisplay();
				SetButtons();
				return;
			}
			default:
			case ENUM_ROI_RESET:
			{
				continue;            // Start again.
			}
		}

		if (bResult)
		{
			if (pSheet->m_pExtractedCharImage)
			{
				bResult = m_pFastOCR->DestroyLVObject(&(pSheet->m_pExtractedCharImage));
			}

			if (bResult)
			{
				pSheet->m_pExtractedCharImage = m_pFastOCR->CreateLVObject(COR_OBJ_IMAGE);
				bResult = pSheet->m_pExtractedCharImage != NULL;
			}
		}

		if (bResult)
		{
			//
			// Extract sub-image corresponding to roi from the original image.
			// abb: details -- since getData produces a vector of ROI's which
			// gets passed here to extract, the extract operator produces a 
			// vector of images. Therefore we need to call getElem to get the
			// first element of that vector: ie the first (and only) image
			// extracted from the original start image.
			//

      CorObj obj1 = {0}, obj2 = {0};
			bResult = CorObj_type(m_pObjRoi) == COR_OBJ_VECTOR;
			if ( bResult ) 
			{
				CorOpRtn rc = cor_getElem(m_pObjRoi, &obj1, &obj2, 0, 0);

			  bResult = CorObjRelease1(m_pObjRoi, 0);
				memset(m_pObjRoi, 0, sizeof(CorObj));

				bResult = CorObjRelease1(&obj2, 0);
				memset(&obj2, 0, sizeof(CorObj));

        *m_pObjRoi = obj1;
				memset(&obj1, 0, sizeof(CorObj));

				bResult &= rc == COR_OP_OK;
			}

			if ( bResult )
			{
				// hack to keep all rectangles the same size (the same size as the first character in font)
				CorGeom *pGeom;
				CorFpoint *pFpoint;
				float top, left, w, h;
				pGeom = (CorGeom *)CorObj_user(m_pObjRoi);
				pFpoint = (CorFpoint *)CorObj_mdData(&pGeom->plist);
				left = pFpoint[0].x; 
				top = pFpoint[0].y;
				w = pFpoint[1].x;
				h = pFpoint[1].y;

				if (!mbSizeSet) { 
					mfHeight = h - top + 1;
					mfWidth =		w - left + 1;
					mbSizeSet = TRUE;
				}	
				pFpoint[1].x = left + mfWidth - 1;
				pFpoint[1].y = top + mfHeight - 1;

				bResult = m_pFastOCR->ExtractLVImage(m_pObjStartImage, 
																					 m_pObjRoi, 
																					 pSheet->m_pExtractedCharImage);
			}
		}

		if (bResult)
		{
			//
			// Disable some additional buttons.
			//
			GetDlgItem(IDC_ROI_OK)->EnableWindow(FALSE);
			GetDlgItem(IDC_ROI_CANCEL)->EnableWindow(FALSE);
			GetDlgItem(IDC_ROI_RESET)->EnableWindow(FALSE);

			//
			// Set the program 'state'
			//
			pSheet->m_enumProgramState = PROGRAM_STATE_CHAR_IMAGE_SELECTED;

			//
			// Switch to the second page of the property sheet.
			//
			bResult = pSheet->SetActivePage(&pSheet->m_Page2);

			break;          // exit while loop.
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// The character image Region of Interest (ROI) has been selected by the 
// user.  This method causes to 'getData' operator to return.
//
void CSVPropertyPageSelectCharImages::OnRoiOk() 
{
	m_enumROIGetDataResult = ENUM_ROI_OK;

	ASSERT(corGetDataHandle);
	int bResult = cor_getDataSet(corGetDataHandle, 
													     COR_GETDATA_APPLY, 1,
										           0);
}

////////////////////////////////////////////////////////////////////////////
//
//
// The character image ROI has been canceled by the user.
//
void CSVPropertyPageSelectCharImages::OnRoiCancel() 
{
	m_enumROIGetDataResult = ENUM_ROI_CANCEL;

	ASSERT(corGetDataHandle);
	int bResult = cor_getDataSet(corGetDataHandle, 
													     COR_GETDATA_APPLY, 0,
										           0);
}

/////////////////////////////////////////////////////////////////////////////
//
// abb: Since the existing logic in "OnButtonStartNewCharacter" assumes
// that cor_getDataCreate() returns after this handler is called, I am
// sending the cancel message to it so that it will do just that. 
// This could simply send the reset msg instead, which does not 
// destroy the getData operator.
//
void CSVPropertyPageSelectCharImages::OnRoiReset() 
{
	m_enumROIGetDataResult = ENUM_ROI_RESET;

	ASSERT(corGetDataHandle);
	int bResult = cor_getDataSet(corGetDataHandle, 
													     COR_GETDATA_APPLY, 0,
										           0);
}

void CSVPropertyPageSelectCharImages::OnDeltaposSpin1(
    NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	m_fScaleFactor -= (double)((double)pNMUpDown->iDelta * 0.05 );
	if(m_fScaleFactor < 0.50)
	{
	m_fScaleFactor = 0.50;
	}
	else
	if(m_fScaleFactor > 10.0)
	{
	m_fScaleFactor = 10.0;
	}

	DisplayScaleFactor();

	//
	// Display image with new scaling.
	//
	SetDisplay();
	SetButtons();

	*pResult = 0;
}

void CSVPropertyPageSelectCharImages::OnTimer(UINT nIDEvent) 
{
	ASSERT(nIDEvent == __SKIP_START_BUTTON);

	KillTimer(nIDEvent);

	PostMessage(WM_COMMAND,IDC_BUTTON_ADD);

	CPropertyPage::OnTimer(nIDEvent);
}

void CSVPropertyPageSelectCharImages::SetButtons()
{
	//
	// Disable the buttons that we will not use for now
	//
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(TRUE);
	GetDlgItem(IDC_ROI_OK)->EnableWindow(FALSE);
	GetDlgItem(IDC_ROI_CANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_ROI_RESET)->EnableWindow(FALSE);
	m_spinCtrlScaleFactor.EnableWindow(TRUE);

	//
	// Disable buttons in parent sheet
	//
	GetParent()->GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetParent()->GetDlgItem(IDCANCEL)->EnableWindow(TRUE);

	///((CSVPropertySheetFontTraining *)GetParent())->m_Page2.EnableWindow(TRUE);

	//
	// Set the directions control text.
	//
	m_csStaticDirections = "Press 'Select Char Image to Train'";
	UpdateData(FALSE);           // to controls
}

