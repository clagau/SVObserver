//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVOCVGeneralDlg.h"
#include "SVOCVSheet.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	SVOCVGeneralDlg::SVOCVGeneralDlg(SVOCVSheet& rParent) : CPropertyPage(SVOCVGeneralDlg::IDD)
	, m_rParent(rParent)
	{
		//{{AFX_DATA_INIT(SVOCVGeneralDlg)
		//}}AFX_DATA_INIT
	}

	SVOCVGeneralDlg::~SVOCVGeneralDlg()
	{
	}

	HRESULT SVOCVGeneralDlg::SetInspectionData()
	{
		HRESULT Result{ S_OK };

		UpdateData(true);

		m_rParent.GetValues().Set<CString>(SvPb::OCVFontFileNameEId, m_fontFilename);
		m_rParent.GetValues().Set<CString>(SvPb::OCVConstraintsFileNameEId, m_constraintsFilename);
		m_rParent.GetValues().Set<CString>(SvPb::OCVControlsFileNameEId, m_controlsFilename);

		Result = m_rParent.GetValues().Commit();

		return Result;
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

		m_fontFilename = m_rParent.GetValues().Get<CString>(SvPb::OCVFontFileNameEId);
		m_constraintsFilename = m_rParent.GetValues().Get<CString>(SvPb::OCVConstraintsFileNameEId);
		m_controlsFilename = m_rParent.GetValues().Get<CString>(SvPb::OCVControlsFileNameEId);
	
		UpdateData(false);

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
		CString Path = AfxGetApp()->GetProfileString(_T( "Settings" ),
													 _T( "WitFontFilePath" ),
													 SvStl::GlobalPath::Inst().GetRunPath().c_str() );

		svfncFileName.SetDefaultPathName( std::string(Path) );

		UpdateData( TRUE );

		svfncFileName.SetFullFileName( m_fontFilename );
		if ( svfncFileName.SelectFile() )
		{
			if ( svfncFileName.GetFullFileName().empty() )
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_NoFontFileSpec, SvStl::SourceFileParams(StdMessageParams)); 
			}
			else
			{
				if( 0 != SvUl::CompareNoCase( svfncFileName.GetExtension(), std::string( _T(".mfo") ) ) )
				{
					SvDef::StringVector msgList;
					msgList.push_back(svfncFileName.GetFullFileName());
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_FontNotMfoExt, msgList, SvStl::SourceFileParams(StdMessageParams)); 
				}
				else
				{
					//
					// Check for the existance of the font trained file.
					//
					CFileStatus rStatus;
					if ( !CFile::GetStatus( svfncFileName.GetFullFileName().c_str(), rStatus ) )
					{
						SvDef::StringVector msgList;
						msgList.push_back(svfncFileName.GetFullFileName());
						SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_FontFileNotExist, msgList, SvStl::SourceFileParams(StdMessageParams)); 
					}
					else if ( rStatus.m_size <= 0 )
					{
						SvDef::StringVector msgList;
						msgList.push_back(svfncFileName.GetFullFileName());
						SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_FontFileEmpty, msgList, SvStl::SourceFileParams(StdMessageParams));
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
		CString Path = AfxGetApp()->GetProfileString(_T( "Settings" ),
													 _T( "WitFontFilePath" ),
													 SvStl::GlobalPath::Inst().GetRunPath().c_str() );

		svfncFileName.SetDefaultPathName( std::string(Path) );

		UpdateData( TRUE );

		svfncFileName.SetFullFileName( m_fontFilename );
		if ( svfncFileName.SelectFile() )
		{
			if ( svfncFileName.GetFullFileName().empty() )
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_NoFontConstraintsFileSpec, SvStl::SourceFileParams(StdMessageParams)); 
			}
			else
			{
				if ( 0 != SvUl::CompareNoCase( svfncFileName.GetExtension(), std::string( _T(".mfo") ) )  )
				{
					SvDef::StringVector msgList;
					msgList.push_back(svfncFileName.GetFullFileName());
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_FontConstraintsRequiredMfoExt, msgList, SvStl::SourceFileParams(StdMessageParams)); 
				}
				else
				{
					//
					// Check for the existance of the font trained file.
					//
					CFileStatus rStatus;
					if ( !CFile::GetStatus( svfncFileName.GetFullFileName().c_str(), rStatus ) )
					{
						SvDef::StringVector msgList;
						msgList.push_back(svfncFileName.GetFullFileName() );
						SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_FontConstraintsFileNotExist, msgList, SvStl::SourceFileParams(StdMessageParams)); 
					}
					else if ( rStatus.m_size <= 0 )
					{
						SvDef::StringVector msgList;
						msgList.push_back(svfncFileName.GetFullFileName() );
						SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_FontConstraintsFileEmpty, msgList, SvStl::SourceFileParams(StdMessageParams)); 
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

		svfncFileName.SetDefaultPathName( std::string(Path) );

		UpdateData( TRUE );

		svfncFileName.SetFullFileName( m_fontFilename );
		if ( svfncFileName.SelectFile() )
		{
			if ( svfncFileName.GetFullFileName().empty() )
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_NoFontControlsFileSpec, SvStl::SourceFileParams(StdMessageParams)); 
			}
			else
			{
				if( 0 != SvUl::CompareNoCase( svfncFileName.GetExtension(), std::string( _T(".mfo") ) ) )
				{
					SvDef::StringVector msgList;
					msgList.push_back( svfncFileName.GetFullFileName() );
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_FontControlsRequiredMfoExt, msgList, SvStl::SourceFileParams(StdMessageParams)); 
				}
				else
				{
					//
					// Check for the existance of the font trained file.
					//
					CFileStatus rStatus;
					if ( !CFile::GetStatus( svfncFileName.GetFullFileName().c_str(), rStatus ) )
					{
						SvDef::StringVector msgList;
						msgList.push_back( svfncFileName.GetFullFileName() );
						SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_FontControlsFileNotExist, msgList, SvStl::SourceFileParams(StdMessageParams)); 
					}
					else if ( rStatus.m_size <= 0 )
					{
						SvDef::StringVector msgList;
						msgList.push_back( svfncFileName.GetFullFileName() );
						SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_FontControlsFileEmpty, msgList, SvStl::SourceFileParams(StdMessageParams)); 
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
} //namespace SvOg
