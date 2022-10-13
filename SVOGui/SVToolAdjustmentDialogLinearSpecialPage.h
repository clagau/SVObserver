//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogLinearSpecialPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogLinearSpecialPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:36:50  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVOGuiUtility/DataController.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvOg
{
	class SVToolAdjustmentDialogLinearSpecialPage : public CPropertyPage
	{
	public:
		SVToolAdjustmentDialogLinearSpecialPage(uint32_t inspectionId, uint32_t taskObjectId);
		virtual ~SVToolAdjustmentDialogLinearSpecialPage();

	protected:
		HRESULT SetInspectionData();

		enum { IDD = IDD_TA_LINEAR_TOOL_PROPERTIES_DIALOG };
		CButton	m_ctlUseRotation;
		SvMc::SVEnumerateCombo	m_ctlProfileOrientation;
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG(SVToolAdjustmentDialogLinearSpecialPageClass)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangeComboProfileOrientation();
		afx_msg void OnCheckRotation();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		SvOgu::ValueController m_values;
	};
} //namespace SvOg
