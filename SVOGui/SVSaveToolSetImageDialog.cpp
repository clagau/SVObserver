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
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVStatusLibrary/MessageManagerResource.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "TextDefinesSvOg.h"
#include "SVMessage/SVMessage.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace Seidenader { namespace SVOGui
{
	SVSaveToolSetImageDialogClass::SVSaveToolSetImageDialogClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, CWnd* pParent /*=NULL*/)
	: CDialog(SVSaveToolSetImageDialogClass::IDD, pParent)
	, m_ImageController(rInspectionID, rTaskObjectID)
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
			
				CString csPathPlusFileName = _T("");
				CString csPath = AfxGetApp()->GetProfileString(	_T("Settings"), _T("ImagesFilePath"), _T("C:\\Images"));	// Default
			
				svfncImageFile.SetFileType(SV_IMAGE_SOURCE_FILE_TYPE);
				svfncImageFile.SetPathName(csPath);
			
				BOOL bResult = svfncImageFile.SaveFile(); // Show Save File Dialog
			
				if (bResult)
				{
					csPath = svfncImageFile.GetPathName();
					AfxGetApp()->WriteProfileString(_T("Settings"), _T("ImagesFilePath"), csPath);
				
					csPathPlusFileName = svfncImageFile.GetFullFileName();
					HRESULT hr = m_ImageController.SaveImage(SVString(imageName), SVString(csPathPlusFileName));
					if (S_OK != hr)
					{
						if (E_INVALIDARG == hr)
						{
							SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
							Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_UnKnownFileFormat, StdMessageParams, SvOi::Err_10071 );
						}
						else
						{
							SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
							Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_FailedToSaveImage, StdMessageParams, SvOi::Err_10072 );
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
			CString name;
			m_availableImagesComboCtrl.GetLBText(index, name);
		
			IPictureDisp* pImage = m_ImageController.GetImage(SVString(name));
			m_currentSelectedImageCtrl.setImage(pImage);
			m_currentSelectedImageCtrl.Refresh();
		}
	}
} /* namespace SVOGui */ } /* namespace Seidenader */

