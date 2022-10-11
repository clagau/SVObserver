//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProfileEdgeMarkerAdjustmentPage
//* .File Name       : $Workfile:   SVProfileEdgeMarkerAdjustmentPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   18 Jul 2014 03:59:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "Definitions/Color.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/LinearEdgeEnums.h"
#include "GuiValueHelper.h"
#include "ImageController.h"
#include "SVUtilityLibrary/RaiiLifeFlag.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace
{
bool isVertical(uint32_t inspectionId, uint32_t analyzerId)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getextentparameterrequest();
	pRequest->set_objectid(analyzerId);

	HRESULT hr = SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getextentparameterresponse())
	{
		auto extentParameter = responseCmd.getextentparameterresponse().parameters();
		auto valuePair = std::find_if(extentParameter.begin(), extentParameter.end(), [](const auto value) { return value.type() == SvPb::SVExtentPropertyRotationAngle; });
		if (extentParameter.end() != valuePair)
		{
			return (90 == valuePair->value());
		}
	}
	assert(false);
	return false;
}
}

namespace SvOg
{
#pragma region Constructor
SVProfileEdgeMarkerAdjustmentPage::SVProfileEdgeMarkerAdjustmentPage(uint32_t inspectionID, uint32_t egdeObjectId, const std::array<SvPb::EmbeddedIdEnum, EdgeIdCount>& rEdgeEmbeddedIds, uint32_t analyzerID, UINT nIDCaption /* = 0 */ )
: SVEdgeMarkerAdjustmentPageClass(inspectionID, egdeObjectId, rEdgeEmbeddedIds, analyzerID, nIDCaption, SVProfileEdgeMarkerAdjustmentPage::IDD )
, m_analyzerValues{BoundValues {inspectionID, analyzerID}}
{
	//{{AFX_DATA_INIT(SVProfileEdgeMarkerAdjustmentPage)
	//}}AFX_DATA_INIT
}

SVProfileEdgeMarkerAdjustmentPage::~SVProfileEdgeMarkerAdjustmentPage()
{
	if (m_bEdgeA) //disable both show flags with first destructor
	{
		// Set the values to the original values before the page was displayed
		m_analyzerValues.Set<bool>(SvPb::ShowAllEdgeAOverlaysEId, false);
		if (m_existEdgeB)
		{
			m_analyzerValues.Set<bool>(SvPb::ShowAllEdgeBOverlaysEId, false);
		}
		m_analyzerValues.Commit();
	}
}
#pragma endregion Constructor

#pragma region MFC Methods
void SVProfileEdgeMarkerAdjustmentPage::OnCancel()
{
	SVEdgeMarkerAdjustmentPageClass::OnCancel();
}

BOOL SVProfileEdgeMarkerAdjustmentPage::OnSetActive()
{
	BOOL bRetVal = SVEdgeMarkerAdjustmentPageClass::OnSetActive();

	// Update point sets (needed for 2 edges)
	m_dialogImage.Refresh();

	return bRetVal;
}

void SVProfileEdgeMarkerAdjustmentPage::DoDataExchange( CDataExchange* pDX )
{
	SVEdgeMarkerAdjustmentPageClass::DoDataExchange( pDX );
	//{{AFX_DATA_MAP(SVProfileEdgeMarkerAdjustmentPage)
	DDX_Control(pDX, IDC_UPPER_VALUE, m_UpperSelectedEditCtrl);
	DDX_Control(pDX, IDC_BUTTON1, m_UpperSelectedButton);
	DDX_Control(pDX, IDC_UPPER_PERCENT_DIFF, m_UpperMaxDiffEditCtrl);
	DDX_Control(pDX, IDC_BUTTON2, m_UpperMaxDiffButton);
	DDX_Control(pDX, IDC_UPPER_MAX_OFFSET, m_UpperMaxOffsetEditCtrl);
	DDX_Control(pDX, IDC_BUTTON3, m_UpperMaxOffsetButton);
	DDX_Control(pDX, IDC_UPPER_MIN_OFFSET, m_UpperMinOffsetEditCtrl);
	DDX_Control(pDX, IDC_BUTTON4, m_UpperMinOffsetButton);
	DDX_Control(pDX, IDC_LOWER_VALUE, m_LowerSelectedEditCtrl);
	DDX_Control(pDX, IDC_BUTTON5, m_LowerSelectedButton);
	DDX_Control(pDX, IDC_LOWER_PERCENT_DIFF, m_LowerMaxDiffEditCtrl);
	DDX_Control(pDX, IDC_BUTTON6, m_LowerMaxDiffButton);
	DDX_Control(pDX, IDC_LOWER_MAX_OFFSET, m_LowerMaxOffsetEditCtrl);
	DDX_Control(pDX, IDC_BUTTON7, m_LowerMaxOffsetButton);
	DDX_Control(pDX, IDC_LOWER_MIN_OFFSET, m_LowerMinOffsetEditCtrl);
	DDX_Control(pDX, IDC_BUTTON8, m_LowerMinOffsetButton);
	DDX_Radio(pDX, IDC_LOWER_USER_SELECTABLE, m_lowerThresholdOption);
	DDX_Radio(pDX, IDC_UPPER_USER_SELECTABLE, m_upperThresholdOption);
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
	//}}AFX_DATA_MAP
}

BOOL SVProfileEdgeMarkerAdjustmentPage::OnInitDialog()
{
	SVEdgeMarkerAdjustmentPageClass::OnInitDialog();

	m_analyzerValues.Init();

	m_LowerSelectedWidget = std::make_unique<LinkedValueWidgetHelper>(m_LowerSelectedEditCtrl, m_LowerSelectedButton, m_InspectionID, m_edgeObjectId, m_rEdgeEmbeddedIds[EdgeLowerThresholdSelected], &m_values, LinkedValueWidgetHelper::getCheckValueFunction(0, 255));
	m_LowerMinOffsetWidget = std::make_unique<LinkedValueWidgetHelper>(m_LowerMinOffsetEditCtrl, m_LowerMinOffsetButton, m_InspectionID, m_edgeObjectId, m_rEdgeEmbeddedIds[LowerThresholdMinPlusOffset], &m_values, LinkedValueWidgetHelper::getCheckValueFunction(0, 255));
	m_LowerMaxOffsetWidget = std::make_unique<LinkedValueWidgetHelper>(m_LowerMaxOffsetEditCtrl, m_LowerMaxOffsetButton, m_InspectionID, m_edgeObjectId, m_rEdgeEmbeddedIds[LowerThresholdMaxMinusOffset], &m_values, LinkedValueWidgetHelper::getCheckValueFunction(0, 255));
	m_LowerMaxDiffWidget = std::make_unique<LinkedValueWidgetHelper>(m_LowerMaxDiffEditCtrl, m_LowerMaxDiffButton, m_InspectionID, m_edgeObjectId, m_rEdgeEmbeddedIds[LowerThresholdMaxMinusPercentDiff], &m_values, LinkedValueWidgetHelper::getCheckValueFunction(0, 100));
	m_UpperSelectedWidget = std::make_unique<LinkedValueWidgetHelper>(m_UpperSelectedEditCtrl, m_UpperSelectedButton, m_InspectionID, m_edgeObjectId, m_rEdgeEmbeddedIds[EdgeUpperThresholdSelected], &m_values, LinkedValueWidgetHelper::getCheckValueFunction(0, 255));
	m_UpperMinOffsetWidget = std::make_unique<LinkedValueWidgetHelper>(m_UpperMinOffsetEditCtrl, m_UpperMinOffsetButton, m_InspectionID, m_edgeObjectId, m_rEdgeEmbeddedIds[UpperThresholdMinPlusOffset], &m_values, LinkedValueWidgetHelper::getCheckValueFunction(0, 255));
	m_UpperMaxOffsetWidget = std::make_unique<LinkedValueWidgetHelper>(m_UpperMaxOffsetEditCtrl, m_UpperMaxOffsetButton, m_InspectionID, m_edgeObjectId, m_rEdgeEmbeddedIds[UpperThresholdMaxMinusOffset], &m_values, LinkedValueWidgetHelper::getCheckValueFunction(0, 255));
	m_UpperMaxDiffWidget = std::make_unique<LinkedValueWidgetHelper>(m_UpperMaxDiffEditCtrl, m_UpperMaxDiffButton, m_InspectionID, m_edgeObjectId, m_rEdgeEmbeddedIds[UpperThresholdMaxMinusPercentDiff], &m_values, LinkedValueWidgetHelper::getCheckValueFunction(0, 100));

	GetInspectionData();

	ImageController imageCtrl {m_InspectionID, m_analyzerId};
	const auto& rInputData = imageCtrl.GetInputData(SvPb::ImageInputEId, m_analyzerId);
	auto analyzerImageID = rInputData.connected_objectid();
	auto* pPicture = imageCtrl.GetImage(analyzerImageID);
	if (pPicture)
	{
		m_dialogImage.AddTab("Image");
		m_dialogImage.setImage(pPicture, 0);
		m_dialogImage.Refresh();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}

void SVProfileEdgeMarkerAdjustmentPage::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	SVEdgeMarkerAdjustmentPageClass::OnHScroll( nSBCode, nPos, pScrollBar );

	UpdateData( TRUE ); // get data of dialog	

	if( &UpperSliderCtrl == reinterpret_cast<CSliderCtrl*>(pScrollBar) )
	{
		auto embeddedId = getValueEIdFromThresholdOption(true);
		if (SvPb::NoEmbeddedId != embeddedId)
		{
			auto data = m_values.Get<LinkedValueData>(embeddedId);
			if (SvPb::LinkedSelectedOption::DirectValue == data.m_selectedOption)
			{
				byte value = static_cast<unsigned char>(m_normalizerController.calcRealValueFromLocalValues((double)UpperSliderCtrl.GetPos()));
				data.m_Value = static_cast<long>(value);
				data.m_directValue = data.m_Value;
				m_values.Set(embeddedId, data);
				StrUpper.Format("%u", value);
			}
		}
	}

	if( &LowerSliderCtrl == reinterpret_cast<CSliderCtrl*>(pScrollBar) )
	{
		auto embeddedId = getValueEIdFromThresholdOption(false);
		if (SvPb::NoEmbeddedId != embeddedId)
		{
			auto data = m_values.Get<LinkedValueData>(embeddedId);
			if (SvPb::LinkedSelectedOption::DirectValue == data.m_selectedOption)
			{
				byte value = static_cast<unsigned char>(m_normalizerController.calcRealValueFromLocalValues((double)LowerSliderCtrl.GetPos()));
				data.m_Value = static_cast<long>(value);
				data.m_directValue = data.m_Value;
				m_values.Set(embeddedId, data);
				StrLower.Format("%u", value);
			}
		}
	}

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnUseFlagChanged()
{
	UpdateData(TRUE);
	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeUpperSelected()
{
	if (nullptr != m_UpperSelectedWidget && m_upperThresholdOption == SvDef::SV_USE_SELECTED)
	{
		OnChangeValue(m_UpperSelectedWidget);
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeUpperPercentDiff()
{
	if(nullptr != m_UpperMaxDiffWidget && m_upperThresholdOption == SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF)
	{
		OnChangeValue(m_UpperMaxDiffWidget);
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeUpperMaxOffset()
{
	if(nullptr != m_UpperMaxOffsetWidget && m_upperThresholdOption == SvDef::SV_USE_MAX_MINUS_OFFSET)
	{
		OnChangeValue(m_UpperMaxOffsetWidget);
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeUpperMinOffset()
{
	if(nullptr != m_UpperMinOffsetWidget && m_upperThresholdOption == SvDef::SV_USE_MIN_PLUS_OFFSET)
	{
		OnChangeValue(m_UpperMinOffsetWidget);
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeLowerSelected()
{
	if (nullptr != m_LowerSelectedWidget && m_lowerThresholdOption == SvDef::SV_USE_SELECTED)
	{
		OnChangeValue(m_LowerSelectedWidget);
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeLowerPercentDiff()
{
	if(nullptr != m_LowerMaxDiffWidget && m_lowerThresholdOption != SvDef::SV_USE_SELECTED)
	{
		OnChangeValue(m_LowerMaxDiffWidget);
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeLowerMaxOffset()
{
	if(nullptr != m_LowerMaxOffsetWidget && m_lowerThresholdOption != SvDef::SV_USE_SELECTED)
	{
		OnChangeValue(m_LowerMaxOffsetWidget);
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeLowerMinOffset()
{
	if(nullptr != m_LowerMinOffsetWidget && m_lowerThresholdOption != SvDef::SV_USE_SELECTED)
	{
		OnChangeValue(m_LowerMinOffsetWidget);
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnUpperSelectedButton()
{
	if (m_UpperSelectedWidget)
	{
		m_UpperSelectedWidget->OnButton();
	}
	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnUpperPercentDiffButton()
{
	if (m_UpperMaxDiffWidget)
	{
		m_UpperMaxDiffWidget->OnButton();
	}
	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnUpperMaxOffsetButton()
{
	if (m_UpperMaxOffsetWidget)
	{
		m_UpperMaxOffsetWidget->OnButton();
	}
	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnUpperMinOffsetButton()
{
	if (m_UpperMinOffsetWidget)
	{
		m_UpperMinOffsetWidget->OnButton();
	}
	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnLowerSelectedButton()
{
	if (m_LowerSelectedWidget)
	{
		m_LowerSelectedWidget->OnButton();
	}
	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnLowerPercentDiffButton()
{
	if (m_LowerMaxDiffWidget)
	{
		m_LowerMaxDiffWidget->OnButton();
	}
	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnLowerMaxOffsetButton()
{
	if (m_LowerMaxOffsetWidget)
	{
		m_LowerMaxOffsetWidget->OnButton();
	}
	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnLowerMinOffsetButton()
{
	if (m_LowerMinOffsetWidget)
	{
		m_LowerMinOffsetWidget->OnButton();
	}
	SetInspectionData();
}

BEGIN_MESSAGE_MAP( SVProfileEdgeMarkerAdjustmentPage, SVEdgeMarkerAdjustmentPageClass )
	//{{AFX_MSG_MAP(SVProfileEdgeMarkerAdjustmentPage)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_UPPER_USER_SELECTABLE, OnUseFlagChanged)
	ON_BN_CLICKED(IDC_LOWER_USER_SELECTABLE, OnUseFlagChanged)
	ON_BN_CLICKED(IDC_UPPER_USE_MAX_MINUS_DIFF, OnUseFlagChanged)
	ON_BN_CLICKED(IDC_UPPER_USE_MAX_MINUS_OFFSET, OnUseFlagChanged)
	ON_BN_CLICKED(IDC_UPPER_USE_MIN_PLUS_OFFSET, OnUseFlagChanged)
	ON_BN_CLICKED(IDC_LOWER_USE_MAX_MINUS_DIFF, OnUseFlagChanged)
	ON_BN_CLICKED(IDC_LOWER_USE_MAX_MINUS_OFFSET, OnUseFlagChanged)
	ON_BN_CLICKED(IDC_LOWER_USE_MIN_PLUS_OFFSET, OnUseFlagChanged)
	ON_EN_CHANGE(IDC_UPPER_VALUE, OnChangeUpperSelected)
	ON_EN_CHANGE(IDC_UPPER_PERCENT_DIFF, OnChangeUpperPercentDiff)
	ON_EN_CHANGE(IDC_UPPER_MAX_OFFSET, OnChangeUpperMaxOffset)
	ON_EN_CHANGE(IDC_UPPER_MIN_OFFSET, OnChangeUpperMinOffset)
	ON_EN_CHANGE(IDC_LOWER_VALUE, OnChangeLowerSelected)
	ON_EN_CHANGE(IDC_LOWER_PERCENT_DIFF, OnChangeLowerPercentDiff)
	ON_EN_CHANGE(IDC_LOWER_MAX_OFFSET, OnChangeLowerMaxOffset)
	ON_EN_CHANGE(IDC_LOWER_MIN_OFFSET, OnChangeLowerMinOffset)
	ON_BN_CLICKED(IDC_BUTTON1, OnUpperSelectedButton)
	ON_BN_CLICKED(IDC_BUTTON2, OnUpperPercentDiffButton)
	ON_BN_CLICKED(IDC_BUTTON3, OnUpperMaxOffsetButton)
	ON_BN_CLICKED(IDC_BUTTON4, OnUpperMinOffsetButton)
	ON_BN_CLICKED(IDC_BUTTON5, OnLowerSelectedButton)
	ON_BN_CLICKED(IDC_BUTTON6, OnLowerPercentDiffButton)
	ON_BN_CLICKED(IDC_BUTTON7, OnLowerMaxOffsetButton)
	ON_BN_CLICKED(IDC_BUTTON8, OnLowerMinOffsetButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma endregion MFC Methods

#pragma region Public Methods
HRESULT SVProfileEdgeMarkerAdjustmentPage::GetInspectionData()
{
	HRESULT Result{ S_OK };

	bool bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseLowerThresholdSelectable]);
	if(bState)
	{
		m_lowerThresholdOption = SvDef::SV_USE_SELECTED;
	}
	else
	{
		bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseLowerThresholdMaxMinusPercentDiff]);
		if(bState)
		{
			m_lowerThresholdOption = SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF;
		}
		else
		{
			bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseLowerThresholdMaxMinusOffset]);
			if(bState)
			{
				m_lowerThresholdOption = SvDef::SV_USE_MAX_MINUS_OFFSET;
			}
			else
			{
				bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseLowerThresholdMinPlusOffset]);
				if(bState)
				{
					m_lowerThresholdOption = SvDef::SV_USE_MIN_PLUS_OFFSET;
				}
			}
		}
	}

	bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseUpperThresholdSelectable]);
	if(bState)
	{
		m_upperThresholdOption = SvDef::SV_USE_SELECTED;
	}
	else
	{
		bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseUpperThresholdMaxMinusPercentDiff]);
		if(bState)
		{
			m_upperThresholdOption = SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF;
		}
		else
		{
			bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseUpperThresholdMaxMinusOffset]);
			if(bState)
			{
				m_upperThresholdOption = SvDef::SV_USE_MAX_MINUS_OFFSET;
			}
			else
			{
				bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseUpperThresholdMinPlusOffset]);
				if(bState)
				{
					m_upperThresholdOption = SvDef::SV_USE_MIN_PLUS_OFFSET;
				}
			}
		}
	}

	UpdateData(false); // set data to dialog

	updateControls();

	Result = SVEdgeMarkerAdjustmentPageClass::GetInspectionData();

	updateGraphDisplay();

	return Result;
}

HRESULT SVProfileEdgeMarkerAdjustmentPage::SetInspectionData()
{
	HRESULT Result{ S_OK };

	UpdateData(true);

	m_values.Set<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseLowerThresholdSelectable], SvDef::SV_USE_SELECTED == m_lowerThresholdOption);
	m_values.Set<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseLowerThresholdMaxMinusPercentDiff], SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF == m_lowerThresholdOption);
	m_values.Set<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseLowerThresholdMaxMinusOffset], SvDef::SV_USE_MAX_MINUS_OFFSET == m_lowerThresholdOption);
	m_values.Set<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseLowerThresholdMinPlusOffset], SvDef::SV_USE_MIN_PLUS_OFFSET == m_lowerThresholdOption);
	
	m_values.Set<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseUpperThresholdSelectable], SvDef::SV_USE_SELECTED == m_upperThresholdOption);
	m_values.Set<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseUpperThresholdMaxMinusPercentDiff], SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF == m_upperThresholdOption);
	m_values.Set<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseUpperThresholdMaxMinusOffset], SvDef::SV_USE_MAX_MINUS_OFFSET == m_upperThresholdOption);
	m_values.Set<bool>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::UseUpperThresholdMinPlusOffset], SvDef::SV_USE_MIN_PLUS_OFFSET == m_upperThresholdOption);

	//init need to be done, because the other tab (A or B) could change the values before.
	m_analyzerValues.Init();
	if (m_bEdgeA)
	{
		m_analyzerValues.Set<bool>(SvPb::ShowAllEdgeAOverlaysEId, true);
		if (m_existEdgeB)
		{
			m_analyzerValues.Set<bool>(SvPb::ShowAllEdgeBOverlaysEId, false);
		}
	}
	else
	{
		m_analyzerValues.Set<bool>(SvPb::ShowAllEdgeAOverlaysEId, false);
		m_analyzerValues.Set<bool>(SvPb::ShowAllEdgeBOverlaysEId, true);
	}
	m_analyzerValues.Commit();
	
	//Note in the base class the values commit function is called
	Result = SVEdgeMarkerAdjustmentPageClass::SetInspectionData();

	return Result;
}
#pragma endregion Public Methods

