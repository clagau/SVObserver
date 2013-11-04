//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayMatchDlg
//* .File Name       : $Workfile:   SVOCRGrayMatchDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:59:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRGrayDialog.h"
#include "SVOCRGrayMatchDlg.h"
#include "SVOCRGrayAnalyzerResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOCRGrayMatchDlg property page

IMPLEMENT_DYNCREATE(SVOCRGrayMatchDlg, CPropertyPage)

SVOCRGrayMatchDlg::SVOCRGrayMatchDlg() : CPropertyPage(SVOCRGrayMatchDlg::IDD)
{
	//{{AFX_DATA_INIT(SVOCRGrayMatchDlg)
	m_MatchFilename = _T("");
	m_MatchString = _T("");
	m_useMatchFile = -1;
	m_nPixelsForSpace = 0;
	m_nRadioSort = -1;
	//}}AFX_DATA_INIT
}

SVOCRGrayMatchDlg::~SVOCRGrayMatchDlg()
{
}

HRESULT SVOCRGrayMatchDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	UpdateData(TRUE);

	SVOCRGrayDialogClass * pParent = (SVOCRGrayDialogClass *)GetParent();
	SVOCRGrayAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRGrayParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();
	
	pOCRParameters->STRmatchString = m_MatchString;
	switch (m_useMatchFile)
	{
		case 0:
			pOCRParameters->useMatchFile = 1;
			break;
		case 1:
			pOCRParameters->useMatchFile = 0;
			break;
		default:;
	}
	
	if( pOCRAnalyzerResult )
	{
		l_hrOk = AddInputRequest( &( pOCRAnalyzerResult->msvfnvocGrayMatchStringFileName ), m_MatchFilename );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( pOCRAnalyzerResult->valueBlobSortDirection ), m_nRadioSort + 1 );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( pOCRAnalyzerResult->valuePixelsForASpace ), m_nPixelsForSpace );
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

void SVOCRGrayMatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCRGrayMatchDlg)
	DDX_Control(pDX, IDC_PIXELS_FOR_SPACE, m_wndPixelsForSpace);
	DDX_Control(pDX, IDC_SPIN1, m_spinPixelsForSpace);
	DDX_Control(pDX, IDC_FONT_BROWSE_CMD, m_buttonBrowse);
	DDX_Control(pDX, IDC_MATCHSTRING1, m_editMatchString);
	DDX_Control(pDX, IDC_MATCHFILENAME, m_editMatchFilename);
	DDX_Text(pDX, IDC_MATCHFILENAME, m_MatchFilename);
	DDX_Text(pDX, IDC_MATCHSTRING1, m_MatchString);
	DDX_Radio(pDX, IDC_USE_MATCH_FILE, m_useMatchFile);
	DDX_Text(pDX, IDC_PIXELS_FOR_SPACE, m_nPixelsForSpace);
	DDX_Radio(pDX, IDC_RADIO_SORT1, m_nRadioSort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCRGrayMatchDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVOCRGrayMatchDlg)
	ON_BN_CLICKED(IDC_FONT_BROWSE_CMD, OnFontBrowseCmd)
	ON_BN_CLICKED(IDC_USE_MATCH_STRING, OnUseMatchString)
	ON_BN_CLICKED(IDC_USE_MATCH_FILE, OnUseMatchFile)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOCRGrayMatchDlg message handlers

BOOL SVOCRGrayMatchDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SVOCRGrayDialogClass * pParent = (SVOCRGrayDialogClass *)GetParent();
	SVOCRGrayAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRGrayParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();
	
	SetTaskObject( pOCRAnalyzerResult );

	pOCRAnalyzerResult->GetMatchStringFileName( m_MatchFilename );

	m_MatchString   = pOCRParameters->STRmatchString;
	switch (pOCRParameters->useMatchFile)
	{
		case 1:
		{
			m_useMatchFile = 0;
			OnUseMatchFile();
			break;
		}
		case 0:
		{
			m_useMatchFile = 1;
			OnUseMatchString();
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
	pOCRAnalyzerResult->valueBlobSortDirection.GetValue(lTemp);
	m_nRadioSort = (int)(lTemp - 1);
	
	//
	// Pixel spacing between blobs to assume a space char.
	//
	pOCRAnalyzerResult->valuePixelsForASpace.GetValue(lTemp);
	m_nPixelsForSpace = (int)lTemp;
	m_spinPixelsForSpace.SetBase(10);
	m_spinPixelsForSpace.SetRange(-1, 1600);
	m_spinPixelsForSpace.SetPos(m_nPixelsForSpace);
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCRGrayMatchDlg::UpdateOCRParameters ()
{
	//
	// Make sure the dialog has been 'created' - 14 Apr 1999 - frb.
	//
	if (!GetSafeHwnd())
	{
		return;
	}

	SetInspectionData();

	return;
}


BOOL SVOCRGrayMatchDlg::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateOCRParameters ();
	return CPropertyPage::OnKillActive();
}
// eof: SVOCRGrayMatchDlg.cpp

void SVOCRGrayMatchDlg::OnFontBrowseCmd() 
{
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

	svfncFileName.SetFullFileName( m_MatchFilename );

	if ( svfncFileName.SelectFile() )
	{
		AfxGetApp()->WriteProfileString( _T( "Settings" ),
		                                 _T( "OCRMatchStringFilePath" ),
										 svfncFileName.GetPathName() );

		m_MatchFilename = svfncFileName.GetFullFileName();

		UpdateData( FALSE );
	}
}

void SVOCRGrayMatchDlg::OnUseMatchString() 
{
	m_editMatchString.EnableWindow (TRUE);
	m_buttonBrowse.EnableWindow (FALSE);
	m_editMatchFilename.EnableWindow (FALSE);
}

void SVOCRGrayMatchDlg::OnUseMatchFile() 
{
	m_editMatchString.EnableWindow (FALSE);
	m_buttonBrowse.EnableWindow (TRUE);
	m_editMatchFilename.EnableWindow (TRUE);
}

void SVOCRGrayMatchDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString strTemp;
	
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	m_nPixelsForSpace = pNMUpDown->iPos;
	strTemp.Format(_T("%d"), m_nPixelsForSpace);
	m_wndPixelsForSpace.SetWindowText(strTemp);
	
	*pResult = 0;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRGrayMatchDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:59:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Aug 2005 09:40:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Aug 2005 07:35:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 12:53:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Nov 2002 10:26:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed ValueObject semantics to work with buckets
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Jun 2001 08:56:58   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: OnInitDialog, UpdateOCRParameters, OnFontBrowseCmd.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 Feb 2001 13:31:44   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1.1.0   16 May 2001 14:10:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  202
 * SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 May 2000 10:12:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  146
 * SCR Title:  Fix OCR Gray Scale Analyzer Configuration Dialog
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This change was in response to SCR146.  These updates fix most of the problems with the Gray Scale OCR Dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:44:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/