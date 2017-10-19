//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSaveToolSetImageDialog
//* .File Name       : $Workfile:   SVSaveToolSetImageDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   26 Jun 2014 18:14:58  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVSaveToolSetImageDialog.h"
#include "SVLibrary/SVFileNameClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "TextDefinesSvOg.h"
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace SvOg
{
	SVSaveToolSetImageDialogClass::SVSaveToolSetImageDialogClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, CWnd* pParent /*=nullptr*/)
	: CDialog(SVSaveToolSetImageDialogClass::IDD, pParent)
	, m_ImageController(rInspectionID, rTaskObjectID, SVNotSetSubObjectType, true)
	{
		//{{AFX_DATA_INIT(SVSaveToolSetImageDialogClass)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}

	void SVSaveToolSetImageDialogClass::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVSaveToolSetImageDialogClass)
		DDX_Control(pDX, IDC_DIALOGIMAGE, m_currentSelectedImageCtrl);
		DDX_Control(pDX, IDC_AVAILABLE_IMAGES_COMBO, m_availableImagesComboCtrl);
		//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(SVSaveToolSetImageDialogClass, CDialog)
		//{{AFX_MSG_MAP(SVSaveToolSetImageDialogClass)
		ON_BN_CLICKED(ID_SAVE_BUTTON, OnSaveButton)
		ON_CBN_SELCHANGE(IDC_AVAILABLE_IMAGES_COMBO, OnSelchangeAvailableImagesCombo)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVSaveToolSetImageDialogClass message handlers

	BOOL SVSaveToolSetImageDialogClass::OnInitDialog() 
	{
		CDialog::OnInitDialog();
		m_ImageController.Init();

		const SvUl::NameGuidList& availImages = m_ImageController.GetAvailableImageList();
		for (SvUl::NameGuidList::const_iterator it = availImages.begin();it != availImages.end();++it)
		{
			m_availableImagesComboCtrl.AddString(it->first.c_str());
		}
		m_availableImagesComboCtrl.SetCurSel(0);
		m_currentSelectedImageCtrl.AddTab(_T("Image")); 
		setImages();

		UpdateData( FALSE );
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVSaveToolSetImageDialogClass::OnSelchangeAvailableImagesCombo() 
	{
		UpdateData(true);	

		setImages();

		UpdateData(false);
	}

	void SVSaveToolSetImageDialogClass::OnSaveButton() 
	{
		UpdateData(true);

		int index = m_availableImagesComboCtrl.GetCurSel();
		if (index != CB_ERR)
		{
			CString imageName;
			m_availableImagesComboCtrl.GetLBText(index, imageName);
			if (!imageName.IsEmpty())
			{
				SVFileNameClass	svfncImageFile;
			
				SVString Path = AfxGetApp()->GetProfileString(	_T("Settings"), _T("ImagesFilePath"), _T("C:\\Images"));	// Default
			
				svfncImageFile.SetFileType(SV_IMAGE_SOURCE_FILE_TYPE);
				svfncImageFile.SetPathName( Path.c_str() );
			
				bool bResult = svfncImageFile.SaveFile(); // Show Save File Dialog
			
				if (bResult)
				{
					Path = svfncImageFile.GetPathName();
					AfxGetApp()->WriteProfileString(_T("Settings"), _T("ImagesFilePath"), Path.c_str());
				
					SVString PathFileName = svfncImageFile.GetFullFileName();
					HRESULT hr = m_ImageController.SaveImage(SVString(imageName), PathFileName);
					if (S_OK != hr)
					{
						if (E_INVALIDARG == hr)
						{
							SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
							Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_UnKnownFileFormat, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10071 );
						}
						else
						{
							SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
							Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_FailedToSaveImage, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10072 );
						}
					}
				}
			}
		}	
	}

	void SVSaveToolSetImageDialogClass::setImages()
	{
		int index = m_availableImagesComboCtrl.GetCurSel();

		// Display current selected image...
		if (index != CB_ERR)
		{
			CString Name;
			m_availableImagesComboCtrl.GetLBText(index, Name);
		
			IPictureDisp* pImage = m_ImageController.GetImage( SVString(Name) );
			m_currentSelectedImageCtrl.setImage(pImage);
			m_currentSelectedImageCtrl.Refresh();
		}
	}
} //namespace SvOg