#pragma region Protected Methods
SvPb::EmbeddedIdEnum SVProfileEdgeMarkerAdjustmentPage::getValueEIdFromThresholdOption(bool isUpper) const
{
	if (isUpper)
	{
		switch (m_upperThresholdOption)
		{
			case SvDef::SV_USE_SELECTED:
				return m_rEdgeEmbeddedIds[EdgeUpperThresholdSelected];
			case SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF:
				return m_rEdgeEmbeddedIds[UpperThresholdMaxMinusPercentDiff];
			case SvDef::SV_USE_MAX_MINUS_OFFSET:
				return m_rEdgeEmbeddedIds[UpperThresholdMaxMinusOffset];
			case SvDef::SV_USE_MIN_PLUS_OFFSET:
				return m_rEdgeEmbeddedIds[UpperThresholdMinPlusOffset];
			default:
				return SvPb::NoEmbeddedId;
		}
	}
	else
	{
		switch (m_lowerThresholdOption)
		{
			case SvDef::SV_USE_SELECTED:
				return m_rEdgeEmbeddedIds[EdgeLowerThresholdSelected];
			case SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF:
				return m_rEdgeEmbeddedIds[LowerThresholdMaxMinusPercentDiff];
			case SvDef::SV_USE_MAX_MINUS_OFFSET:
				return m_rEdgeEmbeddedIds[LowerThresholdMaxMinusOffset];
			case SvDef::SV_USE_MIN_PLUS_OFFSET:
				return m_rEdgeEmbeddedIds[LowerThresholdMinPlusOffset];
			default:
				return SvPb::NoEmbeddedId;
		}
	}
}

