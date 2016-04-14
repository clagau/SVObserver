//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEdgeMarkerAdjustmentPageClass
//* .File Name       : $Workfile:   SVEdgeMarkerAdjustmentPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   02 Jul 2014 13:08:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEdgeMarkerAdjustmentPageClass.h"
#include "SVGlobal.h"
#include "SVObjectAppClass.h"
#include "SVAnalyzer.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite SVEdgeMarkerAdjustmentPageClass 

IMPLEMENT_DYNCREATE( SVEdgeMarkerAdjustmentPageClass, CPropertyPage )

SVEdgeMarkerAdjustmentPageClass::SVEdgeMarkerAdjustmentPageClass( UINT nIDCaption /* = 0 */ , int id /* = IDD */) 
: CPropertyPage( id, nIDCaption )
, PCurrentAnalyzer( nullptr )
, m_pvoEdgeDirection( nullptr )
, m_pvoEdgePolarisation( nullptr )
, m_pvoEdgeSelect( nullptr )
, m_pvoEdgeSelectThisValue( nullptr )
, m_pvoEdgeIsFixedEdgeMarker( nullptr )
, m_pvoEdgePosition( nullptr )
, m_pvoEdgePositionOffsetValue( nullptr )
, m_pvoEdgeLowerThresholdValue( nullptr )
, m_pvoEdgeUpperThresholdValue( nullptr )
, StrLower( _T("") )
, StrUpper( _T("") )
, StrPositionOffset( _T("") )
, StrEdgeSelectThis( _T("") )
{
	//{{AFX_DATA_INIT(SVEdgeMarkerAdjustmentPageClass)
	//}}AFX_DATA_INIT
}

SVEdgeMarkerAdjustmentPageClass::~SVEdgeMarkerAdjustmentPageClass()
{
	m_pvoEdgeDirection = NULL;
	m_pvoEdgePolarisation = NULL;
	m_pvoEdgeSelect = NULL;
	m_pvoEdgeSelectThisValue = NULL;
	m_pvoEdgeIsFixedEdgeMarker = NULL;
	m_pvoEdgePosition = NULL;
	m_pvoEdgePositionOffsetValue = NULL;
	m_pvoEdgeLowerThresholdValue = NULL;
	m_pvoEdgeUpperThresholdValue = NULL;
}

