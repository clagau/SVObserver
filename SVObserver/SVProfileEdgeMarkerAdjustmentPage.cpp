//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "Definitions/LinearEdgeEnums.h"
#include "SVOGui/ImageController.h"
#include "Tools/SVTool.h"
#include "AnalyzerOperators/SVAnalyzer.h"
#include "Operators/SVLinearEdgeProcessingClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
SVProfileEdgeMarkerAdjustmentPage::SVProfileEdgeMarkerAdjustmentPage(uint32_t inspectionID, uint32_t taskObjectID, const std::vector<SvPb::EmbeddedIdEnum>& rEdgeEmbeddedIds, uint32_t analyzerID, UINT nIDCaption /* = 0 */ )
: SVEdgeMarkerAdjustmentPageClass(inspectionID, taskObjectID, rEdgeEmbeddedIds, analyzerID, nIDCaption, SVProfileEdgeMarkerAdjustmentPage::IDD )
, m_lowerThresholdMaxOffset( 0 )
, m_lowerThresholdMinOffset( 0 )
, m_lowerThresholdMaxPercentDiff( 0 )
, m_upperThresholdMaxOffset( 0 )
, m_upperThresholdMinOffset( 0 )
, m_upperThresholdMaxPercentDiff( 0 )
, m_lowerThresholdOption( 0 )
, m_upperThresholdOption( 0 )
{
	//{{AFX_DATA_INIT(SVProfileEdgeMarkerAdjustmentPage)
	//}}AFX_DATA_INIT
}

