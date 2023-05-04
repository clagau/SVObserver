//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgFileImageSourcePage
//* .File Name       : $Workfile:   TADlgFileImageSourcePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgFileImageSourcePage.h"

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	TADlgFileImageSourcePage::TADlgFileImageSourcePage(uint32_t inspectionId, uint32_t taskObjectId)
		: CPropertyPage(TADlgFileImageSourcePage::IDD)
		, m_ImageController(inspectionId, taskObjectId)
		, m_values{ SvOgu::BoundValues{ inspectionId, taskObjectId } }
	{
		//{{AFX_DATA_INIT(TADlgFileImageSourcePage)
		//}}AFX_DATA_INIT
	}

	TADlgFileImageSourcePage::~TADlgFileImageSourcePage()
	{
	}

	HRESULT TADlgFileImageSourcePage::SetInspectionData()
	{
		HRESULT hr = S_OK;

		SetData();
		hr = m_values.Commit(SvOgu::PostAction::doReset | SvOgu::PostAction::doRunOnce);
		GetData();

		return hr;
	}

	void TADlgFileImageSourcePage::refresh()
	{
		SetInspectionData();
		setImages();
	}

	void TADlgFileImageSourcePage::GetData()
	{
		m_PathName = m_values.Get<CString>(SvPb::PathNameEId);
		m_BContinuousReload = m_values.Get<bool>(SvPb::ContinuousReloadEId);
		m_BIsColorImage = m_values.Get<bool>(SvPb::IsColorImageCheckEId);
		UpdateData(false); // Set data to dialog
	}

	void TADlgFileImageSourcePage::SetData()
	{
		UpdateData(true); // get data from dialog
		// We want to Show C:\RUN instead of C:\Images or whatever directory was used to select the image
		// So don't use the bound field
		CString name = m_svfncImageSourceFile.GetFullFileName().c_str();
		if (!name.IsEmpty())
		{
			m_values.Set<CString>(SvPb::PathNameEId, name);
		}
		m_values.Set<bool>(SvPb::ContinuousReloadEId, m_BContinuousReload  ? true : false);
		m_values.Set<bool>(SvPb::IsColorImageCheckEId, m_BIsColorImage ? true : false);
	}

	void TADlgFileImageSourcePage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TADlgFileImageSourcePage)
		DDX_Control(pDX, IDC_IMAGE, m_imageCtrl);
		DDX_Text(pDX, IDC_IMAGE_SOURCE_EDIT, m_PathName);
		DDX_Check(pDX, IDC_RELOAD_CHECK, m_BContinuousReload);
		DDX_Check(pDX, IDC_COLOR_CHECK, m_BIsColorImage);
		//}}AFX_DATA_MAP
	}

	//******************************************************************************
	// Message Map Entries
	//******************************************************************************

	BEGIN_MESSAGE_MAP(TADlgFileImageSourcePage, CPropertyPage)
		//{{AFX_MSG_MAP(TADlgFileImageSourcePage)
		ON_BN_CLICKED(ID_BROWSE_BUTTON, OnBrowseButton)
		ON_BN_CLICKED(IDC_RELOAD_CHECK, OnReloadCheck)
		ON_BN_CLICKED(IDC_COLOR_CHECK, OnColorImageCheck)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	//******************************************************************************
	// Message Handler
	//******************************************************************************

	BOOL TADlgFileImageSourcePage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();
		m_ImageController.Init();
		m_values.Init();
		GetData();

		m_imageCtrl.AddTab(_T("Tool Input")); 

		setImages();
	
		return true;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	void TADlgFileImageSourcePage::OnBrowseButton() 
	{
		UpdateData(true);
	
		// Try to read the current image file path name from registry...
		std::string Path = AfxGetApp()->GetProfileString(	_T("Settings"),_T( "ImagesFilePath" ), _T("C:\\Images")).GetString();
		
		m_svfncImageSourceFile.SetPathName(Path.c_str());
		m_svfncImageSourceFile.SetFileType(SvFs::FileType::SourceFile);
		if (m_svfncImageSourceFile.SelectFile())
		{
			// Save the directory selected from..
			Path = m_svfncImageSourceFile.GetPathName();
			
			// Write this path name back to registry...to initialize registry.
			AfxGetApp()->WriteProfileString(_T("Settings"), _T("ImagesFilePath"), Path.c_str());

			refresh();
		}
	}

	void TADlgFileImageSourcePage::OnReloadCheck() 
	{
		refresh();
	}

	void TADlgFileImageSourcePage::OnColorImageCheck()
	{
		refresh();
	}

	void TADlgFileImageSourcePage::setImages()
	{
		IPictureDisp* pImage = m_ImageController.GetImage(getFirstResultImageId(m_ImageController));
		m_imageCtrl.setImage(pImage, 0);
		m_imageCtrl.Refresh();
	}
} //namespace SvOg

