//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgPerspectiveWarpPage
//* .File Name       : $Workfile:   TADlgPerspectiveWarpPage  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:22:22  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVOGuiUtility/DataController.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvOg
{
	class TADlgPerspectiveWarpPage : public CPropertyPage
	{
	// Construction
	public:
		TADlgPerspectiveWarpPage( uint32_t inspectionId, uint32_t taskObjectId, int id = IDD );
		virtual ~TADlgPerspectiveWarpPage();

		void SetInspectionData();

		enum { IDD = IDD_TA_PERSPECTIVE_TOOL_DIALOG };
		SvMc::SVEnumerateCombo	m_FunctionCombo;
		SvMc::SVEnumerateCombo	m_cbInterpolation;
		protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	protected:
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangePerspectiveUnwarpTypeCombo();
		afx_msg void OnSelchangeInterpolationModeCombo(); 
		afx_msg void OnDestroy();
		DECLARE_MESSAGE_MAP()

	private:
		bool setExtentImageProperty(SvPb::SVExtentPropertyEnum eProperty, double value);
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		SvOgu::ValueController m_values;
		long m_lLastWarpType;
	};
} //namespace SvOg
