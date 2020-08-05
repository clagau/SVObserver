//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2General.h
/// This is the class for the general tab of Blob Analyzer 2
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOResource/resource.h"
#include "BlobAnalyzer2General.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "InspectionCommands/CommandExternalHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	static LPCTSTR NoImageTag = _T("(No Image Available)");

	BEGIN_MESSAGE_MAP(BlobAnalyzer2General, CPropertyPage)
		//{{AFX_MSG_MAP(BlobAnalyzer2General)
		ON_BN_CLICKED(IDC_CHECK1, OnCheckGrayImage)
		ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	BlobAnalyzer2General::BlobAnalyzer2General(uint32_t inspectionID, uint32_t toolId, uint32_t taskObjectID)
		: CPropertyPage(BlobAnalyzer2General::IDD)
		, m_AnalyzerImageController(inspectionID, taskObjectID)
		, m_ToolImageController(inspectionID, toolId)
		, m_InspectionID(inspectionID)
		, m_toolId(toolId)
		, m_TaskObjectID(taskObjectID)
		, m_Values{ SvOg::BoundValues{ inspectionID, taskObjectID } }
	{
	}

	BlobAnalyzer2General::~BlobAnalyzer2General()
	{
	}

	HRESULT BlobAnalyzer2General::setInspectionData()
	{
		UpdateData(TRUE); // get data from dialog

		m_Values.Set<long>(SvPb::BlobColorEId, m_blobColor);
		m_Values.Set<long>(SvPb::ConnectivityBlobEId, m_connectivityType);
		m_Values.Set<long>(SvPb::MaxBlobDataArraySizeEId, m_maxBlobArraySize);
		m_Values.Set<bool>(SvPb::IsGrayImageEId, (TRUE == m_useGrayImage));

		return m_Values.Commit(PostAction::doReset | PostAction::doRunOnce);
	}

	void BlobAnalyzer2General::setImages()
	{
		IPictureDisp* pResultImage = m_ToolImageController.GetImage(getFirstResultImageId(m_ToolImageController));
		m_resultImage.setImage(pResultImage);
		m_resultImage.Refresh();

		IPictureDisp* pImage = m_AnalyzerImageController.GetImage(m_selectedGrayName);
		m_grayImage.setImage(pImage);
		m_grayImage.Refresh();
	}

	void BlobAnalyzer2General::refresh()
	{
		// Refresh dialog image...
		setImages();
	}


	void BlobAnalyzer2General::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVToolAdjustmentDialogFilterPageClass)
		DDX_Text(pDX, IDC_EDIT_MAX_BLOB_DATA_ARRAY_SIZE, m_maxBlobArraySize);
		DDV_MinMaxLong(pDX, m_maxBlobArraySize, 1, 10000);
		DDX_Control(pDX, IDC_FIRST_IMAGE, m_resultImage);
		DDX_Control(pDX, IDC_SECOND_IMAGE, m_grayImage);
		DDX_Control(pDX, IDC_COMBO1, m_availableGrayImageListBox);
		DDX_Radio(pDX, IDC_BLACK_RADIO, m_blobColor);
		DDX_Radio(pDX, IDC_4_RADIO, m_connectivityType);
		DDX_Check(pDX, IDC_CHECK1, m_useGrayImage);
		//}}AFX_DATA_MAP
	}

	BOOL BlobAnalyzer2General::OnSetActive()
	{
		setImages();
		return CPropertyPage::OnSetActive();
	}

	void BlobAnalyzer2General::OnOK()
	{
		/*HRESULT hResult = */setInspectionData();
		__super::OnOK();
	}

	BOOL BlobAnalyzer2General::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		m_resultImage.AddTab(_T("Result Image"));
		m_grayImage.AddTab(_T("Blob Identifier Image"));
		m_ToolImageController.Init();
		m_AnalyzerImageController.Init();
		
		m_Values.Init();
		m_blobColor = m_Values.Get<long>(SvPb::BlobColorEId);
		m_connectivityType = m_Values.Get<long>(SvPb::ConnectivityBlobEId);
		m_maxBlobArraySize = m_Values.Get<long>(SvPb::MaxBlobDataArraySizeEId);
		m_useGrayImage = m_Values.Get<bool>(SvPb::IsGrayImageEId);

		const SvUl::NameObjectIdList& rAvailableImageList = m_AnalyzerImageController.GetAvailableImageList();
		const SvUl::InputNameObjectIdPairList& rImageList = m_AnalyzerImageController.GetInputImageList(SvDef::InvalidObjectId, 3);
		if (rImageList.size())
		{
			m_inputGrayName = rImageList.begin()->first;
			m_selectedGrayName = rImageList.begin()->second.first;
		}
		else
		{
			m_selectedGrayName.clear();
		}

		m_availableGrayImageListBox.Init(rAvailableImageList, m_selectedGrayName, NoImageTag);
		setImages();
		
		UpdateData(FALSE);
		OnCheckGrayImage();

		refresh();
		return TRUE;
		// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	void BlobAnalyzer2General::OnSelchangeCombo1()
	{
		UpdateData(TRUE); // get data from dialog

		int index = m_availableGrayImageListBox.GetCurSel();
		if (LB_ERR != index)
		{
			bool bIsValid = m_AnalyzerImageController.IsToolValid();
			CString imageName;
			m_availableGrayImageListBox.GetLBText(index, imageName);
			if (!imageName.IsEmpty() && imageName != NoImageTag)
			{
				m_selectedGrayName = imageName;
				//setImage must be before ConnectToImage because ConnectToImage does a reset and then it cannot get the image.
				setImages();
				m_AnalyzerImageController.ConnectToImage(m_inputGrayName, m_selectedGrayName);
				refresh();
				SvStl::MessageContainerVector errorMessages;
				HRESULT result = m_AnalyzerImageController.ResetTask(errorMessages);

				if (bIsValid && S_OK != result)
				{
					SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_ToolPositionError, SvStl::SourceFileParams(StdMessageParams), 0);
				}
			}
		}
	}

	void BlobAnalyzer2General::OnCheckGrayImage()
	{
		UpdateData(true); // get data of dialog

		if (!m_useGrayImage)
		{
			bool redoFlag = checkIfGrayFeatureUsedAndRemoveThem();
			if (redoFlag)
			{
				m_useGrayImage = true;
				UpdateData(false);
				return;
			}
		}

		CWnd* pWnd(GetDlgItem(IDC_COMBO1));
		if (nullptr != pWnd)
		{
			pWnd->EnableWindow(m_useGrayImage);
		}
		pWnd = GetDlgItem(IDC_SECOND_IMAGE);
		if (nullptr != pWnd)
		{
			pWnd->ShowWindow(m_useGrayImage? SW_SHOW : SW_HIDE);
		}
	}

	bool BlobAnalyzer2General::checkIfGrayFeatureUsedAndRemoveThem()
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getfeaturesrequest();
		pRequest->set_objectid(m_TaskObjectID);

		HRESULT hResult = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (hResult == S_OK)
		{
			if (responseCmd.has_getfeaturesresponse())
			{
				auto featureData = responseCmd.getfeaturesresponse().list();
				bool isGrayFeature = std::any_of(featureData.begin(), featureData.end(), [](const auto& rEntry) { return rEntry.is_gray(); });
				if (isGrayFeature)
				{
					int iRet = MessageBox(_T("If you disable gray image, all gray feature will be removed. Do you want this?"), NULL, MB_YESNO);
					if (IDYES == iRet)
					{
						//Remove all gray feature
						featureData.erase(std::remove_if(featureData.begin(), featureData.end(), [](const auto& rEntry) {return rEntry.is_gray(); }), featureData.end());
						auto* pRequestSet = requestCmd.mutable_setfeaturesrequest();
						pRequestSet->set_objectid(m_TaskObjectID);
						*pRequestSet->mutable_list() = featureData;

						hResult = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
						assert(hResult == S_OK && responseCmd.has_setfeaturesresponse());
						return false;
					}
					else
					{
						return true;
					}
				}
			}
			else
			{
				hResult = E_FAIL;
			}
		}
		assert(S_OK == hResult);
		return false;
	}
}  //namespace SvOg
