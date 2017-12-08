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
#include "Definitions/Color.h"
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "SVLinearAnalyzerClass.h"
#include "SVLinearEdgeProcessingClass.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVOCore/SVTool.h"
#include "SVOGui/DisplayHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define USE_SLIDER					0
#define USE_MAX_MINUS_PERCENT_DIFF	1
#define USE_MAX_MINUS_OFFSET		2
#define USE_MIN_PLUS_OFFSET			3

IMPLEMENT_DYNCREATE( SVProfileEdgeMarkerAdjustmentPageClass, SVEdgeMarkerAdjustmentPageClass )
#pragma endregion Declarations

#pragma region Constructor
SVProfileEdgeMarkerAdjustmentPageClass::SVProfileEdgeMarkerAdjustmentPageClass( UINT nIDCaption /* = 0 */ ) 
: SVEdgeMarkerAdjustmentPageClass( nIDCaption, SVProfileEdgeMarkerAdjustmentPageClass::IDD )
, m_lowerThresholdMaxOffset( 0 )
, m_lowerThresholdMinOffset( 0 )
, m_lowerThresholdMaxPercentDiff( 0 )
, m_upperThresholdMaxOffset( 0 )
, m_upperThresholdMinOffset( 0 )
, m_upperThresholdMaxPercentDiff( 0 )
, m_lowerThresholdOption( 0 )
, m_upperThresholdOption( 0 )
{
	//{{AFX_DATA_INIT(SVProfileEdgeMarkerAdjustmentPageClass)
	//}}AFX_DATA_INIT
}

SVProfileEdgeMarkerAdjustmentPageClass::~SVProfileEdgeMarkerAdjustmentPageClass()
{
}
#pragma endregion Constructor

#pragma region MFC Methods
void SVProfileEdgeMarkerAdjustmentPageClass::OnCancel()
{
	SVEdgeMarkerAdjustmentPageClass::OnCancel();
}

BOOL SVProfileEdgeMarkerAdjustmentPageClass::OnSetActive()
{
	BOOL bRetVal = SVEdgeMarkerAdjustmentPageClass::OnSetActive();

	// Update point sets (needed for 2 edges)
	m_dialogImage.Refresh();

	return bRetVal;
}

