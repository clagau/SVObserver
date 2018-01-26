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
#include "Definitions/LinearEdgeEnums.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	SVEdgeMarkerAdjustmentPageClass::SVEdgeMarkerAdjustmentPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SVGuidVector& rEdgeEmbeddedGuids, UINT nIDCaption /*= 0*/, int ID /*= IDD*/)
	: CPropertyPage(ID, nIDCaption)
	, m_Values{ SvOg::BoundValues{ rInspectionID, rTaskObjectID } }
	, m_rInspectionID{ rInspectionID }
	, m_rTaskObjectID{ rTaskObjectID }
	, m_rEdgeEmbeddedGuids{ rEdgeEmbeddedGuids }
	, StrLower( _T("") )
	, StrUpper( _T("") )
	, StrPositionOffset( _T("") )
	, StrEdgeSelectThis( _T("") )
	{
	}

	SVEdgeMarkerAdjustmentPageClass::~SVEdgeMarkerAdjustmentPageClass()
	{
	}

	HRESULT SVEdgeMarkerAdjustmentPageClass::GetInspectionData()
	{
		HRESULT Result = S_OK;

		DWORD Upper( 0 );
		DWORD Lower( 0 );

		Upper = m_Values.Get<DWORD>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeUpperThresholdValueGuid]);
		Lower = m_Values.Get<DWORD>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeLowerThresholdValueGuid]);

		Result = UpdateSliderData( Lower, Upper );

		bool bIsFixedEdgeMarker = m_Values.Get<bool>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeIsFixedEdgeMarkerGuid]);

		if( bIsFixedEdgeMarker )
		{
			CheckRadioButton( IDC_SEARCHED_EDGE_MARKER_RADIO, IDC_FIXED_EDGE_MARKER_RADIO, IDC_FIXED_EDGE_MARKER_RADIO );
		}
		else
		{
			CheckRadioButton( IDC_SEARCHED_EDGE_MARKER_RADIO, IDC_FIXED_EDGE_MARKER_RADIO, IDC_SEARCHED_EDGE_MARKER_RADIO );
		}

		long Direction = m_Values.Get<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeDirectionGuid]);
		switch( Direction )
		{
			case SvDef::SV_HEAD_TO_TAIL_DIRECTION:
				CheckRadioButton( IDC_DIRECTION_HEAD_TO_TAIL_RADIO, IDC_DIRECTION_TAIL_TO_HEAD_RADIO, IDC_DIRECTION_HEAD_TO_TAIL_RADIO );
				break;

			case SvDef::SV_TAIL_TO_HEAD_DIRECTION:
				CheckRadioButton( IDC_DIRECTION_HEAD_TO_TAIL_RADIO, IDC_DIRECTION_TAIL_TO_HEAD_RADIO, IDC_DIRECTION_TAIL_TO_HEAD_RADIO );
				break;
		}

		long EdgeSelect = m_Values.Get<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeSelectGuid]);
		switch( EdgeSelect )
		{
			case SvDef::SV_FIRST_EDGE:
			{
				CheckRadioButton( IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_THIS_RADIO, IDC_EDGE_SELECT_FIRST_RADIO );

				CWnd* pWnd = GetDlgItem( IDC_EDGE_SELECT_THIS_EDIT );
				if (nullptr != pWnd)
				{
					pWnd->EnableWindow(false);
				}
				break;
			}
			case SvDef::SV_LAST_EDGE:
			{
				CheckRadioButton( IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_THIS_RADIO, IDC_EDGE_SELECT_LAST_RADIO );

				CWnd* pWnd = GetDlgItem( IDC_EDGE_SELECT_THIS_EDIT );
				if (nullptr != pWnd)
				{
					pWnd->EnableWindow(false);
				}
				break;
			}
			case SvDef::SV_THIS_EDGE:
			{
				CheckRadioButton( IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_THIS_RADIO, IDC_EDGE_SELECT_THIS_RADIO );

				CWnd* pWnd = GetDlgItem( IDC_EDGE_SELECT_THIS_EDIT );
				if (nullptr != pWnd)
				{
					pWnd->EnableWindow(false);
				}
				break;
			}
		}

		double dValue = m_Values.Get<double>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeSelectThisValueGuid]);
		StrEdgeSelectThis.Format( "%.2f", dValue );

		long Polarisation = m_Values.Get<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgePolarisationGuid]);
		switch( Polarisation )
		{
			case SvDef::SV_POSITIVE_POLARISATION:
			{
				CheckRadioButton(IDC_POLARISATION_POSITIVE_RADIO, IDC_POLARISATION_ANY_RADIO, IDC_POLARISATION_POSITIVE_RADIO);
				break;
			}

			case SvDef::SV_NEGATIVE_POLARISATION:
			{
				CheckRadioButton(IDC_POLARISATION_POSITIVE_RADIO, IDC_POLARISATION_ANY_RADIO, IDC_POLARISATION_NEGATIVE_RADIO);
				break;
			}

			case SvDef::SV_ANY_POLARISATION:
			{
				CheckRadioButton(IDC_POLARISATION_POSITIVE_RADIO, IDC_POLARISATION_ANY_RADIO, IDC_POLARISATION_ANY_RADIO);
				break;
			}
		}

		long Position = m_Values.Get<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgePositionGuid]);
		switch(Position)
		{
			case SvDef::SV_START_POSITION:
			{
				CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_START_RADIO );

				CWnd* pWnd = GetDlgItem( IDC_POSITION_OFFSET_EDIT );
				if( nullptr != pWnd )
				{
					pWnd->EnableWindow(false);
				}
				break;
			}
			case SvDef::SV_CENTER_POSITION:
			{
				CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_CENTER_RADIO );

				CWnd* pWnd = GetDlgItem( IDC_POSITION_OFFSET_EDIT );
				if( nullptr != pWnd )
				{
					pWnd->EnableWindow(false);
				}
				break;
			}
			case SvDef::SV_END_POSITION:
			{
				CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_END_RADIO );

				CWnd* pWnd = GetDlgItem( IDC_POSITION_OFFSET_EDIT );
				if( nullptr != pWnd )
				{
					pWnd->EnableWindow(false);
				}
				break;
			}
			case SvDef::SV_OFFSET_POSITION:
			{
				CheckRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO, IDC_POSITION_OFFSET_RADIO );

				CWnd* pWnd = GetDlgItem( IDC_POSITION_OFFSET_EDIT );
				if( nullptr != pWnd )
				{
					pWnd->EnableWindow(true);
				}
				break;
			}
		}

		dValue = m_Values.Get<double>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgePositionOffsetGuid]);
		StrPositionOffset.Format( "%.2f", dValue );

		// Acivate controls regarding the analyzer requirements
		// or let them deactivated...
		std::vector<UINT> EnableWindowVector;
		if( m_bEnableThreshold )
		{
			const std::vector<UINT> CtrlIDs =
			{
				IDC_THRESHOLD_FRAME_STATIC,  IDC_UPPER_THRESHOLD_SLIDER, IDC_LOWER_THRESHOLD_SLIDER, 
				IDC_LOWER_THRESHOLD_STATIC, IDC_UPPER_THRESHOLD_STATIC
			};
			EnableWindowVector.insert(EnableWindowVector.end(), CtrlIDs.begin(), CtrlIDs.end());
		}

		if( m_bEnablePosition )
		{
			const std::vector<UINT> CtrlIDs =
			{
				IDC_POSITION_FRAME_STATIC,  IDC_POSITION_CENTER_RADIO, IDC_POSITION_END_RADIO,
				IDC_POSITION_OFFSET_RADIO, IDC_POSITION_START_RADIO, IDC_FIXED_EDGE_MARKER_RADIO
			};
			EnableWindowVector.insert(EnableWindowVector.end(), CtrlIDs.begin(), CtrlIDs.end());
		}
		else
		{
			if( CWnd* pWnd = GetDlgItem( IDC_POSITION_OFFSET_EDIT ) )
			{
				pWnd->EnableWindow(false);
			}
		}

		if( m_bEnableEdgeSelect )
		{
			const std::vector<UINT> CtrlIDs =
			{
				IDC_EDGESELECT_FRAME_STATIC,  IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_LAST_RADIO,
				IDC_EDGE_SELECT_THIS_RADIO, IDC_SEARCHED_EDGE_MARKER_RADIO
			};
			EnableWindowVector.insert(EnableWindowVector.end(), CtrlIDs.begin(), CtrlIDs.end());
		}
		else
		{
			if (CWnd* pWnd = GetDlgItem(IDC_EDGE_SELECT_THIS_EDIT))
			{
				pWnd->EnableWindow(false);
			}
		}

		if( m_bEnablePolarisation )
		{
			const std::vector<UINT> CtrlIDs =
			{
				IDC_POLARISATION_FRAME_STATIC,  IDC_POLARISATION_ANY_RADIO, IDC_POLARISATION_NEGATIVE_RADIO,
				IDC_POLARISATION_POSITIVE_RADIO, IDC_SEARCHED_EDGE_MARKER_RADIO
			};
			EnableWindowVector.insert(EnableWindowVector.end(), CtrlIDs.begin(), CtrlIDs.end());
		}

		if( m_bEnableDirection )
		{
			const std::vector<UINT> CtrlIDs =
			{
				IDC_DIRECTION_FRAME_STATIC,  IDC_DIRECTION_HEAD_TO_TAIL_RADIO, IDC_DIRECTION_TAIL_TO_HEAD_RADIO, IDC_SEARCHED_EDGE_MARKER_RADIO
			};
			EnableWindowVector.insert(EnableWindowVector.end(), CtrlIDs.begin(), CtrlIDs.end());
		}

		for (auto const& rEntry : EnableWindowVector)
		{
			if (CWnd* pWnd = GetDlgItem(rEntry))
			{
				pWnd->EnableWindow(true);
			}
		}

		UpdateData( FALSE );

		return Result;
	}

	HRESULT SVEdgeMarkerAdjustmentPageClass::SetInspectionData()
	{
		HRESULT Result{ S_OK };

		UpdateData(true);

		DWORD Value = static_cast<DWORD> (atol(StrUpper));
		m_Values.Set<DWORD>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeUpperThresholdValueGuid], Value);
		Value = static_cast<DWORD> (atol(StrLower));
		m_Values.Set<DWORD>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeLowerThresholdValueGuid], Value);

		switch ( GetCheckedRadioButton( IDC_SEARCHED_EDGE_MARKER_RADIO, IDC_FIXED_EDGE_MARKER_RADIO ) )
		{
			case IDC_SEARCHED_EDGE_MARKER_RADIO:
			{
				m_Values.Set<bool>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeIsFixedEdgeMarkerGuid], false);
				break;
			}
			case IDC_FIXED_EDGE_MARKER_RADIO:
			{
				m_Values.Set<bool>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeIsFixedEdgeMarkerGuid], true);
				break;
			}
		}

		switch ( GetCheckedRadioButton( IDC_DIRECTION_HEAD_TO_TAIL_RADIO, IDC_DIRECTION_TAIL_TO_HEAD_RADIO ) )
		{
			case IDC_DIRECTION_HEAD_TO_TAIL_RADIO:
			{
				m_Values.Set<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeDirectionGuid], static_cast<long> (SvDef::SV_HEAD_TO_TAIL_DIRECTION));
				break;
			}
			case IDC_DIRECTION_TAIL_TO_HEAD_RADIO:
			{
				m_Values.Set<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeDirectionGuid], static_cast<long> (SvDef::SV_TAIL_TO_HEAD_DIRECTION));
				break;
			}
		}

		switch ( GetCheckedRadioButton( IDC_EDGE_SELECT_FIRST_RADIO, IDC_EDGE_SELECT_THIS_RADIO ) )
		{
			case IDC_EDGE_SELECT_FIRST_RADIO:
			{
				m_Values.Set<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeSelectGuid], static_cast<long> (SvDef::SV_FIRST_EDGE));
				break;
			}
			case IDC_EDGE_SELECT_LAST_RADIO:
			{
				m_Values.Set<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeSelectGuid], static_cast<long> (SvDef::SV_LAST_EDGE));
				break;
			}
			case IDC_EDGE_SELECT_THIS_RADIO:
			{
				m_Values.Set<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeSelectGuid], static_cast<long> (SvDef::SV_THIS_EDGE));
				m_Values.Set<double>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgeSelectThisValueGuid], atof(StrEdgeSelectThis));
				break;
			}
		}

		switch ( GetCheckedRadioButton( IDC_POLARISATION_POSITIVE_RADIO, IDC_POLARISATION_ANY_RADIO ) )
		{
			case IDC_POLARISATION_ANY_RADIO:
			{
				m_Values.Set<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgePolarisationGuid], static_cast<long> (SvDef::SV_ANY_POLARISATION));
				break;
			}
			case IDC_POLARISATION_NEGATIVE_RADIO:
			{
				m_Values.Set<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgePolarisationGuid], static_cast<long> (SvDef::SV_NEGATIVE_POLARISATION));
				break;
			}
			case IDC_POLARISATION_POSITIVE_RADIO:
			{
				m_Values.Set<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgePolarisationGuid], static_cast<long> (SvDef::SV_POSITIVE_POLARISATION));
				break;
			}
		}

		switch ( GetCheckedRadioButton( IDC_POSITION_START_RADIO, IDC_POSITION_OFFSET_RADIO ) )
		{
			case IDC_POSITION_CENTER_RADIO:
			{
				m_Values.Set<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgePositionGuid], static_cast<long> (SvDef::SV_CENTER_POSITION));
				break;
			}
			case IDC_POSITION_START_RADIO:
			{
				m_Values.Set<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgePositionGuid], static_cast<long> (SvDef::SV_START_POSITION));
				break;
			}
			case IDC_POSITION_END_RADIO:
			{
				m_Values.Set<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgePositionGuid], static_cast<long> (SvDef::SV_END_POSITION));
				break;
			}
			case IDC_POSITION_OFFSET_RADIO:
			{
				m_Values.Set<long>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgePositionGuid], static_cast<long> (SvDef::SV_OFFSET_POSITION));
				m_Values.Set<double>(m_rEdgeEmbeddedGuids[EdgeEmbeddedGuids::EdgePositionOffsetGuid], atof(StrPositionOffset));
				break;
			}
		}

		m_Values.Commit();

		if( S_OK != GetInspectionData() )
		{
			Result = E_FAIL;
		}

		return Result;
	}

	void SVEdgeMarkerAdjustmentPageClass::SetNormalizer( const SVValueBaseNormalizerClass &l_rsvNormalizer )
	{
		Normalizer = l_rsvNormalizer;
		LowerNormalizer = l_rsvNormalizer;
	}

	HRESULT SVEdgeMarkerAdjustmentPageClass::UpdateSliderData(DWORD Lower, DWORD Upper)
	{
		HRESULT l_hrOk = S_OK;

		StrUpper.Format( "%u", Upper);
		StrLower.Format( "%u", Lower);

		setScrollPos(&UpperSliderCtrl, Upper);
		setScrollPos(&LowerSliderCtrl, Lower);

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
	// Behandlungsroutinen f�r Nachrichten SVEdgeMarkerAdjustmentPageClass 

	BOOL SVEdgeMarkerAdjustmentPageClass::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		//Enum and m_rEdgeEmbeddedGuids size must be the same
		assert(EdgeEmbeddedGuids::EdgeGuidCount == m_rEdgeEmbeddedGuids.size());

		m_Values.Init();

		if( GUID_NULL == m_rTaskObjectID )
		{
			GetParent()->SendMessage(WM_CLOSE);
		}

		setScrollRange( &UpperSliderCtrl, static_cast<int>(Normalizer.GetRealRangeMin()), static_cast<int>(Normalizer.GetRealRangeMax()) );
		setScrollRange( &LowerSliderCtrl, static_cast<int>(LowerNormalizer.GetRealRangeMin()), static_cast<int>(LowerNormalizer.GetRealRangeMax()) );

		// initialize edge parameters
		GetInspectionData();

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
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
} //namespace SvOg
