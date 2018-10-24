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
#include "SVObserver.h"
#include "RangeXDialogClass.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

IMPLEMENT_DYNAMIC(RangeXDialogClass, CDialog)

RangeXDialogClass::RangeXDialogClass(const GUID& rInspectionID, const GUID& rTaskObjectID, const GUID& rRangeID, CWnd* parent /*=nullptr*/)
: CDialog(RangeXDialogClass::IDD, parent)
, m_rangeController(rInspectionID, rTaskObjectID, rRangeID)
, m_objectSelector(rInspectionID, rTaskObjectID)
{
	m_rangeController.Init();
}

RangeXDialogClass::~RangeXDialogClass()
{
}

void RangeXDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FAILHIGH, m_FailHigh);
	DDX_Text(pDX, IDC_EDIT_WARNHIGH, m_WarnHigh);
	DDX_Text(pDX, IDC_EDIT_WARNLOW, m_WarnLow);
	DDX_Text(pDX, IDC_EDIT_FAILLOW, m_FailLow);
	DDX_Control(pDX, IDC_BUTTON_FAILHIGH, m_ButtonFailHigh);
	DDX_Control(pDX, IDC_BUTTON_WARNHIGH, m_ButtonWarnHigh);
	DDX_Control(pDX, IDC_BUTTON_WARNLOW, m_ButtonWarnLow);
	DDX_Control(pDX, IDC_BUTTON_FAILLOW, m_ButtonFailLow);
}

BEGIN_MESSAGE_MAP(RangeXDialogClass, CDialog)
	ON_BN_CLICKED(IDOK, &RangeXDialogClass::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &RangeXDialogClass::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_FAILHIGH, &RangeXDialogClass::OnBnClickedFailHighIndirect)
	ON_BN_CLICKED(IDC_BUTTON_WARNHIGH, &RangeXDialogClass::OnBnClickedWarnlHighIndirect)
	ON_BN_CLICKED(IDC_BUTTON_WARNLOW, &RangeXDialogClass::OnBnClickedWarnLowIndirect)
	ON_BN_CLICKED(IDC_BUTTON_FAILLOW, &RangeXDialogClass::OnBnClickedFailedLowIndirect)
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
		SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
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

	// Put the Down Arrow on the Button
	m_downArrowBitmap.LoadOEMBitmap( OBM_DNARROW );

	//(HBITMAP) is a call to the overloaded function operator HBITMAP and no c style cast
	m_ButtonFailHigh.SetBitmap( ( HBITMAP )m_downArrowBitmap );
	m_ButtonWarnHigh.SetBitmap( ( HBITMAP )m_downArrowBitmap );
	m_ButtonWarnLow.SetBitmap( ( HBITMAP )m_downArrowBitmap );
	m_ButtonFailLow.SetBitmap( ( HBITMAP )m_downArrowBitmap );

	SetDlgData();

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

void RangeXDialogClass::SetDlgData()
{
	try
	{
		m_FailHigh = m_rangeController.Get(m_rangeController.FailHigh).c_str();
		m_FailLow = m_rangeController.Get(m_rangeController.FailLow).c_str();
		m_WarnHigh = m_rangeController.Get(m_rangeController.WarnHigh).c_str();
		m_WarnLow = m_rangeController.Get(m_rangeController.WarnLow).c_str();
	}
	catch (...)
	{
		assert(false);
		SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Init_RangeControlFailed, SvStl::SourceFileParams(StdMessageParams));
	}
	UpdateData(false);
}

bool RangeXDialogClass::ShowObjectSelector( std::string& rName, RangeEnum::ERange fieldEnum)
{
	std::string Title = m_rangeController.GetOwnerName();
	Title += _T(": ");
	Title += RangeEnum::ERange2String(fieldEnum).c_str();

	return m_objectSelector.Show(rName, Title, this, GUID_NULL, SvCmd::RangeSelectorFilterType);
}

// @TODO:  The next 4 methods are very similar.  Consider refactoring to call a common method.  Otherwise, clean up the camelcase and result checking in all the methods.
void RangeXDialogClass::OnBnClickedFailHighIndirect()
{
	std::string Value(m_FailHigh);
	if (ShowObjectSelector(Value, RangeEnum::ER_FailHigh))
	{
		m_FailHigh = Value.c_str();
		UpdateData(false);
	}
}

void RangeXDialogClass::OnBnClickedWarnlHighIndirect()
{
	std::string Value(m_WarnHigh);
	if (ShowObjectSelector(Value, RangeEnum::ER_WarnHigh))
	{
		m_WarnHigh = Value.c_str();
		UpdateData(false);
	}
}

void RangeXDialogClass::OnBnClickedWarnLowIndirect()
{
	std::string Value(m_WarnLow);
	if (ShowObjectSelector(Value, RangeEnum::ER_WarnLow))
	{
		m_WarnLow = Value.c_str();
		UpdateData(false);
	}
}

void RangeXDialogClass::OnBnClickedFailedLowIndirect()
{
	std::string Value(m_FailLow);
	if (ShowObjectSelector(Value, RangeEnum::ER_FailLow))
	{
		m_FailLow = Value.c_str();
		UpdateData(false);
	}
}

HRESULT RangeXDialogClass::SetInspectionData()
{
	UpdateData(true); // get data from dialog

	// Validate Entered data for existance and if within bounds		
	m_rangeController.IsFieldValid(SvStl::Tid_FailHigh, static_cast<LPCSTR>(m_FailHigh));
	m_rangeController.IsFieldValid(SvStl::Tid_FailLow, static_cast<LPCSTR>(m_FailLow));
	m_rangeController.IsFieldValid(SvStl::Tid_WarnHigh, static_cast<LPCSTR>(m_WarnHigh));
	m_rangeController.IsFieldValid(SvStl::Tid_WarnLow, static_cast<LPCSTR>(m_WarnLow));

	m_rangeController.Set(m_rangeController.FailHigh, static_cast<LPCSTR>(m_FailHigh));
	m_rangeController.Set(m_rangeController.FailLow, static_cast<LPCSTR>(m_FailLow));
	m_rangeController.Set(m_rangeController.WarnHigh, static_cast<LPCSTR>(m_WarnHigh));
	m_rangeController.Set(m_rangeController.WarnLow, static_cast<LPCSTR>(m_WarnLow));

	return m_rangeController.Commit();
}