void SVProfileEdgeMarkerAdjustmentPageClass::DoDataExchange( CDataExchange* pDX )
{
	SVEdgeMarkerAdjustmentPageClass::DoDataExchange( pDX );
	//{{AFX_DATA_MAP(SVProfileEdgeMarkerAdjustmentPageClass)
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

BOOL SVProfileEdgeMarkerAdjustmentPageClass::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SetTaskObject( m_pCurrentAnalyzer );

	if( ! m_pCurrentAnalyzer )
		GetParent()->SendMessage( WM_CLOSE );

	SVToolClass* pTool = dynamic_cast<SVToolClass*>( m_pCurrentAnalyzer->GetOwner() );

	RunOnce( pTool->GetUniqueObjectID() );
	GetInspectionData();

	// Get the Image for this tool
	const SVImageInfoClass* pImageInfo = pTool->getFirstImageInfo();
	if( nullptr != pImageInfo )
	{
		SVImageClass* pImage = nullptr;
		pImageInfo->GetOwnerImage( pImage );
		m_dialogImage.AddTab("Image");
		m_dialogImage.setImage( pImage, 0 );
		m_dialogImage.Refresh();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	SVEdgeMarkerAdjustmentPageClass::OnHScroll( nSBCode, nPos, pScrollBar );

	UpdateData( TRUE ); // get data of dialog	

	if( &UpperSliderCtrl == ( CSliderCtrl* ) pScrollBar )
	{
		switch( m_upperThresholdOption )
		{
			case USE_SLIDER:
			double dUpper;
			dUpper = Normalizer.CalcRealValue( ( double ) UpperSliderCtrl.GetPos() );
			StrUpper.Format( "%u", (int)dUpper );
			break;

			case USE_MAX_MINUS_PERCENT_DIFF:
			m_upperThresholdMaxPercentDiff = (unsigned char) Normalizer.CalcRealValue( ( double ) UpperSliderCtrl.GetPos() );
			StrUpper.Format( "%u", m_upperThresholdMaxPercentDiff );
			break;

			case USE_MAX_MINUS_OFFSET:
			m_upperThresholdMaxOffset =	(unsigned char) Normalizer.CalcRealValue( ( double ) UpperSliderCtrl.GetPos() );
			StrUpper.Format( "%u", m_upperThresholdMaxOffset );
			break;

			case USE_MIN_PLUS_OFFSET:
			m_upperThresholdMinOffset = (unsigned char) Normalizer.CalcRealValue( ( double ) UpperSliderCtrl.GetPos() );
			StrUpper.Format( "%u", m_upperThresholdMinOffset );
			break;
		}
	}

	if( &LowerSliderCtrl == ( CSliderCtrl* ) pScrollBar )
	{
		switch( m_lowerThresholdOption )
		{
			case USE_SLIDER:
			double dLower;
			dLower = LowerNormalizer.CalcRealValue( ( double ) LowerSliderCtrl.GetPos() );
			StrLower.Format( "%u", (int)dLower );
			break;

			case USE_MAX_MINUS_PERCENT_DIFF:
			m_lowerThresholdMaxPercentDiff = (unsigned char) LowerNormalizer.CalcRealValue( ( double ) LowerSliderCtrl.GetPos() );
			StrLower.Format( "%u", m_lowerThresholdMaxPercentDiff );
			break;

			case USE_MAX_MINUS_OFFSET:
			m_lowerThresholdMaxOffset = (unsigned char) LowerNormalizer.CalcRealValue( ( double ) LowerSliderCtrl.GetPos() );
			StrLower.Format( "%u", m_lowerThresholdMaxOffset );
			break;

			case USE_MIN_PLUS_OFFSET:
			m_lowerThresholdMinOffset = (unsigned char) LowerNormalizer.CalcRealValue( ( double ) LowerSliderCtrl.GetPos() );
			StrLower.Format( "%u", m_lowerThresholdMinOffset );
			break;
		}
	}

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnUpperUserSelectable()
{
	UpdateData(TRUE);

	double dUpper;

	dUpper = Normalizer.CalcRealValue( ( double ) UpperSliderCtrl.GetPos() );

	StrUpper.Format( "%u", (int)dUpper );

	UpdateData(FALSE);

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnLowerUserSelectable()
{
	UpdateData(TRUE);

	double dLower;

	dLower = LowerNormalizer.CalcRealValue( ( double ) LowerSliderCtrl.GetPos() );

	StrLower.Format( "%u", (int)dLower );

	UpdateData(FALSE);

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnUpperUseMaxMinusDiff()
{
	UpdateData(TRUE);

	StrUpper.Format( "%u", m_upperThresholdMaxPercentDiff );

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnUpperUseMaxMinusOffset()
{
	UpdateData(TRUE);

	StrUpper.Format( "%u", m_upperThresholdMaxOffset );

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnUpperUseMinPlusOffset()
{
	UpdateData(TRUE);

	StrUpper.Format( "%u", m_upperThresholdMinOffset );

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnLowerUseMaxMinusDiff()
{
	UpdateData(TRUE);

	StrLower.Format( "%u", m_lowerThresholdMaxPercentDiff );

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnLowerUseMaxMinusOffset()
{
	UpdateData(TRUE);

	StrLower.Format( "%u", m_lowerThresholdMaxOffset );

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnLowerUseMinPlusOffset()
{
	UpdateData(TRUE);

	StrLower.Format( "%u", m_lowerThresholdMinOffset );

	UpdateData( FALSE ); // set data to dialog

	SetInspectionData();
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnChangeUpperPercentDiff()
{
	if( m_upperThresholdOption != USE_SLIDER )
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

void SVProfileEdgeMarkerAdjustmentPageClass::OnChangeUpperMaxOffset()
{
	if( m_upperThresholdOption != USE_SLIDER )
	{
		UpdateData(TRUE);

		StrUpper.Format( "%u", m_upperThresholdMaxOffset );

		UpdateData( FALSE ); // set data to dialog

		SetInspectionData();
	}
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnChangeUpperMinOffset()
{
	if( m_upperThresholdOption != USE_SLIDER )
	{
		UpdateData(TRUE);

		StrUpper.Format( "%u", m_upperThresholdMinOffset );

		UpdateData( FALSE ); // set data to dialog

		SetInspectionData();
	}
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnChangeLowerPercentDiff()
{
	if( m_lowerThresholdOption != USE_SLIDER )
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

void SVProfileEdgeMarkerAdjustmentPageClass::OnChangeLowerMaxOffset()
{
	if( m_lowerThresholdOption != USE_SLIDER )
	{
		UpdateData(TRUE);

		StrLower.Format( "%u", m_lowerThresholdMaxOffset );

		UpdateData( FALSE ); // set data to dialog

		SetInspectionData();
	}
}

void SVProfileEdgeMarkerAdjustmentPageClass::OnChangeLowerMinOffset()
{
	if( m_lowerThresholdOption != USE_SLIDER )
	{
		UpdateData(TRUE);

		StrLower.Format( "%u", m_lowerThresholdMinOffset );

		UpdateData( FALSE ); // set data to dialog

		SetInspectionData();
	}
}

BEGIN_MESSAGE_MAP( SVProfileEdgeMarkerAdjustmentPageClass, SVEdgeMarkerAdjustmentPageClass )
	//{{AFX_MSG_MAP(SVProfileEdgeMarkerAdjustmentPageClass)
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
HRESULT SVProfileEdgeMarkerAdjustmentPageClass::GetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	BOOL state;

	m_pvoUseLowerThresholdSelectable->GetValue( state );
	if( state )
	{
		m_lowerThresholdOption = USE_SLIDER;
	}
	else
	{
		m_pvoUseLowerThresholdMaxMinusPercentDiff->GetValue( state );
		if( state )
		{
			m_lowerThresholdOption = USE_MAX_MINUS_PERCENT_DIFF;
		}
		else
		{
			m_pvoUseLowerThresholdMaxMinusOffset->GetValue( state );
			if( state )
			{
				m_lowerThresholdOption = USE_MAX_MINUS_OFFSET;
			}
			else
			{
				m_pvoUseLowerThresholdMinPlusOffset->GetValue( state );
				if( state )
				{
					m_lowerThresholdOption = USE_MIN_PLUS_OFFSET;
				}
			}
		}
	}

	DWORD value;

	m_pvoLowerThresholdMaxMinusPercentDiff->GetValue( value );
	m_lowerThresholdMaxPercentDiff = (unsigned char)value;

	m_pvoLowerThresholdMaxMinusOffset->GetValue( value );
	m_lowerThresholdMaxOffset = (unsigned char)value;

	m_pvoLowerThresholdMinPlusOffset->GetValue( value );
	m_lowerThresholdMinOffset = (unsigned char)value;

	m_pvoUseUpperThresholdSelectable->GetValue( state );
	if( state )
	{
		m_upperThresholdOption = USE_SLIDER;
	}
	else
	{
		m_pvoUseUpperThresholdMaxMinusPercentDiff->GetValue( state );
		if( state )
		{
			m_upperThresholdOption = USE_MAX_MINUS_PERCENT_DIFF;
		}
		else
		{
			m_pvoUseUpperThresholdMaxMinusOffset->GetValue( state );
			if( state )
			{
				m_upperThresholdOption = USE_MAX_MINUS_OFFSET;
			}
			else
			{
				m_pvoUseUpperThresholdMinPlusOffset->GetValue( state );
				if( state )
				{
					m_upperThresholdOption = USE_MIN_PLUS_OFFSET;
				}
			}
		}
	}

	m_pvoUpperThresholdMaxMinusPercentDiff->GetValue( value );
	m_upperThresholdMaxPercentDiff = (unsigned char)value;

	m_pvoUpperThresholdMaxMinusOffset->GetValue( value );
	m_upperThresholdMaxOffset = (unsigned char)value;

	m_pvoUpperThresholdMinPlusOffset->GetValue( value );
	m_upperThresholdMinOffset = (unsigned char)value;

	UpdateData( FALSE ); // set data to dialog

	updateControls();

	l_hrOk = SVEdgeMarkerAdjustmentPageClass::GetInspectionData();

	updateGraphDisplay();

	return l_hrOk;
}

HRESULT SVProfileEdgeMarkerAdjustmentPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	UpdateData( TRUE );

	switch( m_lowerThresholdOption )
	{
		case USE_SLIDER:
		{
			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdSelectable, TRUE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusPercentDiff, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusOffset, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMinPlusOffset, (double)FALSE );
			}

			break;
		}

		case USE_MAX_MINUS_PERCENT_DIFF:
		{
			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoLowerThresholdMaxMinusPercentDiff, m_lowerThresholdMaxPercentDiff );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdSelectable, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusPercentDiff, TRUE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusOffset, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMinPlusOffset, (double)FALSE );
			}

			break;
		}

		case USE_MAX_MINUS_OFFSET:
		{
			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoLowerThresholdMaxMinusOffset, m_lowerThresholdMaxOffset );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdSelectable, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusPercentDiff, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusOffset, TRUE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMinPlusOffset, (double)FALSE );
			}

			break;
		}

		case USE_MIN_PLUS_OFFSET:
		{
			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoLowerThresholdMinPlusOffset, m_lowerThresholdMinOffset );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdSelectable, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusPercentDiff, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusOffset, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMinPlusOffset, TRUE );
			}

			break;
		}
	}

	switch( m_upperThresholdOption )
	{
		case USE_SLIDER:
		{
			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdSelectable, TRUE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusPercentDiff, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusOffset, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMinPlusOffset, (double)FALSE );
			}

			break;
		}

		case USE_MAX_MINUS_PERCENT_DIFF:
		{
			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUpperThresholdMaxMinusPercentDiff, m_upperThresholdMaxPercentDiff );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdSelectable, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusPercentDiff, TRUE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusOffset, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMinPlusOffset, (double)FALSE );
			}

			break;
		}

		case USE_MAX_MINUS_OFFSET:
		{
			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUpperThresholdMaxMinusOffset, m_upperThresholdMaxOffset );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdSelectable, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusPercentDiff, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusOffset, TRUE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMinPlusOffset, (double)FALSE );
			}

			break;
		}

		case USE_MIN_PLUS_OFFSET:
		{
			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUpperThresholdMinPlusOffset, m_upperThresholdMinOffset );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdSelectable, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusPercentDiff, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusOffset, (double)FALSE );
			}

			if( S_OK == l_hrOk )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMinPlusOffset, TRUE );
			}

			break;
		}
	}

	SVLinearAnalyzerClass* l_psvLinear = dynamic_cast<SVLinearAnalyzerClass *>(m_pCurrentAnalyzer);
	
	if( nullptr != l_psvLinear )
	{
		if( m_bEdgeA )
		{
			if( S_OK == l_hrOk && nullptr != l_psvLinear->m_svShowAllEdgeAOverlays.GetOwner() )
			{
				l_hrOk = AddInputRequest( &( l_psvLinear->m_svShowAllEdgeAOverlays ), TRUE );
			}

			if( S_OK == l_hrOk && nullptr != l_psvLinear->m_svShowAllEdgeBOverlays.GetOwner() )
			{
				l_hrOk = AddInputRequest( &( l_psvLinear->m_svShowAllEdgeBOverlays ), (double)FALSE );
			}
		}
		else
		{
			if( S_OK == l_hrOk && nullptr != l_psvLinear->m_svShowAllEdgeAOverlays.GetOwner() )
			{
				l_hrOk = AddInputRequest( &( l_psvLinear->m_svShowAllEdgeAOverlays ), (double)FALSE );
			}

			if( S_OK == l_hrOk && nullptr != l_psvLinear->m_svShowAllEdgeBOverlays.GetOwner() )
			{
				l_hrOk = AddInputRequest( &( l_psvLinear->m_svShowAllEdgeBOverlays ), TRUE );
			}
		}
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = SVEdgeMarkerAdjustmentPageClass::SetInspectionData();
	}

	return l_hrOk;
}
#pragma endregion Public Methods