SVProfileEdgeMarkerAdjustmentPage::~SVProfileEdgeMarkerAdjustmentPage()
{
	if (nullptr != m_pAnalyzerValues)
	{
		// Set the values to the original values before the page was displayed
		m_pAnalyzerValues->Set<bool>(SvPb::ShowAllEdgeAOverlaysEId, false);
		m_pAnalyzerValues->Set<bool>(SvPb::ShowAllEdgeBOverlaysEId, false);
		m_pAnalyzerValues->Commit();
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
	DDX_Control(pDX, IDC_LOWER_MIN_OFFSET, m_LowerMinOffsetEditCtrl);
	DDX_Control(pDX, IDC_LOWER_MAX_OFFSET, m_LowerMaxOffsetEditCtrl);
	DDX_Control(pDX, IDC_LOWER_PERCENT_DIFF, m_LowerMaxDiffEditCtrl);
	DDX_Control(pDX, IDC_UPPER_MIN_OFFSET, m_UpperMinOffsetEditCtrl);
	DDX_Control(pDX, IDC_UPPER_MAX_OFFSET, m_UpperMaxOffsetEditCtrl);
	DDX_Control(pDX, IDC_UPPER_PERCENT_DIFF, m_UpperMaxDiffEditCtrl);
	DDX_Radio(pDX, IDC_LOWER_USER_SELECTABLE, m_lowerThresholdOption);
	DDX_Radio(pDX, IDC_UPPER_USER_SELECTABLE, m_upperThresholdOption);
	DDX_Text(pDX, IDC_LOWER_MAX_OFFSET, m_lowerThresholdMaxOffset);
	DDX_Text(pDX, IDC_LOWER_MIN_OFFSET, m_lowerThresholdMinOffset);
	DDX_Text(pDX, IDC_LOWER_PERCENT_DIFF, m_lowerThresholdMaxPercentDiff);
	DDV_MinMaxInt( pDX, m_lowerThresholdMaxPercentDiff, 0, 100);
	DDX_Text(pDX, IDC_UPPER_MAX_OFFSET, m_upperThresholdMaxOffset);
	DDX_Text(pDX, IDC_UPPER_MIN_OFFSET, m_upperThresholdMinOffset);
	DDX_Text(pDX, IDC_UPPER_PERCENT_DIFF, m_upperThresholdMaxPercentDiff);
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
	DDV_MinMaxInt( pDX, m_upperThresholdMaxPercentDiff, 0, 100);
	//}}AFX_DATA_MAP
}

BOOL SVProfileEdgeMarkerAdjustmentPage::OnInitDialog()
{
	SVEdgeMarkerAdjustmentPageClass::OnInitDialog();

	m_pEdge = dynamic_cast<SvOp::SVLinearEdgeProcessingClass*> (SvOi::getObject(m_TaskObjectID));
	m_pTool = (m_pEdge == nullptr ) ? nullptr : dynamic_cast<SvTo::SVToolClass*> (m_pEdge->GetAncestorInterface(SvPb::SVToolObjectType));
	m_pAnalyzer = (m_pEdge == nullptr) ? nullptr : dynamic_cast<SvAo::SVAnalyzerClass*> (m_pEdge->GetAncestorInterface(SvPb::SVAnalyzerObjectType));
	if (nullptr != m_pAnalyzer)
	{
		m_pAnalyzerValues = std::shared_ptr<SvOg::ValueController> {new SvOg::ValueController{SvOg::BoundValues {m_InspectionID, m_pAnalyzer->getObjectId()}}};
		m_pAnalyzerValues->Init();
	}

	GetInspectionData();

	if (nullptr != m_pTool)
	{
		// Get the Image for this tool
		const SVImageInfoClass* pImageInfo = m_pTool->getFirstImageInfo();
		if (nullptr != pImageInfo)
		{
			SvIe::SVImageClass* pImage = nullptr;
			pImageInfo->GetOwnerImage(pImage);
			if(nullptr != pImage)
			{
				m_dialogImage.AddTab("Image");
				m_dialogImage.setImage(pImage->getLastImage(), 0);
				m_dialogImage.Refresh();
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void SVProfileEdgeMarkerAdjustmentPage::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	SVEdgeMarkerAdjustmentPageClass::OnHScroll( nSBCode, nPos, pScrollBar );

	UpdateData( TRUE ); // get data of dialog	

	if( &UpperSliderCtrl == ( CSliderCtrl* ) pScrollBar )
	{
		switch( m_upperThresholdOption )
		{
			case SvDef::SV_USE_SLIDER:
			double dUpper;
			dUpper = m_normalizerController.calcRealValueFromLocalValues((double)UpperSliderCtrl.GetPos());
			StrUpper.Format( "%u", (int)dUpper );
			break;

			case SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF:
			m_upperThresholdMaxPercentDiff = (unsigned char)m_normalizerController.calcRealValueFromLocalValues((double)UpperSliderCtrl.GetPos());
			StrUpper.Format( "%u", m_upperThresholdMaxPercentDiff );
			break;

			case SvDef::SV_USE_MAX_MINUS_OFFSET:
			m_upperThresholdMaxOffset = (unsigned char)m_normalizerController.calcRealValueFromLocalValues((double)UpperSliderCtrl.GetPos());
			StrUpper.Format( "%u", m_upperThresholdMaxOffset );
			break;

			case SvDef::SV_USE_MIN_PLUS_OFFSET:
			m_upperThresholdMinOffset = (unsigned char)m_normalizerController.calcRealValueFromLocalValues((double)UpperSliderCtrl.GetPos());
			StrUpper.Format( "%u", m_upperThresholdMinOffset );
			break;
		}
	}

	if( &LowerSliderCtrl == ( CSliderCtrl* ) pScrollBar )
	{
		switch( m_lowerThresholdOption )
		{
			case SvDef::SV_USE_SLIDER:
			double dLower;
			dLower = m_lowerNormalizerController.calcRealValueFromLocalValues((double)LowerSliderCtrl.GetPos());
			StrLower.Format( "%u", (int)dLower );
			break;

			case SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF:
			m_lowerThresholdMaxPercentDiff = (unsigned char)m_lowerNormalizerController.calcRealValueFromLocalValues((double)LowerSliderCtrl.GetPos());
			StrLower.Format( "%u", m_lowerThresholdMaxPercentDiff );
			break;

			case SvDef::SV_USE_MAX_MINUS_OFFSET:
			m_lowerThresholdMaxOffset = (unsigned char)m_lowerNormalizerController.calcRealValueFromLocalValues((double)LowerSliderCtrl.GetPos());
			StrLower.Format( "%u", m_lowerThresholdMaxOffset );
			break;

			case SvDef::SV_USE_MIN_PLUS_OFFSET:
			m_lowerThresholdMinOffset = (unsigned char)m_lowerNormalizerController.calcRealValueFromLocalValues((double)LowerSliderCtrl.GetPos());
			StrLower.Format( "%u", m_lowerThresholdMinOffset );
			break;
		}
	}

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnUpperUserSelectable()
{
	UpdateData(TRUE);

	double dUpper;

	dUpper = m_normalizerController.calcRealValueFromLocalValues((double)UpperSliderCtrl.GetPos());

	StrUpper.Format( "%u", (int)dUpper );

	UpdateData(FALSE);

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnLowerUserSelectable()
{
	UpdateData(TRUE);

	double dLower;

	dLower = m_lowerNormalizerController.calcRealValueFromLocalValues((double)LowerSliderCtrl.GetPos());

	StrLower.Format( "%u", (int)dLower );

	UpdateData(FALSE);

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnUpperUseMaxMinusDiff()
{
	UpdateData(TRUE);

	StrUpper.Format( "%u", m_upperThresholdMaxPercentDiff );

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnUpperUseMaxMinusOffset()
{
	UpdateData(TRUE);

	StrUpper.Format( "%u", m_upperThresholdMaxOffset );

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnUpperUseMinPlusOffset()
{
	UpdateData(TRUE);

	StrUpper.Format( "%u", m_upperThresholdMinOffset );

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnLowerUseMaxMinusDiff()
{
	UpdateData(TRUE);

	StrLower.Format( "%u", m_lowerThresholdMaxPercentDiff );

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnLowerUseMaxMinusOffset()
{
	UpdateData(TRUE);

	StrLower.Format( "%u", m_lowerThresholdMaxOffset );

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnLowerUseMinPlusOffset()
{
	UpdateData(TRUE);

	StrLower.Format( "%u", m_lowerThresholdMinOffset );

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeUpperPercentDiff()
{
	if( m_upperThresholdOption != SvDef::SV_USE_SLIDER )
	{
		int l_prevValue = m_upperThresholdMaxPercentDiff;
		if( UpdateData(TRUE) )
		{
			StrUpper.Format( "%u", m_upperThresholdMaxPercentDiff );

			UpdateData( FALSE ); // set data to dialog

			SetInspectionData();
		}
		else
		{
			m_upperThresholdMaxPercentDiff = l_prevValue;
			UpdateData(FALSE);
		}
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeUpperMaxOffset()
{
	if( m_upperThresholdOption != SvDef::SV_USE_SLIDER )
	{
		UpdateData(TRUE);

		StrUpper.Format( "%u", m_upperThresholdMaxOffset );

		UpdateData( FALSE ); // set data to dialog

		SetInspectionData();
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeUpperMinOffset()
{
	if( m_upperThresholdOption != SvDef::SV_USE_SLIDER )
	{
		UpdateData(TRUE);

		StrUpper.Format( "%u", m_upperThresholdMinOffset );

		UpdateData( FALSE ); // set data to dialog

		SetInspectionData();
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeLowerPercentDiff()
{
	if( m_lowerThresholdOption != SvDef::SV_USE_SLIDER )
	{
		int l_PrevValue = m_lowerThresholdMaxPercentDiff;
		if( UpdateData(TRUE) )
		{
			StrLower.Format( "%u", m_lowerThresholdMaxPercentDiff );

			UpdateData( FALSE ); // set data to dialog

			SetInspectionData();
		}
		else
		{
			m_lowerThresholdMaxPercentDiff = l_PrevValue;
			UpdateData(FALSE);
		}
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeLowerMaxOffset()
{
	if( m_lowerThresholdOption != SvDef::SV_USE_SLIDER )
	{
		UpdateData(TRUE);

		StrLower.Format( "%u", m_lowerThresholdMaxOffset );

		UpdateData( FALSE ); // set data to dialog

		SetInspectionData();
	}
}

void SVProfileEdgeMarkerAdjustmentPage::OnChangeLowerMinOffset()
{
	if( m_lowerThresholdOption != SvDef::SV_USE_SLIDER )
	{
		UpdateData(TRUE);

		StrLower.Format( "%u", m_lowerThresholdMinOffset );

		UpdateData( FALSE ); // set data to dialog

		SetInspectionData();
	}
}

BEGIN_MESSAGE_MAP( SVProfileEdgeMarkerAdjustmentPage, SVEdgeMarkerAdjustmentPageClass )
	//{{AFX_MSG_MAP(SVProfileEdgeMarkerAdjustmentPage)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_UPPER_USER_SELECTABLE, OnUpperUserSelectable)
	ON_BN_CLICKED(IDC_LOWER_USER_SELECTABLE, OnLowerUserSelectable)
	ON_BN_CLICKED(IDC_UPPER_USE_MAX_MINUS_DIFF, OnUpperUseMaxMinusDiff)
	ON_BN_CLICKED(IDC_UPPER_USE_MAX_MINUS_OFFSET, OnUpperUseMaxMinusOffset)
	ON_BN_CLICKED(IDC_UPPER_USE_MIN_PLUS_OFFSET, OnUpperUseMinPlusOffset)
	ON_BN_CLICKED(IDC_LOWER_USE_MAX_MINUS_DIFF, OnLowerUseMaxMinusDiff)
	ON_BN_CLICKED(IDC_LOWER_USE_MAX_MINUS_OFFSET, OnLowerUseMaxMinusOffset)
	ON_BN_CLICKED(IDC_LOWER_USE_MIN_PLUS_OFFSET, OnLowerUseMinPlusOffset)
	ON_EN_CHANGE(IDC_UPPER_PERCENT_DIFF, OnChangeUpperPercentDiff)
	ON_EN_CHANGE(IDC_UPPER_MAX_OFFSET, OnChangeUpperMaxOffset)
	ON_EN_CHANGE(IDC_UPPER_MIN_OFFSET, OnChangeUpperMinOffset)
	ON_EN_CHANGE(IDC_LOWER_PERCENT_DIFF, OnChangeLowerPercentDiff)
	ON_EN_CHANGE(IDC_LOWER_MAX_OFFSET, OnChangeLowerMaxOffset)
	ON_EN_CHANGE(IDC_LOWER_MIN_OFFSET, OnChangeLowerMinOffset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma endregion MFC Methods

#pragma region Public Methods
HRESULT SVProfileEdgeMarkerAdjustmentPage::GetInspectionData()
{
	HRESULT Result{ S_OK };

	bool bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdSelectable]);
	if(bState)
	{
		m_lowerThresholdOption = SvDef::SV_USE_SLIDER;
	}
	else
	{
		bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMaxMinusPercentDiff]);
		if(bState)
		{
			m_lowerThresholdOption = SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF;
		}
		else
		{
			bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMaxMinusOffset]);
			if(bState)
			{
				m_lowerThresholdOption = SvDef::SV_USE_MAX_MINUS_OFFSET;
			}
			else
			{
				bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMinPlusOffset]);
				if(bState)
				{
					m_lowerThresholdOption = SvDef::SV_USE_MIN_PLUS_OFFSET;
				}
			}
		}
	}

	DWORD Value = m_values.Get<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::LowerThresholdMaxMinusPercentDiff]);
	m_lowerThresholdMaxPercentDiff = static_cast<unsigned char> (Value);

	Value = m_values.Get<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::LowerThresholdMaxMinusOffset]);
	m_lowerThresholdMaxOffset = static_cast<unsigned char> (Value);

	Value = m_values.Get<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::LowerThresholdMinPlusOffset]);
	m_lowerThresholdMinOffset = static_cast<unsigned char> (Value);

	bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdSelectable]);
	if(bState)
	{
		m_upperThresholdOption = SvDef::SV_USE_SLIDER;
	}
	else
	{
		bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMaxMinusPercentDiff]);
		if(bState)
		{
			m_upperThresholdOption = SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF;
		}
		else
		{
			bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMaxMinusOffset]);
			if(bState)
			{
				m_upperThresholdOption = SvDef::SV_USE_MAX_MINUS_OFFSET;
			}
			else
			{
				bState = m_values.Get<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMinPlusOffset]);
				if(bState)
				{
					m_upperThresholdOption = SvDef::SV_USE_MIN_PLUS_OFFSET;
				}
			}
		}
	}

	Value = m_values.Get<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UpperThresholdMaxMinusPercentDiff]);
	m_upperThresholdMaxPercentDiff = static_cast<unsigned char> (Value);

	Value = m_values.Get<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UpperThresholdMaxMinusOffset]);
	m_upperThresholdMaxOffset = static_cast<unsigned char> (Value);

	Value = m_values.Get<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UpperThresholdMinPlusOffset]);
	m_upperThresholdMinOffset = static_cast<unsigned char> (Value);

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

	switch(m_lowerThresholdOption)
	{
		case SvDef::SV_USE_SLIDER:
		{
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdSelectable], true);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMaxMinusPercentDiff], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMaxMinusOffset], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMinPlusOffset], false);
			break;
		}

		case SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF:
		{
			DWORD Value = static_cast<DWORD> (m_lowerThresholdMaxPercentDiff);
			m_values.Set<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::LowerThresholdMaxMinusPercentDiff], Value);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdSelectable], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMaxMinusPercentDiff], true);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMaxMinusOffset], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMinPlusOffset], false);
			break;
		}

		case SvDef::SV_USE_MAX_MINUS_OFFSET:
		{
			DWORD Value = static_cast<DWORD> (m_lowerThresholdMaxOffset);
			m_values.Set<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::LowerThresholdMaxMinusOffset], Value);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdSelectable], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMaxMinusPercentDiff], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMaxMinusOffset], true);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMinPlusOffset], false);
			break;
		}

		case SvDef::SV_USE_MIN_PLUS_OFFSET:
		{
			DWORD Value = static_cast<DWORD> (m_lowerThresholdMinOffset);
			m_values.Set<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::LowerThresholdMinPlusOffset], Value);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdSelectable], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMaxMinusPercentDiff], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMaxMinusOffset], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseLowerThresholdMinPlusOffset], true);
			break;
		}
	}

	switch(m_upperThresholdOption)
	{
		case SvDef::SV_USE_SLIDER:
		{
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdSelectable], true);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMaxMinusPercentDiff], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMaxMinusOffset], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMinPlusOffset], false);
			break;
		}

		case SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF:
		{
			DWORD Value = static_cast<DWORD> (m_upperThresholdMaxPercentDiff);
			m_values.Set<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UpperThresholdMaxMinusPercentDiff], Value);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdSelectable], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMaxMinusPercentDiff], true);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMaxMinusOffset], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMinPlusOffset], false);
			break;
		}

		case SvDef::SV_USE_MAX_MINUS_OFFSET:
		{
			DWORD Value = static_cast<DWORD> (m_upperThresholdMaxOffset);
			m_values.Set<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UpperThresholdMaxMinusOffset], Value);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdSelectable], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMaxMinusPercentDiff], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMaxMinusOffset], true);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMinPlusOffset], false);
			break;
		}

		case SvDef::SV_USE_MIN_PLUS_OFFSET:
		{
			DWORD Value = static_cast<DWORD> (m_upperThresholdMinOffset);
			m_values.Set<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UpperThresholdMinPlusOffset], Value);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdSelectable], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMaxMinusPercentDiff], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMaxMinusOffset], false);
			m_values.Set<bool>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::UseUpperThresholdMinPlusOffset], true);
			break;
		}
	}

	if (nullptr != m_pAnalyzerValues)
	{
		//init need to be done, because the other tab (A or B) could change the values before.
		m_pAnalyzerValues->Init();
		if (m_bEdgeA)
		{
			m_pAnalyzerValues->Set<bool>(SvPb::ShowAllEdgeAOverlaysEId, true);
			m_pAnalyzerValues->Set<bool>(SvPb::ShowAllEdgeBOverlaysEId, false);
		}
		else
		{
			m_pAnalyzerValues->Set<bool>(SvPb::ShowAllEdgeAOverlaysEId, false);
			m_pAnalyzerValues->Set<bool>(SvPb::ShowAllEdgeBOverlaysEId, true);
		}
		m_pAnalyzerValues->Commit();
	}

	//Note in the base class the values commit function is called
	Result = SVEdgeMarkerAdjustmentPageClass::SetInspectionData();

	return Result;
}
#pragma endregion Public Methods