HRESULT SVProfileEdgeMarkerAdjustmentPage::UpdateSliderData()
{
	DWORD upper{0};
	DWORD lower{0};

	SvPb::EmbeddedIdEnum upperEmbeddedId = getValueEIdFromThresholdOption(true);
	if (SvPb::NoEmbeddedId != upperEmbeddedId)
	{
		auto data = m_values.Get<LinkedValueData>(upperEmbeddedId);
		upper = data.m_Value;
		UpperSliderCtrl.EnableWindow(SvPb::LinkedSelectedOption::DirectValue == data.m_selectedOption);
	}
	else
	{
		UpperSliderCtrl.EnableWindow(false);
	}
	
	SvPb::EmbeddedIdEnum lowerEmbeddedId = getValueEIdFromThresholdOption(false);
	if (SvPb::NoEmbeddedId != lowerEmbeddedId)
	{
		auto data = m_values.Get<LinkedValueData>(lowerEmbeddedId);
		lower = data.m_Value;
		LowerSliderCtrl.EnableWindow(SvPb::LinkedSelectedOption::DirectValue == data.m_selectedOption);
	}
	else
	{
		LowerSliderCtrl.EnableWindow(false);
	}

	return SVEdgeMarkerAdjustmentPageClass::UpdateSliderData(lower, upper);
}