#pragma region Protected Methods
HRESULT SVProfileEdgeMarkerAdjustmentPageClass::UpdateSliderData( double p_dLower, double p_dUpper )
{
	double l_dUpper = 0.0;
	double l_dLower = 0.0;

	switch( m_upperThresholdOption )
	{
		case USE_SLIDER:
			l_dUpper = p_dUpper;
			break;

		case USE_MAX_MINUS_PERCENT_DIFF:
			l_dUpper = m_upperThresholdMaxPercentDiff;
			break;

		case USE_MAX_MINUS_OFFSET:
			l_dUpper = m_upperThresholdMaxOffset;
			break;

		case USE_MIN_PLUS_OFFSET:
			l_dUpper = m_upperThresholdMinOffset;
			break;

		default:
			// Do nothing.
			break;
	}

	switch( m_lowerThresholdOption )
	{
		case USE_SLIDER:
			l_dLower = p_dLower;
			break;

		case USE_MAX_MINUS_PERCENT_DIFF:
			l_dLower = m_lowerThresholdMaxPercentDiff;
			break;

		case USE_MAX_MINUS_OFFSET:
			l_dLower = m_lowerThresholdMaxOffset;
			break;

		case USE_MIN_PLUS_OFFSET:
			l_dLower = m_lowerThresholdMinOffset;
			break;

		default:
			// Do nothing.
			break;
	}

	return SVEdgeMarkerAdjustmentPageClass::UpdateSliderData( l_dLower, l_dUpper );
}

