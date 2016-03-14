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

#pragma region Includes
#include "stdafx.h"
#include "SVOCRGrayDialog.h"
#include "SVOCRGrayGeneralDlg.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVOCRGrayAnalyzerResult.h"
#pragma endregion Includes

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

