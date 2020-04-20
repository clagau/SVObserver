//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogPassFailPageClass
//* .File Name       : $Workfile:   SVTADlgPassFailPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:44  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <string>
#include "SVTADlgPassFailPage.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogPassFailPageClass, CPropertyPage)
		//{{AFX_MSG_MAP(SVToolAdjustmentDialogPassFailPageClass)
		ON_BN_CLICKED(IDC_BUTTON_FAILHIGH, &SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailHighIndirect)
		ON_BN_CLICKED(IDC_BUTTON_WARNHIGH, &SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnlHighIndirect)
		ON_BN_CLICKED(IDC_BUTTON_WARNLOW, &SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnLowIndirect)
		ON_BN_CLICKED(IDC_BUTTON_FAILLOW, &SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailedLowIndirect)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
	#pragma endregion Declarations

	#pragma region Constructor
	SVToolAdjustmentDialogPassFailPageClass::SVToolAdjustmentDialogPassFailPageClass(uint32_t inspectionID, uint32_t taskID, UINT captionID)
	: CPropertyPage( SVToolAdjustmentDialogPassFailPageClass::IDD, captionID )
	, RangeController(inspectionID, taskID)
	, m_objectSelector (inspectionID, taskID)
	{
		Init();
	}
	#pragma endregion Constructor

	SVToolAdjustmentDialogPassFailPageClass::~SVToolAdjustmentDialogPassFailPageClass()
	{
	}

	#pragma region Public Methods
	bool SVToolAdjustmentDialogPassFailPageClass::QueryAllowExit()
	{
		UpdateData(true);

		return UpdateRangeValues();
	}
	#pragma endregion Public Methods

	HRESULT SVToolAdjustmentDialogPassFailPageClass::SetInspectionData()
	{
		UpdateData(true); // get data from dialog

		// Validate Entered data for existance and if within bounds		
		IsFieldValid(SvStl::Tid_FailHigh, static_cast<LPCSTR>(m_FailHigh));
		IsFieldValid(SvStl::Tid_FailLow, static_cast<LPCSTR>(m_FailLow));
		IsFieldValid(SvStl::Tid_WarnHigh, static_cast<LPCSTR>(m_WarnHigh));
		IsFieldValid(SvStl::Tid_WarnLow, static_cast<LPCSTR>(m_WarnLow));
		
		Set(FailHigh, static_cast<LPCSTR>(m_FailHigh));
		Set(FailLow, static_cast<LPCSTR>(m_FailLow));
		Set(WarnHigh, static_cast<LPCSTR>(m_WarnHigh));
		Set(WarnLow, static_cast<LPCSTR>(m_WarnLow));

		return Commit();
	}

	#pragma region Protected Methods
	void SVToolAdjustmentDialogPassFailPageClass::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);

		//{{AFX_DATA_MAP(SVToolAdjustmentDialogPassFailPageClass)
		DDX_Text(pDX, IDC_EDIT_FAILHIGH, m_FailHigh);
		DDX_Text(pDX, IDC_EDIT_WARNHIGH, m_WarnHigh);
		DDX_Text(pDX, IDC_EDIT_WARNLOW, m_WarnLow);
		DDX_Text(pDX, IDC_EDIT_FAILLOW, m_FailLow);
		DDX_Control(pDX, IDC_BUTTON_FAILHIGH, m_ButtonFailHigh);
		DDX_Control(pDX, IDC_BUTTON_WARNHIGH, m_ButtonWarnHigh);
		DDX_Control(pDX, IDC_BUTTON_WARNLOW, m_ButtonWarnLow);
		DDX_Control(pDX, IDC_BUTTON_FAILLOW, m_ButtonFailLow);
		//}}AFX_DATA_MAP
	}

	void SVToolAdjustmentDialogPassFailPageClass::InitData()
	{
		try
		{
			m_FailHigh = Get(FailHigh).c_str();
			m_FailLow = Get(FailLow).c_str();
			m_WarnHigh = Get(WarnHigh).c_str();
			m_WarnLow  = Get(WarnLow).c_str();
		}
		catch (...)
		{
			assert(false);
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Init_RangeControlFailed, SvStl::SourceFileParams(StdMessageParams));
		}
	}

	BOOL SVToolAdjustmentDialogPassFailPageClass::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		// Put the Down Arrow on the Button
		m_downArrowBitmap.LoadOEMBitmap(OBM_DNARROW);

		//(HBITMAP) is a call to the overloaded function operator HBITMAP and no c style cast
		// And it is NOT needed as the implicit cast will call the casting operator.
		m_ButtonFailHigh.SetBitmap(m_downArrowBitmap);
		m_ButtonWarnHigh.SetBitmap(m_downArrowBitmap);
		m_ButtonWarnLow.SetBitmap(m_downArrowBitmap);
		m_ButtonFailLow.SetBitmap(m_downArrowBitmap);

		InitData();
		UpdateData(false); // set data to dialog

		return true;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	BOOL SVToolAdjustmentDialogPassFailPageClass::OnSetActive() 
	{
		InitData();
		return CPropertyPage::OnSetActive();
	}

	BOOL SVToolAdjustmentDialogPassFailPageClass::OnKillActive() 
	{
		UpdateData(true);
		bool bOk = UpdateRangeValues();

		if (bOk)
		{
			return CPropertyPage::OnKillActive();
		}
		return bOk;
	}

	void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailHighIndirect()
	{
		UpdateData();
		std::string Value( m_FailHigh );
		if( ShowObjectSelector( Value, RangeEnum::ER_FailHigh) )
		{
			m_FailHigh = Value.c_str();
			UpdateData( false );
		}
	}

	void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnlHighIndirect()
	{
		UpdateData();
		std::string Value( m_WarnHigh );
		if( ShowObjectSelector( Value, RangeEnum::ER_WarnHigh) )
		{
			m_WarnHigh = Value.c_str();
			UpdateData( false );
		}
	}

	void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedWarnLowIndirect()
	{
		UpdateData();
		std::string Value( m_WarnLow );
		if( ShowObjectSelector( Value, RangeEnum::ER_WarnLow) )
		{
			m_WarnLow = Value.c_str();
			UpdateData( false );
		}
	}

	void SVToolAdjustmentDialogPassFailPageClass::OnBnClickedFailedLowIndirect()
	{
		UpdateData();
		std::string Value( m_FailLow );
		if( ShowObjectSelector( Value, RangeEnum::ER_FailLow) )
		{
			m_FailLow = Value.c_str();
			UpdateData( false );
		}
	}
	#pragma endregion Protected Methods

	#pragma region Private Methods
	bool SVToolAdjustmentDialogPassFailPageClass::UpdateRangeValues()
	{
		try
		{
			return (S_OK == SetInspectionData());
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
			Msg.setMessage( rSvE.getMessage() );
			return false;
		}
	}

	bool SVToolAdjustmentDialogPassFailPageClass::ShowObjectSelector(std::string& rName, RangeEnum::ERange fieldEnum)
	{
		std::string Title = GetOwnerName();
		Title += _T(": ");
		Title += RangeEnum::ERange2String(fieldEnum);

		return m_objectSelector.Show( rName, Title, this, SvPb::SelectorFilter::rangeValue);
	}

	#pragma endregion Private Methods
} //namespace SvOg

