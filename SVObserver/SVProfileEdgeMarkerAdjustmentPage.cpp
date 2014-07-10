//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProfileEdgeMarkerAdjustmentPage
//* .File Name       : $Workfile:   SVProfileEdgeMarkerAdjustmentPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   10 Jul 2014 17:41:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "SVLinearAnalyzerClass.h"
#include "SVLinearEdgeProcessingClass.h"
#include "SVImageToLineProject.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVSVIMStateClass.h"
#include "SVTool.h"
#include "SVOMFCLibrary\DisplayHelper.h"
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

	SetTaskObject( PCurrentAnalyzer );

	if( ! PCurrentAnalyzer )
		GetParent()->SendMessage( WM_CLOSE );

	if( SV_IS_KIND_OF( PCurrentAnalyzer->GetOwner(), SVTaskObjectClass ) )
	{
		pAnalyzerOwner = ( SVTaskObjectClass* ) PCurrentAnalyzer->GetOwner();
	}

	SVLinearAnalyzerClass *l_psvLinear = dynamic_cast<SVLinearAnalyzerClass *>(PCurrentAnalyzer);

	if( nullptr == l_psvLinear )
	{
		if( PCurrentAnalyzer )
		{
			SVObjectTypeInfoStruct lineObjectInfo;
			lineObjectInfo.ObjectType = SVLineObjectType;

			pLine = ( SVLineClass* )::SVSendMessage( PCurrentAnalyzer, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&lineObjectInfo) );
		}
		
		// Get the Thesholding Options from the SVImageToLineProjectClass
		SVObjectTypeInfoStruct info;
		info.ObjectType = SVImageToLineProjectObjectType;

		SVImageToLineProjectClass *pProject = reinterpret_cast<SVImageToLineProjectClass *>(SVSendMessage( PCurrentAnalyzer, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info) ));
	
		if( nullptr != pProject )
		{
			GetParent()->SendMessage( WM_CLOSE );
		}
	}
	
	SVToolClass* pTool = dynamic_cast<SVToolClass*>( PCurrentAnalyzer->GetOwner() );

	RunOnce( pTool );
	GetInspectionData();

	// Get the Image for this tool
	SVImageInfoClass* pImageInfo = ( SVImageInfoClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_IMAGE_INFO, NULL, NULL );
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
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdSelectable, TRUE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusPercentDiff, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusOffset, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMinPlusOffset, (double)FALSE );
			}

			break;
		}

		case USE_MAX_MINUS_PERCENT_DIFF:
		{
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoLowerThresholdMaxMinusPercentDiff, m_lowerThresholdMaxPercentDiff );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdSelectable, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusPercentDiff, TRUE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusOffset, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMinPlusOffset, (double)FALSE );
			}

			break;
		}

		case USE_MAX_MINUS_OFFSET:
		{
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoLowerThresholdMaxMinusOffset, m_lowerThresholdMaxOffset );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdSelectable, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusPercentDiff, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusOffset, TRUE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMinPlusOffset, (double)FALSE );
			}

			break;
		}

		case USE_MIN_PLUS_OFFSET:
		{
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoLowerThresholdMinPlusOffset, m_lowerThresholdMinOffset );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdSelectable, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusPercentDiff, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseLowerThresholdMaxMinusOffset, (double)FALSE );
			}

			if( l_hrOk == S_OK )
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
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdSelectable, TRUE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusPercentDiff, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusOffset, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMinPlusOffset, (double)FALSE );
			}

			break;
		}

		case USE_MAX_MINUS_PERCENT_DIFF:
		{
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUpperThresholdMaxMinusPercentDiff, m_upperThresholdMaxPercentDiff );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdSelectable, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusPercentDiff, TRUE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusOffset, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMinPlusOffset, (double)FALSE );
			}

			break;
		}

		case USE_MAX_MINUS_OFFSET:
		{
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUpperThresholdMaxMinusOffset, m_upperThresholdMaxOffset );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdSelectable, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusPercentDiff, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusOffset, TRUE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMinPlusOffset, (double)FALSE );
			}

			break;
		}

		case USE_MIN_PLUS_OFFSET:
		{
			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUpperThresholdMinPlusOffset, m_upperThresholdMinOffset );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdSelectable, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusPercentDiff, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMaxMinusOffset, (double)FALSE );
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUseUpperThresholdMinPlusOffset, TRUE );
			}

			break;
		}
	}

	SVLinearAnalyzerClass *l_psvLinear = dynamic_cast<SVLinearAnalyzerClass *>(PCurrentAnalyzer);
	
	if( nullptr != l_psvLinear )
	{
		if( m_bEdgeA )
		{
			if( l_hrOk == S_OK && l_psvLinear->m_svShowAllEdgeAOverlays.GetOwner() != nullptr )
			{
				l_hrOk = AddInputRequest( &( l_psvLinear->m_svShowAllEdgeAOverlays ), TRUE );
			}

			if( l_hrOk == S_OK && l_psvLinear->m_svShowAllEdgeBOverlays.GetOwner() != nullptr )
			{
				l_hrOk = AddInputRequest( &( l_psvLinear->m_svShowAllEdgeBOverlays ), (double)FALSE );
			}
		}
		else
		{
			if( l_hrOk == S_OK && l_psvLinear->m_svShowAllEdgeAOverlays.GetOwner() != nullptr )
			{
				l_hrOk = AddInputRequest( &( l_psvLinear->m_svShowAllEdgeAOverlays ), (double)FALSE );
			}

			if( l_hrOk == S_OK && l_psvLinear->m_svShowAllEdgeBOverlays.GetOwner() != nullptr )
			{
				l_hrOk = AddInputRequest( &( l_psvLinear->m_svShowAllEdgeBOverlays ), TRUE );
			}
		}
	}

	if( l_hrOk == S_OK )
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
	SVLinearAnalyzerClass *l_psvLinear = dynamic_cast<SVLinearAnalyzerClass *>(PCurrentAnalyzer);

	// Remove old points
	m_dialogImage.RemoveAllOverlays(0);

	if( nullptr != l_psvLinear )
	{
		SVLinearEdgeProcessingClass* pEdge = nullptr;
		if( m_bEdgeA )
		{
			pEdge = l_psvLinear->GetEdgeA();
		}
		else
		{
			pEdge = l_psvLinear->GetEdgeB();
		}

		if( nullptr == pEdge )
		{
			return;
		}


		//set HistogramOverlay and edges
		setGraphOverlayToPicture(pEdge);

		//set Threshold Bars
		long thresholdValue = pEdge->getLowerThresholdValue();
		if ( 0 <= thresholdValue )
		{
			setMarkerOverlayToPicture(thresholdValue);
		}
		thresholdValue = pEdge->getUpperThresholdValue();
		if ( 0 <= thresholdValue )
		{
			setMarkerOverlayToPicture(thresholdValue);
		}
	}

	// Update the Graph on the Dialog
	m_dialogImage.Refresh();
}

