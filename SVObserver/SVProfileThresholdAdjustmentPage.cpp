//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProfileThresholdAdjustmentPage
//* .File Name       : $Workfile:   SVProfileThresholdAdjustmentPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 11:59:58  $
//******************************************************************************

#include "stdafx.h"
#include "SVIPDoc.h"
#include "SVImageToLineProject.h"
#include "SVProfileThresholdAdjustmentPage.h"
#include "SVSVIMStateClass.h"
#include "SVLinePixelCountingAnalyzer.h"
#include "SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define USE_SLIDER					0
#define USE_MAX_MINUS_PERCENT_DIFF	1
#define USE_MIN_PLUS_OFFSET			2
#define USE_MAX_MINUS_OFFSET		3

/////////////////////////////////////////////////////////////////////////////
// SVProfileThresholdAdjustmentPageClass 

IMPLEMENT_DYNCREATE(SVProfileThresholdAdjustmentPageClass, SVThresholdAdjustmentPageClass)

HRESULT SVProfileThresholdAdjustmentPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	UpdateData(TRUE);

	switch( m_lowerThresholdOption )
	{
		case USE_SLIDER:
		{
			double dLower;

			dLower = LowerNormalizer.CalcRealValue( ( double ) LowerSliderCtrl.GetPos() );

			StrLower.Format( "%u", (int)dLower );

			setScrollPos( &LowerSliderCtrl, (int) dLower );

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoLowerThreshold, dLower );
			}

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
			m_lowerThresholdMaxPercentDiff = (unsigned char) LowerNormalizer.CalcRealValue( ( double ) LowerSliderCtrl.GetPos() );

			StrLower.Format( "%u", m_lowerThresholdMaxPercentDiff );

			setScrollPos( &LowerSliderCtrl, m_lowerThresholdMaxPercentDiff );

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
			m_lowerThresholdMaxOffset = (unsigned char) LowerNormalizer.CalcRealValue( ( double ) LowerSliderCtrl.GetPos() );

			StrLower.Format( "%u", m_lowerThresholdMaxOffset );

			setScrollPos( &LowerSliderCtrl, m_lowerThresholdMaxOffset );

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
			m_lowerThresholdMinOffset = (unsigned char) LowerNormalizer.CalcRealValue( ( double ) LowerSliderCtrl.GetPos() );

			StrLower.Format( "%u", m_lowerThresholdMinOffset );

			setScrollPos( &LowerSliderCtrl, m_lowerThresholdMinOffset );

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
			double dUpper;

			dUpper = Normalizer.CalcRealValue( ( double ) UpperSliderCtrl.GetPos() );

			StrUpper.Format( "%u", (int)dUpper );

			setScrollPos( &UpperSliderCtrl, (int) dUpper );

			if( l_hrOk == S_OK )
			{
				l_hrOk = AddInputRequest( m_pvoUpperThreshold, dUpper );
			}

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
			m_upperThresholdMaxPercentDiff = (unsigned char) Normalizer.CalcRealValue( ( double ) UpperSliderCtrl.GetPos() );

			StrUpper.Format( "%u", m_upperThresholdMaxPercentDiff );

			setScrollPos( &UpperSliderCtrl, m_upperThresholdMaxPercentDiff );

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
			m_upperThresholdMaxOffset =	(unsigned char) Normalizer.CalcRealValue( ( double ) UpperSliderCtrl.GetPos() );

			StrUpper.Format( "%u", m_upperThresholdMaxOffset );

			setScrollPos( &UpperSliderCtrl, m_upperThresholdMaxOffset );

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
			m_upperThresholdMinOffset = (unsigned char) Normalizer.CalcRealValue( ( double ) UpperSliderCtrl.GetPos() );

			StrUpper.Format( "%u", m_upperThresholdMinOffset );

			setScrollPos( &UpperSliderCtrl, m_upperThresholdMinOffset );

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

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequestMarker();
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = RunOnce( PCurrentAnalyzer->GetTool() );
	}

	UpdateData(FALSE);

	return l_hrOk;
}