#pragma region Protected Methods

#pragma region Private Methods
void SVProfileEdgeMarkerAdjustmentPageClass::updateControls()
{
	switch( m_lowerThresholdOption )
	{
		case USE_SLIDER:
		m_LowerMaxDiffEditCtrl.EnableWindow(FALSE);
		m_LowerMaxOffsetEditCtrl.EnableWindow(FALSE);
		m_LowerMinOffsetEditCtrl.EnableWindow(FALSE);

		// Set Slider Range to 0 - 255
		setScrollRange( &LowerSliderCtrl, 0, 255 );
		break;

		case USE_MAX_MINUS_PERCENT_DIFF:
		m_LowerMaxDiffEditCtrl.EnableWindow(TRUE);
		m_LowerMaxOffsetEditCtrl.EnableWindow(FALSE);
		m_LowerMinOffsetEditCtrl.EnableWindow(FALSE);

		// Set Slider Range to 0 - 100
		setScrollRange( &LowerSliderCtrl, 0, 100 );
		break;

		case USE_MAX_MINUS_OFFSET:
		m_LowerMaxDiffEditCtrl.EnableWindow(FALSE);
		m_LowerMaxOffsetEditCtrl.EnableWindow(TRUE);
		m_LowerMinOffsetEditCtrl.EnableWindow(FALSE);

		// Set Slider Range to 0 - 255
		setScrollRange( &LowerSliderCtrl, 0, 255 );
		break;

		case USE_MIN_PLUS_OFFSET:
		m_LowerMaxDiffEditCtrl.EnableWindow(FALSE);
		m_LowerMaxOffsetEditCtrl.EnableWindow(FALSE);
		m_LowerMinOffsetEditCtrl.EnableWindow(TRUE);

		// Set Slider Range to 0 - 255
		setScrollRange( &LowerSliderCtrl, 0, 255 );
		break;
	}

	switch( m_upperThresholdOption )
	{
		case USE_SLIDER:
		m_UpperMaxDiffEditCtrl.EnableWindow(FALSE);
		m_UpperMaxOffsetEditCtrl.EnableWindow(FALSE);
		m_UpperMinOffsetEditCtrl.EnableWindow(FALSE);

		// Set Slider Range to 0 - 255
		setScrollRange( &UpperSliderCtrl, 0, 255 );
		break;

		case USE_MAX_MINUS_PERCENT_DIFF:
		m_UpperMaxDiffEditCtrl.EnableWindow(TRUE);
		m_UpperMaxOffsetEditCtrl.EnableWindow(FALSE);
		m_UpperMinOffsetEditCtrl.EnableWindow(FALSE);

		// Set Slider Range to 0 - 100
		setScrollRange( &UpperSliderCtrl, 0, 100 );
		break;

		case USE_MAX_MINUS_OFFSET:
		m_UpperMaxDiffEditCtrl.EnableWindow(FALSE);
		m_UpperMaxOffsetEditCtrl.EnableWindow(TRUE);
		m_UpperMinOffsetEditCtrl.EnableWindow(FALSE);

		// Set Slider Range to 0 - 255
		setScrollRange( &UpperSliderCtrl, 0, 255 );
		break;

		case USE_MIN_PLUS_OFFSET:
		m_UpperMaxDiffEditCtrl.EnableWindow(FALSE);
		m_UpperMaxOffsetEditCtrl.EnableWindow(FALSE);
		m_UpperMinOffsetEditCtrl.EnableWindow(TRUE);

		// Set Slider Range to 0 - 255
		setScrollRange( &UpperSliderCtrl, 0, 255 );
		break;
	}
}