#pragma region Protected Methods
HRESULT SVProfileEdgeMarkerAdjustmentPage::UpdateSliderData(DWORD Lower, DWORD Upper)
{
	DWORD l_Upper{ 0 };
	DWORD l_Lower{ 0 };

	switch( m_upperThresholdOption )
	{
		case SvDef::SV_USE_SLIDER:
			l_Upper = Upper;
			break;

		case SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF:
			l_Upper = m_upperThresholdMaxPercentDiff;
			break;

		case SvDef::SV_USE_MAX_MINUS_OFFSET:
			l_Upper = m_upperThresholdMaxOffset;
			break;

		case SvDef::SV_USE_MIN_PLUS_OFFSET:
			l_Upper = m_upperThresholdMinOffset;
			break;

		default:
			// Do nothing.
			break;
	}

	switch( m_lowerThresholdOption )
	{
		case SvDef::SV_USE_SLIDER:
			l_Lower = Lower;
			break;

		case SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF:
			l_Lower = m_lowerThresholdMaxPercentDiff;
			break;

		case SvDef::SV_USE_MAX_MINUS_OFFSET:
			l_Lower = m_lowerThresholdMaxOffset;
			break;

		case SvDef::SV_USE_MIN_PLUS_OFFSET:
			l_Lower = m_lowerThresholdMinOffset;
			break;

		default:
			// Do nothing.
			break;
	}

	return SVEdgeMarkerAdjustmentPageClass::UpdateSliderData(l_Lower, l_Upper );
}

