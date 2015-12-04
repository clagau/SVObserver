//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVGeneralDlg
//* .File Name       : $Workfile:   SVOCVGeneralDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:03:00  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOCVDialog.h"
#include "SVOCVGeneralDlg.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVOCVAnalyzerResult.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOCVGeneralDlg property page

IMPLEMENT_DYNCREATE(SVOCVGeneralDlg, CPropertyPage)

SVOCVGeneralDlg::SVOCVGeneralDlg() : CPropertyPage(SVOCVGeneralDlg::IDD)
{
	//{{AFX_DATA_INIT(SVOCVGeneralDlg)
	m_fontFilename = _T("");
	m_constraintsFilename = _T("");
	m_controlsFilename = _T("");
	//}}AFX_DATA_INIT
}

SVOCVGeneralDlg::~SVOCVGeneralDlg()
{
}

HRESULT SVOCVGeneralDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	UpdateData (TRUE);
	
	SVOCVDialogClass *pParent = (SVOCVDialogClass *) GetParent();
	SVOCVAnalyzeResultClass *pOCVAnalyzerResult = pParent->pOCVAnalyzerResult;

	if( pOCVAnalyzerResult )
	{
		UpdateData(TRUE);

		l_hrOk = AddInputRequest( &( pOCVAnalyzerResult->m_fnvoFontFileName ), m_fontFilename );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( pOCVAnalyzerResult->m_fnvoConstraintsFileName ), m_constraintsFilename );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( &( pOCVAnalyzerResult->m_fnvoControlsFileName ), m_controlsFilename );
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

void SVOCVGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCVGeneralDlg)
	DDX_Text(pDX, IDC_FONTFILENAME, m_fontFilename);
	DDX_Text(pDX, IDC_CONSTRAINTSFILENAME, m_constraintsFilename);
	DDX_Text(pDX, IDC_CONTROLSFILENAME, m_controlsFilename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCVGeneralDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVOCVGeneralDlg)
	ON_BN_CLICKED(IDC_FONT_BROWSE_CMD, OnFontBrowseCmd)
	ON_BN_CLICKED(IDC_CONSTRAINTS_BROWSE_CMD, OnConstraintsBrowseCmd)
	ON_BN_CLICKED(IDC_CONTROLS_BROWSE_CMD, OnControlsBrowseCmd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOCVGeneralDlg message handlers

BOOL SVOCVGeneralDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SVOCVDialogClass *pParent = (SVOCVDialogClass*) GetParent();
	SVOCVAnalyzeResultClass *pOCVAnalyzerResult = pParent->pOCVAnalyzerResult;
	
	SetTaskObject( pOCVAnalyzerResult );

	pOCVAnalyzerResult->m_fnvoFontFileName.GetValue( m_fontFilename );
	pOCVAnalyzerResult->m_fnvoConstraintsFileName.GetValue( m_constraintsFilename );
	pOCVAnalyzerResult->m_fnvoControlsFileName.GetValue( m_controlsFilename );
	
	UpdateData (FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void SVOCVGeneralDlg::OnFontBrowseCmd() 
{
	SVFileNameClass svfncFileName;

	svfncFileName.SetFileType( SV_OCV_FONT_FILE_TYPE );

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
			if ( csTemp.CompareNoCase( _T( ".mfo" ) ) != 0 )
			{
				CString s;
				s.Format( _T( "ERROR: Font File Requires '.mfo' Extension: %s" ),
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


void SVOCVGeneralDlg::OnConstraintsBrowseCmd() 
{
	SVFileNameClass svfncFileName;

	svfncFileName.SetFileType( SV_OCV_CONSTRAINTS_FILE_TYPE );

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
			AfxMessageBox(_T("ERROR: No Font Constraints File Specified"));
		}
		else
		{
			csTemp = svfncFileName.GetExtension();
			if ( csTemp.CompareNoCase( _T( ".mfo" ) ) != 0 )
			{
				CString s;
				s.Format( _T( "ERROR: Font Constraints File Requires '.mfo' Extension: %s" ),
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
					s.Format( _T( "ERROR: Font Constraints File Does Not Exist: %s" ),
					          svfncFileName.GetFullFileName() );
					AfxMessageBox(s);
				}
				else if ( rStatus.m_size <= 0 )
				{
					CString s;
					s.Format( _T( "ERROR: Font Constraints File Empty: %s" ),
					          svfncFileName.GetFullFileName() );
					AfxMessageBox(s);
				}
				else
				{
					m_constraintsFilename = svfncFileName.GetFullFileName();

					UpdateData( FALSE );
				}
			}
		}
	}
}



void SVOCVGeneralDlg::OnControlsBrowseCmd() 
{
	SVFileNameClass svfncFileName;

	svfncFileName.SetFileType( SV_OCV_CONTROLS_FILE_TYPE );

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
			AfxMessageBox(_T("ERROR: No Font Controls File Specified"));
		}
		else
		{
			csTemp = svfncFileName.GetExtension();
			if ( csTemp.CompareNoCase( _T( ".mfo" ) ) != 0 )
			{
				CString s;
				s.Format( _T( "ERROR: Font Controls File Requires '.mfo' Extension: %s" ),
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
					s.Format( _T( "ERROR: Font Controls File Does Not Exist: %s" ),
					          svfncFileName.GetFullFileName() );
					AfxMessageBox(s);
				}
				else if ( rStatus.m_size <= 0 )
				{
					CString s;
					s.Format( _T( "ERROR: Font Controls File Empty: %s" ),
					          svfncFileName.GetFullFileName() );
					AfxMessageBox(s);
				}
				else
				{
					m_controlsFilename = svfncFileName.GetFullFileName();

					UpdateData( FALSE );
				}
			}
		}
	}
}

void SVOCVGeneralDlg::UpdateOCVParameters ()
{
	if( !GetSafeHwnd() )
	{
		return;
	}
	
	SetInspectionData();
	
	return;
}

BOOL SVOCVGeneralDlg::OnKillActive() 
{
	UpdateOCVParameters();

	return CPropertyPage::OnKillActive();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCVGeneralDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:03:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Oct 2005 14:02:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed index 1 from GetValue calls.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Aug 2005 07:53:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Sep 2003 13:07:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/