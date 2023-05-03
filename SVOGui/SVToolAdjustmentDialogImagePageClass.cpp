//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogImagePageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogImagePageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAdjustmentDialogImagePageClass.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
static LPCTSTR ImageTag = _T("Image");
static LPCTSTR NoImageTag = _T("(No Image Available)");

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogImagePageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogImagePageClass)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolAdjustmentDialogImagePageClass::SVToolAdjustmentDialogImagePageClass(uint32_t inspectionId, uint32_t taskObjectId, SvPb::SVObjectSubTypeEnum SubType /*= SvPb::SVImageMonoType*/, int id /*=IDD*/)
	: CPropertyPage(id)
	, m_ImageController {inspectionId, taskObjectId, SubType}
	, m_ToolsizeHelper(inspectionId, taskObjectId)
{
}

SVToolAdjustmentDialogImagePageClass::~SVToolAdjustmentDialogImagePageClass()
{}

HRESULT SVToolAdjustmentDialogImagePageClass::SetInspectionData()
{
	UpdateData(true); // get data from dialog

	HRESULT hr = m_ImageController.ToolRunOnce();

	UpdateData(false);

	return hr;
}

void SVToolAdjustmentDialogImagePageClass::refresh()
{
	SetInspectionData();
	m_dialogImage.Refresh();
}

void SVToolAdjustmentDialogImagePageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogImagePageClass)
	DDX_Control(pDX, IDC_COMBO1, m_availableSourceImageListBox);
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
	//}}AFX_DATA_MAP
}

BOOL SVToolAdjustmentDialogImagePageClass::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_ToolsizeHelper.InitValues();
	m_ImageController.Init();
	const SvUl::NameObjectIdList& rAvailableImageList = m_ImageController.GetAvailableImageList();
	auto selectedImageName = m_ImageController.GetInputData(SvPb::ImageInputEId).connected_objectdottedname();

	m_availableSourceImageListBox.Init(rAvailableImageList, selectedImageName, NoImageTag);
	m_dialogImage.AddTab(ImageTag);
	m_dialogImage.setImage(m_ImageController.GetImage(selectedImageName));
	m_dialogImage.Refresh();

	UpdateData(false); // set data to dialog

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVToolAdjustmentDialogImagePageClass::OnSelchangeCombo1()
{
	UpdateData(TRUE); // get data from dialog

	int index = m_availableSourceImageListBox.GetCurSel();
	if (LB_ERR != index)
	{
		bool bIsValid = m_ImageController.IsToolValid();
		CString imageName;
		m_availableSourceImageListBox.GetLBText(index, imageName);
		if (!imageName.IsEmpty() && imageName != NoImageTag)
		{
			std::string svImageName(imageName);
			//setImage must be before ConnectToImage because ConnectToImage does a reset and then it cannot get the image.
			IPictureDisp* pImage = m_ImageController.GetImage(svImageName);
			m_dialogImage.setImage(pImage);
			m_ImageController.ConnectToImage(SvPb::ImageInputEId, svImageName);
			refresh();
			SvStl::MessageContainerVector errorMessages;
			HRESULT result = m_ImageController.ResetTask(errorMessages);

			if (bIsValid && S_OK != result)
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);



				if (m_ToolsizeHelper.CanResizeToParent())
				{
					auto answer = Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_ToolPositionError_Three_Button, SvStl::SourceFileParams(StdMessageParams), SvDef::InvalidObjectId, MB_YESNOCANCEL);
					switch (answer)
					{
						case IDNO:
						{
							m_ToolsizeHelper.SetExtentToParent();
							m_ToolsizeHelper.InitValues();
							break;
						}
						case IDYES:
						{
							m_ToolsizeHelper.SetAllToolSizeMode(SvDef::TSAutoFit,false);
							m_ToolsizeHelper.InitValues();
							break;
						}
						default:
							break;
					}
				}
				else
				{
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_ToolPositionError, SvStl::SourceFileParams(StdMessageParams));
				}

			}

		}
	}
}
} //namespace SvOg