SVProfileThresholdAdjustmentPageClass::SVProfileThresholdAdjustmentPageClass( UINT nIDCaption /* = 0 */ ) 
: SVThresholdAdjustmentPageClass( nIDCaption, SVProfileThresholdAdjustmentPageClass::IDD )
{
	//{{AFX_DATA_INIT(SVProfileThresholdAdjustmentPageClass)
	m_lowerThresholdMaxOffset = 0;
	m_lowerThresholdMinOffset = 0;
	m_lowerThresholdMaxPercentDiff = 0;
	m_upperThresholdMaxOffset = 0;
	m_upperThresholdMinOffset = 0;
	m_upperThresholdMaxPercentDiff = 0;
	m_lowerThresholdOption = 0;
	m_upperThresholdOption = 0;
	//}}AFX_DATA_INIT
}

SVProfileThresholdAdjustmentPageClass::~SVProfileThresholdAdjustmentPageClass()
{
}

void SVProfileThresholdAdjustmentPageClass::getThresholdParams()
{
	// Update DDX variables

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
}

void SVProfileThresholdAdjustmentPageClass::setThresholdParams()
{
	SetInspectionData();
}

void SVProfileThresholdAdjustmentPageClass::updateControls()
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

void SVProfileThresholdAdjustmentPageClass::setInitialValues()
{
	if ( m_pvoUpperThreshold && m_pvoLowerThreshold )
	{
		switch( m_upperThresholdOption )
		{
			case USE_SLIDER:
				double dUpper;
				m_pvoUpperThreshold->GetValue(dUpper);
				setScrollPos( &UpperSliderCtrl, (int)dUpper );
				StrUpper.Format( "%u", (int)dUpper);
				break;

			case USE_MAX_MINUS_PERCENT_DIFF:
				setScrollPos( &UpperSliderCtrl, m_upperThresholdMaxPercentDiff );
				StrUpper.Format( "%u", m_upperThresholdMaxPercentDiff );
				break;

			case USE_MAX_MINUS_OFFSET:
				setScrollPos( &UpperSliderCtrl, m_upperThresholdMaxOffset );
				StrUpper.Format( "%u", m_upperThresholdMaxOffset );
				break;

			case USE_MIN_PLUS_OFFSET:
				setScrollPos( &UpperSliderCtrl, m_upperThresholdMinOffset );
				StrUpper.Format( "%u", m_upperThresholdMinOffset );
				break;
		}

		switch( m_lowerThresholdOption )
		{
			case USE_SLIDER:
				double dLower;
				m_pvoLowerThreshold->GetValue(dLower);
				setScrollPos( &LowerSliderCtrl, (int)dLower);
				StrLower.Format( "%u", (int)dLower );
				break;

			case USE_MAX_MINUS_PERCENT_DIFF:
				setScrollPos( &LowerSliderCtrl, m_lowerThresholdMaxPercentDiff );
				StrLower.Format( "%u", m_lowerThresholdMaxPercentDiff );
				break;

			case USE_MAX_MINUS_OFFSET:
				setScrollPos( &LowerSliderCtrl, m_lowerThresholdMaxOffset );
				StrLower.Format( "%u", m_lowerThresholdMaxOffset );
				break;

			case USE_MIN_PLUS_OFFSET:
				setScrollPos( &LowerSliderCtrl, m_lowerThresholdMinOffset );
				StrLower.Format( "%u", m_lowerThresholdMinOffset );
				break;
		}

		UpdateData( FALSE ); // set data to dialog

		updateGraphDisplay();
	}
}

