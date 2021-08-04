//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRangeXDialog D
//* .File Name       : $Workfile:   RangeXDialogClass.cpp  $
//* .Description     : RangeXDialogClass this dialog is used instead of RangeDialogclass when indirect values for the rangeobjects are allowed
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   26 Jan 2015 11:17:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "RangeXDialogClass.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

RangeXDialogClass::RangeXDialogClass(uint32_t inspectionID, uint32_t toolId, uint32_t taskID, uint32_t rangeID, CWnd* parent /*=nullptr*/)
: CDialog(RangeXDialogClass::IDD, parent)
, m_rangeController(inspectionID, taskID, rangeID)
, m_toolId (toolId)
{
	m_rangeController.Init();
}

RangeXDialogClass::~RangeXDialogClass()
{
}

void RangeXDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_FAILHIGH, m_RangeButtons[RangeEnum::ER_FailHigh]);
	DDX_Control(pDX, IDC_BUTTON_WARNHIGH, m_RangeButtons[RangeEnum::ER_WarnHigh]);
	DDX_Control(pDX, IDC_BUTTON_WARNLOW, m_RangeButtons[RangeEnum::ER_WarnLow]);
	DDX_Control(pDX, IDC_BUTTON_FAILLOW, m_RangeButtons[RangeEnum::ER_FailLow]);
	DDX_Control(pDX, IDC_EDIT_FAILHIGH, m_RangeEdits[RangeEnum::ER_FailHigh]);
	DDX_Control(pDX, IDC_EDIT_WARNHIGH, m_RangeEdits[RangeEnum::ER_WarnHigh]);
	DDX_Control(pDX, IDC_EDIT_WARNLOW, m_RangeEdits[RangeEnum::ER_WarnLow]);
	DDX_Control(pDX, IDC_EDIT_FAILLOW, m_RangeEdits[RangeEnum::ER_FailLow]);
}

BEGIN_MESSAGE_MAP(RangeXDialogClass, CDialog)
	ON_BN_CLICKED(IDOK, &RangeXDialogClass::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &RangeXDialogClass::OnBnClickedCancel)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_FAILHIGH, IDC_BUTTON_FAILLOW, OnButtonRange)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_FAILHIGH, IDC_EDIT_FAILLOW, OnKillFocusRange)
END_MESSAGE_MAP()

void RangeXDialogClass::OnBnClickedOk()
{
	try
	{
		if (S_OK == SetInspectionData())
		{
			CDialog::OnOK();
		}
	}
	catch (const SvStl::MessageContainer& rSvE)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(rSvE.getMessage());
	}
}

void RangeXDialogClass::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

BOOL RangeXDialogClass::OnInitDialog()
{
	__super::OnInitDialog();

	for (int i = 0; i < m_RangeWidgets.size(); ++i)
	{
		m_RangeWidgets[i] = m_rangeController.createWidget(static_cast<RangeEnum::ERange>(i), m_RangeEdits[i], m_RangeButtons[i]);
	}

	std::string Title = m_rangeController.GetOwnerName();
	SetWindowText( Title.c_str() );

	// Ensure the title is readable
	CRect rect;
	CSize size;
	GetWindowRect(&rect);
	CDC* pDC = GetDC();
	if(nullptr != pDC )
	{
		size = pDC->GetTextExtent( Title.c_str() );
		ReleaseDC(pDC);
	}

	if (size.cx > rect.Width())
	{
		int borderWidth = GetSystemMetrics(SM_CXDLGFRAME);
		//make the windows with 24 pixel wider then the text.
		int width = size.cx + (borderWidth * 2) + 24;
		SetWindowPos(&CWnd::wndTop, 0, 0, width, rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}

	return true;
}

void RangeXDialogClass::OnButtonRange(UINT nID)
{
	int index(nID - IDC_BUTTON_FAILHIGH);

	assert(m_RangeWidgets.size() > index && 0 <= index && m_RangeWidgets[index]);
	if (m_RangeWidgets.size() > index && 0 <= index && m_RangeWidgets[index])
	{
		m_RangeWidgets[index]->OnButton();
	}
}

void RangeXDialogClass::OnKillFocusRange(UINT nID)
{
	int index(nID - IDC_EDIT_FAILHIGH);

	assert(m_RangeWidgets.size() > index && 0 <= index && m_RangeWidgets[index]);
	if (m_RangeWidgets.size() > index && 0 <= index && m_RangeWidgets[index])
	{
		m_RangeWidgets[index]->EditboxToValue();
	}
}

HRESULT RangeXDialogClass::SetInspectionData()
{
	UpdateData(true); // get data from dialog

	// Validate Entered data for existence and if within bounds		
	m_rangeController.IsFieldValid(RangeEnum::ERange::ER_FailHigh);
	m_rangeController.IsFieldValid(RangeEnum::ERange::ER_FailLow);
	m_rangeController.IsFieldValid(RangeEnum::ERange::ER_WarnHigh);
	m_rangeController.IsFieldValid(RangeEnum::ERange::ER_WarnLow);

	return m_rangeController.Commit();
}
