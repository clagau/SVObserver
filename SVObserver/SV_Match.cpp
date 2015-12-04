//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV_Match
//* .File Name       : $Workfile:   SV_Match.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SV_match.h"
#include "SVOCRDialog.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVOCRAnalyzerResult.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOCRMatchDlg property page

IMPLEMENT_DYNCREATE(SVOCRMatchDlg, CPropertyPage)

SVOCRMatchDlg::SVOCRMatchDlg() : CPropertyPage(SVOCRMatchDlg::IDD)
{
	nPixelsForSpace = -1;

	//{{AFX_DATA_INIT(SVOCRMatchDlg)
	m_nRadioSort = -1;
	m_csPixelsForSpace = _T("");
	m_nRadio1 = -1;
	//}}AFX_DATA_INIT
}

SVOCRMatchDlg::~SVOCRMatchDlg()
{
}

HRESULT SVOCRMatchDlg::SetInspectionData()
{
	CString strMatchStringFileName;

	HRESULT l_hrOk = S_FALSE;

	SVOCRDialogClass *pParent = (SVOCRDialogClass *)GetParent();
	SVOCRAnalyzeResultClass *pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();

	UpdateData(TRUE);

	m_matchString1Edit.GetWindowText( pOCRParameters->STRmatchString );
	m_matchFilenameEdit.GetWindowText( strMatchStringFileName );

	switch(m_nRadio1)
	{
		case 0:
		{
			pOCRParameters->useMatchFile = 1;
	    break;
		}
		case 1:
		{
		  pOCRParameters->useMatchFile = 0;
			break;
		}
		default:
		{
			break;
		}
	}

	l_hrOk = AddInputRequest( &( pOCRAnalyzerResult->msvfnvocMatchStringFileName ), strMatchStringFileName );

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &( pOCRAnalyzerResult->valueBlobSortDirection ), m_nRadioSort + 1 );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequest( &( pOCRAnalyzerResult->valuePixelsForASpace ), nPixelsForSpace );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequestMarker();
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = RunOnce( pOCRAnalyzerResult->GetTool() );
	}

	return l_hrOk;
}

void SVOCRMatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCRMatchDlg)
	DDX_Control(pDX, IDC_SCROLL_PIXELS_FOR_SPACE, m_scrollPixelsForSpace);
	DDX_Control(pDX, IDC_MATCHSTRING1, m_matchString1Edit);
	DDX_Control(pDX, IDC_MATCHFILENAME, m_matchFilenameEdit);
	DDX_Control(pDX, IDC_FONT_BROWSE_CMD, m_browseButton);
	DDX_Radio(pDX, IDC_RADIO_SORT1, m_nRadioSort);
	DDX_Text(pDX, IDC_PIXELS_FOR_SPACE, m_csPixelsForSpace);
	DDX_Radio(pDX, IDC_RADIO1, m_nRadio1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCRMatchDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVOCRMatchDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_FONT_BROWSE_CMD, OnMatchStringFileBrowseCmd)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOCRMatchDlg message handlers

BOOL SVOCRMatchDlg::OnInitDialog() 
{
	CString csTemp;

	CPropertyPage::OnInitDialog();

	SVOCRDialogClass *pParent = (SVOCRDialogClass *)GetParent();
	SVOCRAnalyzeResultClass *pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();
	
	SetTaskObject( pOCRAnalyzerResult );

	m_matchString1Edit.SetWindowText(pOCRParameters->STRmatchString);

	pOCRAnalyzerResult->GetMatchStringFileName( csTemp );
	m_matchFilenameEdit.SetWindowText( csTemp );
	
	switch( pOCRParameters->useMatchFile )
	{
		case 1:
		{
			m_nRadio1 = 0;
			OnRadio1();
			break;
		}
		case 0:
		{
			m_nRadio1 = 1;
			OnRadio2();
			break;
		}
		default:
		{
			break;
		}
	}
	
	//
	// Blob sorting for OCR
	//
	long lTemp;

	pOCRAnalyzerResult->valueBlobSortDirection.GetValue( lTemp );
	m_nRadioSort = (int)( lTemp - 1 );

	//
	// Pixel spacing between blobs to assume a space char.
	//
	pOCRAnalyzerResult->valuePixelsForASpace.GetValue( lTemp );
	nPixelsForSpace = (int)( lTemp );

	UpdatePixelsForSpace();

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCRMatchDlg::OnRadio1() 
{
	m_matchString1Edit.EnableWindow (FALSE);
	m_browseButton.EnableWindow (TRUE);
	m_matchFilenameEdit.EnableWindow (TRUE);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCRMatchDlg::OnRadio2() 
{
	m_matchString1Edit.EnableWindow (TRUE);
	m_browseButton.EnableWindow (FALSE);
	m_matchFilenameEdit.EnableWindow (FALSE);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCRMatchDlg::OnMatchStringFileBrowseCmd() 
{
	CString csTemp;
	SVFileNameClass svfncFileName;

	svfncFileName.SetFileType( SV_OCR_MATCH_STRING_FILE_TYPE );

	//
	// Try to read the current image file path name from registry...
	//
	CString csPath = AfxGetApp()->GetProfileString(	_T("Settings"),
	                                                _T("OCRMatchStringFilePath"),
	                                                _T("C:\\RUN") );

	svfncFileName.SetDefaultPathName( csPath );

	UpdateData( TRUE );

	m_matchFilenameEdit.GetWindowText( csTemp );
	svfncFileName.SetFullFileName( csTemp );
	if ( svfncFileName.SelectFile() )
	{
		AfxGetApp()->WriteProfileString( _T( "Settings" ),
		                                 _T( "OCRMatchStringFilePath" ),
																		 svfncFileName.GetPathName() );

		m_matchFilenameEdit.SetWindowText( svfncFileName.GetFullFileName() );

		UpdateData( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL SVOCRMatchDlg::UpdateOCRParameters ()
{
	BOOL l_bRet = TRUE;
	//
	// Make sure the dialog has been 'created' - 14 Apr 1999 - frb.
	//
	if(!GetSafeHwnd())
	{
		return TRUE;
	}

	l_bRet = SetInspectionData() == S_OK;

	return l_bRet;
}


BOOL SVOCRMatchDlg::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateOCRParameters ();
	return CPropertyPage::OnKillActive();
}

void SVOCRMatchDlg::UpdatePixelsForSpace()
{
	m_csPixelsForSpace.Format("%d",nPixelsForSpace);
}

void SVOCRMatchDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if(pScrollBar == &m_scrollPixelsForSpace)
	{
		UpdateData(TRUE);

		switch(nSBCode)
		{
			case SB_LINEUP:
				nPixelsForSpace++;
				break;
			case SB_LINEDOWN:
				nPixelsForSpace--;
				break;
			default:;
		}
		if(nPixelsForSpace < -1)
		{
			nPixelsForSpace = -1;
		}
		if(nPixelsForSpace > 1600)
		{
			nPixelsForSpace = 1600;
		}
		UpdatePixelsForSpace();
		UpdateData(FALSE);
	}
	
	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SV_Match.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:18:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   16 Aug 2005 08:45:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed un-necessary casting from AddInputRequest method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   08 Aug 2005 15:19:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated dialog to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   25 Jun 2003 15:00:38   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  349
   SCR Title:  Fix Bug I0001101 Enter text in minimum scale field
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed method UpdateOCRParameters to return a BOOL value
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   17 Apr 2003 16:14:42   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   19 Nov 2002 09:46:22   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added in index value for the SetValue
   
   /////////////////////////////////////////////////////////////////////////////////////
*/