void SVProfileEdgeMarkerAdjustmentPageClass::updateGraphDisplay()
{
	BOOL l_bOk = TRUE;
	SVLinearAnalyzerClass* psvLinear = dynamic_cast<SVLinearAnalyzerClass *>(m_pCurrentAnalyzer);

	// Remove old points
	m_dialogImage.RemoveAllOverlays(0);

	if( nullptr != psvLinear )
	{
		SVLinearEdgeProcessingClass* pEdge = nullptr;
		if( m_bEdgeA )
		{
			pEdge = psvLinear->GetEdgeA();
		}
		else
		{
			pEdge = psvLinear->GetEdgeB();
		}

		if( nullptr == pEdge )
		{
			return;
		}

		//check if graphic vertical
		SVImageExtentClass svAnalyzerExtents;
		psvLinear->GetImageExtent(svAnalyzerExtents);
		double rotationAngle = 0;
		svAnalyzerExtents.GetPosition().GetExtentProperty(SVExtentPropertyRotationAngle, rotationAngle);
		bool bVertical = (90 == rotationAngle);

		//set HistogramOverlay and edges
		setGraphOverlayToPicture(pEdge, bVertical);

		//set Threshold Bars
		long thresholdValue = pEdge->getLowerThresholdValue();
		if ( 0 <= thresholdValue )
		{
			setMarkerOverlayToPicture(thresholdValue, bVertical);
		}
		thresholdValue = pEdge->getUpperThresholdValue();
		if ( 0 <= thresholdValue )
		{
			setMarkerOverlayToPicture(thresholdValue, bVertical);
		}
	}

	// Update the Graph on the Dialog
	m_dialogImage.Refresh();
}

