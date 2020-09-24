//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgPassFailPage
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
	BEGIN_MESSAGE_MAP(SVTADlgPassFailPage, CPropertyPage)
		//{{AFX_MSG_MAP(SVTADlgPassFailPage)
		ON_BN_CLICKED(IDC_BUTTON_FAILHIGH, &SVTADlgPassFailPage::OnBnClickedFailHighIndirect)
		ON_BN_CLICKED(IDC_BUTTON_WARNHIGH, &SVTADlgPassFailPage::OnBnClickedWarnlHighIndirect)
		ON_BN_CLICKED(IDC_BUTTON_WARNLOW, &SVTADlgPassFailPage::OnBnClickedWarnLowIndirect)
		ON_BN_CLICKED(IDC_BUTTON_FAILLOW, &SVTADlgPassFailPage::OnBnClickedFailedLowIndirect)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
	#pragma endregion Declarations

	#pragma region Constructor
	SVTADlgPassFailPage::SVTADlgPassFailPage(uint32_t inspectionID, uint32_t taskID, UINT captionID)
	: CPropertyPage( SVTADlgPassFailPage::IDD, captionID )
	, RangeController(inspectionID, taskID)
	, m_objectSelector (inspectionID, taskID)
	{
		Init();
	}
	#pragma endregion Constructor

	SVTADlgPassFailPage::~SVTADlgPassFailPage()
	{
	}

	#pragma region Public Methods
	bool SVTADlgPassFailPage::QueryAllowExit()
	{
		UpdateData(true);

		return UpdateRangeValues();
	}
	#pragma endregion Public Methods

	HRESULT SVTADlgPassFailPage::SetInspectionData()
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
	void SVTADlgPassFailPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);

		//{{AFX_DATA_MAP(SVTADlgPassFailPage)
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

	void SVTADlgPassFailPage::InitData()
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
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Init_RangeControlFailed, SvStl::SourceFileParams(StdMessageParams));
		}
	}

	BOOL SVTADlgPassFailPage::OnInitDialog() 
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

	BOOL SVTADlgPassFailPage::OnSetActive() 
	{
		InitData();
		return CPropertyPage::OnSetActive();
	}

	BOOL SVTADlgPassFailPage::OnKillActive() 
	{
		UpdateData(true);
		bool bOk = UpdateRangeValues();

		if (bOk)
		{
			return CPropertyPage::OnKillActive();
		}
		return bOk;
	}

	void SVTADlgPassFailPage::OnBnClickedFailHighIndirect()
	{
		UpdateData();
		std::string Value( m_FailHigh );
		if( ShowObjectSelector( Value, RangeEnum::ER_FailHigh) )
		{
			m_FailHigh = Value.c_str();
			UpdateData( false );
		}
	}

	void SVTADlgPassFailPage::OnBnClickedWarnlHighIndirect()
	{
		UpdateData();
		std::string Value( m_WarnHigh );
		if( ShowObjectSelector( Value, RangeEnum::ER_WarnHigh) )
		{
			m_WarnHigh = Value.c_str();
			UpdateData( false );
		}
	}

	void SVTADlgPassFailPage::OnBnClickedWarnLowIndirect()
	{
		UpdateData();
		std::string Value( m_WarnLow );
		if( ShowObjectSelector( Value, RangeEnum::ER_WarnLow) )
		{
			m_WarnLow = Value.c_str();
			UpdateData( false );
		}
	}

	void SVTADlgPassFailPage::OnBnClickedFailedLowIndirect()
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
	bool SVTADlgPassFailPage::UpdateRangeValues()
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

	bool SVTADlgPassFailPage::ShowObjectSelector(std::string& rName, RangeEnum::ERange fieldEnum)
	{
		std::string Title = GetOwnerName();
		Title += _T(": ");
		Title += RangeEnum::ERange2String(fieldEnum);

		return m_objectSelector.Show(rName, Title, this, SvPb::SelectorFilter::excludeSameLineage, SvPb::allNumberValueObjects);
	}

	#pragma endregion Private Methods
} //namespace SvOg

