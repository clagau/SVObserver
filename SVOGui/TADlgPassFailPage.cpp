//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgPassFailPage
//* .File Name       : $Workfile:   TADlgPassFailPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:44  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <string>
#include "TADlgPassFailPage.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(TADlgPassFailPage, CPropertyPage)
		//{{AFX_MSG_MAP(TADlgPassFailPage)
		ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_FAILHIGH, IDC_BUTTON_FAILLOW, OnButtonRange)
		ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_FAILHIGH, IDC_EDIT_FAILLOW, OnKillFocusRange)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
	#pragma endregion Declarations

	#pragma region Constructor
	TADlgPassFailPage::TADlgPassFailPage(uint32_t inspectionID, uint32_t taskID, UINT captionID)
	: CPropertyPage( TADlgPassFailPage::IDD, captionID )
	, m_rangeController(inspectionID, taskID)
	, m_toolId(taskID)
	{
		m_rangeController.Init();
	}
	#pragma endregion Constructor

	TADlgPassFailPage::~TADlgPassFailPage()
	{
	}

	#pragma region Public Methods
	bool TADlgPassFailPage::QueryAllowExit()
	{
		UpdateData(true);

		return UpdateRangeValues();
	}
	#pragma endregion Public Methods

	HRESULT TADlgPassFailPage::SetInspectionData()
	{
		UpdateData(true); // get data from dialog

			// Validate Entered data for existence and if within bounds		
		m_rangeController.IsFieldValid(RangeEnum::ERange::ER_FailHigh);
		m_rangeController.IsFieldValid(RangeEnum::ERange::ER_FailLow);
		m_rangeController.IsFieldValid(RangeEnum::ERange::ER_WarnHigh);
		m_rangeController.IsFieldValid(RangeEnum::ERange::ER_WarnLow);

		return m_rangeController.Commit();
	}

	#pragma region Protected Methods
	void TADlgPassFailPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);

		//{{AFX_DATA_MAP(TADlgPassFailPage)
		DDX_Control(pDX, IDC_BUTTON_FAILHIGH, m_RangeButtons[RangeEnum::ER_FailHigh]);
		DDX_Control(pDX, IDC_BUTTON_WARNHIGH, m_RangeButtons[RangeEnum::ER_WarnHigh]);
		DDX_Control(pDX, IDC_BUTTON_WARNLOW, m_RangeButtons[RangeEnum::ER_WarnLow]);
		DDX_Control(pDX, IDC_BUTTON_FAILLOW, m_RangeButtons[RangeEnum::ER_FailLow]);
		DDX_Control(pDX, IDC_EDIT_FAILHIGH, m_RangeEdits[RangeEnum::ER_FailHigh]);
		DDX_Control(pDX, IDC_EDIT_WARNHIGH, m_RangeEdits[RangeEnum::ER_WarnHigh]);
		DDX_Control(pDX, IDC_EDIT_WARNLOW, m_RangeEdits[RangeEnum::ER_WarnLow]);
		DDX_Control(pDX, IDC_EDIT_FAILLOW, m_RangeEdits[RangeEnum::ER_FailLow]);
		//}}AFX_DATA_MAP
	}

	BOOL TADlgPassFailPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		for (int i = 0; i < m_RangeWidgets.size(); ++i)
		{
			m_RangeWidgets[i] = m_rangeController.createWidget(static_cast<RangeEnum::ERange>(i), m_RangeEdits[i], m_RangeButtons[i]);
		}

		UpdateData(false); // set data to dialog

		return true;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur�ckgeben
	}

	BOOL TADlgPassFailPage::OnSetActive() 
	{
		m_rangeController.Init();
		for (auto& rWidget : m_RangeWidgets)
		{
			rWidget->update();
		}
		return CPropertyPage::OnSetActive();
	}

	BOOL TADlgPassFailPage::OnKillActive() 
	{
		UpdateData(true);

		auto* pWnd = GetFocus();
		if (nullptr != pWnd)
		{
			auto nId = pWnd->GetDlgCtrlID();
			if (std::ranges::any_of(m_RangeEdits, [nId](const auto& rEntry) { return nId == rEntry.GetDlgCtrlID(); }))
			{
				OnKillFocusRange(nId);
			}
		}

		bool bOk = UpdateRangeValues();

		if (bOk)
		{
			bOk = CPropertyPage::OnKillActive();
		}
		return bOk;
	}

	void TADlgPassFailPage::OnButtonRange(UINT nID)
	{
		int index(nID - IDC_BUTTON_FAILHIGH);

		Log_Assert(m_RangeWidgets.size() > index && 0 <= index && m_RangeWidgets[index]);
		if (m_RangeWidgets.size() > index && 0 <= index && m_RangeWidgets[index])
		{
			m_RangeWidgets[index]->OnButton();
		}
	}

	void TADlgPassFailPage::OnKillFocusRange(UINT nID)
	{
		int index(nID - IDC_EDIT_FAILHIGH);

		Log_Assert(m_RangeWidgets.size() > index && 0 <= index && m_RangeWidgets[index]);
		if (m_RangeWidgets.size() > index && 0 <= index && m_RangeWidgets[index])
		{
			m_RangeWidgets[index]->EditboxToValue();
		}
	}
	#pragma endregion Protected Methods

	#pragma region Private Methods
	bool TADlgPassFailPage::UpdateRangeValues()
	{
		try
		{
			return (S_OK == SetInspectionData());
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Msg.setMessage( rSvE.getMessage() );
			return false;
		}
	}
	#pragma endregion Private Methods
} //namespace SvOg

