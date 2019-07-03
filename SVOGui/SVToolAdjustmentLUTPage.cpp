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
#pragma endregion Includes

#pragma region Declarations
static const std::array<SVGUID, 4> m_stretchValueGuids {SVLUTMinInputObjectGuid, SVLUTMaxInputObjectGuid, SVLUTMinOutputObjectGuid, SVLUTMaxOutputObjectGuid};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOg
{

const std::vector<int> cCommonControls = {IDC_CONTINUOUS_RECALC_CHECK, IDC_LUT_MODE_COMBO, IDC_LUT_GRAPH};
const std::vector<int> cFormulaControls = {IDC_FORMULA_BUTTON, IDC_CLIP_MODE_CHECK};
const std::vector<int> cClipControls = {IDC_UPPER_EDIT, IDC_UPPER_SLIDER, IDC_UPPER_STATIC, IDC_LOWER_EDIT, IDC_LOWER_SLIDER, IDC_LOWER_STATIC};
const std::vector<int> cStretchnControls = {IDC_LUT_MIN_INPUT_EDIT, IDC_LUT_MAX_INPUT_EDIT, IDC_LUT_MIN_OUTPUT_EDIT, IDC_LUT_MAX_OUTPUT_EDIT,
IDC_LUT_MIN_INPUT_SLIDER, IDC_LUT_MAX_INPUT_SLIDER, IDC_LUT_MIN_OUTPUT_SLIDER, IDC_LUT_MAX_OUTPUT_SLIDER,
IDC_LUT_MIN_INPUT_STATIC, IDC_LUT_MAX_INPUT_STATIC, IDC_LUT_MIN_OUTPUT_STATIC, IDC_LUT_MAX_OUTPUT_STATIC};

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
		ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_LUT_MIN_INPUT_EDIT, IDC_LUT_MAX_OUTPUT_EDIT, OnEditLostFocus)
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

		for (int i = 0; i < MAX_STRETCH_CONTROLS; i++)
		{
			long value = static_cast<long> (m_stretchSliders[i].GetPos());
			m_Values.Set<long>(m_stretchValueGuids[i], value);
		}

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
		DDX_Control(pDX, IDC_LUT_MIN_INPUT_SLIDER, m_stretchSliders[0]);
		DDX_Control(pDX, IDC_LUT_MAX_INPUT_SLIDER, m_stretchSliders[1]);
		DDX_Control(pDX, IDC_LUT_MIN_OUTPUT_SLIDER, m_stretchSliders[2]);
		DDX_Control(pDX, IDC_LUT_MAX_OUTPUT_SLIDER, m_stretchSliders[3]);
		DDX_Control(pDX, IDC_LUT_GRAPH, m_LUTGraph);
		DDX_Control(pDX, IDC_LUT_MODE_COMBO, m_LUTModeCombo);
		DDX_Control(pDX, IDC_YAXISLABEL, m_yAxisLabel);
		DDX_Text(pDX, IDC_UPPER_EDIT, m_strUpperClipValue);
		DDX_Text(pDX, IDC_LOWER_EDIT, m_strLowerClipValue);
		DDX_Text(pDX, IDC_LUT_MIN_INPUT_EDIT, m_strStretchValues[0]);
		DDX_Text(pDX, IDC_LUT_MAX_INPUT_EDIT, m_strStretchValues[1]);
		DDX_Text(pDX, IDC_LUT_MIN_OUTPUT_EDIT, m_strStretchValues[2]);
		DDX_Text(pDX, IDC_LUT_MAX_OUTPUT_EDIT, m_strStretchValues[3]);
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

		// Set Stretch Slider Data...
		for (auto& slider : m_stretchSliders)
		{
			slider.SetRange(0, 255);
		}

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
		std::string Caption;
		SvPb::InspectionCmdMsgs request, response;
		SvPb::GetObjectParametersRequest* pGetObjectNameRequest = request.mutable_getobjectparametersrequest();

		SvPb::SetGuidInProtoBytes(pGetObjectNameRequest->mutable_objectid(), m_rTaskObjectID);
		HRESULT hr = SvCmd::InspectionCommands(m_rInspectionID, request, &response);
		if (S_OK == hr && response.has_getobjectparametersresponse())
		{
			Caption = response.getobjectparametersresponse().name();
		}
		
		Caption += _T(" ") + SvUl::LoadStdString(IDS_FORMULA_STRING);
		SVFormulaEditorSheetClass dlg(m_rInspectionID, m_rTaskObjectID, {SvPb::SVEquationObjectType, SvPb::SVLUTEquationObjectType}, Caption.c_str());
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

		if (pSlider == &m_upperSlider || pSlider == &m_lowerSlider ||
			pSlider == &m_stretchSliders[0] || pSlider == &m_stretchSliders[1] ||
			pSlider == &m_stretchSliders[2] || pSlider == &m_stretchSliders[3])
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

		if (pSlider == &m_upperSlider || pSlider == &m_lowerSlider ||
			pSlider == &m_stretchSliders[0] || pSlider == &m_stretchSliders[1] ||
			pSlider == &m_stretchSliders[2] || pSlider == &m_stretchSliders[3])
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

	void SVToolAdjustmentLUTPage::OnEditLostFocus(UINT nID)
	{
		int buttonIndex(nID - IDC_LUT_MIN_INPUT_EDIT);
		if (0 <= buttonIndex && MAX_STRETCH_CONTROLS > buttonIndex)
		{
			int lClip = 0;
			UpdateData(true); // Get data from dialog...
			int valNumbers = sscanf(m_strStretchValues[buttonIndex], _T("%d"), &lClip);
			if (1 == valNumbers)
			{
				m_stretchSliders[buttonIndex].SetPos(lClip);
			}
			refresh();
		}		
	}

	void SVToolAdjustmentLUTPage::refresh(bool bSave /*= true*/)
	{
		if( bSave )
		{ 
			SetInspectionData(); 
		}

		refreshLUTGraph();
		refreshClip();
		refreshStretch();

		m_bUseLUT = m_Values.Get<bool>(SVUseLUTObjectGuid);

		long lLUTMode = m_Values.Get<long>(SVLUTModeObjectGuid);
		m_LUTModeCombo.SetCurSelItemData(lLUTMode);
		switch( lLUTMode )
		{
			case SvPb::LUTIdentity:	// Identity...
			case SvPb::LUTInversion: // Inversion...
			case SvPb::LUTSign: // Sign...
			{
				// Deactivate Mouse Proc Func of SVDlgGraph Control...
				m_LUTGraph.SetMousePointProcFunc(nullptr, nullptr);
				hideAllUncommonControls();
				break;
			}
				
			case SvPb::LUTClip: // Clip...
			{
				// Deactivate Mouse Proc Func of SVDlgGraph Control...
				m_LUTGraph.SetMousePointProcFunc(nullptr, nullptr);
				showClipControls();
				break;
			}

			case SvPb::LUTFormula: // Formula...
			{
				// Deactivate Mouse Proc Func of SVDlgGraph Control...
				m_LUTGraph.SetMousePointProcFunc(nullptr, nullptr);
				showFormulaControls();
				m_isFormulaClip = m_LutEquation.Get<bool>(SVLUTEquationClipFlagObjectGuid);
				break;
			}

			case SvPb::LUTFreeForm: // Free Form...
			{
				// Set Mouse Proc Func of SVDlgGraph Control...
				m_LUTGraph.SetMousePointProcFunc(SVLutGraphMousePointFunction, this);
				hideAllUncommonControls();
				break;
			}

			case SvPb::LUTStretch: // Stretch...
			{
				// Deactivate Mouse Proc Func of SVDlgGraph Control...
				m_LUTGraph.SetMousePointProcFunc(nullptr, nullptr);
				showStretchControls();
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

	void SVToolAdjustmentLUTPage::refreshStretch()
	{
		for (int i = 0; i < MAX_STRETCH_CONTROLS; i++)
		{
			long value = m_Values.Get<long>(m_stretchValueGuids[i]);
			m_strStretchValues[i].Format(_T("%d"), value);
			m_stretchSliders[i].SetPos(static_cast<int> (value));
		}
	}

	void SVToolAdjustmentLUTPage::showFormulaControls()
	{
		std::vector<int> otherControls = cClipControls;
		otherControls.insert(otherControls.end(), cStretchnControls.begin(), cStretchnControls.end());

		CWnd* pWnd = nullptr;
		for (const int id : otherControls)
		{
			if (pWnd = GetDlgItem(id))
			{
				pWnd->ShowWindow(SW_HIDE);
			}
		}
		// Show Formula Button...
		for (const int id : cFormulaControls)
		{
			if (pWnd = GetDlgItem(id))
			{
				pWnd->ShowWindow(SW_SHOW);
			}
		}
	}

	void SVToolAdjustmentLUTPage::showClipControls()
	{
		std::vector<int> otherControls = cStretchnControls;
		otherControls.insert(otherControls.end(), cFormulaControls.begin(), cFormulaControls.end());

		CWnd* pWnd = nullptr;
		for (const int id : otherControls)
		{
			if (pWnd = GetDlgItem(id))
			{
				pWnd->ShowWindow(SW_HIDE);
			}
		}

		// Show Clip Slider, Edit, Captions...
		for (const int id : cClipControls)
		{
			if (pWnd = GetDlgItem(id))
			{
				pWnd->ShowWindow(SW_SHOW);
			}
		}
	}

	void SVToolAdjustmentLUTPage::showStretchControls()
	{
		std::vector<int> otherControls = cClipControls;
		otherControls.insert(otherControls.end(), cFormulaControls.begin(), cFormulaControls.end());

		CWnd* pWnd = nullptr;
		for (const int id : otherControls)
		{
			if (pWnd = GetDlgItem(id))
			{
				pWnd->ShowWindow(SW_HIDE);
			}
		}

		for (const int id : cStretchnControls)
		{
			if (pWnd = GetDlgItem(id))
			{
				pWnd->ShowWindow(SW_SHOW);
			}
		}
	}

	void SVToolAdjustmentLUTPage::hideAllUncommonControls()
	{
		std::vector<int> allControls = cFormulaControls;
		allControls.insert(allControls.end(), cClipControls.begin(), cClipControls.end());
		allControls.insert(allControls.end(), cStretchnControls.begin(), cStretchnControls.end());

		CWnd* pWnd = nullptr;
		// Hide all controls...
		for (const int id : allControls)
		{
			if (pWnd = GetDlgItem(id))
			{
				pWnd->ShowWindow(SW_HIDE);
			}
		}
	}

	void SVToolAdjustmentLUTPage::enableAllControls(BOOL isEnable)
	{
		CWnd* pWnd = nullptr;

		std::vector<int> allControls = cCommonControls;
		allControls.insert(allControls.end(), cFormulaControls.begin(), cFormulaControls.end());
		allControls.insert(allControls.end(), cClipControls.begin(), cClipControls.end());
		allControls.insert(allControls.end(), cStretchnControls.begin(), cStretchnControls.end());

		// Set controls activation states...
		for (const int id : allControls)
		{
			if (pWnd = GetDlgItem(id))
			{
				pWnd->EnableWindow(isEnable);
			}
		}

	}
	#pragma endregion Protected Methods
} //namespace SvOg