HRESULT SVEdgeMarkerAdjustmentPageClass::GetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	double l_dUpper = 0.0;
	double l_dLower = 0.0;

	l_hrOk = m_pvoEdgeUpperThresholdValue->GetValue( l_dUpper );

	if( l_hrOk == S_OK )
	{
		l_hrOk = m_pvoEdgeLowerThresholdValue->GetValue( l_dLower );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = UpdateSliderData( l_dLower, l_dUpper );
	}

	BOOL bIsFixedEdgeMarker;
	m_pvoEdgeIsFixedEdgeMarker->GetValue(bIsFixedEdgeMarker);

	if( bIsFixedEdgeMarker )
	{
		CheckRadioButton( IDC_SEARCHED_EDGE_MARKER_RADIO, IDC_FIXED_EDGE_MARKER_RADIO, IDC_FIXED_EDGE_MARKER_RADIO );
	}
	else
	{
		CheckRadioButton( IDC_SEARCHED_EDGE_MARKER_RADIO, IDC_FIXED_EDGE_MARKER_RADIO, IDC_SEARCHED_EDGE_MARKER_RADIO );
	}

	DWORD dwDirection;
	m_pvoEdgeDirection->GetValue(dwDirection);
	switch( dwDirection )
	{
		case SV_HEAD_TO_TAIL_DIRECTION:
			CheckRadioButton( IDC_DIRECTION_HEAD_TO_TAIL_RADIO, IDC_DIRECTION_TAIL_TO_HEAD_RADIO, IDC_DIRECTION_HEAD_TO_TAIL_RADIO );
			break;

		case SV_TAIL_TO_HEAD_DIRECTION:
			CheckRadioButton( IDC_DIRECTION_HEAD_TO_TAIL_RADIO, IDC_DIRECTION_TAIL_TO_HEAD_RADIO, IDC_DIRECTION_TAIL_TO_HEAD_RADIO );
			break;
	}

	DWORD dwEdgeSelect;
	m_pvoEdgeSelect->GetValue(dwEdgeSelect);
	switch( dwEdgeSelect )
	{
		case SV_FIRST_EDGE:
		{
			CheckRadioButton( IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_THIS_RADIO, IDC_EDGE_SELECT_FIRST_RADIO );

			CWnd* pWnd = GetDlgItem( IDC_EDGE_SELECT_THIS_EDIT );
			if( pWnd )
				pWnd->EnableWindow( FALSE );

			break;
		}
		case SV_LAST_EDGE:
		{
			CheckRadioButton( IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_THIS_RADIO, IDC_EDGE_SELECT_LAST_RADIO );

			CWnd* pWnd = GetDlgItem( IDC_EDGE_SELECT_THIS_EDIT );
			if( pWnd )
				pWnd->EnableWindow( FALSE );

			break;
		}
		case SV_THIS_EDGE:
		{
			CheckRadioButton( IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_THIS_RADIO, IDC_EDGE_SELECT_THIS_RADIO );

			CWnd* pWnd = GetDlgItem( IDC_EDGE_SELECT_THIS_EDIT );
			if( pWnd )
				pWnd->EnableWindow();

			break;
		}
	}

	double dValue;
	m_pvoEdgeSelectThisValue->GetValue( dValue );
	StrEdgeSelectThis.Format( "%.2f", dValue );

	DWORD dwPolarisation;
	m_pvoEdgePolarisation->GetValue(dwPolarisation);
	switch( dwPolarisation )
	{
		case SV_POSITIVE_POLARISATION:
			CheckRadioButton( IDC_POLARISATION_POSITIVE_RADIO, IDC_POLARISATION_ANY_RADIO, IDC_POLARISATION_POSITIVE_RADIO );
			break;

		case SV_NEGATIVE_POLARISATION:
			CheckRadioButton( IDC_POLARISATION_POSITIVE_RADIO, IDC_POLARISATION_ANY_RADIO, IDC_POLARISATION_NEGATIVE_RADIO );
			break;

		case SV_ANY_POLARISATION:
			CheckRadioButton( IDC_POLARISATION_POSITIVE_RADIO, IDC_POLARISATION_ANY_RADIO, IDC_POLARISATION_ANY_RADIO );
			break;
	}

	DWORD dwPosition;
	m_pvoEdgePosition->GetValue(dwPosition);
	switch( dwPosition )
	{
		case SV_START_POSITION:
		{
			CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_START_RADIO );

			CWnd* pWnd = GetDlgItem( IDC_POSITION_OFFSET_EDIT );
			if( pWnd )
				pWnd->EnableWindow( FALSE );

			break;
		}
		case SV_CENTER_POSITION:
		{
			CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_CENTER_RADIO );

			CWnd* pWnd = GetDlgItem( IDC_POSITION_OFFSET_EDIT );
			if( pWnd )
				pWnd->EnableWindow( FALSE );

			break;
		}
		case SV_END_POSITION:
		{
			CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_END_RADIO );

			CWnd* pWnd = GetDlgItem( IDC_POSITION_OFFSET_EDIT );
			if( pWnd )
				pWnd->EnableWindow( FALSE );

			break;
		}
		case SV_OFFSET_POSITION:
		{
			CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_OFFSET_RADIO );

			CWnd* pWnd = GetDlgItem( IDC_POSITION_OFFSET_EDIT );
			if( pWnd )
				pWnd->EnableWindow();

			break;
		}
	}

	m_pvoEdgePositionOffsetValue->GetValue( dValue );
	StrPositionOffset.Format( "%.2f", dValue );

	// Acivate controls regarding the analyzer requirements
	// or let them deactivated...
	CWnd* pWnd;

	if( m_bEnableThreshold )
	{
		if( pWnd = GetDlgItem( IDC_THRESHOLD_FRAME_STATIC ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_UPPER_THRESHOLD_SLIDER ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_LOWER_THRESHOLD_SLIDER ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_LOWER_THRESHOLD_STATIC ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_UPPER_THRESHOLD_STATIC ) )
			pWnd->EnableWindow();
	}

	if( m_bEnablePosition )
	{
		if( pWnd = GetDlgItem( IDC_POSITION_FRAME_STATIC ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_POSITION_CENTER_RADIO ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_POSITION_END_RADIO ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_POSITION_OFFSET_RADIO ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_POSITION_START_RADIO ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_FIXED_EDGE_MARKER_RADIO ) )
			pWnd->EnableWindow();
	}
	else
	{
		if( pWnd = GetDlgItem( IDC_POSITION_OFFSET_EDIT ) )
			pWnd->EnableWindow( FALSE );
	}

	if( m_bEnableEdgeSelect )
	{
		if( pWnd = GetDlgItem( IDC_EDGESELECT_FRAME_STATIC ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_EDGE_SELECT_FIRST_RADIO ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_EDGE_SELECT_LAST_RADIO ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_EDGE_SELECT_THIS_RADIO ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_SEARCHED_EDGE_MARKER_RADIO ) )
			pWnd->EnableWindow();
	}
	else
	{
		if( pWnd = GetDlgItem( IDC_EDGE_SELECT_THIS_EDIT ) )
			pWnd->EnableWindow( FALSE );
	}

	if( m_bEnablePolarisation )
	{
		if( pWnd = GetDlgItem( IDC_POLARISATION_FRAME_STATIC ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_POLARISATION_ANY_RADIO ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_POLARISATION_NEGATIVE_RADIO ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_POLARISATION_POSITIVE_RADIO ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_SEARCHED_EDGE_MARKER_RADIO ) )
			pWnd->EnableWindow();
	}

	if( m_bEnableDirection )
	{
		if( pWnd = GetDlgItem( IDC_DIRECTION_FRAME_STATIC ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_DIRECTION_HEAD_TO_TAIL_RADIO ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_DIRECTION_TAIL_TO_HEAD_RADIO ) )
			pWnd->EnableWindow();
		if( pWnd = GetDlgItem( IDC_SEARCHED_EDGE_MARKER_RADIO ) )
			pWnd->EnableWindow();
	}

	UpdateData( FALSE );

	return l_hrOk;
}