#pragma region Protected Methods

#pragma region Private Methods
void SVProfileEdgeMarkerAdjustmentPage::updateControls()
{
	m_LowerSelectedWidget->EnableGuiElements(SvDef::SV_USE_SELECTED == m_lowerThresholdOption);
	m_LowerMaxDiffWidget->EnableGuiElements(SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF == m_lowerThresholdOption);
	m_LowerMaxOffsetWidget->EnableGuiElements(SvDef::SV_USE_MAX_MINUS_OFFSET == m_lowerThresholdOption);
	m_LowerMinOffsetWidget->EnableGuiElements(SvDef::SV_USE_MIN_PLUS_OFFSET == m_lowerThresholdOption);
	switch (m_lowerThresholdOption)
	{
		case SvDef::SV_USE_SELECTED:
			// Set Slider Range to 0 - 255
			setScrollRange(&LowerSliderCtrl, 0, 255);
			break;

		case SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF:
			// Set Slider Range to 0 - 100
			setScrollRange(&LowerSliderCtrl, 0, 100);
			break;

		case SvDef::SV_USE_MAX_MINUS_OFFSET:
			// Set Slider Range to 0 - 255
			setScrollRange(&LowerSliderCtrl, 0, 255);
			break;

		case SvDef::SV_USE_MIN_PLUS_OFFSET:
			// Set Slider Range to 0 - 255
			setScrollRange(&LowerSliderCtrl, 0, 255);
			break;
	}

	m_UpperSelectedWidget->EnableGuiElements(SvDef::SV_USE_SELECTED == m_upperThresholdOption);
	m_UpperMaxDiffWidget->EnableGuiElements(SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF == m_upperThresholdOption);
	m_UpperMaxOffsetWidget->EnableGuiElements(SvDef::SV_USE_MAX_MINUS_OFFSET == m_upperThresholdOption);
	m_UpperMinOffsetWidget->EnableGuiElements(SvDef::SV_USE_MIN_PLUS_OFFSET == m_upperThresholdOption);
	switch( m_upperThresholdOption )
	{
		case SvDef::SV_USE_SELECTED:
			// Set Slider Range to 0 - 255
			setScrollRange(&UpperSliderCtrl, 0, 255);
			break;

		case SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF:
			// Set Slider Range to 0 - 100
			setScrollRange(&UpperSliderCtrl, 0, 100);
			break;

		case SvDef::SV_USE_MAX_MINUS_OFFSET:
			// Set Slider Range to 0 - 255
			setScrollRange(&UpperSliderCtrl, 0, 255);
			break;

		case SvDef::SV_USE_MIN_PLUS_OFFSET:
			// Set Slider Range to 0 - 255
			setScrollRange(&UpperSliderCtrl, 0, 255);
			break;
	}
}

