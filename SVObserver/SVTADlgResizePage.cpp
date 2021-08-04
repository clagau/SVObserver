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
#include "SVOGui/LinkedValueWidgetHelper.h"
#pragma endregion Includes


#pragma region Message Map
BEGIN_MESSAGE_MAP(SVTADlgResizePage, CPropertyPage)
	ON_EN_KILLFOCUS(IDC_EDIT_CONTENT_WIDTH_FACTOR, OnContentWidthFactorChanged)
	ON_EN_KILLFOCUS(IDC_EDIT_CONTENT_HEIGHT_FACTOR, OnContentHeightFactorChanged)
	ON_EN_KILLFOCUS(IDC_EDIT_FORMAT_WIDTH_FACTOR, OnFormatWidthFactorChanged)
	ON_EN_KILLFOCUS(IDC_EDIT_FORMAT_HEIGHT_FACTOR, OnFormatHeightFactorChanged)
	ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnInterpolationModeChanged)
	ON_CBN_SELCHANGE(IDC_OVERSCAN_COMBO, OnOverscanChanged)
	ON_BN_CLICKED(IDC_BUTTON_CONTENT_WIDTH_FACTOR, OnButtonContentWidthFactor)
	ON_BN_CLICKED(IDC_BUTTON_CONTENT_HEIGHT_FACTOR, OnButtonContentHeightFactor)
	ON_BN_CLICKED(IDC_BUTTON_FORMAT_WIDTH_FACTOR, OnButtonFormatWidthFactor)
	ON_BN_CLICKED(IDC_BUTTON_FORMAT_HEIGHT_FACTOR, OnButtonFormatHeightFactor)

END_MESSAGE_MAP()
#pragma endregion Message Map

SVTADlgResizePage::SVTADlgResizePage(uint32_t inspectionID, uint32_t taskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id)
	: CPropertyPage(id)
	, m_ParentDialog(Parent)
	, m_ROITabHandle(0)
	, m_OutputTabHandle(0)
	, m_ImageController(inspectionID, taskObjectID)
	, m_inspectionID(inspectionID)
	, m_toolID(taskObjectID)
	, m_resizeValueController{ SvOg::BoundValues{ inspectionID, taskObjectID } }
{
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

	SvOg::ObjectSelectorData osData;
	osData.m_attribute = SvPb::viewable;

	m_contentScaleWidgets[0] = std::make_unique<SvOg::LinkedValueWidgetHelper>(m_contentScaleEdit[0], m_contentScaleButton[0], m_inspectionID, m_toolID, SvPb::ExtentWidthFactorContentEId, &m_resizeValueController, osData);
	m_contentScaleWidgets[1] = std::make_unique<SvOg::LinkedValueWidgetHelper>(m_contentScaleEdit[1], m_contentScaleButton[1], m_inspectionID, m_toolID, SvPb::ExtentHeightFactorContentEId, &m_resizeValueController, osData);

	m_formatScaleWidgets[0] = std::make_unique<SvOg::LinkedValueWidgetHelper>(m_formatScaleEdit[0], m_formatScaleButton[0], m_inspectionID, m_toolID, SvPb::ExtentWidthFactorFormatEId, &m_resizeValueController, osData);
	m_formatScaleWidgets[1] = std::make_unique<SvOg::LinkedValueWidgetHelper>(m_formatScaleEdit[1], m_formatScaleButton[1], m_inspectionID, m_toolID, SvPb::ExtentHeightFactorFormatEId, &m_resizeValueController, osData);
	
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
	m_formatScaleWidgets[Width]->OnButton();
	CommitAndCheckNewParameterValues();
}

void SVTADlgResizePage::OnButtonFormatHeightFactor()
{
	m_formatScaleWidgets[Height]->OnButton();
	CommitAndCheckNewParameterValues();
}

void SVTADlgResizePage::OnButtonContentWidthFactor()
{
	m_contentScaleWidgets[Width]->OnButton();
	CommitAndCheckNewParameterValues();
}

void SVTADlgResizePage::OnButtonContentHeightFactor()
{
	m_contentScaleWidgets[Height]->OnButton();
	CommitAndCheckNewParameterValues();
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
}

bool SVTADlgResizePage::CommitAndCheckNewParameterValues()
{
	CommitValuesFromDialog();

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

	m_resizeValueController.Commit();
}

HRESULT SVTADlgResizePage::SetupResizeImageControl()
{
	HRESULT	hr = S_OK;

	SvUl::NameObjectIdList namesAndIds = m_ImageController.GetResultImages();
	//we want to look just at the part of the dotted name after the last dot
	for (auto& rNameAndId : namesAndIds)
	{
		auto positionOfLastDot = rNameAndId.first.rfind(_T("."));
		if (positionOfLastDot != std::string::npos)
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

void SVTADlgResizePage::OnInterpolationModeChanged()
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
	CommitAndCheckNewParameterValues();
}

void SVTADlgResizePage::OnOverscanChanged()
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
	CommitAndCheckNewParameterValues();
}

void SVTADlgResizePage::OnContentWidthFactorChanged()
{
	m_contentScaleWidgets[Width]->EditboxToValue();
	CommitAndCheckNewParameterValues();
}

void SVTADlgResizePage::OnContentHeightFactorChanged()
{
	m_contentScaleWidgets[Height]->EditboxToValue();
	CommitAndCheckNewParameterValues();
}

void SVTADlgResizePage::OnFormatWidthFactorChanged()
{
	m_formatScaleWidgets[Width]->EditboxToValue();
	CommitAndCheckNewParameterValues();
}

void SVTADlgResizePage::OnFormatHeightFactorChanged()
{
	m_formatScaleWidgets[Height]->EditboxToValue();
	CommitAndCheckNewParameterValues();
}