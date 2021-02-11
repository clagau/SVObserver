//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015, 2020 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// SVTADlgResizePage.cpp
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <limits>
#include "SVOResource\resource.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVTADlgResizePage.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVMatroxLibrary/SVMatroxEnums.h"
#pragma endregion Includes


#pragma region Message Map
BEGIN_MESSAGE_MAP(SVTADlgResizePage, CPropertyPage)
	ON_EN_KILLFOCUS(IDC_EDIT_CONTENT_WIDTH_FACTOR, OnAnyItemChanged)
	ON_EN_KILLFOCUS(IDC_EDIT_CONTENT_HEIGHT_FACTOR, OnAnyItemChanged)
	ON_EN_KILLFOCUS(IDC_EDIT_FORMAT_WIDTH_FACTOR, OnAnyItemChanged)
	ON_EN_KILLFOCUS(IDC_EDIT_FORMAT_HEIGHT_FACTOR, OnAnyItemChanged)
	ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnAnyItemChanged)
	ON_BN_CLICKED(IDC_BUTTON_CONTENT_WIDTH_FACTOR, OnButtonContentWidthFactor)
	ON_BN_CLICKED(IDC_BUTTON_CONTENT_HEIGHT_FACTOR, OnButtonContentHeightFactor)
	ON_BN_CLICKED(IDC_BUTTON_FORMAT_WIDTH_FACTOR, OnButtonFormatWidthFactor)
	ON_BN_CLICKED(IDC_BUTTON_FORMAT_HEIGHT_FACTOR, OnButtonFormatHeightFactor)

END_MESSAGE_MAP()
#pragma endregion Message Map

const std::vector<SvPb::EmbeddedIdEnum> SVTADlgResizePage::ms_allScaleFactorEIDs{
		SvPb::ExtentWidthFactorContentLinkEId,
		SvPb::ExtentWidthFactorContentEId,
		SvPb::ExtentHeightFactorContentLinkEId,
		SvPb::ExtentHeightFactorContentEId,
		SvPb::ExtentWidthFactorFormatLinkEId,
		SvPb::ExtentWidthFactorFormatEId,
		SvPb::ExtentHeightFactorFormatLinkEId,
		SvPb::ExtentHeightFactorFormatEId
};


SVTADlgResizePage::SVTADlgResizePage(uint32_t inspectionID, uint32_t taskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id)
	: CPropertyPage(id)
	, m_ParentDialog(Parent)
	, m_ROITabHandle(0)
	, m_OutputTabHandle(0)
	, m_ImageController(inspectionID, taskObjectID)
	, m_inspectionID(inspectionID)
	, m_toolID(taskObjectID)
	, m_resizeValueController{ SvOg::BoundValues{ inspectionID, taskObjectID } }
	, m_formatWidthHelper(m_formatScaleEdit[ScaleFactorDimension::Width],
		SvPb::ExtentWidthFactorFormatEId,
		&(m_formatScaleButton[ScaleFactorDimension::Width]),
		SvPb::ExtentWidthFactorFormatLinkEId,
		m_resizeValueController)
	, m_formatHeightHelper(m_formatScaleEdit[ScaleFactorDimension::Height],
		SvPb::ExtentHeightFactorFormatEId,
		&(m_formatScaleButton[ScaleFactorDimension::Height]),
		SvPb::ExtentHeightFactorFormatLinkEId,
		m_resizeValueController)
	, m_resizeValueSelector(inspectionID, SvDef::InvalidObjectId, SvPb::viewable)
	, m_contentWidthHelper(m_contentScaleEdit[ScaleFactorDimension::Width],
		SvPb::ExtentWidthFactorContentEId,
		&(m_contentScaleButton[ScaleFactorDimension::Width]),
		SvPb::ExtentWidthFactorContentLinkEId,
		m_resizeValueController)
	, m_contentHeightHelper(m_contentScaleEdit[ScaleFactorDimension::Height],
		SvPb::ExtentHeightFactorContentEId,
		&(m_contentScaleButton[ScaleFactorDimension::Height]),
		SvPb::ExtentHeightFactorContentLinkEId,
		m_resizeValueController),
	m_allEditHelpers{
		std::ref(m_contentWidthHelper), std::ref(m_contentHeightHelper),
		std::ref(m_formatWidthHelper), std::ref(m_formatHeightHelper)}
{
	SvOg::ValueEditWidgetHelper::EnsureDownArrowBitmapIsLoaded();

	m_resizeValueController.Init();
}

SVTADlgResizePage::~SVTADlgResizePage()
{
}

void SVTADlgResizePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SVTADlgResizePage)
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_DialogImage);
	DDX_Control(pDX, IDC_EDIT_CONTENT_WIDTH_FACTOR, m_contentScaleEdit[ScaleFactorDimension::Width]);
	DDX_Control(pDX, IDC_EDIT_CONTENT_HEIGHT_FACTOR, m_contentScaleEdit[ScaleFactorDimension::Height]);
	DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_InterpolationModeCombo);
	DDX_Control(pDX, IDC_EDIT_FORMAT_WIDTH_FACTOR, m_formatScaleEdit[ScaleFactorDimension::Width]);
	DDX_Control(pDX, IDC_EDIT_FORMAT_HEIGHT_FACTOR, m_formatScaleEdit[ScaleFactorDimension::Height]);
	DDX_Control(pDX, IDC_BUTTON_FORMAT_WIDTH_FACTOR, m_contentScaleButton[ScaleFactorDimension::Width]);
	DDX_Control(pDX, IDC_BUTTON_FORMAT_HEIGHT_FACTOR, m_contentScaleButton[ScaleFactorDimension::Height]);
	DDX_Control(pDX, IDC_BUTTON_CONTENT_WIDTH_FACTOR, m_formatScaleButton[ScaleFactorDimension::Width]);
	DDX_Control(pDX, IDC_BUTTON_CONTENT_HEIGHT_FACTOR, m_formatScaleButton[ScaleFactorDimension::Height]);
	
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// SVTADlgResizePage message handlers

BOOL SVTADlgResizePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	for (const auto pair : c_allInterpolationModesNameValuePairs)
	{
		m_InterpolationModeByModeName[pair.first] = pair.second;
		m_InterpolationModeNameByMode[pair.second] = pair.first;
	}

	HRESULT hr = S_OK;
	m_ImageController.Init();
	
	if (nullptr == m_ParentDialog)
	{
		//	This should not be able to happen.
		hr = SVMSG_SVO_5001_NULLPARENT;
	}
	else
	{
		ResetAndFillInterpolationModeComboBox();
		hr = SetupResizeImageControl();
	}

	if (S_OK != hr)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(hr, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 5015);
	}
	else
	{
		const SvUl::NameObjectIdList& rImageList = m_ImageController.GetResultImages();

		std::string toolName = GetToolname();
		std::string tmpString = toolName + _T(".") + SvUl::LoadStdString(IDS_OBJECTNAME_ROIIMAGE);
		m_ROIImageID = SvUl::FindObjectId(rImageList, tmpString);
		tmpString = toolName + _T(".") + SvUl::LoadStdString(IDS_OBJECTNAME_IMAGE1);
		m_OutputImageID = SvUl::FindObjectId(rImageList, tmpString);
	}

	GetAndDisplayValuesFromTool();

	UpdateData(FALSE); // dialog being initialized.

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL SVTADlgResizePage::OnSetActive()
{
	GetAndDisplayValuesFromTool();
	UpdateImages();
	return CPropertyPage::OnSetActive();
}

void SVTADlgResizePage::OnButtonFormatWidthFactor()
{
	PickValue(m_formatScaleButton[ScaleFactorDimension::Width], m_formatScaleEdit[ScaleFactorDimension::Width], IDS_OBJECTNAME_FORMAT_WIDTH_SCALE);
}

void SVTADlgResizePage::OnButtonFormatHeightFactor()
{
	PickValue(m_formatScaleButton[ScaleFactorDimension::Height], m_formatScaleEdit[ScaleFactorDimension::Height], IDS_OBJECTNAME_FORMAT_HEIGHT_SCALE);
}

void SVTADlgResizePage::OnButtonContentWidthFactor()
{
	PickValue(m_contentScaleButton[ScaleFactorDimension::Width], m_contentScaleEdit[ScaleFactorDimension::Width], IDS_OBJECTNAME_CONTENT_WIDTH_SCALE);
}

void SVTADlgResizePage::OnButtonContentHeightFactor()
{
	PickValue(m_contentScaleButton[ScaleFactorDimension::Height], m_contentScaleEdit[ScaleFactorDimension::Height], IDS_OBJECTNAME_CONTENT_HEIGHT_SCALE);
}

void SVTADlgResizePage::PickValue(CButton& rButton, CEdit& rEdit, UINT ResourceID)
{
	CString Temp;

	rButton.GetWindowText(Temp);
	std::string Value = Temp;
	std::string Title = SvUl::LoadStdString(ResourceID);
	if (m_resizeValueSelector.Show(Value, Title, this, SvPb::allValueObjects, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed, m_toolID))
	{
		rEdit.SetWindowText(Value.c_str());
	}

	OnAnyItemChanged();
}

void SVTADlgResizePage::ResetAndFillInterpolationModeComboBox()
{
	m_InterpolationModeNames.clear();

	m_InterpolationModeCombo.ResetContent();
	
	for (auto modename : SvDef::c_allInterpolationModeNames)
	{
		m_InterpolationModeCombo.AddString(modename);
		m_InterpolationModeNames.emplace_back(modename);
	}

	m_InterpolationModeCombo.SetCurSel(0); //default: auto
}

