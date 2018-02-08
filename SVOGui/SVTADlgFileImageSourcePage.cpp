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
#include "SVObjectLibrary/SVClsids.h"

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	SVToolAdjustmentDialogFileImageSourcePageClass::SVToolAdjustmentDialogFileImageSourcePageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID)
		: CPropertyPage(SVToolAdjustmentDialogFileImageSourcePageClass::IDD)
		, m_ImageController(rInspectionID, rTaskObjectID)
		, m_Values{ SvOg::BoundValues{ rInspectionID, rTaskObjectID } }
	{
		//{{AFX_DATA_INIT(SVToolAdjustmentDialogFileImageSourcePageClass)
		m_PathName = _T("");
		m_BContinuousReload = FALSE;
		//}}AFX_DATA_INIT
	}

	SVToolAdjustmentDialogFileImageSourcePageClass::~SVToolAdjustmentDialogFileImageSourcePageClass()
	{
	}

	HRESULT SVToolAdjustmentDialogFileImageSourcePageClass::SetInspectionData()
	{
		HRESULT hr = S_OK;

		SetData();
		hr = m_Values.Commit(SvOg::doResetRunOnce);
		m_Values.Init(); // because we want to show C:\run and not C:\Images, etc...
		GetData();

		return hr;
	}

	void SVToolAdjustmentDialogFileImageSourcePageClass::refresh()
	{
		SetInspectionData();
		setImages();
	}

	void SVToolAdjustmentDialogFileImageSourcePageClass::GetData()
	{
		m_PathName = m_Values.Get<CString>(SVPathNameObjectGuid);
		m_BContinuousReload = m_Values.Get<bool>(SVContinuousReloadObjectGuid);
		UpdateData(false); // Set data to dialog
	}

	void SVToolAdjustmentDialogFileImageSourcePageClass::SetData()
	{
		UpdateData(true); // get data from dialog
		// We want to Show C:\RUN instead of C:\Images or whatever directory was used to select the image
		// So don't use the bound field
		CString name = m_svfncImageSourceFile.GetFullFileName().c_str();
		if (!name.IsEmpty())
		{
			m_Values.Set<CString>(SVPathNameObjectGuid, name);
		}
		m_Values.Set<bool>(SVContinuousReloadObjectGuid, m_BContinuousReload  ? true : false);
	}

	void SVToolAdjustmentDialogFileImageSourcePageClass::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVToolAdjustmentDialogFileImageSourcePageClass)
		DDX_Control(pDX, IDC_IMAGE, m_imageCtrl);
		DDX_Text(pDX, IDC_IMAGE_SOURCE_EDIT, m_PathName);
		DDX_Check(pDX, IDC_RELOAD_CHECK, m_BContinuousReload);
		//}}AFX_DATA_MAP
	}

	//******************************************************************************
	// Message Map Entries
	//******************************************************************************

	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogFileImageSourcePageClass, CPropertyPage)
		//{{AFX_MSG_MAP(SVToolAdjustmentDialogFileImageSourcePageClass)
		ON_BN_CLICKED(ID_BROWSE_BUTTON, OnBrowseButton)
		ON_BN_CLICKED(IDC_RELOAD_CHECK, OnReloadCheck)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	//******************************************************************************
	// Message Handler
	//******************************************************************************

	BOOL SVToolAdjustmentDialogFileImageSourcePageClass::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();
		m_ImageController.Init();
		m_Values.Init();
		GetData();

		m_imageCtrl.AddTab(_T("Tool Input")); 

		setImages();
	
		return true;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	void SVToolAdjustmentDialogFileImageSourcePageClass::OnBrowseButton() 
	{
		UpdateData(true);
	
		// Try to read the current image file path name from registry...
		std::string Path = AfxGetApp()->GetProfileString(	_T("Settings"),_T( "ImagesFilePath" ), _T("C:\\Images"));
		
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

	void SVToolAdjustmentDialogFileImageSourcePageClass::OnReloadCheck() 
	{
		refresh();
	}

	void SVToolAdjustmentDialogFileImageSourcePageClass::setImages()
	{
		const SvUl::NameGuidList& images = m_ImageController.GetResultImages();
		SvUl::NameGuidList::const_iterator it = images.begin();
		if (it != images.end())
		{
			IPictureDisp* pImage = m_ImageController.GetImage(it->second.ToGUID());
			m_imageCtrl.setImage(pImage, 0);
			m_imageCtrl.Refresh();
		}
	}
} //namespace SvOg

