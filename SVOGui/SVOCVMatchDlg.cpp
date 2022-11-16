//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVOCVSheet.h"
#include "SVOCVMatchDlg.h"
#include "FilesystemUtilities/FileHelper.h"
#include "SVStatusLibrary/GlobalPath.h"

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVOCVMatchDlg, CPropertyPage)
		//{{AFX_MSG_MAP(SVOCVMatchDlg)
		ON_BN_CLICKED(IDC_FONT_BROWSE_CMD, OnFontBrowseCmd)
		ON_BN_CLICKED(IDC_USE_MATCH_STRING, OnUseMatchString)
		ON_BN_CLICKED(IDC_USE_MATCH_FILE, OnUseMatchFile)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVOCVMatchDlg::SVOCVMatchDlg(SVOCVSheet& rParent) : CPropertyPage(SVOCVMatchDlg::IDD)
	, m_rParent{ rParent }
	, m_useMatchFile{ -1 }
	, m_nRadioOperation{ -1 }
	{
	}

	SVOCVMatchDlg::~SVOCVMatchDlg()
	{
	}

	HRESULT SVOCVMatchDlg::SetInspectionData()
	{
		HRESULT Result{ S_OK };

		UpdateData(true);
	
		m_rParent.GetValues().Set<CString>(SvPb::OCVMatchStringFileNameEId, m_MatchFilename);
		m_rParent.GetValues().Set<CString>(SvPb::OCVMatchStringEId, m_MatchString);
		m_rParent.GetValues().Set<bool>(SvPb::OCVUseMatchFileEId, m_useMatchFile ? false : true);
		m_rParent.GetValues().Set<bool>(SvPb::OCVPerformOCREId, m_nRadioOperation ? false : true);

		Result = m_rParent.GetValues().Commit();

		return Result;
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

	BOOL SVOCVMatchDlg::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		m_MatchFilename = m_rParent.GetValues().Get<CString>(SvPb::OCVMatchStringFileNameEId);
		m_MatchString = m_rParent.GetValues().Get<CString>(SvPb::OCVMatchStringEId);
		bool bUseMatchFile = m_rParent.GetValues().Get<bool>(SvPb::OCVUseMatchFileEId);
		bool bPerformOCR = m_rParent.GetValues().Get<bool>(SvPb::OCVPerformOCREId);
		m_nRadioOperation = bPerformOCR ? 0 : 1;

		if (bUseMatchFile)
		{
			m_useMatchFile = 0;
			OnUseMatchFile();
		}
		else
		{
			m_useMatchFile = 1;
			OnUseMatchString();
		}
	
		UpdateData(false);
	
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}


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
		SvFs::FileHelper svfncFileName;

		svfncFileName.SetFileType( SvFs::FileType::ocvMatch );

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
} //namespace SvOg