#pragma region Protected Methods

#pragma region Private Methods
void SVProfileEdgeMarkerAdjustmentPage::updateControls()
{
	switch( m_lowerThresholdOption )
	{
		case SvDef::SV_USE_SLIDER:
		m_LowerMaxDiffEditCtrl.EnableWindow(FALSE);
		m_LowerMaxOffsetEditCtrl.EnableWindow(FALSE);
		m_LowerMinOffsetEditCtrl.EnableWindow(FALSE);

		// Set Slider Range to 0 - 255
		setScrollRange( &LowerSliderCtrl, 0, 255 );
		break;

		case SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF:
		m_LowerMaxDiffEditCtrl.EnableWindow(TRUE);
		m_LowerMaxOffsetEditCtrl.EnableWindow(FALSE);
		m_LowerMinOffsetEditCtrl.EnableWindow(FALSE);

		// Set Slider Range to 0 - 100
		setScrollRange( &LowerSliderCtrl, 0, 100 );
		break;

		case SvDef::SV_USE_MAX_MINUS_OFFSET:
		m_LowerMaxDiffEditCtrl.EnableWindow(FALSE);
		m_LowerMaxOffsetEditCtrl.EnableWindow(TRUE);
		m_LowerMinOffsetEditCtrl.EnableWindow(FALSE);

		// Set Slider Range to 0 - 255
		setScrollRange( &LowerSliderCtrl, 0, 255 );
		break;

		case SvDef::SV_USE_MIN_PLUS_OFFSET:
		m_LowerMaxDiffEditCtrl.EnableWindow(FALSE);
		m_LowerMaxOffsetEditCtrl.EnableWindow(FALSE);
		m_LowerMinOffsetEditCtrl.EnableWindow(TRUE);

		// Set Slider Range to 0 - 255
		setScrollRange( &LowerSliderCtrl, 0, 255 );
		break;
	}

	switch( m_upperThresholdOption )
	{
		case SvDef::SV_USE_SLIDER:
		m_UpperMaxDiffEditCtrl.EnableWindow(FALSE);
		m_UpperMaxOffsetEditCtrl.EnableWindow(FALSE);
		m_UpperMinOffsetEditCtrl.EnableWindow(FALSE);

		// Set Slider Range to 0 - 255
		setScrollRange( &UpperSliderCtrl, 0, 255 );
		break;

		case SvDef::SV_USE_MAX_MINUS_PERCENT_DIFF:
		m_UpperMaxDiffEditCtrl.EnableWindow(TRUE);
		m_UpperMaxOffsetEditCtrl.EnableWindow(FALSE);
		m_UpperMinOffsetEditCtrl.EnableWindow(FALSE);

		// Set Slider Range to 0 - 100
		setScrollRange( &UpperSliderCtrl, 0, 100 );
		break;

		case SvDef::SV_USE_MAX_MINUS_OFFSET:
		m_UpperMaxDiffEditCtrl.EnableWindow(FALSE);
		m_UpperMaxOffsetEditCtrl.EnableWindow(TRUE);
		m_UpperMinOffsetEditCtrl.EnableWindow(FALSE);

		// Set Slider Range to 0 - 255
		setScrollRange( &UpperSliderCtrl, 0, 255 );
		break;

		case SvDef::SV_USE_MIN_PLUS_OFFSET:
		m_UpperMaxDiffEditCtrl.EnableWindow(FALSE);
		m_UpperMaxOffsetEditCtrl.EnableWindow(FALSE);
		m_UpperMinOffsetEditCtrl.EnableWindow(TRUE);

		// Set Slider Range to 0 - 255
		setScrollRange( &UpperSliderCtrl, 0, 255 );
		break;
	}
}

