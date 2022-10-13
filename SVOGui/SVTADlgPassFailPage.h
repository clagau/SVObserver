//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgPassFailPage
//* .File Name       : $Workfile:   SVTADlgPassFailPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:16:30  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "ISVPropertyPageDialog.h"
#include "SVOGuiUtility/RangeController.h"
#include "Definitions/RangeEnum.h"
#include "SVOGuiUtility/LinkedValueWidgetHelper.h"
#pragma endregion Includes

namespace SvOg
{
	class SVTADlgPassFailPage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
	{
	public:
#pragma region Constructor
		SVTADlgPassFailPage(uint32_t inspectionID, uint32_t taskID, UINT captionID = 0);
		SVTADlgPassFailPage();
		virtual ~SVTADlgPassFailPage();
#pragma endregion Constructor

#pragma region Public Methods
		virtual bool QueryAllowExit() override;
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(SVTADlgPassFailPage)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
		virtual BOOL OnInitDialog() override;
		virtual BOOL OnSetActive() override;
		virtual BOOL OnKillActive() override;
		//}}AFX_VIRTUAL

		//{{AFX_MSG(SVTADlgPassFailPage)
		void OnButtonRange(UINT nID);
		void OnKillFocusRange(UINT nID);
		//{{AFX_MSG
#pragma region Protected Methods

#pragma region Privated Methods
#pragma region Privated Methods

#pragma region Member Variables
	protected:
		//{{AFX_DATA(SVTADlgPassFailPage)
		enum { IDD = IDD_TA_PASS_FAIL_DIALOG };
		std::array<std::unique_ptr<SvOgu::LinkedValueWidgetHelper>, RangeEnum::ER_COUNT> m_RangeWidgets;
		std::array<CButton, RangeEnum::ER_COUNT> m_RangeButtons;
		std::array<CEdit, RangeEnum::ER_COUNT> m_RangeEdits;
		//}}AFX_DATA

	private:
		HRESULT SetInspectionData();
		bool UpdateRangeValues();
		SvOgu::RangeController m_rangeController;
		uint32_t m_toolId;
#pragma endregion Member Variables
	};
} //namespace SvOg
