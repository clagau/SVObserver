//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgLUTPage
//* .File Name       : $Workfile:   SVTADlgLUTPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 12:50:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAdjustmentLUTPage.h"
#include "SVFormulaEditorSheet.h"
#include "SVObjectLibrary/SVClsids.h"
#include "Definitions/SVUserMessage.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "GuiValueHelper.h"
#include "ObjectInterfaces/IObjectManager.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVToolAdjustmentLUTPage, CPropertyPage)
		ON_BN_CLICKED(IDC_ACTIVATE_CHECK, OnActivateCheck)
		ON_CBN_SELCHANGE(IDC_LUT_MODE_COMBO, OnSelChangeLutModeCombo)
		ON_BN_CLICKED(IDC_FORMULA_BUTTON, OnLUTFormulaButton)
		ON_BN_CLICKED(IDC_CONTINUOUS_RECALC_CHECK, OnContinuousRecalcCheck)
		ON_BN_CLICKED(IDC_CLIP_MODE_CHECK, OnClipModeCheck)
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_MOUSEMOVE()
		ON_WM_VSCROLL()
		ON_WM_HSCROLL()
		ON_EN_KILLFOCUS(IDC_LOWER_EDIT, OnLowerEditLostFocus)
		ON_EN_KILLFOCUS(IDC_UPPER_EDIT, OnUpperEditLostFocus)
		ON_MESSAGE(SV_REFRESH_DIALOG, OnGraphRefresh)
	END_MESSAGE_MAP()

	#pragma region callback functions
	BOOL CALLBACK SVLutGraphMousePointFunction( POINT Point, LPVOID pData )
	{
		if(nullptr != pData)
		{
			SVToolAdjustmentLUTPage* pDlg = reinterpret_cast <SVToolAdjustmentLUTPage*> (pData);
			pDlg->GetValues().Set<BYTE>(SVOutputLUTVectorObjectGuid, static_cast<BYTE> (Point.y), Point.x);

			return true;
		}

		return false;
	}
	#pragma endregion callback functions

	#pragma region Constructor
	SVToolAdjustmentLUTPage::SVToolAdjustmentLUTPage(const GUID& rInspectionID, const GUID& rTaskObjectID, const GUID& rLutEquationID)
	: CPropertyPage(SVToolAdjustmentLUTPage::IDD)
	, m_rInspectionID{ rInspectionID }
	, m_rTaskObjectID{ rTaskObjectID }
	, m_Values{ SvOg::BoundValues{ rInspectionID, rTaskObjectID } }
	, m_LutEquation{ SvOg::BoundValues{ rInspectionID, rLutEquationID } }
	, m_bUseLUT{ false }
	, m_bContinuousRecalcLUT{ false }
	, m_isFormulaClip{true}
	{
	}

	SVToolAdjustmentLUTPage::~SVToolAdjustmentLUTPage()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	HRESULT SVToolAdjustmentLUTPage::SetInspectionData()
	{
		HRESULT Result{ E_FAIL };

		UpdateData(true); // get data from dialog

		m_LutEquation.Set<bool>(SVLUTEquationClipFlagObjectGuid, m_isFormulaClip ? true : false);
		m_LutEquation.Commit(SvOg::PostAction::doNothing);

		long lUpperClip = static_cast<long> (m_upperSlider.GetPos());
		m_Values.Set<long>(SVLUTUpperClipObjectGuid, lUpperClip);

		long lLowerClip = static_cast<long> (m_lowerSlider.GetPos());
		m_Values.Set<long>(SVLUTLowerClipObjectGuid, lLowerClip);

		m_Values.Set<bool>(SVContinuousRecalcLUTObjectGuid, m_bContinuousRecalcLUT ? true : false);
		m_Values.Set<bool>(SVUseLUTObjectGuid, m_bUseLUT ? true : false);

		int CurrentSelection = m_LUTModeCombo.GetCurSel();
		if(0 <= CurrentSelection)
		{
			long lValue = ( long ) m_LUTModeCombo.GetItemData(CurrentSelection);
			m_Values.Set<long>(SVLUTModeObjectGuid, lValue);
		}
	
		Result = m_Values.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);

		return Result;
	}
	#pragma endregion Public Methods

	#pragma region Protected Methods

	#pragma region virtual
	void SVToolAdjustmentLUTPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVToolAdjustmentLUTPage)
		DDX_Control(pDX, IDC_UPPER_SLIDER, m_upperSlider);
		DDX_Control(pDX, IDC_LOWER_SLIDER, m_lowerSlider);
		DDX_Control(pDX, IDC_LUT_GRAPH, m_LUTGraph);
		DDX_Control(pDX, IDC_LUT_MODE_COMBO, m_LUTModeCombo);
		DDX_Control(pDX, IDC_YAXISLABEL, m_yAxisLabel);
		DDX_Text(pDX, IDC_UPPER_EDIT, m_strUpperClipValue);
		DDX_Text(pDX, IDC_LOWER_EDIT, m_strLowerClipValue);
		DDX_Check(pDX, IDC_ACTIVATE_CHECK, m_bUseLUT);
		DDX_Check(pDX, IDC_CONTINUOUS_RECALC_CHECK, m_bContinuousRecalcLUT);
		DDX_Check(pDX, IDC_CLIP_MODE_CHECK, m_isFormulaClip);
		//}}AFX_DATA_MAP
	}

	BOOL SVToolAdjustmentLUTPage::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		m_Values.Init();
		m_LutEquation.Init();

		// Rotate the text for the vertical axis.
		m_yAxisLabel.SetRotationAngle( 90, true );

		// Set Normalize Mode of Graph Control...
		m_LUTGraph.SetNormalizeMode( SvMc::SVNormalize2D );

		// Set Clip Slider Data...
		m_upperSlider.SetRange( 0, 255 );
		m_lowerSlider.SetRange( 0, 255 );

		// Get Use Lut Flag...
		m_bUseLUT = m_Values.Get<bool>(SVUseLUTObjectGuid);
		m_bContinuousRecalcLUT = m_Values.Get<bool>(SVContinuousRecalcLUTObjectGuid);

		const SvOi::NameValueVector& rLUTModeList = m_Values.GetEnumTypes(SVLUTModeObjectGuid);
		m_LUTModeCombo.SetEnumTypes(rLUTModeList);

		m_isFormulaClip = m_LutEquation.Get<bool>(SVLUTEquationClipFlagObjectGuid);

		refresh( false );

		UpdateData( false );

		return true;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}
	#pragma endregion virtual

	void SVToolAdjustmentLUTPage::OnActivateCheck()
	{
		refresh();
	}

	void SVToolAdjustmentLUTPage::OnSelChangeLutModeCombo()
	{
		refresh();
	}

	void SVToolAdjustmentLUTPage::OnLUTFormulaButton()
	{
		std::string Text = SvUl::LoadStdString(IDS_FORMULA_STRING);

		SvOi::IObjectClass* pObject = SvOi::getObject(m_rTaskObjectID);
		SvDef::SVObjectTypeInfoStruct lutObjectInfo;
		lutObjectInfo.ObjectType = SvPb::SVUnaryImageOperatorObjectType;
		lutObjectInfo.SubType = SvPb::SVLUTOperatorObjectType;
		SvOi::IObjectClass* pLUTOperator = pObject->getFirstObject(lutObjectInfo);

		std::string Caption = pLUTOperator->GetName();
		Caption += _T( " " )+ Text;

		const GUID& rObjectID = pLUTOperator->GetUniqueObjectID();
		SvDef::SVObjectTypeInfoStruct info(SvPb::SVEquationObjectType, SvPb::SVLUTEquationObjectType);
		SVFormulaEditorSheetClass dlg( m_rInspectionID, rObjectID, info, Caption.c_str() );

		dlg.DoModal();

		refresh();
	}

	void SVToolAdjustmentLUTPage::OnContinuousRecalcCheck()
	{
		refresh();
	}

	void SVToolAdjustmentLUTPage::OnClipModeCheck()
	{
		refresh();
	}

	LRESULT SVToolAdjustmentLUTPage::OnGraphRefresh( WPARAM mp1, LPARAM mp2 )
	{
		refresh();

		return 0;
	}

	void SVToolAdjustmentLUTPage::OnLButtonDown(UINT nFlags, CPoint point)
	{
		CWnd* pWnd = ChildWindowFromPoint( point );
		if( pWnd == &m_LUTGraph )
		{
			ClientToScreen( &point );
			pWnd->ScreenToClient( &point );
			pWnd->SendMessage( WM_LBUTTONDOWN, nFlags, MAKELPARAM( point.x, point.y ) );
		}
		else
		{
			CPropertyPage::OnLButtonDown(nFlags, point);
		}
	}

	void SVToolAdjustmentLUTPage::OnLButtonUp(UINT nFlags, CPoint point)
	{
		CWnd* pWnd = ChildWindowFromPoint( point );
		if( pWnd == &m_LUTGraph )
		{
			ClientToScreen( &point );
			pWnd->ScreenToClient( &point );
			pWnd->SendMessage( WM_LBUTTONUP, nFlags, MAKELPARAM( point.x, point.y ) );
		}
		else
		{
			CPropertyPage::OnLButtonUp(nFlags, point);
		}
	}

	void SVToolAdjustmentLUTPage::OnMouseMove(UINT nFlags, CPoint point)
	{
		CWnd* pWnd = ChildWindowFromPoint( point );
		if( pWnd == &m_LUTGraph )
		{
			ClientToScreen( &point );
			pWnd->ScreenToClient( &point );
			pWnd->SendMessage( WM_MOUSEMOVE, nFlags, MAKELPARAM( point.x, point.y ) );
		}
		else
		{
			CPropertyPage::OnMouseMove(nFlags, point);
		}
	}

	void SVToolAdjustmentLUTPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		UpdateData( TRUE ); // Get data from dialog...

		CSliderCtrl* pSlider = ( CSliderCtrl* ) pScrollBar;

		if( pSlider == &m_upperSlider )
		{
			refresh();

			return;
		}
		else if( pSlider == &m_lowerSlider )
		{
			refresh();

			return;
		}

		CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
	}

	void SVToolAdjustmentLUTPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		UpdateData(true); // Get data from dialog...

		CSliderCtrl* pSlider = ( CSliderCtrl* ) pScrollBar;

		if( pSlider == &m_upperSlider )
		{
			refresh();

			return;
		}
		else if( pSlider == &m_lowerSlider )
		{
			refresh();

			return;
		}
	
		CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
	}

	void SVToolAdjustmentLUTPage::OnLowerEditLostFocus()
	{
		int lClip = 0;
		UpdateData(true); // Get data from dialog...
		int valNumbers = sscanf(m_strLowerClipValue, _T("%d"), &lClip);
		if (1 == valNumbers)
		{
			m_lowerSlider.SetPos( lClip );
		}
		refresh();
	}

	void SVToolAdjustmentLUTPage::OnUpperEditLostFocus()
	{
		int lClip = 0;
		UpdateData(true); // Get data from dialog...
		int valNumbers = sscanf(m_strUpperClipValue, _T("%d"), &lClip);
		if (1 == valNumbers)
		{
			m_upperSlider.SetPos( lClip );
		}
		refresh();
	}

	void SVToolAdjustmentLUTPage::refresh(bool bSave /*= true*/)
	{
		if( bSave )
		{ 
			SetInspectionData(); 
		}

		refreshLUTGraph();
		refreshClip();

		m_bUseLUT = m_Values.Get<bool>(SVUseLUTObjectGuid);

		long lLUTMode = m_Values.Get<long>(SVLUTModeObjectGuid);
		m_LUTModeCombo.SetCurSelItemData(lLUTMode);
		switch( lLUTMode )
		{
			case 0:	// Identity...
			case 1: // Inversion...
			case 2: // Sign...
			{
				// Deactivate Mouse Proc Func of SVDlgGraph Control...
				m_LUTGraph.SetMousePointProcFunc(nullptr, nullptr);
				hideAllUncommonControls();
				break;
			}
				
			case 3: // Clip...
			{
				// Deactivate Mouse Proc Func of SVDlgGraph Control...
				m_LUTGraph.SetMousePointProcFunc(nullptr, nullptr);
				showClipControls();
				break;
			}

			case 4: // Formula...
			{
				// Deactivate Mouse Proc Func of SVDlgGraph Control...
				m_LUTGraph.SetMousePointProcFunc(nullptr, nullptr);
				showFormulaControls();
				m_isFormulaClip = m_LutEquation.Get<bool>(SVLUTEquationClipFlagObjectGuid);
				break;
			}

			case 5: // Free Form...
			{
				// Set Mouse Proc Func of SVDlgGraph Control...
				m_LUTGraph.SetMousePointProcFunc(SVLutGraphMousePointFunction, this);
				hideAllUncommonControls();
				break;
			}

			default: // Unknown Mode...
			{
				// Deactivate Mouse Proc Func of SVDlgGraph Control...
				m_LUTGraph.SetMousePointProcFunc(nullptr, nullptr);
				hideAllUncommonControls();
				break;
			}
		}

		m_bContinuousRecalcLUT = m_Values.Get<bool>(SVContinuousRecalcLUTObjectGuid);

		if( !m_bUseLUT )
		{	// Deactivate Mouse Proc Func of SVDlgGraph Control...
			m_LUTGraph.SetMousePointProcFunc( nullptr, nullptr );
		}

		enableAllControls( m_bUseLUT );
		UpdateData(false); // set data to dialog
	}

	void SVToolAdjustmentLUTPage::refreshLUTGraph()
	{
		std::vector<BYTE> byteVector = ConvertVariantSafeArrayToVector<BYTE>(m_Values.Get<_variant_t>(SVOutputLUTVectorObjectGuid));
		m_LUTGraph.SetPoints(byteVector);
	}

	void SVToolAdjustmentLUTPage::refreshClip()
	{
		long lUpperClip = m_Values.Get<long>(SVLUTUpperClipObjectGuid);

		m_strUpperClipValue.Format( _T("%d"), lUpperClip );
		m_upperSlider.SetPos(static_cast<int> (lUpperClip));

		long lLowerClip = m_Values.Get<long>(SVLUTLowerClipObjectGuid);

		m_strLowerClipValue.Format( _T("%d"), lLowerClip );
		m_lowerSlider.SetPos(static_cast<int> (lLowerClip));
	}

	void SVToolAdjustmentLUTPage::showFormulaControls()
	{
		CWnd* pWnd = nullptr;
		// Show Formula Button...
		if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
			pWnd->ShowWindow( SW_SHOW );
		if( pWnd = GetDlgItem( IDC_CLIP_MODE_CHECK ) )
			pWnd->ShowWindow( SW_SHOW );
		// Hide Clip Mode controls and others...
		if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
			pWnd->ShowWindow( SW_HIDE );
	}

	void SVToolAdjustmentLUTPage::showClipControls()
	{
		CWnd* pWnd = nullptr;
		// Show Clip Slider, Edit, Captions...
		if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
			pWnd->ShowWindow( SW_SHOW );
		if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
			pWnd->ShowWindow( SW_SHOW );
		if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
			pWnd->ShowWindow( SW_SHOW );
		if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
			pWnd->ShowWindow( SW_SHOW );
		if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
			pWnd->ShowWindow( SW_SHOW );
		if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
			pWnd->ShowWindow( SW_SHOW );
		// Hide Formula Button and others...
		if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_CLIP_MODE_CHECK ) )
			pWnd->ShowWindow( SW_HIDE );
	}

	void SVToolAdjustmentLUTPage::hideAllUncommonControls()
	{
		CWnd* pWnd = nullptr;
		// Hide all controls...
		if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_CLIP_MODE_CHECK ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
			pWnd->ShowWindow( SW_HIDE );
		if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
			pWnd->ShowWindow( SW_HIDE );
	}

	void SVToolAdjustmentLUTPage::enableAllControls(BOOL isEnable)
	{
		CWnd* pWnd = nullptr;
		// Set controls activation states...
		if( pWnd = GetDlgItem( IDC_CONTINUOUS_RECALC_CHECK ) )
			pWnd->EnableWindow( isEnable );
		if( pWnd = GetDlgItem( IDC_LUT_MODE_COMBO ) )
			pWnd->EnableWindow( isEnable );
		if( pWnd = GetDlgItem( IDC_LUT_GRAPH ) )
			pWnd->EnableWindow( isEnable );
		if( pWnd = GetDlgItem( IDC_FORMULA_BUTTON ) )
			pWnd->EnableWindow( isEnable );
		if( pWnd = GetDlgItem( IDC_CLIP_MODE_CHECK ) )
			pWnd->EnableWindow( isEnable );
		if( pWnd = GetDlgItem( IDC_UPPER_EDIT ) )
			pWnd->EnableWindow( isEnable );
		if( pWnd = GetDlgItem( IDC_UPPER_SLIDER ) )
			pWnd->EnableWindow( isEnable );
		if( pWnd = GetDlgItem( IDC_UPPER_STATIC ) )
			pWnd->EnableWindow( isEnable );
		if( pWnd = GetDlgItem( IDC_LOWER_EDIT ) )
			pWnd->EnableWindow( isEnable );
		if( pWnd = GetDlgItem( IDC_LOWER_SLIDER ) )
			pWnd->EnableWindow( isEnable );
		if( pWnd = GetDlgItem( IDC_LOWER_STATIC ) )
			pWnd->EnableWindow( isEnable );
	}
	#pragma endregion Protected Methods
} //namespace SvOg