HRESULT SVEdgeMarkerAdjustmentPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	if( PCurrentAnalyzer )
	{
		UpdateData( TRUE ); // get data of dialog	

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( m_pvoEdgeUpperThresholdValue, StrUpper );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = AddInputRequest( m_pvoEdgeLowerThresholdValue, StrLower );
		}

		switch ( GetCheckedRadioButton( IDC_SEARCHED_EDGE_MARKER_RADIO, IDC_FIXED_EDGE_MARKER_RADIO ) )
		{
			case IDC_SEARCHED_EDGE_MARKER_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgeIsFixedEdgeMarker, (double)FALSE );
				}

				break;
			}
			case IDC_FIXED_EDGE_MARKER_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgeIsFixedEdgeMarker, TRUE );
				}

				break;
			}
		}

		switch ( GetCheckedRadioButton( IDC_DIRECTION_HEAD_TO_TAIL_RADIO, IDC_DIRECTION_TAIL_TO_HEAD_RADIO ) )
		{
			case IDC_DIRECTION_HEAD_TO_TAIL_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgeDirection, SV_HEAD_TO_TAIL_DIRECTION );
				}

				break;
			}
			case IDC_DIRECTION_TAIL_TO_HEAD_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgeDirection, SV_TAIL_TO_HEAD_DIRECTION );
				}

				break;
			}
		}

		switch ( GetCheckedRadioButton( IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_THIS_RADIO ) )
		{
			case IDC_EDGE_SELECT_FIRST_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgeSelect, SV_FIRST_EDGE );
				}

				break;
			}
			case IDC_EDGE_SELECT_LAST_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgeSelect, SV_LAST_EDGE );
				}

				break;
			}
			case IDC_EDGE_SELECT_THIS_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgeSelect, SV_THIS_EDGE );
				}

				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgeSelectThisValue, atof( StrEdgeSelectThis ) );
				}

				break;
			}
		}

		switch ( GetCheckedRadioButton( IDC_POLARISATION_POSITIVE_RADIO, IDC_POLARISATION_ANY_RADIO ) )
		{
			case IDC_POLARISATION_ANY_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgePolarisation, SV_ANY_POLARISATION );
				}

				break;
			}
			case IDC_POLARISATION_NEGATIVE_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgePolarisation, SV_NEGATIVE_POLARISATION );
				}

				break;
			}
			case IDC_POLARISATION_POSITIVE_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgePolarisation, SV_POSITIVE_POLARISATION );
				}

				break;
			}
		}

		switch ( GetCheckedRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO ) )
		{
			case IDC_POSITION_CENTER_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgePosition, SV_CENTER_POSITION );
				}

				break;
			}
			case IDC_POSITION_END_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgePosition, SV_END_POSITION );
				}

				break;
			}
			case IDC_POSITION_OFFSET_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgePosition, SV_OFFSET_POSITION );
				}

				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgePositionOffsetValue, atof( StrPositionOffset ) );
				}

				break;
			}
			case IDC_POSITION_START_RADIO:
			{
				if( l_hrOk == S_OK )
				{
					l_hrOk = AddInputRequest( m_pvoEdgePosition, SV_START_POSITION );
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

		if( GetInspectionData() != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

void SVEdgeMarkerAdjustmentPageClass::SetNormalizer( const SVValueBaseNormalizerClass &l_rsvNormalizer )
{
	Normalizer = l_rsvNormalizer;
	LowerNormalizer = l_rsvNormalizer;
}

HRESULT SVEdgeMarkerAdjustmentPageClass::UpdateSliderData( double p_dLower, double p_dUpper )
{
	HRESULT l_hrOk = S_OK;

	StrUpper.Format( "%u", static_cast< int >( p_dUpper ) );
	StrLower.Format( "%u", static_cast< int >( p_dLower ) );

	setScrollPos( &UpperSliderCtrl, static_cast<int>(p_dUpper) );
	setScrollPos( &LowerSliderCtrl, static_cast<long>(p_dLower) );

	return l_hrOk;
}

void SVEdgeMarkerAdjustmentPageClass::setScrollRange( CSliderCtrl* pSliderCtrl, int min, int max )
{
	SVValueBaseNormalizerClass* pNormalizer;

	if( pSliderCtrl == &UpperSliderCtrl )
		pNormalizer = &Normalizer;
	else
		pNormalizer = &LowerNormalizer;

	// Revise the scale ?
	pNormalizer->SetRealRange( ( double )min, ( double )max );
	pNormalizer->SetNormalRange( (double)min, (double)max);

	pSliderCtrl->SetRange( ( int ) pNormalizer->GetNormalRangeMin(), ( int ) pNormalizer->GetNormalRangeMax(), TRUE );
	pSliderCtrl->SetTic( ( int ) ( pNormalizer->GetNormalRangeBaseSize() / 2 + pNormalizer->GetNormalRangeMin() ) );
	int pageSize = ( int ) ( ( double ) ( pNormalizer->GetNormalRangeBaseSize() / pNormalizer->GetRealRangeBaseSize() ) );
	pSliderCtrl->SetPageSize( pageSize > 0 ? pageSize : 1 );
}

void SVEdgeMarkerAdjustmentPageClass::setScrollPos( CSliderCtrl* pSliderCtrl, int pos )
{
	SVValueBaseNormalizerClass* pNormalizer;

	if( pSliderCtrl == &UpperSliderCtrl )
		pNormalizer = &Normalizer;
	else
		pNormalizer = &LowerNormalizer;

	pSliderCtrl->SetPos( (int)pNormalizer->CalcNormalizedValue( ( double ) pos ) );
}

void SVEdgeMarkerAdjustmentPageClass::DoDataExchange( CDataExchange* pDX )
{
	CPropertyPage::DoDataExchange( pDX );
	//{{AFX_DATA_MAP(SVEdgeMarkerAdjustmentPageClass)
	DDX_Control(pDX, IDC_POSITION_OFFSET_EDIT, m_editPositionOffset);
	DDX_Control(pDX, IDC_EDGE_SELECT_THIS_EDIT, m_editSelectThis);
	DDX_Control(pDX, IDC_UPPER_THRESHOLD_SLIDER, UpperSliderCtrl);
	DDX_Control(pDX, IDC_LOWER_THRESHOLD_SLIDER, LowerSliderCtrl);
	DDX_Text(pDX, IDC_LOWER_THRESHOLD_STATIC, StrLower);
	DDX_Text(pDX, IDC_UPPER_THRESHOLD_STATIC, StrUpper);
	DDX_Text(pDX, IDC_POSITION_OFFSET_EDIT, StrPositionOffset);
	DDX_Text(pDX, IDC_EDGE_SELECT_THIS_EDIT, StrEdgeSelectThis);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( SVEdgeMarkerAdjustmentPageClass, CPropertyPage )
	//{{AFX_MSG_MAP(SVEdgeMarkerAdjustmentPageClass)
	ON_BN_CLICKED(IDC_DIRECTION_HEAD_TO_TAIL_RADIO, OnDirectionHeadToTailRadio)
	ON_BN_CLICKED(IDC_DIRECTION_TAIL_TO_HEAD_RADIO, OnDirectionTailToHeadRadio)
	ON_BN_CLICKED(IDC_EDGE_SELECT_FIRST_RADIO, OnEdgeSelectFirstRadio)
	ON_BN_CLICKED(IDC_EDGE_SELECT_LAST_RADIO, OnEdgeSelectLastRadio)
	ON_BN_CLICKED(IDC_EDGE_SELECT_THIS_RADIO, OnEdgeSelectThisRadio)
	ON_BN_CLICKED(IDC_FIXED_EDGE_MARKER_RADIO, OnFixedEdgeMarkerRadio)
	ON_BN_CLICKED(IDC_POLARISATION_ANY_RADIO, OnPolarisationAnyRadio)
	ON_BN_CLICKED(IDC_POLARISATION_NEGATIVE_RADIO, OnPolarisationNegativeRadio)
	ON_BN_CLICKED(IDC_POLARISATION_POSITIVE_RADIO, OnPolarisationPositiveRadio)
	ON_BN_CLICKED(IDC_POSITION_CENTER_RADIO, OnPositionCenterRadio)
	ON_BN_CLICKED(IDC_POSITION_END_RADIO, OnPositionEndRadio)
	ON_BN_CLICKED(IDC_POSITION_OFFSET_RADIO, OnPositionOffsetRadio)
	ON_BN_CLICKED(IDC_POSITION_START_RADIO, OnPositionStartRadio)
	ON_BN_CLICKED(IDC_SEARCHED_EDGE_MARKER_RADIO, OnSearchedEdgeMarkerRadio)
	ON_WM_VSCROLL()
	ON_EN_KILLFOCUS(IDC_EDGE_SELECT_THIS_EDIT, OnChangeEdgeSelectThisEdit)
	ON_EN_CHANGE(IDC_POSITION_OFFSET_EDIT, OnChangePositionOffsetEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVEdgeMarkerAdjustmentPageClass 

BOOL SVEdgeMarkerAdjustmentPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	SetTaskObject( PCurrentAnalyzer );

	if( ! PCurrentAnalyzer )
		GetParent()->SendMessage( WM_CLOSE );

	setScrollRange( &UpperSliderCtrl, static_cast<int>(Normalizer.GetRealRangeMin()), static_cast<int>(Normalizer.GetRealRangeMax()) );
	setScrollRange( &LowerSliderCtrl, static_cast<int>(LowerNormalizer.GetRealRangeMin()), static_cast<int>(LowerNormalizer.GetRealRangeMax()) );

	// initialize edge parameters
	GetInspectionData();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


void SVEdgeMarkerAdjustmentPageClass::OnDirectionHeadToTailRadio() 
{
	CheckRadioButton( IDC_DIRECTION_HEAD_TO_TAIL_RADIO, IDC_DIRECTION_TAIL_TO_HEAD_RADIO, IDC_DIRECTION_HEAD_TO_TAIL_RADIO );

	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnDirectionTailToHeadRadio() 
{
	CheckRadioButton( IDC_DIRECTION_HEAD_TO_TAIL_RADIO, IDC_DIRECTION_TAIL_TO_HEAD_RADIO, IDC_DIRECTION_TAIL_TO_HEAD_RADIO );

	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnEdgeSelectFirstRadio() 
{
	CheckRadioButton( IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_THIS_RADIO, IDC_EDGE_SELECT_FIRST_RADIO );

	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnEdgeSelectLastRadio() 
{
	CheckRadioButton( IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_THIS_RADIO, IDC_EDGE_SELECT_LAST_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnEdgeSelectThisRadio() 
{
	CheckRadioButton( IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_THIS_RADIO, IDC_EDGE_SELECT_THIS_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnChangeEdgeSelectThisEdit() 
{
	CheckRadioButton( IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_THIS_RADIO, IDC_EDGE_SELECT_THIS_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnPolarisationNegativeRadio() 
{
	CheckRadioButton( IDC_POLARISATION_POSITIVE_RADIO, IDC_POLARISATION_ANY_RADIO, IDC_POLARISATION_NEGATIVE_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnPolarisationPositiveRadio() 
{
	CheckRadioButton( IDC_POLARISATION_POSITIVE_RADIO, IDC_POLARISATION_ANY_RADIO, IDC_POLARISATION_POSITIVE_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnPolarisationAnyRadio() 
{
	CheckRadioButton( IDC_POLARISATION_POSITIVE_RADIO, IDC_POLARISATION_ANY_RADIO, IDC_POLARISATION_ANY_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnSearchedEdgeMarkerRadio()
{
	CheckRadioButton( IDC_SEARCHED_EDGE_MARKER_RADIO, IDC_FIXED_EDGE_MARKER_RADIO, IDC_SEARCHED_EDGE_MARKER_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnFixedEdgeMarkerRadio()
{
	CheckRadioButton( IDC_SEARCHED_EDGE_MARKER_RADIO, IDC_FIXED_EDGE_MARKER_RADIO, IDC_FIXED_EDGE_MARKER_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnPositionStartRadio() 
{
	CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_START_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnPositionCenterRadio() 
{
	CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_CENTER_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnPositionEndRadio() 
{
	CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_END_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnPositionOffsetRadio() 
{
	CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_OFFSET_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnChangePositionOffsetEdit() 
{
	CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_OFFSET_RADIO );
	
	SetInspectionData();
}

void SVEdgeMarkerAdjustmentPageClass::OnCancel() 
{
	CPropertyPage ::OnCancel();
}


void SVEdgeMarkerAdjustmentPageClass::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
	if( &UpperSliderCtrl == ( CSliderCtrl* ) pScrollBar )
	{
		double dUpperThreshold = Normalizer.CalcRealValue(UpperSliderCtrl.GetPos());

		StrUpper.Format( "%u", (int)dUpperThreshold );

		UpdateData( FALSE );

		SetInspectionData();
	}

	if( &LowerSliderCtrl == ( CSliderCtrl* ) pScrollBar )
	{
		double dLowerThreshold = Normalizer.CalcRealValue(LowerSliderCtrl.GetPos());

		StrLower.Format( "%u", (int)dLowerThreshold );

		UpdateData( FALSE );

		SetInspectionData();
	}

	CPropertyPage ::OnVScroll( nSBCode, nPos, pScrollBar );
}


BOOL SVEdgeMarkerAdjustmentPageClass::OnSetActive() 
{
	// Run the Tool/Analyzer & update the Display
	SetInspectionData();
			
	return CPropertyPage ::OnSetActive();
}

