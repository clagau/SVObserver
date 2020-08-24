//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCylindricalWarpDlg
//* .File Name       : $Workfile:   SVCylindricalWarpDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:05:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "DataController.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvOg
{
	class SVCylindricalWarpDlg : public CPropertyPage
	{
	public:
		long m_lLastWarpType;

		SVCylindricalWarpDlg(uint32_t inspectionId, uint32_t taskObjectId);
		virtual ~SVCylindricalWarpDlg();

		//{{AFX_DATA(SVCylindricalWarpDlg)
		enum { IDD = IDD_TA_CYLINDRICAL_TOOL_DIALOG };
		SvMc::SVEnumerateCombo	m_cbInterpolation;
		SvMc::SVEnumerateCombo	m_cbOrientation;
		double	m_dWarpAngle{ 0.0 };
		//}}AFX_DATA

		protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	// Implementation
	protected:
		HRESULT SetInspectionData(bool ResetTool = false);

		//{{AFX_MSG(SVCylindricalWarpDlg)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangeOrientationCombo();
		afx_msg void OnSelchangeInterpolationModeCombo();
		afx_msg void OnKillfocusWarpAngleEdit();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		ValueController m_values;
	};
} //namespace SvOg
