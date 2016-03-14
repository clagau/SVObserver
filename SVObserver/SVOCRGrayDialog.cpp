//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayDialog
//* .File Name       : $Workfile:   SVOCRGrayDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:04:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRGrayDialog.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVMFCControls\CSVOCRGrayFontTrain.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVUnaryImageOperatorList.h"
#include "SVOCRGrayAnalyzerResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVOCRGrayDialogClass::SVOCRGrayDialogClass(CWnd* pParent /*=NULL*/)
	: CPropertySheet(IDS_OCRGRAY_PARAMETERS, pParent)
{

    pOCRAnalyzerResult     = NULL;
	pTool				= NULL;
	pDocument			= NULL;

	lBlobSortDirectionSaved = 0L;
	lPixelsForASpaceSaved = 0L;

	AddPageDialogs ();
	//{{AFX_DATA_INIT(SVOCRGrayDialogClass)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVOCRGrayDialogClass::~SVOCRGrayDialogClass()
{
	RemovePage (&m_GeneralParamsDlg);
	RemovePage (&m_MatchStringParamsDlg);
	RemovePage (&m_CandidateParamsDlg);
	RemovePage (&m_MatchThresholdParamsDlg);
}

HRESULT SVOCRGrayDialogClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	if( pOCRAnalyzerResult )
	{
		l_hrOk = AddInputRequest( &( pOCRAnalyzerResult->valueBlobSortDirection ), lBlobSortDirectionSaved );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( pOCRAnalyzerResult->valuePixelsForASpace ), lPixelsForASpaceSaved );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( pOCRAnalyzerResult->GetTool() );
		}
	}

	return l_hrOk;
}

void SVOCRGrayDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertySheet::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCRGrayDialogClass)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCRGrayDialogClass, CPropertySheet)
	//{{AFX_MSG_MAP(SVOCRGrayDialogClass)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
    ON_COMMAND( ID_FONT_TRAINING, OnFontTraining )
	ON_COMMAND( IDOK, OnOK)
	ON_COMMAND( IDCANCEL, OnCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL SVOCRGrayDialogClass::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	SetTaskObject( pOCRAnalyzerResult );

	ASSERT(pOCRAnalyzerResult);
	SVOCRGrayParamStruct* pOriginalOCRParams = &pOCRAnalyzerResult->GetOCRParameters();

	if( pOriginalOCRParams && pTool	&& pDocument && pOCRAnalyzerResult )
	{
		//
		// Save the parameters for possible CANCEL
		//
		OCRParametersSaved = *pOriginalOCRParams;      // operator=()
		pOCRAnalyzerResult->valueBlobSortDirection.GetValue(lBlobSortDirectionSaved);
		pOCRAnalyzerResult->valuePixelsForASpace.GetValue(lPixelsForASpaceSaved);
	}

	UpdateData( FALSE );
	
	AddButtons();       // Edit Font and Test buttons.

	return bResult;
}

