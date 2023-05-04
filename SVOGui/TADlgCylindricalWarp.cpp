//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgCylindricalWarpDlg
//* .File Name       : $Workfile:   TADlgCylindricalWarpDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:12:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgCylindricalWarp.h"
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

	BEGIN_MESSAGE_MAP(TADlgCylindricalWarp, CDialog)
		//{{AFX_MSG_MAP(TADlgCylindricalWarpDlg)
		ON_CBN_SELCHANGE(IDC_ORIENTATION, OnSelchangeOrientationCombo)
		ON_WM_DESTROY()
		ON_CBN_SELCHANGE(IDC_INTERPOLATION_MODE_COMBO, OnSelchangeInterpolationModeCombo)
		ON_EN_KILLFOCUS(IDC_WARP_ANGLE_EDIT, OnKillfocusWarpAngleEdit)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	TADlgCylindricalWarp::TADlgCylindricalWarp(uint32_t inspectionId, uint32_t taskObjectId)
	: CPropertyPage(IDD)
	, m_InspectionID{ inspectionId }
	, m_TaskObjectID{ taskObjectId }
	, m_values{ SvOgu::BoundValues{ inspectionId, taskObjectId } }
	{
	}

	TADlgCylindricalWarp::~TADlgCylindricalWarp() 
	{
	}

	void TADlgCylindricalWarp::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TADlgCylindricalWarpDlg)
		DDX_Control(pDX, IDC_INTERPOLATION_MODE_COMBO, m_cbInterpolation);
		DDX_Control(pDX, IDC_ORIENTATION, m_cbOrientation);
		DDX_Text(pDX, IDC_WARP_ANGLE_EDIT, m_dWarpAngle);
		//}}AFX_DATA_MAP
	}

	HRESULT TADlgCylindricalWarp::SetInspectionData(bool ResetTool /*=false*/)
	{
		HRESULT Result{ S_OK };

		UpdateData(true);
		int CurrentSelection = m_cbInterpolation.GetCurSel();
		if (0 <= CurrentSelection)
		{
			long Value = static_cast<long> (m_cbInterpolation.GetItemData(CurrentSelection));
			m_values.Set<long>(SvPb::OutputInterpolationModeEId, Value);
		}
		CurrentSelection = m_cbOrientation.GetCurSel();
		if (0 <= CurrentSelection)
		{
			long Value = static_cast<long> (m_cbOrientation.GetItemData(CurrentSelection));
			m_values.Set<long>(SvPb::WarpTypeEId, Value);
		}
		m_values.Set<double>(SvPb::WarpAngleEId, m_dWarpAngle);
		SvOgu::PostAction commitAction {SvOgu::PostAction::doRunOnce};
		commitAction = commitAction | (ResetTool ? SvOgu::PostAction::doReset : SvOgu::PostAction::doNothing);
		Result = m_values.Commit(commitAction);

		return Result;
	}

	BOOL TADlgCylindricalWarp::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		m_values.Init();

		const SvDef::NameValueVector& rInterpolationModeList = m_values.GetEnumTypes(SvPb::OutputInterpolationModeEId);
		m_cbInterpolation.SetEnumTypes(rInterpolationModeList);
		long CurrentSelection = m_values.Get<long>(SvPb::OutputInterpolationModeEId);
		m_cbInterpolation.SetCurSelItemData(CurrentSelection);

		const SvDef::NameValueVector& WarpTypeList = m_values.GetEnumTypes(SvPb::WarpTypeEId);
		m_cbOrientation.SetEnumTypes(WarpTypeList);
		CurrentSelection = m_values.Get<long>(SvPb::WarpTypeEId);
		m_cbOrientation.SetCurSelItemData(CurrentSelection);

		m_dWarpAngle = m_values.Get<double>(SvPb::WarpAngleEId);

		UpdateData(false);

		return true;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void TADlgCylindricalWarp::OnSelchangeInterpolationModeCombo() 
	{
		SetInspectionData();
	}

	void TADlgCylindricalWarp::OnSelchangeOrientationCombo()
	{
		SetInspectionData(true);
	}

	void TADlgCylindricalWarp::OnKillfocusWarpAngleEdit() 
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
