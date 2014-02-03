//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEdgeMarkerAdjustmentPageClass
//* .File Name       : $Workfile:   SVEdgeMarkerAdjustmentPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 10:32:26  $
//******************************************************************************


#include "stdafx.h"
#include "SVEdgeMarkerAdjustmentPageClass.h"
#include "SVGlobal.h"
#include "SVLineAnalyzer.h"

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
{
	PCurrentAnalyzer = NULL;

	pAnalyzerOwner  = NULL;
	pLine = NULL;

	//set all value object pointers to null
	m_pvoEdgeDirection = NULL;
	m_pvoEdgePolarisation = NULL;
	m_pvoEdgeSelect = NULL;
	m_pvoEdgeSelectThisValue = NULL;
	m_pvoEdgeIsFixedEdgeMarker = NULL;
	m_pvoEdgePosition = NULL;
	m_pvoEdgePositionOffsetValue = NULL;
	m_pvoEdgeLowerThresholdValue = NULL;
	m_pvoEdgeUpperThresholdValue = NULL;

	//{{AFX_DATA_INIT(SVEdgeMarkerAdjustmentPageClass)
	StrLower = _T("");
	StrUpper = _T("");
	StrPositionOffset = _T("");
	StrEdgeSelectThis = _T("");
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
		l_hrOk = GetSliderData( l_dLower, l_dUpper );
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

HRESULT SVEdgeMarkerAdjustmentPageClass::GetSliderData( double p_dLower, double p_dUpper )
{
	HRESULT l_hrOk = S_OK;

	StrUpper.Format( "%u", (int)p_dUpper );
	StrLower.Format( "%u", (int)p_dLower );

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

	if( SV_IS_KIND_OF( PCurrentAnalyzer->GetOwner(), SVTaskObjectClass ) )
	{
		pAnalyzerOwner = ( SVTaskObjectClass* ) PCurrentAnalyzer->GetOwner();
	}

	if( PCurrentAnalyzer )
	{
		SVObjectTypeInfoStruct lineObjectInfo;
		lineObjectInfo.ObjectType = SVLineObjectType;

		pLine = ( SVLineClass* )::SVSendMessage( PCurrentAnalyzer, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<LONG_PTR>(&lineObjectInfo) );
	}

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
	SVLineAnalyzerClass *l_psvAnalyzer = dynamic_cast<SVLineAnalyzerClass *>(PCurrentAnalyzer);

	if( l_psvAnalyzer != NULL )
	{
		l_psvAnalyzer->m_bSetupEdgeA = m_bEdgeA;
	}

	// Run the Tool/Analyzer & update the Display
	SetInspectionData();
			
	return CPropertyPage ::OnSetActive();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVEdgeMarkerAdjustmentPageClass.cpp_v  $
 * 
 *    Rev 1.1   01 Feb 2014 10:32:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:23:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   11 Mar 2013 07:22:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  811
 * SCR Title:  Fix Linear and profile Tool Threshold Slider for Max - % Dif
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a call to SetNormalizeRange in SetScrollRange function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   27 Jan 2011 11:06:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jun 2007 12:46:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Oct 2005 12:00:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed Fixed Edge marker message handler to be kill focus instead of en change.  This fixes the behavior problem with the edit box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Sep 2005 10:25:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added functionality to check appropriate button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Aug 2005 09:32:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Aug 2005 13:41:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to fix issues with data synchrization.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Aug 2005 15:51:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   18 Jul 2005 11:50:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Added static_cast for setting Threshold Value Objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   17 Feb 2005 14:51:20   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   09 Dec 2003 12:30:26   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated line analyzer dialogs to store edge control parameters differently in the value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   04 Dec 2003 13:08:36   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   21 Nov 2003 11:55:56   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems with value objects.  Was setting to last set and not to bucket 1
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   19 Nov 2003 10:24:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Fixed problems with using This Edge
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   17 Nov 2003 08:14:30   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems with the edge/threshold parameters with the value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   13 Nov 2003 07:37:36   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems where it was using the edge struct instead of the value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   08 Oct 2003 16:23:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   08 Oct 2003 11:06:34   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     using the value objects for the edge parameters
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   22 Apr 2003 11:25:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   20 Nov 2002 08:34:28   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed COM support
   Changed ValueObject semantics to work with buckets
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   10 May 2001 15:44:02   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  197
   SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed SVObserver state code in:
   updateDisplay
   updateDisplay
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   06 Sep 2000 16:31:42   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  179
   SCR Title:  Create Color SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to include color SVIM changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   14 Jun 2000 13:41:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  158
   SCR Title:  Profile Tool Lower Threshold Setup problem
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added initEdgeParameters method.
   Revised OnInitDialog to call initEdgeParameters.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   29 Feb 2000 14:28:34   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  106
   SCR Title:  Math Tool - List processing
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed Close Button on caption.
   Added Handler for WM_SYSCOMMAND to restrict SC_CLOSE message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   07 Feb 2000 15:06:58   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  94
   SCR Title:  New Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to correct problem with constructors in
   SVEdgeMarkerAdjustmentPageClass and
   SVThresholdAdjustmentPageClass which were changed for
   the new profile tool
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   04 Feb 2000 09:36:18   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  94
   SCR Title:  New Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to reduce redundant code.
   Revised constructor so this class can be used as a base class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 13:08:02   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   Nov 11 1999 08:54:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Add code to 'save' the location of the measurement property sheet.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   09 Nov 1999 14:22:06   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added updateLineThresholds method called by 
   OnSetActive and OnVscroll 
   (SVImageToLineProject needs to be updated first before
   running)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   Oct 05 1999 09:52:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Edit controls repaired.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   Sep 10 1999 18:09:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     No Change.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   30 Aug 1999 19:24:20   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to use SVRunStatsuClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   27 Aug 1999 13:35:12   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
     Ported to 3.0.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1   Aug 26 1999 18:31:06   jim
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  Steve Jones
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