void SVProfileEdgeMarkerAdjustmentPageClass::setGraphOverlayToPicture( SVLinearEdgeProcessingClass* pEdge )
{
	std::vector<double> histogramData;
	pEdge->GetInputLinearData()->GetValues( histogramData );
	long handle = -1;
	size_t sizePointsArray = histogramData.size();
	double* points = new double[sizePointsArray * 2];
	for (size_t i = 0; i < sizePointsArray; i++)
	{
		points[i*2] = static_cast<double>(i);
		points[i*2+1] = static_cast<double>(histogramData[i]);
	}

	COleSafeArray arraySafe;
	arraySafe.CreateOneDim(VT_R8, static_cast< DWORD >( sizePointsArray * 2 ), points);
	std::map<long,_variant_t> ParMap;
	ParMap[CDSVPictureDisplay::P_Type ] = static_cast<long>(CDSVPictureDisplay::GraphROI);
	ParMap[CDSVPictureDisplay::P_SubType ] = static_cast<long>(CDSVPictureDisplay::Scale2ViewAreaPerParameter);
	ParMap[CDSVPictureDisplay::P_Color] = static_cast<long>(SV_DEFAULT_SUB_FUNCTION_COLOR_1);
	ParMap[CDSVPictureDisplay::P_SelectedColor] = static_cast<long>(SV_DEFAULT_SUB_FUNCTION_COLOR_1);
	ParMap[CDSVPictureDisplay::P_AllowEdit] = static_cast<long>(0);
	ParMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;
	ParMap[CDSVPictureDisplay::P_Is_Flip_Vertical] = true;
	ParMap[CDSVPictureDisplay::P_X_Min] = 0;
	ParMap[CDSVPictureDisplay::P_X_Max] = sizePointsArray;
	ParMap[CDSVPictureDisplay::P_Y_Min] = 0;
	ParMap[CDSVPictureDisplay::P_Y_Max] = pEdge->m_dwColorNumber-1;

	m_dialogImage.AddOverlay( 0, ParMap, &handle );

	//set linear Edge lines
	std::vector<double> linearEdge;
	pEdge->m_svLinearEdges.GetValues( linearEdge );
	ParMap[CDSVPictureDisplay::P_Color] = static_cast<long>(SV_DEFAULT_SUB_FUNCTION_COLOR_2);
	ParMap[CDSVPictureDisplay::P_SelectedColor] = static_cast<long>(SV_DEFAULT_SUB_FUNCTION_COLOR_2);
	ParMap[CDSVPictureDisplay::P_Is_Flip_Vertical] = false;
	ParMap[CDSVPictureDisplay::P_Y_Max] = m_egdeLinesGraphMaxY;
	points[1] = m_egdeLinesStartPos; // y of first point
	points[3] = m_egdeLinesStopPos; // y of second point
	for (size_t i = 0; i < linearEdge.size(); i++)
	{
		points[0] = linearEdge[i]; // x of first point
		points[2] = linearEdge[i]; // x of second point
		arraySafe.CreateOneDim(VT_R8, 4, points); // two points are four values
		ParMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;
		m_dialogImage.AddOverlay(0, ParMap, &handle );
	}

	//set selected Edge line
	double distance;
	pEdge->GetOutputEdgeDistance( distance );
	points[0] = distance; // x of first point
	points[2] = distance; // x of second point
	arraySafe.CreateOneDim(VT_R8, 4, points); // two points are four values
	ParMap[CDSVPictureDisplay::P_Color] = static_cast<long>(pEdge->GetObjectColor());
	ParMap[CDSVPictureDisplay::P_SelectedColor] = static_cast<long>(pEdge->GetObjectColor());
	ParMap[CDSVPictureDisplay::P_ARRAY_XY] = arraySafe;
	m_dialogImage.AddOverlay(0, ParMap, &handle );

	delete[] points;
}