void SVProfileEdgeMarkerAdjustmentPage::updateGraphDisplay()
{
	// Remove old points
	m_dialogImage.RemoveAllOverlays(0);

	//check if graphic vertical
	bool bVertical = isVertical(m_InspectionID, m_analyzerId);
	//set HistogramOverlay and edges
	setGraphOverlayToPicture(bVertical);

	//set Threshold Bars
	DWORD thresholdValue = m_values.Get<DWORD>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::EdgeLowerThresholdValue]);
	setMarkerOverlayToPicture(thresholdValue, bVertical);

	thresholdValue = m_values.Get<DWORD>(m_rEdgeEmbeddedIds[EdgeEmbeddedEnum::EdgeUpperThresholdValue]);
	setMarkerOverlayToPicture(thresholdValue, bVertical);

	// Update the Graph on the Dialog
	m_dialogImage.Refresh();
}

void SVProfileEdgeMarkerAdjustmentPage::setGraphOverlayToPicture(bool bVertical)
{
	m_analyzerValues.Init();

	auto HistogramData = ConvertVariantSafeArrayToVector<double>(m_values.Get<_variant_t>(SvPb::LinearDataInputEId));
	long handle = -1;
	size_t sizePointsArray = HistogramData.size();
	double* points = new double[sizePointsArray * 2];
	for (size_t i = 0; i < sizePointsArray; i++)
	{
		points[i * 2] = bVertical ? static_cast<double>(HistogramData[i]) : static_cast<double>(i);
		points[i * 2 + 1] = (!bVertical) ? static_cast<double>(HistogramData[i]) : static_cast<double>(i);
	}

	COleSafeArray arraySafe;
	arraySafe.CreateOneDim(VT_R8, static_cast<DWORD>(sizePointsArray * 2), points);
	std::map<long, _variant_t> ParMap;
	ParMap[CDSVPictureDisplay::P_Type] = static_cast<long>(CDSVPictureDisplay::GraphROI);
	ParMap[CDSVPictureDisplay::P_SubType_X] = static_cast<long>(bVertical ? CDSVPictureDisplay::ViewArea_ScalePerParameter : CDSVPictureDisplay::ImageArea_ScalePerParameter);
	ParMap[CDSVPictureDisplay::P_SubType_Y] = static_cast<long>(bVertical ? CDSVPictureDisplay::ImageArea_ScalePerParameter : CDSVPictureDisplay::ViewArea_ScalePerParameter);
	ParMap[CDSVPictureDisplay::P_Color] = static_cast<long>(SvDef::DefaultSubFunctionColor1);
	ParMap[CDSVPictureDisplay::P_SelectedColor] = static_cast<long>(SvDef::DefaultSubFunctionColor1);
	ParMap[CDSVPictureDisplay::P_AllowEdit] = static_cast<long>(0);
	ParMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;
	if (!bVertical)
	{
		ParMap[CDSVPictureDisplay::P_Is_Flip_Vertical] = true;
	}
	ParMap[CDSVPictureDisplay::P_X_Min] = 0;
	ParMap[bVertical ? CDSVPictureDisplay::P_Y_Max : CDSVPictureDisplay::P_X_Max] = sizePointsArray;
	ParMap[CDSVPictureDisplay::P_Y_Min] = 0;
	ParMap[bVertical ? CDSVPictureDisplay::P_X_Max : CDSVPictureDisplay::P_Y_Max] = SvDef::cMax8BitPixelValue;
	m_dialogImage.AddOverlay(0, ParMap, &handle);

	//set linear Edge lines
	auto LinearEdges = ConvertVariantSafeArrayToVector<double>(m_values.Get<_variant_t>(SvPb::LinearEdgesEId));
	ParMap[CDSVPictureDisplay::P_Color] = static_cast<long> (SvDef::DefaultSubFunctionColor2);
	ParMap[CDSVPictureDisplay::P_SelectedColor] = static_cast<long> (SvDef::DefaultSubFunctionColor2);
	ParMap[CDSVPictureDisplay::P_Is_Flip_Vertical] = false;
	ParMap[bVertical ? CDSVPictureDisplay::P_X_Max : CDSVPictureDisplay::P_Y_Max] = m_egdeLinesGraphMaxY;
	points[bVertical ? 0 : 1] = m_egdeLinesStartPos; // first point
	points[bVertical ? 2 : 3] = m_egdeLinesStopPos; // second point
	for (size_t i = 0; i < LinearEdges.size(); i++)
	{
		points[bVertical ? 1 : 0] = LinearEdges[i]; // first point
		points[bVertical ? 3 : 2] = LinearEdges[i]; // second point
		arraySafe.CreateOneDim(VT_R8, 4, points); // two points are four values
		ParMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;
		m_dialogImage.AddOverlay(0, ParMap, &handle);
	}

	//set selected Edge line
	if (SvPb::NoEmbeddedId != m_rEdgeEmbeddedIds[EdgeDistance])
	{
		double distance = m_analyzerValues.Get<double>(m_rEdgeEmbeddedIds[EdgeDistance]);
		points[bVertical ? 1 : 0] = distance; // first point
		points[bVertical ? 3 : 2] = distance; // x of second point
		arraySafe.CreateOneDim(VT_R8, 4, points); // two points are four values
		long color = m_values.Get<long>(SvPb::ColorEId);
		ParMap[CDSVPictureDisplay::P_Color] = static_cast<long>(color);
		ParMap[CDSVPictureDisplay::P_SelectedColor] = static_cast<long>(color);
		ParMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;
		m_dialogImage.AddOverlay(0, ParMap, &handle);
	}

	delete[] points;
}

