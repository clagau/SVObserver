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

#pragma region Includes
#include "stdafx.h"
#include "SVOCVDialog.h"
#include "SVOCVMatchDlg.h"
#include "SVOCVAnalyzerResult.h"
#include "SVStatusLibrary\GlobalPath.h"

#pragma endregion Includes

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
	
	SVOCVDialogClass* pParent = dynamic_cast<SVOCVDialogClass*>(GetParent());
	SVOCVAnalyzeResultClass *pOCVAnalyzerResult = pParent->GetAnalyzerResult();
	
	if( pOCVAnalyzerResult )
	{
		l_hrOk = AddInputRequest( SVObjectReference(&pOCVAnalyzerResult->m_fnvoMatchStringFileName), _variant_t(m_MatchFilename) );

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequest( SVObjectReference(&pOCVAnalyzerResult->m_svoMatchString), _variant_t(m_MatchString) );
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequest( SVObjectReference(&pOCVAnalyzerResult->m_bvoUseMatchFile), _variant_t(m_useMatchFile ? false : true) );
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequest( SVObjectReference(&pOCVAnalyzerResult->m_bvoPerformOCR), _variant_t(m_nRadioOperation ? false : true) );
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = RunOnce( pOCVAnalyzerResult->GetTool()->GetUniqueObjectID() );
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
	
	SVOCVDialogClass* pParent = dynamic_cast<SVOCVDialogClass*>(GetParent());
	SVOCVAnalyzeResultClass* pOCVAnalyzerResult = pParent->GetAnalyzerResult();
	
	SetTaskObject( pOCVAnalyzerResult );

	std::string Value;
	pOCVAnalyzerResult->m_fnvoMatchStringFileName.GetValue( Value );
	m_MatchFilename = Value.c_str();
	pOCVAnalyzerResult->m_svoMatchString.GetValue( Value );
	m_MatchString = Value.c_str();
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
	CString Path = AfxGetApp()->GetProfileString(	_T("Settings"),
	                                                _T("OCVMatchStringFilePath"),
	                                                SvStl::GlobalPath::Inst().GetRunPath().c_str());

	svfncFileName.SetDefaultPathName( std::string(Path) );

	UpdateData( TRUE );

	svfncFileName.SetFullFileName( m_MatchFilename );

	if ( svfncFileName.SelectFile() )
	{
		AfxGetApp()->WriteProfileString( _T( "Settings" ),
		                                 _T( "OCVMatchStringFilePath" ),
										 svfncFileName.GetPathName().c_str() );

		m_MatchFilename = svfncFileName.GetFullFileName().c_str();

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