void SVProfileEdgeMarkerAdjustmentPageClass::setGraphOverlayToPicture(SVLinearEdgeProcessingClass* pEdge, bool bVertical)
{
	std::vector<double> HistogramData;
	pEdge->GetInputLinearData(HistogramData);
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
	ParMap[bVertical ? CDSVPictureDisplay::P_X_Max : CDSVPictureDisplay::P_Y_Max] = pEdge->m_dwColorNumber - 1;
	m_dialogImage.AddOverlay(0, ParMap, &handle);

	//set linear Edge lines
	std::vector<double> LinearEdges;
	pEdge->m_svLinearEdges.GetArrayValues(LinearEdges);
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
	pEdge->GetOutputEdgeDistance(distance);
	points[bVertical ? 0 : 1] = distance; // first point
	points[bVertical ? 2 : 3] = distance; // x of second point
	arraySafe.CreateOneDim(VT_R8, 4, points); // two points are four values
	ParMap[CDSVPictureDisplay::P_Color] = static_cast<long>(pEdge->GetObjectColor());
	ParMap[CDSVPictureDisplay::P_SelectedColor] = static_cast<long>(pEdge->GetObjectColor());
	ParMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;
	m_dialogImage.AddOverlay(0, ParMap, &handle);

	delete[] points;
}

void SVProfileEdgeMarkerAdjustmentPageClass::setMarkerOverlayToPicture(unsigned long value, bool bVertical, long allowType /*= CDSVPictureDisplay::AllowNone */)
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