void SVOCRGrayDialogClass::AddPageDialogs ()
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	AddPage (&m_GeneralParamsDlg);
	AddPage (&m_MatchStringParamsDlg);
	AddPage (&m_CandidateParamsDlg);
	AddPage (&m_MatchThresholdParamsDlg);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCRGrayDialogClass::OnOK()
{
	m_GeneralParamsDlg.UpdateOCRParameters ();
	m_MatchStringParamsDlg.UpdateOCRParameters ();
	m_CandidateParamsDlg.UpdateOCRParameters ();
	m_MatchThresholdParamsDlg.UpdateOCRParameters ();

    pDocument->SetModifiedFlag(TRUE);       // Set document 'changed'.

	CPropertySheet::EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCRGrayDialogClass::OnCancel()
{
	//
	// Restore the original parameters.
	//
	ASSERT(pOCRAnalyzerResult);
	SVOCRGrayParamStruct* pOriginalOCRParams = &pOCRAnalyzerResult->GetOCRParameters();
	*pOriginalOCRParams = OCRParametersSaved;      // operator=()

	SetInspectionData();

	CPropertySheet::EndDialog(IDCANCEL);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
void SVOCRGrayDialogClass::OnFontTraining()
{
	SVFileNameClass	svfncFontFile;
    CString csPathImageFile;
    BOOL bResult;

    //AfxMessageBox("OnFontTraining");

    //
    // Check to make sure we have a satisfactory file and path for the font
    // parmeters as result of font training operation.
    //
    CString csPathFontFile;
    
    //
    // Get the file path to use from the property page.
    //
    m_GeneralParamsDlg.UpdateData(TRUE);
	m_GeneralParamsDlg.UpdateOCRParameters();
    csPathFontFile = m_GeneralParamsDlg.m_fontFilename;
	svfncFontFile.SetFullFileName(csPathFontFile);

#ifdef _DEBUG999
{
    CString s;
		s.Format("Font File Path: %s", csPathFontFile);
    AfxMessageBox(s);
}
#endif //_DEBUG

	if (csPathFontFile.IsEmpty())
    {
        AfxMessageBox("ERROR: No Font file name specified");
        return;
    }

    //
    // Other output font file path checking...TBD
    //
    //
	CString csExtention = svfncFontFile.GetExtension();
	if (csExtention.CompareNoCase(_T(".wit")) != 0)
    {
		svfncFontFile.SetExtension(_T(".wit"));
    }

	CString csPath = AfxGetApp()->GetProfileString(	_T("Settings"), 
													_T( "WitFontFilePath" ), 
													_T("C:\\RUN"));               // Default
	
	svfncFontFile.SetPathName(csPath);
	
    //
    // Make sure the font/labels file has a path specified.
    // The following is an ugly hack for the time being.  Make sure Wit
    // does not put the font/labels file in C:\Wit\Images as default and
    // that the file does does not get into root of C:
    //

	m_GeneralParamsDlg.m_fontFilename = svfncFontFile.GetFullFileName();
	m_GeneralParamsDlg.UpdateData(FALSE);
	m_GeneralParamsDlg.UpdateOCRParameters();

    //
    // Get the MIL image, convert to WIT image and the write to witImage type file
    //
    if(!pTool)
    {
       AfxMessageBox("ERROR: Font Training Setup: No TOOL specified");
       return;
    }

	SVObjectTypeInfoStruct info;
	info.ObjectType = SVUnaryImageOperatorListObjectType;
	SVObjectClass* pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage( 
		pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info) ));
	if( ! SV_IS_KIND_OF( pObject, SVUnaryImageOperatorListClass ) )
    {
       AfxMessageBox( _T( "ERROR: Font Training Setup: No OperatorList specified" ));
       return;
    }

	info.ObjectType = SVImageObjectType;
	pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage( 
		pObject, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info) ));
	SVImageClass* pImage = dynamic_cast<SVImageClass*>(pObject);
	if( nullptr == pImage )
    {
       AfxMessageBox( _T( "ERROR: Font Training Setup: No Image specified" ));
       return;
    }

  SVSmartHandlePointer ImageHandle;

  if ( pImage->GetImageHandle( ImageHandle ) && !( ImageHandle.empty() ) )
  {
		SVImageBufferHandleImage l_MilHandle;
		ImageHandle->GetData( l_MilHandle );

		
		SVMatroxBufferInterface::SVStatusCode l_Code;

		SVMatroxBuffer milBuffer = l_MilHandle.GetBuffer();
		
		//
		// Write the MIL image to a temporary file in BMP form.
		//
		csPathImageFile = _T("c:\\~witImageTemporary.bmp");

		SVMatroxString l_strTmp = csPathImageFile;
		l_Code = SVMatroxBufferInterface::Export( milBuffer, l_strTmp, SVFileBitmap );
		//MbufExport((char*)(LPCTSTR)csPathImageFile, M_BMP, milBuffer);

#ifdef _DEBUG999
    AfxMessageBox("MIL to BMP completed");
#endif //_DEBUG

    //
    // Create the Font Training OCX 'connection' via IDispatch OLE interface.
    //
    SvMc::CSVOCRGrayFontTrain ocxFontTrain;

    CRect r(0,0,1,1);
	bResult = ocxFontTrain.Create(
		_T("GrayScaleFontTrainingWnd"), //LPCTSTR lpszWindowName,
		WS_POPUP,						//DWORD dwStyle,
		r,								//const RECT& rect, 
		this,							//CWnd* pParentWnd, 
		2 );

    if(!bResult)
    {
       AfxMessageBox("Font Training Failed to Start");
       return;
    }

    //
    // Run the Font Training OCX.
    //
    csPathFontFile = svfncFontFile.GetFullFileName();
	long lResult = ocxFontTrain.DoSVOCRGrayFontTrain(
       (LPCTSTR)csPathImageFile,   // Input file with image containing sample characters.
       (LPCTSTR)csPathFontFile );  // Output from font training
    ASSERT(lResult == 0L);         // ?? default

    //
    // Clean up...
    //
    bResult = ocxFontTrain.DestroyWindow();
    ASSERT(bResult);

    //
    // Delete the temporary file use for the image.
    //
	CFile::Remove((const char *)csPathImageFile);
    //AfxMessageBox("OCX Returned");
  }
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void SVOCRGrayDialogClass::AddButtons()
{
	CString buttonTitle;
	CRect	okRect,cancelRect;
	CRect   fontRect;
	CRect	testRect;
	CRect	clientRect;

	int buttonDist;
	CPoint buttonPos(6,7);
    CSize buttonSize(82,23);

	GetClientRect (&clientRect);
	HWND hWnd = ::GetDlgItem(m_hWnd, IDOK);
	if (hWnd != NULL)
	{
		::GetWindowRect(hWnd, &okRect);
	
		hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
		if (hWnd != NULL)
		{
			::GetWindowRect(hWnd, &cancelRect);

			// Find distance between buttons
			buttonDist = cancelRect.left - okRect.right;
		}
		ScreenToClient(&okRect);

		buttonSize.cx = okRect.Width();
		buttonSize.cy = okRect.Height();

		//
		// Calculate the left position to start placing our two
		// buttons.
		//		
		buttonPos.x = okRect.left - (buttonDist + buttonSize.cx);
		buttonPos.x -= (buttonDist + buttonSize.cx + buttonSize.cx/2);
		buttonPos.y = okRect.top;
	}

    //
	// Create a font based on 'points' i.e. 1/72 of an inch per point.
	//
	m_font.CreatePointFont (80, "MS Sans Serif");

	//
	// Edit Font button. (Make it 1 1/2 the length of Cancel button)
	//
	fontRect.top    = buttonPos.y;
	fontRect.bottom = fontRect.top + buttonSize.cy;
	fontRect.left   = buttonPos.x;
	fontRect.right  = fontRect.left + buttonSize.cx + (buttonSize.cx/2);

	buttonTitle.LoadString (IDS_FONTTRAINING_BUTTON);
	m_editFontButton.Create (buttonTitle, 
							 BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 
							 fontRect, 
							 this, 
							 ID_FONT_TRAINING  //IDS_FONTTRAINING_BUTTON 08 Apr 1999 - frb.
                             );
	m_editFontButton.SetFont (&m_font);

}

