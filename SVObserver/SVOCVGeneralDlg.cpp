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
#include "SVLibrary/SVFileNameClass.h"
#include "SVOCVAnalyzerResult.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary/MessageManager.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/SVString.h"
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
	
	SVOCVDialogClass* pParent = dynamic_cast<SVOCVDialogClass*>(GetParent());
	SVOCVAnalyzeResultClass* pOCVAnalyzerResult = pParent->GetAnalyzerResult();

	if( pOCVAnalyzerResult )
	{
		UpdateData(TRUE);

		l_hrOk = AddInputRequest( SVObjectReference(&pOCVAnalyzerResult->m_fnvoFontFileName), _variant_t(m_fontFilename) );

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequest( SVObjectReference(&pOCVAnalyzerResult->m_fnvoConstraintsFileName), _variant_t(m_constraintsFilename) );
		}

		if( S_OK == l_hrOk )
		{
			l_hrOk = AddInputRequest( SVObjectReference(&pOCVAnalyzerResult->m_fnvoControlsFileName), _variant_t(m_controlsFilename) );
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
	
	SVOCVDialogClass* pParent = dynamic_cast<SVOCVDialogClass*>(GetParent());
	SVOCVAnalyzeResultClass *pOCVAnalyzerResult = pParent->GetAnalyzerResult();
	
	SetTaskObject( pOCVAnalyzerResult );

	SVString Value;
	pOCVAnalyzerResult->m_fnvoFontFileName.GetValue( Value );
	m_fontFilename = Value.c_str();
	pOCVAnalyzerResult->m_fnvoConstraintsFileName.GetValue( Value );
	m_constraintsFilename = Value.c_str();
	pOCVAnalyzerResult->m_fnvoControlsFileName.GetValue( Value );
	m_controlsFilename = Value.c_str();
	
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
	CString Path = AfxGetApp()->GetProfileString(	_T( "Settings" ),
	                                                _T( "WitFontFilePath" ),
	                                                SvStl::GlobalPath::Inst().GetRunPath().c_str() );

	svfncFileName.SetDefaultPathName( SVString(Path) );

	UpdateData( TRUE );

	svfncFileName.SetFullFileName( m_fontFilename );
	if ( svfncFileName.SelectFile() )
	{
		if ( svfncFileName.GetFullFileName().empty() )
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_NoFontFileSpec, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10171 ); 
		}
		else
		{
			if( 0 != SvUl_SF::CompareNoCase( svfncFileName.GetExtension(), SVString( _T(".mfo") ) ) )
			{
				SVStringVector msgList;
				msgList.push_back(svfncFileName.GetFullFileName());
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontNotMfoExt, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10172 ); 
			}
			else
			{
				//
				// Check for the existance of the font trained file.
				//
				CFileStatus rStatus;
				if ( !CFile::GetStatus( svfncFileName.GetFullFileName().c_str(), rStatus ) )
				{
					SVStringVector msgList;
					msgList.push_back(svfncFileName.GetFullFileName());
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontFileNotExist, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10173 ); 
				}
				else if ( rStatus.m_size <= 0 )
				{
					SVStringVector msgList;
					msgList.push_back(svfncFileName.GetFullFileName());
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontFileEmpty, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10174 );
				}
				else
				{
					m_fontFilename = svfncFileName.GetFullFileName().c_str();

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
	CString Path = AfxGetApp()->GetProfileString(	_T( "Settings" ),
	                                                _T( "WitFontFilePath" ),
	                                                SvStl::GlobalPath::Inst().GetRunPath().c_str() );

	svfncFileName.SetDefaultPathName( SVString(Path) );

	UpdateData( TRUE );

	svfncFileName.SetFullFileName( m_fontFilename );
	if ( svfncFileName.SelectFile() )
	{
		if ( svfncFileName.GetFullFileName().empty() )
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_NoFontConstraintsFileSpec, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10175 ); 
		}
		else
		{
			if ( 0 != SvUl_SF::CompareNoCase( svfncFileName.GetExtension(), SVString( _T(".mfo") ) )  )
			{
				SVStringVector msgList;
				msgList.push_back(svfncFileName.GetFullFileName());
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontConstraintsRequiredMfoExt, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10176 ); 
			}
			else
			{
				//
				// Check for the existance of the font trained file.
				//
				CFileStatus rStatus;
				if ( !CFile::GetStatus( svfncFileName.GetFullFileName().c_str(), rStatus ) )
				{
					SVStringVector msgList;
					msgList.push_back(svfncFileName.GetFullFileName() );
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontConstraintsFileNotExist, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10177 ); 
				}
				else if ( rStatus.m_size <= 0 )
				{
					SVStringVector msgList;
					msgList.push_back(svfncFileName.GetFullFileName() );
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontConstraintsFileEmpty, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10178 ); 
				}
				else
				{
					m_constraintsFilename = svfncFileName.GetFullFileName().c_str();

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
	CString Path = AfxGetApp()->GetProfileString(	_T( "Settings" ),
	                                                _T( "WitFontFilePath" ),
	                                                SvStl::GlobalPath::Inst().GetRunPath().c_str() );

	svfncFileName.SetDefaultPathName( SVString(Path) );

	UpdateData( TRUE );

	svfncFileName.SetFullFileName( m_fontFilename );
	if ( svfncFileName.SelectFile() )
	{
		if ( svfncFileName.GetFullFileName().empty() )
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_NoFontControlsFileSpec, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10179 ); 
		}
		else
		{
			if( 0 != SvUl_SF::CompareNoCase( svfncFileName.GetExtension(), SVString( _T(".mfo") ) ) )
			{
				SVStringVector msgList;
				msgList.push_back( svfncFileName.GetFullFileName() );
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontControlsRequiredMfoExt, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10180 ); 
			}
			else
			{
				//
				// Check for the existance of the font trained file.
				//
				CFileStatus rStatus;
				if ( !CFile::GetStatus( svfncFileName.GetFullFileName().c_str(), rStatus ) )
				{
					SVStringVector msgList;
					msgList.push_back( svfncFileName.GetFullFileName() );
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontControlsFileNotExist, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10181 ); 
				}
				else if ( rStatus.m_size <= 0 )
				{
					SVStringVector msgList;
					msgList.push_back( svfncFileName.GetFullFileName() );
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_FontControlsFileEmpty, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10182 ); 
				}
				else
				{
					m_controlsFilename = svfncFileName.GetFullFileName().c_str();

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