void SVTADlgResizePage::GetAndDisplayValuesFromTool()
{
	auto mode = static_cast<InterpolationMode>(m_resizeValueController.Get<long>(SvPb::ResizeInterpolationModeEId));
	auto modestring = m_InterpolationModeNameByMode[mode];

	auto pos = m_InterpolationModeCombo.FindStringExact(0, modestring.c_str());
	if (pos > -1 && pos < m_InterpolationModeNames.size())
	{
		m_InterpolationModeCombo.SetCurSel(pos);
	}

	for (SvOg::ValueEditWidgetHelper& rEditHelper : m_allEditHelpers)
	{
		rEditHelper.ValueToEditbox();
	}
}

void SVTADlgResizePage::getInterpolationModeFromDialog()
{
	CString interpolationModeName;
	m_InterpolationModeCombo.GetLBText(m_InterpolationModeCombo.GetCurSel(), interpolationModeName);

	std::string selectedInterpolation(interpolationModeName.GetBuffer(0));

	if (m_InterpolationModeByModeName.find(selectedInterpolation) == m_InterpolationModeByModeName.end())
	{
		m_InterpolationModeCombo.SetCurSel(0); //default: auto
		m_selectedInterpolationMode = InterpolationMode::Default;
	}
	else
	{
		m_selectedInterpolationMode = m_InterpolationModeByModeName[selectedInterpolation];
	}
}

bool SVTADlgResizePage::CommitAndCheckNewParameterValues()
{
	CommitValuesFromDialog();

#if defined (TRACE_THEM_ALL) || defined (TRACE_RESIZE)

	m_resizeValueController.Init();
	traceScalefactorValues("Committed");

#endif 


	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_resetallobjectsrequest();
	pRequest->set_objectid(m_toolID);
	auto hResult = SvCmd::InspectionCommands(m_inspectionID, requestCmd, &responseCmd);

	if (S_OK != hResult)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidScaleFactor, SvStl::SourceFileParams(StdMessageParams), 0, m_toolID);
		return false;
	}
	if (S_OK != SvCmd::RunOnceSynchronous(m_inspectionID))
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_CannotRunOnce, SvStl::SourceFileParams(StdMessageParams), 0, m_toolID);
		return false;
	}

	UpdateImages();

	return true;
}


void SVTADlgResizePage::CommitValuesFromDialog()
{
	m_resizeValueController.Set<long>(SvPb::ResizeInterpolationModeEId, m_selectedInterpolationMode);

	for (SvOg::ValueEditWidgetHelper& rEditHelper : m_allEditHelpers)
	{
		rEditHelper.EditboxToValue();
	}

	m_resizeValueController.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);

}

HRESULT SVTADlgResizePage::SetupResizeImageControl()
{
	HRESULT	hr = S_OK;

	if (S_OK != m_DialogImage.AddTab(_T("Source ROI"), &m_ROITabHandle))
	{
		hr = SVMSG_SVO_5016_COULDNOTADDTAB;
	}
	else
	{
		if (S_OK != m_DialogImage.AddTab(_T("Tool Result"), &m_OutputTabHandle))
		{
			hr = SVMSG_SVO_5017_COULDNOTADDTAB;
		}
		else
		{
			UpdateImages();
		}
	}
	return hr;
}

void SVTADlgResizePage::UpdateImages()
{
	if (SvDef::InvalidObjectId != m_ROIImageID && SvDef::InvalidObjectId != m_OutputImageID)
	{
		IPictureDisp* pROIImage = m_ImageController.GetImage(m_ROIImageID);
		IPictureDisp* pOutputImage = m_ImageController.GetImage(m_OutputImageID);
		m_DialogImage.setImage(pROIImage, m_ROITabHandle);
		m_DialogImage.setImage(pOutputImage, m_OutputTabHandle);
	}
	m_DialogImage.Refresh();
}

void SVTADlgResizePage::OnAnyItemChanged()
{
	getInterpolationModeFromDialog();

	CommitAndCheckNewParameterValues();
}

std::string SVTADlgResizePage::GetToolname() const
{
	std::string inspectionName;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
	pRequest->set_objectid(m_toolID);

	HRESULT hr = SvCmd::InspectionCommands(m_inspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
	{
		inspectionName = responseCmd.getobjectparametersresponse().name();
	}
	return inspectionName;
}


void SVTADlgResizePage::traceScalefactorValues(const std::string &rHeading, bool alsoAsDouble) const
{
		std::stringstream traceStream;

		traceStream << rHeading << ":" << std::endl;

		bool isLink = true;

		for (SvPb::EmbeddedIdEnum eid: ms_allScaleFactorEIDs)
		{
			auto temp = m_resizeValueController.Get<CString>(eid);

			traceStream << "\t" << eid << ": '" << temp.GetBuffer(0) << "'";

			if (isLink) 
			{
				traceStream << " -> ";
			}
			else
			{
				if (alsoAsDouble)
				{
					auto temp_d = m_resizeValueController.Get<double>(eid);
					traceStream << "/" << temp_d;
				}
				traceStream << std::endl;
			}
			isLink = !isLink;
		}
		OutputDebugString(traceStream.str().c_str());
}