void SVProfileThresholdAdjustmentPageClass::DoDataExchange(CDataExchange* pDX)
{
	SVThresholdAdjustmentPageClass::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVProfileThresholdAdjustmentPageClass)
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
	DDX_Text(pDX, IDC_UPPER_MAX_OFFSET, m_upperThresholdMaxOffset);
	DDX_Text(pDX, IDC_UPPER_MIN_OFFSET, m_upperThresholdMinOffset);
	DDX_Text(pDX, IDC_UPPER_PERCENT_DIFF, m_upperThresholdMaxPercentDiff);
	DDX_Control(pDX, IDC_DIALOGIMAGE, dialogImage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVProfileThresholdAdjustmentPageClass, SVThresholdAdjustmentPageClass)
	//{{AFX_MSG_MAP(SVProfileThresholdAdjustmentPageClass)
	ON_BN_CLICKED(IDC_UPPER_USER_SELECTABLE, OnUpperUserSelectable)
	ON_BN_CLICKED(IDC_LOWER_USER_SELECTABLE, OnLowerUserSelectable)
	ON_BN_CLICKED(IDC_UPPER_USE_MAX_MINUS_DIFF, OnUpperUseMaxMinusDiff)
	ON_BN_CLICKED(IDC_UPPER_USE_MAX_MINUS_OFFSET, OnUpperUseMaxMinusOffset)
	ON_BN_CLICKED(IDC_UPPER_USE_MIN_PLUS_OFFSET, OnUpperUseMinPlusOffset)
	ON_BN_CLICKED(IDC_LOWER_USE_MAX_MINUS_DIFF, OnLowerUseMaxMinusDiff)
	ON_BN_CLICKED(IDC_LOWER_USE_MAX_MINUS_OFFSET, OnLowerUseMaxMinusOffset)
	ON_BN_CLICKED(IDC_LOWER_USE_MIN_PLUS_OFFSET, OnLowerUseMinPlusOffset)
	ON_EN_CHANGE(IDC_UPPER_PERCENT_DIFF, OnChangeUpperPercentDiff)
	ON_EN_CHANGE(IDC_UPPER_MIN_OFFSET, OnChangeUpperMinOffset)
	ON_EN_CHANGE(IDC_UPPER_MAX_OFFSET, OnChangeUpperMaxOffset)
	ON_EN_CHANGE(IDC_LOWER_PERCENT_DIFF, OnChangeLowerPercentDiff)
	ON_EN_CHANGE(IDC_LOWER_MIN_OFFSET, OnChangeLowerMinOffset)
	ON_EN_CHANGE(IDC_LOWER_MAX_OFFSET, OnChangeLowerMaxOffset)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVProfileThresholdAdjustmentPageClass 

