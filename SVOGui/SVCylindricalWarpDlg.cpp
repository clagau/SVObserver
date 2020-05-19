//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCylindricalWarpDlg
//* .File Name       : $Workfile:   SVCylindricalWarpDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:12:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVCylindricalWarpDlg.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	constexpr double cMinWarpAngle = 10.0;
	constexpr double cMaxWarpAngle = 180.0;

	BEGIN_MESSAGE_MAP(SVCylindricalWarpDlg, CDialog)
		//{{AFX_MSG_MAP(SVCylindricalWarpDlg)
		ON_CBN_SELCHANGE(IDC_ORIENTATION, OnSelchangeOrientationCombo)
		ON_WM_DESTROY()
		ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnSelchangeInterpolationModeCombo)
		ON_EN_KILLFOCUS(IDC_WARP_ANGLE_EDIT, OnKillfocusWarpAngleEdit)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVCylindricalWarpDlg::SVCylindricalWarpDlg(uint32_t inspectionId, uint32_t taskObjectId)
	: CPropertyPage(IDD)
	, m_InspectionID{ inspectionId }
	, m_TaskObjectID{ taskObjectId }
	, m_values{ SvOg::BoundValues{ inspectionId, taskObjectId } }
	{
	}

	SVCylindricalWarpDlg::~SVCylindricalWarpDlg() 
	{
	}

	void SVCylindricalWarpDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVCylindricalWarpDlg)
		DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_cbInterpolation);
		DDX_Control(pDX, IDC_ORIENTATION, m_cbOrientation);
		DDX_Text(pDX, IDC_WARP_ANGLE_EDIT, m_dWarpAngle);
		//}}AFX_DATA_MAP
	}

	HRESULT SVCylindricalWarpDlg::SetInspectionData(bool ResetTool /*=false*/)
	{
		HRESULT Result{ S_OK };

		UpdateData(true);
		long Value{ 0L };
		int CurrentSelection = m_cbInterpolation.GetCurSel();
		if (0 <= CurrentSelection)
		{
			Value = static_cast<long> (m_cbInterpolation.GetItemData(CurrentSelection));
			m_values.Set<long>(SvPb::OutputInterpolationModeEId, Value);
		}
		CurrentSelection = m_cbOrientation.GetCurSel();
		if (0 <= CurrentSelection)
		{
			Value = static_cast<long> (m_cbOrientation.GetItemData(CurrentSelection));
			m_values.Set<long>(SvPb::WarpTypeEId, Value);
		}
		m_values.Set<double>(SvPb::WarpAngleEId, m_dWarpAngle);
		SvOg::PostAction commitAction {SvOg::PostAction::doRunOnce};
		commitAction = commitAction | (ResetTool ? SvOg::PostAction::doReset : SvOg::PostAction::doNothing);
		Result = m_values.Commit(commitAction);

		return Result;
	}

	BOOL SVCylindricalWarpDlg::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		m_values.Init();

		const SvOi::NameValueVector& rInterpolationModeList = m_values.GetEnumTypes(SvPb::OutputInterpolationModeEId);
		m_cbInterpolation.SetEnumTypes(rInterpolationModeList);
		long CurrentSelection = m_values.Get<long>(SvPb::OutputInterpolationModeEId);
		m_cbInterpolation.SetCurSelItemData(CurrentSelection);

		const SvOi::NameValueVector& WarpTypeList = m_values.GetEnumTypes(SvPb::WarpTypeEId);
		m_cbOrientation.SetEnumTypes(WarpTypeList);
		CurrentSelection = m_values.Get<long>(SvPb::WarpTypeEId);
		m_cbOrientation.SetCurSelItemData(CurrentSelection);

		m_dWarpAngle = m_values.Get<double>(SvPb::WarpAngleEId);

		UpdateData(false);

		return true;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVCylindricalWarpDlg::OnSelchangeInterpolationModeCombo() 
	{
		SetInspectionData();
	}

	void SVCylindricalWarpDlg::OnSelchangeOrientationCombo()
	{
		SetInspectionData(true);
	}

	void SVCylindricalWarpDlg::OnKillfocusWarpAngleEdit() 
	{
		UpdateData(true);
		if (cMinWarpAngle > m_dWarpAngle)
		{
			m_dWarpAngle = cMinWarpAngle;
		}
		else if (cMaxWarpAngle < m_dWarpAngle)
		{
			m_dWarpAngle = cMaxWarpAngle;
		}
		UpdateData(false);

		SetInspectionData();
	}
} //namespace SvOg