void SVProfileEdgeMarkerAdjustmentPage::setMarkerOverlayToPicture(DWORD value, bool bVertical, long allowType /*= CDSVPictureDisplay::AllowNone */)
{
	long handle = -1;

	std::map<long, _variant_t> ParMap;
	ParMap[CDSVPictureDisplay::P_Type] = static_cast<long>(CDSVPictureDisplay::MarkerROI);
	ParMap[CDSVPictureDisplay::P_SubType] = static_cast<long>(bVertical ? CDSVPictureDisplay::VerticalMarker : CDSVPictureDisplay::HorizontalMarker);
	ParMap[CDSVPictureDisplay::P_Color] = static_cast<long>(SvDef::DefaultSubFunctionColor1);
	ParMap[CDSVPictureDisplay::P_SelectedColor] = static_cast<long>(SvDef::DefaultSubFunctionColor1);
	ParMap[CDSVPictureDisplay::P_AllowEdit] = static_cast<long>(allowType);
	ParMap[CDSVPictureDisplay::P_Min] = static_cast<long>(0);
	ParMap[CDSVPictureDisplay::P_Max] = static_cast<long>(255);
	ParMap[CDSVPictureDisplay::P_VALUE] = static_cast<long>(bVertical ? value : 255 - value);

	m_dialogImage.AddOverlay(0, ParMap, &handle);
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeValue(std::unique_ptr<LinkedValueWidgetHelper>& rWidget)
{
	static bool isStarted = false;
	//Check if function already start to avoid endless recursive calls of this function.
	if (false == isStarted && nullptr != rWidget)
	{
		SvDef::RaiiLifeFlag circularCheck(isStarted);
		UpdateData(TRUE);
		rWidget->EditboxToValue(true);
		SetInspectionData();
	}
}
#pragma endregion Private Methods
}  //namespace SvOg