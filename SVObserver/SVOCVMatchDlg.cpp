//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVMatchDlg
//* .File Name       : $Workfile:   SVOCVMatchDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:03:22  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCVDialog.h"
#include "SVOCVMatchDlg.h"
#include "SVOCVAnalyzerResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOCVMatchDlg property page

IMPLEMENT_DYNCREATE(SVOCVMatchDlg, CPropertyPage)

SVOCVMatchDlg::SVOCVMatchDlg() : CPropertyPage(SVOCVMatchDlg::IDD)
{
	//{{AFX_DATA_INIT(SVOCVMatchDlg)
	m_MatchFilename = _T("");
	m_MatchString = _T("");
	m_useMatchFile = -1;
	m_nRadioOperation = -1;
	//}}AFX_DATA_INIT
}

SVOCVMatchDlg::~SVOCVMatchDlg()
{
}

HRESULT SVOCVMatchDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	UpdateData(TRUE);
	
	SVOCVDialogClass *pParent = (SVOCVDialogClass*) GetParent();
	SVOCVAnalyzeResultClass *pOCVAnalyzerResult = pParent->pOCVAnalyzerResult;
	
	if( pOCVAnalyzerResult )
	{
		l_hrOk = AddInputRequest( &( pOCVAnalyzerResult->m_fnvoMatchStringFileName ), m_MatchFilename );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( pOCVAnalyzerResult->m_svoMatchString ), m_MatchString );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( pOCVAnalyzerResult->m_bvoUseMatchFile ), !m_useMatchFile );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( pOCVAnalyzerResult->m_bvoPerformOCR ), m_nRadioOperation ? 0:1);
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( pOCVAnalyzerResult->GetTool() );
		}
	}

	return l_hrOk;
}

void SVOCVMatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCVMatchDlg)
	DDX_Control(pDX, IDC_FONT_BROWSE_CMD, m_buttonBrowse);
	DDX_Control(pDX, IDC_MATCHSTRING1, m_editMatchString);
	DDX_Control(pDX, IDC_MATCHFILENAME, m_editMatchFilename);
	DDX_Text(pDX, IDC_MATCHFILENAME, m_MatchFilename);
	DDX_Text(pDX, IDC_MATCHSTRING1, m_MatchString);
	DDX_Radio(pDX, IDC_USE_MATCH_FILE, m_useMatchFile);
	DDX_Radio(pDX, IDC_RADIO_OCR, m_nRadioOperation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCVMatchDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVOCVMatchDlg)
	ON_BN_CLICKED(IDC_FONT_BROWSE_CMD, OnFontBrowseCmd)
	ON_BN_CLICKED(IDC_USE_MATCH_STRING, OnUseMatchString)
	ON_BN_CLICKED(IDC_USE_MATCH_FILE, OnUseMatchFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOCVMatchDlg message handlers

BOOL SVOCVMatchDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	BOOL bUseMatchFile;
	BOOL bPerformOCR;
	
	SVOCVDialogClass *pParent = (SVOCVDialogClass *)GetParent();
	SVOCVAnalyzeResultClass *pOCVAnalyzerResult = pParent->pOCVAnalyzerResult;
	
	SetTaskObject( pOCVAnalyzerResult );

	pOCVAnalyzerResult->m_fnvoMatchStringFileName.GetValue( m_MatchFilename );
	pOCVAnalyzerResult->m_svoMatchString.GetValue( m_MatchString );
	pOCVAnalyzerResult->m_bvoUseMatchFile.GetValue( bUseMatchFile );
	pOCVAnalyzerResult->m_bvoPerformOCR.GetValue( bPerformOCR );
	m_nRadioOperation = bPerformOCR ? 0 : 1;

	switch( bUseMatchFile )
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
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCVMatchDlg::UpdateOCVParameters ()
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


BOOL SVOCVMatchDlg::OnKillActive() 
{
	UpdateOCVParameters ();

	return CPropertyPage::OnKillActive();
}

void SVOCVMatchDlg::OnFontBrowseCmd() 
{
	SVFileNameClass svfncFileName;

	svfncFileName.SetFileType( SV_OCV_MATCH_STRING_FILE_TYPE );

	//
	// Try to read the current image file path name from registry...
	//
	CString csPath = AfxGetApp()->GetProfileString(	_T("Settings"),
	                                                _T("OCVMatchStringFilePath"),
	                                                _T("C:\\RUN") );

	svfncFileName.SetDefaultPathName( csPath );

	UpdateData( TRUE );

	svfncFileName.SetFullFileName( m_MatchFilename );

	if ( svfncFileName.SelectFile() )
	{
		AfxGetApp()->WriteProfileString( _T( "Settings" ),
		                                 _T( "OCVMatchStringFilePath" ),
										 svfncFileName.GetPathName() );

		m_MatchFilename = svfncFileName.GetFullFileName();

		UpdateData( FALSE );
	}
}

void SVOCVMatchDlg::OnUseMatchString() 
{
	m_editMatchString.EnableWindow (TRUE);
	m_buttonBrowse.EnableWindow (FALSE);
	m_editMatchFilename.EnableWindow (FALSE);
}

void SVOCVMatchDlg::OnUseMatchFile() 
{
	m_editMatchString.EnableWindow (FALSE);
	m_buttonBrowse.EnableWindow (TRUE);
	m_editMatchFilename.EnableWindow (TRUE);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCVMatchDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:03:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Oct 2005 13:53:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed problem with getting data from bucket 1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Oct 2005 13:08:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed problem with the values from coming bucket 1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Aug 2005 09:40:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Aug 2005 07:53:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Sep 2003 13:07:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/