void SVProfileEdgeMarkerAdjustmentPageClass::setMarkerOverlayToPicture( unsigned long value, long allowType /*= CDSVPictureDisplay::AllowNone */ )
{
	long handle = -1;

	std::map<long,_variant_t> ParMap;
	ParMap[CDSVPictureDisplay::P_Type ] = static_cast<long>(CDSVPictureDisplay::MarkerROI);
	ParMap[CDSVPictureDisplay::P_SubType ] = static_cast<long>(CDSVPictureDisplay::HorizontalMarker);
	ParMap[CDSVPictureDisplay::P_Color] = static_cast<long>(SV_DEFAULT_SUB_FUNCTION_COLOR_1);
	ParMap[CDSVPictureDisplay::P_SelectedColor] = static_cast<long>(SV_DEFAULT_SUB_FUNCTION_COLOR_1);
	ParMap[CDSVPictureDisplay::P_AllowEdit] = static_cast<long>(allowType);
	ParMap[CDSVPictureDisplay::P_Min] = static_cast<long>(0);
	ParMap[CDSVPictureDisplay::P_Max] = static_cast<long>(255);
	ParMap[CDSVPictureDisplay::P_VALUE] = static_cast<long>(255 - value);

	m_dialogImage.AddOverlay(0, ParMap, &handle );
}
#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVProfileEdgeMarkerAdjustmentPage.cpp_v  $
 * 
 *    Rev 1.5   10 Jul 2014 17:41:14   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  902
 * SCR Title:  Change Complex Dialog Image Displays to Use SVPictureDisplay ActiveX
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   new implementation for setGraphOverlayToPicture and use setMarkerOverlayToPicture instead of setLineOverlayToPicture, 
 * to send real value instead of calculated values under certain conditions invalid graph points.
 * remove if-part for lineAnalyzer, because no longer used (old profile tool)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jul 2014 13:06:46   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  902
 * SCR Title:  Change Complex Dialog Image Displays to Use SVPictureDisplay ActiveX
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * cleanup (e.g. use static_cast instead of c-style cast)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2014 12:31:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 12:00:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 08:20:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   22 Apr 2013 14:24:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  811
 * SCR Title:  Fix Linear and profile Tool Threshold Slider for Max - % Dif
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified validation for percent. Used to be BYTE validation, Now it is 0 to 100.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:32:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   01 Aug 2012 13:04:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   25 Jul 2012 14:43:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code and updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   09 Jul 2012 14:11:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   19 Jun 2012 13:49:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   14 Jul 2011 09:01:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to better track resource management, and clear un-used resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   08 Dec 2010 13:29:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   26 Oct 2010 13:46:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  706
 * SCR Title:  Fix Histogram Drawing Problem in Linear Analyzer Dialogs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix indexing issue in dialog associated with the histogram drawing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   16 Dec 2009 12:29:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   30 Jul 2009 12:23:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   05 Oct 2005 11:06:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added code to check if value objects have an owner before access.  This prevent un-register embedded objects from being accessed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   29 Aug 2005 16:22:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added RunOnce to OnInitDialog so Display will have its figures initially updated.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   16 Aug 2005 09:42:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   12 Aug 2005 13:53:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to fix issues with data synchrization.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   09 Aug 2005 08:08:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   19 Jul 2005 14:49:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented new value object Get/Set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   18 Jul 2005 11:50:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added static_cast for setting Threshold Value Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   18 Feb 2005 11:08:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated configuration page to allow configuration for the linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   17 Feb 2005 15:22:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   18 Dec 2003 14:00:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed sub pixel position for fixed edge markers and correct dialog display logic for dual edge profile analyzers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   09 Dec 2003 12:30:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated line analyzer dialogs to store edge control parameters differently in the value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   04 Dec 2003 13:13:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   21 Nov 2003 11:55:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with value objects.  Was setting to last set and not to bucket 1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   08 Oct 2003 11:09:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   using the value objects for the edge parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   12 Aug 2003 09:05:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   adjusted for m_ prefix change in SVDlgImageGraphClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   22 Apr 2003 14:19:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   20 Nov 2002 11:06:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 * Changed ValueObject semantics to work with buckets
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   12 Dec 2001 16:24:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  258
 * SCR Title:  Profile: LinearMeasurement Analyzer doesn't display edges correctly on dialog
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added code to ::OnInitDialog to force the edge control to the right edge so that the dialog displays correctly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   04 Oct 2001 17:04:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 May 2001 15:53:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code in:
 * updateGraphDisplay
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   06 Sep 2000 16:34:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Jun 2000 13:46:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  158
 * SCR Title:  Profile Tool Lower Threshold Setup problem
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added a seperate SVValueBAseNormalizerClass for the Lower Threshold.
 * Revised OnInitDialog to call initEdgeParameters.
 * Revised OnHScroll to use LowerNormalizer
 * Revised OnUpperUserSelectable to update the display.
 * Revised OnLowerUserSelectable to update the display.
 * Revised SetScrollRange to use the uper or lower normalizer.
 * Revised SetScrollPos to use the uper or lower normalizer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   26 Apr 2000 19:00:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added include for SVDlgImageGraph.h.
 * Added SVDlgImageGraph control to the dialog.
 * Revised OnInitDialog to set points (figures) for the 
 * SVDlgImageGraph control.
 * Revised OnSetActive to call 
 * SVDlgImageGraph::RefreshPoints method.
 * Revised UpdateGraphDisplay to call
 * SVDlgImageGraph::RefreshPoints method.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Mar 2000 16:36:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised UpdatedGraphDisplay to only update display once per call.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Feb 2000 15:07:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to correct call to base class constructors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Feb 2000 09:11:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin. New Dialog class for setup of thresholding
 * options for the line analyzers in the profile tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/   