void SVProfileEdgeMarkerAdjustmentPage::updateGraphDisplay()
{
	// Remove old points
	m_dialogImage.RemoveAllOverlays(0);

	if (nullptr != m_pAnalyzer)
	{
		//check if graphic vertical
		double rotationAngle = 0;
		m_pAnalyzer->GetImageExtent().GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, rotationAngle);
		bool bVertical = (90 == rotationAngle);

		//set HistogramOverlay and edges
		setGraphOverlayToPicture(bVertical);

		//set Threshold Bars
		DWORD thresholdValue = m_values.Get<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::EdgeLowerThresholdValue]);
		if (0 <= thresholdValue)
		{
			setMarkerOverlayToPicture(thresholdValue, bVertical);
		}
		thresholdValue = m_values.Get<DWORD>(m_rEdgeEmbeddedIds[SvOg::EdgeEmbeddedEnum::EdgeUpperThresholdValue]);
		if (0 <= thresholdValue)
		{
			setMarkerOverlayToPicture(thresholdValue, bVertical);
		}
	}

	// Update the Graph on the Dialog
	m_dialogImage.Refresh();
}

void SVProfileEdgeMarkerAdjustmentPage::setGraphOverlayToPicture(bool bVertical)
{
	if (nullptr != m_pEdge)
	{
		std::vector<double> HistogramData;
		m_pEdge->GetInputLinearVectorData(HistogramData);
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
		ParMap[bVertical ? CDSVPictureDisplay::P_X_Max : CDSVPictureDisplay::P_Y_Max] = m_pEdge->m_dwColorNumber - 1;
		m_dialogImage.AddOverlay(0, ParMap, &handle);

		//set linear Edge lines
		std::vector<double> LinearEdges;
		m_pEdge->m_svLinearEdges.GetArrayValues(LinearEdges);
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
		double distance;
		m_pEdge->GetOutputEdgeDistance(distance);
		points[bVertical ? 0 : 1] = distance; // first point
		points[bVertical ? 2 : 3] = distance; // x of second point
		arraySafe.CreateOneDim(VT_R8, 4, points); // two points are four values
		ParMap[CDSVPictureDisplay::P_Color] = static_cast<long>(m_pEdge->GetObjectColor());
		ParMap[CDSVPictureDisplay::P_SelectedColor] = static_cast<long>(m_pEdge->GetObjectColor());
		ParMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;
		m_dialogImage.AddOverlay(0, ParMap, &handle);

		delete[] points;
	}
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
#pragma endregion Private Methods
