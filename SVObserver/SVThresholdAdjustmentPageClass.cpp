//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMeasureAnalyzerAdjustment
//* .File Name       : $Workfile:   SVThresholdAdjustmentPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:25:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVThresholdAdjustmentPageClass.h"
#include "SVRunControlLibrary/SVRunStatus.h"
#include "SVIPDoc.h"
#include "SVLine.h"
#include "SVLineAnalyzer.h"
#include "SVSVIMStateClass.h"
#include "SVTaskObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVThresholdAdjustmentPageClass, CPropertyPage)

SVThresholdAdjustmentPageClass::SVThresholdAdjustmentPageClass( UINT nIDCaption /* = 0 */ , int id /* = IDD */ ) 
: CPropertyPage( id, nIDCaption )
{
	PCurrentDocument = NULL;
	PCurrentAnalyzer = NULL;
	//{{AFX_DATA_INIT(SVThresholdAdjustmentPageClass)
	StrUpper = _T("");
	StrLower = _T("");
	//}}AFX_DATA_INIT
}

SVThresholdAdjustmentPageClass::~SVThresholdAdjustmentPageClass()
{
}

HRESULT SVThresholdAdjustmentPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	if( m_pvoUpperThreshold && m_pvoLowerThreshold && m_pAnalyzerOwner )
	{
		UpdateData( TRUE ); // get data of dialog	
	
		double dUpperVal = Normalizer.CalcRealValue( ( double ) UpperSliderCtrl.GetPos());
		StrUpper.Format( "%u", (int)dUpperVal );

		double dLowerVal = Normalizer.CalcRealValue( ( double ) LowerSliderCtrl.GetPos());
		StrLower.Format( "%u", (int)dLowerVal );

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( m_pvoUpperThreshold, dUpperVal );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( m_pvoLowerThreshold, dLowerVal );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequestMarker();
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( m_pAnalyzerOwner->GetTool() );
		}

		UpdateData( FALSE );
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

void SVThresholdAdjustmentPageClass::updateDisplay()
{
	SetInspectionData();
}

void SVThresholdAdjustmentPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVThresholdAdjustmentPageClass)
	DDX_Control(pDX, IDC_UPPER_THRESHOLD_SLIDER, UpperSliderCtrl);
	DDX_Control(pDX, IDC_LOWER_THRESHOLD_SLIDER, LowerSliderCtrl);
	DDX_Text(pDX, IDC_UPPER_THRESHOLD_STATIC, StrUpper);
	DDX_Text(pDX, IDC_LOWER_THRESHOLD_STATIC, StrLower);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVThresholdAdjustmentPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVThresholdAdjustmentPageClass)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVThresholdAdjustmentPageClass 

BOOL SVThresholdAdjustmentPageClass::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	if( nullptr != PCurrentAnalyzer && (m_pAnalyzerOwner = dynamic_cast<SVTaskObjectClass*>( PCurrentAnalyzer->GetOwner() )) )
	{
		SetTaskObject( m_pAnalyzerOwner );
	}

	double dUpper;
	double dLower;
	m_pvoUpperThreshold->GetValue( dUpper );
	m_pvoLowerThreshold->GetValue( dLower );

	StrUpper.Format( "%u", (int)dUpper );
	StrLower.Format( "%u", (int)dLower );

	UpperSliderCtrl.SetRange( ( int ) Normalizer.GetNormalRangeMin(), ( int ) Normalizer.GetNormalRangeMax(), TRUE );
	LowerSliderCtrl.SetRange( ( int ) Normalizer.GetNormalRangeMin(), ( int ) Normalizer.GetNormalRangeMax(), TRUE );
	LowerSliderCtrl.SetTic( ( int ) ( Normalizer.GetNormalRangeBaseSize() / 2 + Normalizer.GetNormalRangeMin() ) );
	UpperSliderCtrl.SetTic( ( int ) ( Normalizer.GetNormalRangeBaseSize() / 2 + Normalizer.GetNormalRangeMin() ) );

	int pageSize = ( int ) ( ( double ) ( Normalizer.GetNormalRangeBaseSize() / Normalizer.GetRealRangeBaseSize() ) );
	LowerSliderCtrl.SetPageSize( pageSize > 0 ? pageSize : 1 );
	UpperSliderCtrl.SetPageSize( pageSize > 0 ? pageSize : 1 );
	UpperSliderCtrl.SetPos( ( int ) Normalizer.CalcNormalizedValue( dUpper ) );
	LowerSliderCtrl.SetPos( ( int ) Normalizer.CalcNormalizedValue( dLower ) );

	UpdateData( FALSE ); // set data to dialog

	updateDisplay();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVThresholdAdjustmentPageClass::OnCancel() 
{
	CPropertyPage::OnCancel();
}

void SVThresholdAdjustmentPageClass::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
	updateDisplay();

	CPropertyPage::OnHScroll( nSBCode, nPos, pScrollBar );
}

