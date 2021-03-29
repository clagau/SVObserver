//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogFileImageSourcePage
//* .File Name       : $Workfile:   SVTADlgFileImageSourcePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgFileImageSourcePage.h"

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	SVTADlgFileImageSourcePage::SVTADlgFileImageSourcePage(uint32_t inspectionId, uint32_t taskObjectId)
		: CPropertyPage(SVTADlgFileImageSourcePage::IDD)
		, m_ImageController(inspectionId, taskObjectId)
		, m_values{ SvOg::BoundValues{ inspectionId, taskObjectId } }
	{
		//{{AFX_DATA_INIT(SVTADlgFileImageSourcePage)
		//}}AFX_DATA_INIT
	}

	SVTADlgFileImageSourcePage::~SVTADlgFileImageSourcePage()
	{
	}

	HRESULT SVTADlgFileImageSourcePage::SetInspectionData()
	{
		HRESULT hr = S_OK;

		SetData();
		hr = m_values.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);
		GetData();

		return hr;
	}

	void SVTADlgFileImageSourcePage::refresh()
	{
		SetInspectionData();
		setImages();
	}

	void SVTADlgFileImageSourcePage::GetData()
	{
		m_PathName = m_values.Get<CString>(SvPb::PathNameEId);
		m_BContinuousReload = m_values.Get<bool>(SvPb::ContinuousReloadEId);
		UpdateData(false); // Set data to dialog
	}

	void SVTADlgFileImageSourcePage::SetData()
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
	}

	void SVTADlgFileImageSourcePage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVTADlgFileImageSourcePage)
		DDX_Control(pDX, IDC_IMAGE, m_imageCtrl);
		DDX_Text(pDX, IDC_IMAGE_SOURCE_EDIT, m_PathName);
		DDX_Check(pDX, IDC_RELOAD_CHECK, m_BContinuousReload);
		//}}AFX_DATA_MAP
	}

	//******************************************************************************
	// Message Map Entries
	//******************************************************************************

	BEGIN_MESSAGE_MAP(SVTADlgFileImageSourcePage, CPropertyPage)
		//{{AFX_MSG_MAP(SVTADlgFileImageSourcePage)
		ON_BN_CLICKED(ID_BROWSE_BUTTON, OnBrowseButton)
		ON_BN_CLICKED(IDC_RELOAD_CHECK, OnReloadCheck)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	//******************************************************************************
	// Message Handler
	//******************************************************************************

	BOOL SVTADlgFileImageSourcePage::OnInitDialog() 
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

	void SVTADlgFileImageSourcePage::OnBrowseButton() 
	{
		UpdateData(true);
	
		// Try to read the current image file path name from registry...
		std::string Path = AfxGetApp()->GetProfileString(	_T("Settings"),_T( "ImagesFilePath" ), _T("C:\\Images")).GetString();
		
		m_svfncImageSourceFile.SetPathName(Path.c_str());
		m_svfncImageSourceFile.SetFileType(SV_IMAGE_SOURCE_FILE_TYPE);
		if (m_svfncImageSourceFile.SelectFile())
		{
			// Save the directory selected from..
			Path = m_svfncImageSourceFile.GetPathName();
			
			// Write this path name back to registry...to initialize registry.
			AfxGetApp()->WriteProfileString(_T("Settings"), _T("ImagesFilePath"), Path.c_str());

			refresh();
		}
	}

	void SVTADlgFileImageSourcePage::OnReloadCheck() 
	{
		refresh();
	}

	void SVTADlgFileImageSourcePage::setImages()
	{
		IPictureDisp* pImage = m_ImageController.GetImage(getFirstResultImageId(m_ImageController));
		m_imageCtrl.setImage(pImage, 0);
		m_imageCtrl.Refresh();
	}
} //namespace SvOg

