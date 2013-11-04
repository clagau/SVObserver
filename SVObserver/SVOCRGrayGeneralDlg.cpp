//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayGeneralDlg
//* .File Name       : $Workfile:   SVOCRGrayGeneralDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:56:46  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRGrayDialog.h"
#include "SVOCRGrayGeneralDlg.h"
#include "SVFileNameClass.h"
#include "SVOCRGrayAnalyzerResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOCRGrayGeneralDlg property page

IMPLEMENT_DYNCREATE(SVOCRGrayGeneralDlg, CPropertyPage)

SVOCRGrayGeneralDlg::SVOCRGrayGeneralDlg() : CPropertyPage(SVOCRGrayGeneralDlg::IDD)
{
	//{{AFX_DATA_INIT(SVOCRGrayGeneralDlg)
	m_yVicinity = 0;
	m_xVicinity = 0;
	m_maxMatches = 0;
	m_minContrast = 0.0f;
	m_fontFilename = _T("");
	m_output = FALSE;
	//}}AFX_DATA_INIT
}

SVOCRGrayGeneralDlg::~SVOCRGrayGeneralDlg()
{
}

HRESULT SVOCRGrayGeneralDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	SVOCRGrayDialogClass * pParent = (SVOCRGrayDialogClass *)GetParent();
	SVOCRGrayAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRGrayParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();

	if( pOCRAnalyzerResult )
	{
		UpdateData(TRUE);

		l_hrOk = AddInputRequest( &( pOCRAnalyzerResult->msvfnvocGrayFontFileName ), m_fontFilename );

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

void SVOCRGrayGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCRGrayGeneralDlg)
	DDX_Text(pDX, IDC_Y_VICINITY, m_yVicinity);
	DDX_Text(pDX, IDC_X_VICINITY, m_xVicinity);
	DDX_Text(pDX, IDC_MAX_MATCHES, m_maxMatches);
	DDX_Text(pDX, IDC_MIN_CONTRAST, m_minContrast);
	DDX_Text(pDX, IDC_FONTFILENAME, m_fontFilename);
	DDX_Check(pDX, IDC_USE_CANDIDATES, m_output);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCRGrayGeneralDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVOCRGrayGeneralDlg)
	ON_BN_CLICKED(IDC_FONT_BROWSE_CMD, OnFontBrowseCmd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOCRGrayGeneralDlg message handlers

BOOL SVOCRGrayGeneralDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SVOCRGrayDialogClass * pParent = (SVOCRGrayDialogClass *)GetParent();
	SVOCRGrayAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRGrayParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();
	
	SetTaskObject( pOCRAnalyzerResult );

	pOCRAnalyzerResult->GetFontFileName( m_fontFilename );

	m_maxMatches   = pOCRParameters->maxMatches;
	m_minContrast  = pOCRParameters->minContrast;
	m_xVicinity    = pOCRParameters->xVicinity;
	m_yVicinity    = pOCRParameters->yVicinity;
	m_output       = pOCRParameters->output;
	
	UpdateData (FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void SVOCRGrayGeneralDlg::OnFontBrowseCmd() 
{
	SVFileNameClass svfncFileName;

	svfncFileName.SetFileType( SV_OCR_FONT_FILE_TYPE );

	//
	// Try to read the current image file path name from registry...
	//
	CString csPath = AfxGetApp()->GetProfileString(	_T( "Settings" ),
	                                                _T( "WitFontFilePath" ),
	                                                _T( "C:\\RUN" ) );

	svfncFileName.SetDefaultPathName( csPath );

	UpdateData( TRUE );

	svfncFileName.SetFullFileName( m_fontFilename );

	if ( svfncFileName.SelectFile() )
	{
		CString csTemp = svfncFileName.GetFullFileName();
		if ( csTemp.IsEmpty() )
		{
			AfxMessageBox(_T("ERROR: No Font File Specified"));
		}
		else
		{
			csTemp = svfncFileName.GetExtension();
			if ( csTemp.CompareNoCase( _T( ".wit" ) ) != 0 )
			{
				CString s;
				s.Format( _T( "ERROR: Font File Requires '.Wit' Extension: %s" ),
					        svfncFileName.GetFullFileName() );
				AfxMessageBox(s);
			}
			else
			{
				//
				// Check for the existance of the font trained file.
				//
				CFileStatus rStatus;
				if ( !CFile::GetStatus( svfncFileName.GetFullFileName(), rStatus ) )
				{
					CString s;
					s.Format( _T( "ERROR: Font File Does Not Exist: %s" ),
					          svfncFileName.GetFullFileName() );
					AfxMessageBox(s);
				}
				else if ( rStatus.m_size <= 0 )
				{
					CString s;
					s.Format( _T( "ERROR: Font File Empty: %s" ),
					          svfncFileName.GetFullFileName() );
					AfxMessageBox(s);
				}
				else
				{
					m_fontFilename = svfncFileName.GetFullFileName();

					UpdateData( FALSE );
				}
			}
		}
	}
}

void SVOCRGrayGeneralDlg::UpdateOCRParameters ()
{
	//
	// Make sure the dialog has been 'created' - 14 Apr 1999 - frb.
	//
	if (!GetSafeHwnd())
	{
		return;
	}
	
	UpdateData (TRUE);
	
	SVOCRGrayDialogClass * pParent = (SVOCRGrayDialogClass *)GetParent();
	SVOCRGrayAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRGrayParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();

	if (pOCRParameters)
	{
		pOCRParameters->maxMatches = m_maxMatches;
		pOCRParameters->minContrast = m_minContrast;
		pOCRParameters->xVicinity = m_xVicinity;
		pOCRParameters->yVicinity = m_yVicinity;
		pOCRParameters->output = m_output;
	}

	SetInspectionData();
	
	return;
}

BOOL SVOCRGrayGeneralDlg::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateOCRParameters ();
	return CPropertyPage::OnKillActive();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRGrayGeneralDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:56:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   09 Aug 2005 07:35:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Apr 2003 12:53:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   31 Oct 2001 15:41:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  251
 * SCR Title:  Inconsistent/Incorrect display of OCR file names
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Removed overwrite of registry setting "WitFontFilePath" in function OnFontBrowseCmd
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Jun 2001 08:54:34   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: OnInitDialog, OnFontBrowseCmd, UpdateOCRParameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   06 Feb 2001 13:32:00   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2.1.0   16 May 2001 14:10:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  202
 * SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Aug 2000 13:24:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  174
 * SCR Title:  Fix Default Path Name Problem with OCR Font File Name Box
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated default registry path information from C:\IMAGE to C:\TEMP and updated the default dialog path from C:\WIT to default registry path.
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
 *    Rev 1.0   16 May 2000 14:44:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/