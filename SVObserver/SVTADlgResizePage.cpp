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
	ON_CBN_SELCHANGE(IDC_OVERSCAN_COMBO, OnAnyItemChanged)
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
	DDX_Control(pDX, IDC_OVERSCAN_COMBO, m_OverscanModeCombo);
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
		ResetAndFillOverscanModeComboBox();
		hr = SetupResizeImageControl();
	}

	if (S_OK != hr)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(hr, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 5015);
	}
	else
	{
		SvUl::NameObjectIdList namesAndIds = m_ImageController.GetResultImages();

		//we want to look just at the part of the dotted name after the last dot
		for (auto& rNameAndId : namesAndIds)
		{
			auto positionOfLastDot = rNameAndId.first.rfind(_T("."));
			if(positionOfLastDot != std::string::npos)
			{
				rNameAndId.first = rNameAndId.first.substr(positionOfLastDot + 1);
			}
		}

		m_ROIImageID = SvUl::FindObjectId(namesAndIds, SvUl::LoadStdString(IDS_OBJECTNAME_ROIIMAGE));
		assert(SvDef::InvalidObjectId != m_ROIImageID);
		if (SvDef::InvalidObjectId == m_ROIImageID)
		{//if the imagename we want still cannot be found we use the position in the list
			if (1 < namesAndIds.size())
			{
				m_ROIImageID = namesAndIds[1].second;
			}
		}
		m_OutputImageID = SvUl::FindObjectId(namesAndIds, SvUl::LoadStdString(IDS_OBJECTNAME_IMAGE1));
		assert(SvDef::InvalidObjectId != m_OutputImageID);
		if (SvDef::InvalidObjectId == m_OutputImageID)
		{//if the imagename we want still cannot be found we use the position in the list
			if (0 < namesAndIds.size())
			{
				m_OutputImageID = namesAndIds[0].second;
			}
		}
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
	PickValue(m_formatScaleEdit[ScaleFactorDimension::Width], IDS_OBJECTNAME_FORMAT_WIDTH_SCALE);
}

void SVTADlgResizePage::OnButtonFormatHeightFactor()
{
	PickValue(m_formatScaleEdit[ScaleFactorDimension::Height], IDS_OBJECTNAME_FORMAT_HEIGHT_SCALE);
}

void SVTADlgResizePage::OnButtonContentWidthFactor()
{
	PickValue(m_contentScaleEdit[ScaleFactorDimension::Width], IDS_OBJECTNAME_CONTENT_WIDTH_SCALE);
}

void SVTADlgResizePage::OnButtonContentHeightFactor()
{
	PickValue(m_contentScaleEdit[ScaleFactorDimension::Height], IDS_OBJECTNAME_CONTENT_HEIGHT_SCALE);
}

void SVTADlgResizePage::PickValue(CEdit& rEdit, UINT ResourceID)
{
	CString Temp;

	rEdit.GetWindowText(Temp);
	std::string Value = Temp.GetString();
	std::string Title = SvUl::LoadStdString(ResourceID);
	if (m_resizeValueSelector.Show(Value, Title, this, SvPb::allNumberValueObjects, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed, m_toolID))
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

void SVTADlgResizePage::ResetAndFillOverscanModeComboBox()
{
	m_OverscanModeNames.clear();
	m_OverscanModeCombo.ResetContent();

	for (auto name : SvDef::c_allUsedOverscanNames)
	{
		m_OverscanModeCombo.AddString(name);
		m_OverscanModeNames.emplace_back(name);
	}

	m_OverscanModeCombo.SetCurSel(0); //default: Enabled
}


void SVTADlgResizePage::GetAndDisplayValuesFromTool()
{
	auto interpolationMode = static_cast<InterpolationMode>(m_resizeValueController.Get<long>(SvPb::ResizeInterpolationModeEId));

	auto interpolationNameAndMode = std::find_if(c_interpolationNamesAndModes.begin(), c_interpolationNamesAndModes.end(),
		[interpolationMode](const std::pair<std::string, InterpolationMode>& rPair)
		{return rPair.second == interpolationMode; });

	if (interpolationNameAndMode != c_interpolationNamesAndModes.end())
	{
		auto pos = m_InterpolationModeCombo.FindStringExact(0, interpolationNameAndMode->first.c_str());
		if (pos > -1 && pos < m_InterpolationModeNames.size())
		{
			m_InterpolationModeCombo.SetCurSel(pos);
		}

	}

	auto overscanMode = static_cast<OverscanMode>(m_resizeValueController.Get<long>(SvPb::ResizeOverscanEId));

	auto overscanNameAndMode = std::find_if(c_overscanNamesAndModes.begin(), c_overscanNamesAndModes.end(),
		[overscanMode](const std::pair<std::string, OverscanMode>& rPair)
		{return rPair.second == overscanMode; });

	if (overscanNameAndMode != c_overscanNamesAndModes.end())
	{
		auto pos = m_OverscanModeCombo.FindStringExact(0, overscanNameAndMode->first.c_str());
		if (pos > -1 && pos < m_OverscanModeNames.size())
		{
			m_OverscanModeCombo.SetCurSel(pos);
		}

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

	auto modeandname = std::find_if(c_interpolationNamesAndModes.begin(), c_interpolationNamesAndModes.end(),
		[selectedInterpolation](const std::pair<std::string, InterpolationMode>& rPair)
		{return rPair.first == selectedInterpolation; });

	if (modeandname == c_interpolationNamesAndModes.end())
	{
		m_InterpolationModeCombo.SetCurSel(0); //default: auto
		m_selectedInterpolationMode = InterpolationMode::Default;
	}
	else
	{
		m_selectedInterpolationMode = modeandname->second;
	}
}

void SVTADlgResizePage::getOverscanModeFromDialog()
{
	CString OverscanModeName;
	m_OverscanModeCombo.GetLBText(m_OverscanModeCombo.GetCurSel(), OverscanModeName);

	std::string selectedOverscan(OverscanModeName.GetBuffer(0));

	auto modeandname = std::find_if(c_overscanNamesAndModes.begin(), c_overscanNamesAndModes.end(),
		[selectedOverscan](const std::pair<std::string, OverscanMode>& rPair)
		{return rPair.first == selectedOverscan; });

	if (modeandname == c_overscanNamesAndModes.end())
	{
		m_OverscanModeCombo.SetCurSel(0); //default: enable
		m_selectedOverscanMode = OverscanMode::OverscanEnable;
	}
	else
	{
		m_selectedOverscanMode = modeandname->second;
	}
}

bool SVTADlgResizePage::CommitAndCheckNewParameterValues()
{
	CommitValuesFromDialog();

#if defined (TRACE_THEM_ALL) || defined (TRACE_RESIZE)

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
	m_resizeValueController.Set<long>(SvPb::ResizeOverscanEId, m_selectedOverscanMode);

	for (SvOg::ValueEditWidgetHelper& rEditHelper : m_allEditHelpers)
	{
		rEditHelper.EditboxToValue();
	}

	m_resizeValueController.Commit();

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
	assert(SvDef::InvalidObjectId != m_ROIImageID && SvDef::InvalidObjectId != m_OutputImageID);

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
	getOverscanModeFromDialog();

	CommitAndCheckNewParameterValues();
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