BOOL SVProfileThresholdAdjustmentPageClass::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SetTaskObject( PCurrentAnalyzer );

	if( ! PCurrentAnalyzer )
		GetParent()->SendMessage( WM_CLOSE );

	if( SV_IS_KIND_OF( PCurrentAnalyzer->GetOwner(), SVTaskObjectClass ) )
	{
		pAnalyzerOwner = ( SVTaskObjectClass* ) PCurrentAnalyzer->GetOwner();
	}

	// Get the SVImageToLineProjectClass
	SVObjectTypeInfoStruct info;
	info.ObjectType = SVImageToLineProjectObjectType;

	pProject = ( SVImageToLineProjectClass * )SVSendMessage( PCurrentAnalyzer, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&info) );
	
	if( !pProject )
	{
		GetParent()->SendMessage( WM_CLOSE );
	}
	else
	{
		getThresholdParams();
		updateControls();
		setInitialValues();

		// Get the Image for this tool
		SVToolClass* pTool = dynamic_cast<SVToolClass*>( PCurrentAnalyzer->GetOwner() );
		SVImageInfoClass* pImageInfo = ( SVImageInfoClass* ) ::SVSendMessage( pTool, SVM_GETFIRST_IMAGE_INFO, NULL, NULL );
		if( pImageInfo )
		{
			// Set relative position of Tool Figures (Point Sets) to the Image
			dialogImage.m_ptPosition.x = 0;
			dialogImage.m_ptPosition.y = 0;

			RunOnce( pTool );

			SVImageClass* l_pImage = NULL;

			pImageInfo->GetOwnerImage( l_pImage );

			// Set the Image
			dialogImage.init( l_pImage );

			updateGraphDisplay();
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void SVProfileThresholdAdjustmentPageClass::OnCancel() 
{
	SVThresholdAdjustmentPageClass::OnCancel();
}

void SVProfileThresholdAdjustmentPageClass::OnUpperUserSelectable() 
{
	setThresholdParams();

	updateControls();

	updateGraphDisplay();
}

void SVProfileThresholdAdjustmentPageClass::OnLowerUserSelectable() 
{
	setThresholdParams();

	updateControls();

	updateGraphDisplay();
}

void SVProfileThresholdAdjustmentPageClass::OnUpperUseMaxMinusDiff() 
{
	setThresholdParams();

	updateControls();

	updateGraphDisplay();
}

void SVProfileThresholdAdjustmentPageClass::OnUpperUseMaxMinusOffset() 
{
	setThresholdParams();

	updateControls();

	updateGraphDisplay();
}

void SVProfileThresholdAdjustmentPageClass::OnUpperUseMinPlusOffset() 
{
	setThresholdParams();

	updateControls();

	updateGraphDisplay();
}

void SVProfileThresholdAdjustmentPageClass::OnLowerUseMaxMinusDiff() 
{
	setThresholdParams();

	updateControls();

	updateGraphDisplay();
}

void SVProfileThresholdAdjustmentPageClass::OnLowerUseMaxMinusOffset() 
{
	setThresholdParams();

	updateControls();

	updateGraphDisplay();
}

void SVProfileThresholdAdjustmentPageClass::OnLowerUseMinPlusOffset() 
{
	setThresholdParams();

	updateControls();

	updateGraphDisplay();
}

void SVProfileThresholdAdjustmentPageClass::OnChangeUpperPercentDiff() 
{
	if( m_upperThresholdOption != USE_SLIDER )
	{
		setThresholdParams();

		updateControls();

		updateGraphDisplay();
	}
}

void SVProfileThresholdAdjustmentPageClass::OnChangeUpperMinOffset() 
{
	if( m_upperThresholdOption != USE_SLIDER )
	{
		setThresholdParams();

		updateControls();

		updateGraphDisplay();
	}
}

void SVProfileThresholdAdjustmentPageClass::OnChangeUpperMaxOffset() 
{
	if( m_upperThresholdOption != USE_SLIDER )
	{
		setThresholdParams();

		updateControls();

		updateGraphDisplay();
	}
}

void SVProfileThresholdAdjustmentPageClass::OnChangeLowerPercentDiff() 
{
	if( m_lowerThresholdOption != USE_SLIDER )
	{
		setThresholdParams();

		updateControls();

		updateGraphDisplay();
	}
}

void SVProfileThresholdAdjustmentPageClass::OnChangeLowerMinOffset() 
{
	if( m_lowerThresholdOption != USE_SLIDER )
	{
		setThresholdParams();

		updateControls();

		updateGraphDisplay();
	}
}

void SVProfileThresholdAdjustmentPageClass::OnChangeLowerMaxOffset() 
{
	if( m_lowerThresholdOption != USE_SLIDER )
	{
		setThresholdParams();

		updateControls();

		updateGraphDisplay();
	}
}

void SVProfileThresholdAdjustmentPageClass::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
	if( m_pvoUpperThreshold && m_pvoLowerThreshold )
	{
		setThresholdParams();

		updateGraphDisplay();
	}

	CPropertyPage::OnHScroll( nSBCode, nPos, pScrollBar );
}

void SVProfileThresholdAdjustmentPageClass::setScrollRange( CSliderCtrl* pSliderCtrl, int min, int max )
{
	SVValueBaseNormalizerClass* pNormalizer;

	if( pSliderCtrl == &UpperSliderCtrl )
		pNormalizer = &Normalizer;
	else
		pNormalizer = &LowerNormalizer;

	// Revise the scale ?
	pNormalizer->SetRealRange( ( double )min, ( double )max );

	pSliderCtrl->SetRange( ( int ) pNormalizer->GetNormalRangeMin(), ( int ) pNormalizer->GetNormalRangeMax(), TRUE );
	pSliderCtrl->SetTic( ( int ) ( pNormalizer->GetNormalRangeBaseSize() / 2 + pNormalizer->GetNormalRangeMin() ) );
	int pageSize = ( int ) ( ( double ) ( pNormalizer->GetNormalRangeBaseSize() / pNormalizer->GetRealRangeBaseSize() ) );
	pSliderCtrl->SetPageSize( pageSize > 0 ? pageSize : 1 );
}

void SVProfileThresholdAdjustmentPageClass::setScrollPos( CSliderCtrl* pSliderCtrl, int pos )
{
	SVValueBaseNormalizerClass* pNormalizer;

	if( pSliderCtrl == &UpperSliderCtrl )
		pNormalizer = &Normalizer;
	else
		pNormalizer = &LowerNormalizer;

	pSliderCtrl->SetPos( ( int ) pNormalizer->CalcNormalizedValue( ( double ) pos ) );
}

void SVProfileThresholdAdjustmentPageClass::updateGraphDisplay()
{
	// Remove old points
	dialogImage.ClearPoints();

		// Get the Pixel Line Stuff from Analyzer
	if( pProject )
	{
		SVLineAnalyzerClass* pAnalyzer = dynamic_cast<SVLineAnalyzerClass*>( pProject->GetAnalyzer());
		if( pAnalyzer && pProject->getInputImage())
		{
			SVExtentMultiLineStruct l_svMultiLine;

			if( pAnalyzer->GetHistogramOverlay( l_svMultiLine ) == S_OK )
			{
				SVDrawObjectListClass l_svDrawObjectList;

				l_svDrawObjectList.AddExtentMultiLineData( l_svMultiLine );

				dialogImage.AddPoints( &l_svDrawObjectList, SVGraphScale );

				l_svMultiLine.Initialize();
			}

			if( pAnalyzer->GetThresholdBarsOverlay( l_svMultiLine ) == S_OK )
			{
				SVDrawObjectListClass l_svDrawObjectList;

				l_svDrawObjectList.AddExtentMultiLineData( l_svMultiLine );

				dialogImage.AddPoints( &l_svDrawObjectList, SVGraphScale );

				l_svMultiLine.Initialize();
			}

			if( pAnalyzer->GetEdgesOverlay( l_svMultiLine ) == S_OK )
			{
				SVDrawObjectListClass l_svDrawObjectList;

				l_svDrawObjectList.AddExtentMultiLineData( l_svMultiLine );

				dialogImage.AddPoints( &l_svDrawObjectList, SVGraphScale );

				l_svMultiLine.Initialize();
			}

			if( pAnalyzer->GetSelectedEdgeOverlays( l_svMultiLine ) == S_OK )
			{
				SVDrawObjectListClass l_svDrawObjectList;

				l_svDrawObjectList.AddExtentMultiLineData( l_svMultiLine );

				dialogImage.AddPoints( &l_svDrawObjectList, SVGraphScale );
			}
		}
	}		

	dialogImage.refresh();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVProfileThresholdAdjustmentPage.cpp_v  $
 * 
 *    Rev 1.1   01 Feb 2014 11:59:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:37:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   01 Aug 2012 13:04:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   25 Jul 2012 14:43:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code and updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   09 Jul 2012 14:11:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   19 Jun 2012 13:49:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   30 Jul 2009 12:23:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   29 Aug 2005 16:22:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added RunOnce to OnInitDialog so Display will have its figures initially updated.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   16 Aug 2005 09:41:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   09 Aug 2005 08:08:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   19 Jul 2005 14:49:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented new value object Get/Set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   18 Jul 2005 11:50:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added static_cast for setting Threshold Value Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   17 Feb 2005 15:22:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   18 Dec 2003 16:12:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed dialog display logic to force the underlaying drawobjects to update.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   09 Dec 2003 12:30:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated line analyzer dialogs to store edge control parameters differently in the value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   04 Dec 2003 13:13:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   21 Nov 2003 11:55:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with value objects.  Was setting to last set and not to bucket 1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   08 Oct 2003 11:09:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   using the value objects for the edge parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   12 Aug 2003 09:05:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   adjusted for m_ prefix change in SVDlgImageGraphClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 Apr 2003 14:19:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   20 Nov 2002 11:06:42   ryoho
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
 *    Rev 1.6   04 Oct 2001 17:04:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 May 2001 15:54:02   rschock
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
 *    Rev 1.4   14 Jun 2000 13:58:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  158
 * SCR Title:  Profile Tool Lower Threshold Setup problem
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVValueBaseNormalizer for the lower threshold.
 * Revised OnInitDialog to use both normalizer variables.
 * Revised OnUpperUserSelectable to update the display.
 * Revised OnLowerUserSelectable to update the display.
 * Revised OnHScroll to use both Normalizers.
 * Revised SetScrollRange to use both Normalizers.
 * Revised SetScrollPos to use both Normalizers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   26 Apr 2000 19:01:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added include for SVDlgImageGraph.h.
 * Added SVDlgImageGraph control to the dialog.
 * Revised OnInitDialog to set points (figures) for the 
 * SVDlgImageGraph control.
 * Revised updateGraphDisplay to call
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
 *    Rev 1.0   04 Feb 2000 09:11:54   sjones
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
