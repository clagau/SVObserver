//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgColorToolPage
//* .File Name       : $Workfile:   TADlgColorToolPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgColorToolPage.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	static LPCTSTR cImageTag = _T("Tool Input");
	static LPCTSTR cNoImageTag = _T("(No Image Available)");

	BEGIN_MESSAGE_MAP(TADlgColorToolPage, CPropertyPage)
		//{{AFX_MSG_MAP(TADlgColorToolPageClass)
		ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
		ON_BN_CLICKED(IDC_CONVERT_TO_HSI, OnConvertToHsi)
		ON_BN_CLICKED(IDC_TRAIN_COLOR, OnTrainColor)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	TADlgColorToolPage::TADlgColorToolPage( uint32_t inspectionId, uint32_t taskObjectId ) 
	: CPropertyPage(TADlgColorToolPage::IDD)
	, m_Images(inspectionId, taskObjectId, SvPb::SVImageColorType)
	, m_InspectionID(inspectionId)
	, m_TaskObjectID(taskObjectId)
	, m_values{ SvOgu::BoundValues{ inspectionId, taskObjectId } }
	, m_convertToHSI{ false }
	{
	}

	TADlgColorToolPage::~TADlgColorToolPage()
	{
	}

	HRESULT TADlgColorToolPage::SetInspectionData()
	{
		HRESULT Result{ E_FAIL };
		UpdateData( true ); // get data from dialog

		m_values.Set<bool>(SvPb::ConvertToHSIEId, m_convertToHSI ? true : false);
		Result = m_values.Commit(SvOgu::PostAction::doReset | SvOgu::PostAction::doRunOnce);

		UpdateData( false );
		return Result;
	}

	void TADlgColorToolPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TADlgColorToolPageClass)
		DDX_Control(pDX, IDC_COMBO1, m_availableSourceImageListBox);
		DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
		DDX_Check(pDX, IDC_CONVERT_TO_HSI, m_convertToHSI);
		//}}AFX_DATA_MAP
	}

	BOOL TADlgColorToolPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		m_Images.Init();
		m_values.Init();
		m_ResultImageID = getFirstResultImageId(m_Images);

		m_convertToHSI = m_values.Get<bool>(SvPb::ConvertToHSIEId);

		const SvUl::NameObjectIdList& rAvailableImageList = m_Images.GetAvailableImageList();
		m_availableSourceImageListBox.Init(rAvailableImageList, m_Images.GetInputData(SvPb::ImageInputEId).connected_objectdottedname(), cNoImageTag);
		m_dialogImage.AddTab(cImageTag);
		SetImage();
		UpdateData( false );

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
	}

	void TADlgColorToolPage::OnSelchangeCombo1()
	{
		UpdateData(true);

		int index = m_availableSourceImageListBox.GetCurSel();
		if (LB_ERR != index)
		{
			CString Text;
			m_availableSourceImageListBox.GetLBText(index, Text);
			std::string ImageName(Text);
			if (!ImageName.empty() && cNoImageTag !=  ImageName)
			{
				m_Images.ConnectToImage(SvPb::ImageInputEId, ImageName);
				SvStl::MessageContainerVector errorMessages;
				HRESULT result = m_Images.ResetTask(errorMessages);
				m_Images.ToolRunOnce();
				SetImage();
				if (S_OK != result)
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_ToolPositionError, SvStl::SourceFileParams(StdMessageParams));
				}
			}
		}
	}

	void TADlgColorToolPage::OnConvertToHsi()
	{
		SetInspectionData();
		SetImage();
	}

	void TADlgColorToolPage::OnTrainColor() 
	{
		SvOi::IObjectClass* pObject = SvOi::getObject(m_TaskObjectID);
		if (nullptr != pObject)
		{
			SvOi::SetupDialogManager(pObject->GetClassID(), m_InspectionID, m_TaskObjectID, GetSafeHwnd());
		}
	}

	void TADlgColorToolPage::SetImage()
	{
		IPictureDisp* pImage = m_Images.GetImage(m_ResultImageID);
		m_dialogImage.setImage(pImage);
		m_dialogImage.Refresh();
	}

} //namespace SvOg
