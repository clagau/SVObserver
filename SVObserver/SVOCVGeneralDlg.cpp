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
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/MessageManagerResource.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "SVStatusLibrary/GlobalPath.h"
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
	                                                SvStl::GlobalPath::Inst().GetRunPath().c_str() );

	svfncFileName.SetDefaultPathName( csPath );

	UpdateData( TRUE );

	svfncFileName.SetFullFileName( m_fontFilename );
	if ( svfncFileName.SelectFile() )
	{
		CString csTemp = svfncFileName.GetFullFileName();
		if ( csTemp.IsEmpty() )
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_NoFontFileSpec, StdMessageParams, SvOi::Err_10171 ); 
		}
		else
		{
			csTemp = svfncFileName.GetExtension();
			if ( csTemp.CompareNoCase( _T( ".mfo" ) ) != 0 )
			{
				SVStringArray msgList;
				msgList.push_back(svfncFileName.GetFullFileName());
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontNotMfoExt, msgList, StdMessageParams, SvOi::Err_10172 ); 
			}
			else
			{
				//
				// Check for the existance of the font trained file.
				//
				CFileStatus rStatus;
				if ( !CFile::GetStatus( svfncFileName.GetFullFileName(), rStatus ) )
				{
					SVStringArray msgList;
					msgList.push_back(svfncFileName.GetFullFileName());
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontFileNotExist, msgList, StdMessageParams, SvOi::Err_10173 ); 
				}
				else if ( rStatus.m_size <= 0 )
				{
					SVStringArray msgList;
					msgList.push_back(svfncFileName.GetFullFileName());
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontFileEmpty, msgList, StdMessageParams, SvOi::Err_10174 );
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
	                                                SvStl::GlobalPath::Inst().GetRunPath().c_str() );

	svfncFileName.SetDefaultPathName( csPath );

	UpdateData( TRUE );

	svfncFileName.SetFullFileName( m_fontFilename );
	if ( svfncFileName.SelectFile() )
	{
		CString csTemp = svfncFileName.GetFullFileName();
		if ( csTemp.IsEmpty() )
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_NoFontConstraintsFileSpec, StdMessageParams, SvOi::Err_10175 ); 
		}
		else
		{
			csTemp = svfncFileName.GetExtension();
			if ( csTemp.CompareNoCase( _T( ".mfo" ) ) != 0 )
			{
				SVStringArray msgList;
				msgList.push_back(svfncFileName.GetFullFileName());
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontConstraintsRequiredMfoExt, msgList, StdMessageParams, SvOi::Err_10176 ); 
			}
			else
			{
				//
				// Check for the existance of the font trained file.
				//
				CFileStatus rStatus;
				if ( !CFile::GetStatus( svfncFileName.GetFullFileName(), rStatus ) )
				{
					SVStringArray msgList;
					msgList.push_back(svfncFileName.GetFullFileName() );
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontConstraintsFileNotExist, msgList, StdMessageParams, SvOi::Err_10177 ); 
				}
				else if ( rStatus.m_size <= 0 )
				{
					SVStringArray msgList;
					msgList.push_back(svfncFileName.GetFullFileName() );
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontConstraintsFileEmpty, msgList, StdMessageParams, SvOi::Err_10178 ); 
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
	                                                SvStl::GlobalPath::Inst().GetRunPath().c_str() );

	svfncFileName.SetDefaultPathName( csPath );

	UpdateData( TRUE );

	svfncFileName.SetFullFileName( m_fontFilename );
	if ( svfncFileName.SelectFile() )
	{
		CString csTemp = svfncFileName.GetFullFileName();
		if ( csTemp.IsEmpty() )
		{
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_NoFontControlsFileSpec, StdMessageParams, SvOi::Err_10179 ); 
		}
		else
		{
			csTemp = svfncFileName.GetExtension();
			if ( csTemp.CompareNoCase( _T( ".mfo" ) ) != 0 )
			{
				SVStringArray msgList;
				msgList.push_back( svfncFileName.GetFullFileName() );
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontControlsRequiredMfoExt, msgList, StdMessageParams, SvOi::Err_10180 ); 
			}
			else
			{
				//
				// Check for the existance of the font trained file.
				//
				CFileStatus rStatus;
				if ( !CFile::GetStatus( svfncFileName.GetFullFileName(), rStatus ) )
				{
					SVStringArray msgList;
					msgList.push_back( svfncFileName.GetFullFileName() );
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontControlsFileNotExist, msgList, StdMessageParams, SvOi::Err_10181 ); 
				}
				else if ( rStatus.m_size <= 0 )
				{
					SVStringArray msgList;
					msgList.push_back( svfncFileName.GetFullFileName() );
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontControlsFileEmpty, msgList, StdMessageParams, SvOi::Err_10182 ); 
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

