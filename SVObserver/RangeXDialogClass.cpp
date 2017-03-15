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
#include "afxdialogex.h"
#include "SVRange.h"
#include "svresult.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
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

RangeXDialogClass::RangeXDialogClass(SVRangeClass* range, CWnd* parent /*=nullptr*/)
: CDialog(RangeXDialogClass::IDD, parent)
, m_RangeClassHelper(range)
, m_LastSelected(RangeEnum::ER_COUNT)
{
}

RangeXDialogClass::~RangeXDialogClass()
{
}

void RangeXDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FAILHIGH, m_EditFailHigh);
	DDX_Control(pDX, IDC_EDIT_WARNHIGH, m_EditWarnHigh);
	DDX_Control(pDX, IDC_EDIT_WARNLOW, m_EditWarnLow);
	DDX_Control(pDX, IDC_EDIT_FAILLOW, m_EditFailLow);
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
	bool bOK = GetDlgData();

	if(bOK)
	{
		SvOi::MessageTextEnum messageId;
		SVStringVector messageList;
		HRESULT hres = m_RangeClassHelper.CheckInternalData(messageId, messageList);
		if( S_OK != hres)
		{
			bOK = false;
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, messageId, messageList, SvStl::SourceFileParams(StdMessageParams), hres); 
		}
	}

	if(bOK)
	{
		m_RangeClassHelper.SetInspectionData();
		CDialog::OnOK();
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

	m_RangeClassHelper.SetRangeTaskObject();
	m_RangeClassHelper.GetAllInspectionData();

	SetDlgData();

	SVString Title = m_RangeClassHelper.GetOwnerName();
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

	return TRUE;
}

void RangeXDialogClass::SetDlgData()
{
	m_EditFailHigh.SetWindowText(m_RangeClassHelper.GetFailHighString().c_str());
	m_EditWarnHigh.SetWindowText(m_RangeClassHelper.GetWarnHighString().c_str());
	m_EditFailLow.SetWindowText(m_RangeClassHelper.GetFailLowString().c_str());
	m_EditWarnLow.SetWindowText(m_RangeClassHelper.GetWarnLowString().c_str());
}

bool RangeXDialogClass::GetDlgData()
{
	bool Result = false;

	try
	{
		CString Text;
		m_EditFailHigh.GetWindowText(Text);
		m_RangeClassHelper.SetInternalData(SvOi::Tid_FailHigh, Text);
		m_EditWarnHigh.GetWindowText(Text);
		m_RangeClassHelper.SetInternalData(SvOi::Tid_WarnHigh, Text);
		m_EditFailLow.GetWindowText(Text);
		m_RangeClassHelper.SetInternalData(SvOi::Tid_FailLow, Text);
		m_EditWarnLow.GetWindowText(Text);
		m_RangeClassHelper.SetInternalData(SvOi::Tid_WarnLow, Text);
		Result = true;
	}
	catch ( const SvStl::MessageContainer& rSvE )
	{
		//Now that we have caught the exception we would like to display it
		SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
		Exception.setMessage( rSvE.getMessage() );
	}

	return Result;
}

bool RangeXDialogClass::ShowObjectSelector( SVString& rName)
{
	bool retValue = m_RangeClassHelper.FillObjectSelector();

	if ( !retValue )
	{
		return retValue;
	}

	if( 0 < rName.size() )
	{
		SVStringSet nameSet;
		nameSet.insert( rName );
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems(nameSet);
	}

	SVString Title = m_RangeClassHelper.GetOwnerName();
	Title += _T(": ");
	Title += RangeEnum::ERange2String(m_LastSelected).c_str();

	SVString mainTabTitle = SvUl_SF::LoadSVString( IDS_SELECT_TOOLSET_OUTPUT );
	SVString FilterTab = SvUl_SF::LoadSVString( IDS_FILTER );

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), mainTabTitle.c_str(), FilterTab.c_str(), this );

	if( IDOK == Result )
	{
		rName = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getLocation();
		return true;
	}

	return false;
}

// @TODO:  The next 4 methods are very similar.  Consider refactoring to call a common method.  Otherwise, clean up the camelcase and result checking in all the methods.
void RangeXDialogClass::OnBnClickedFailHighIndirect()
{
	m_LastSelected = RangeEnum::ER_FailHigh;
	CString Text;
	m_EditFailHigh.GetWindowText(Text);
	SVString Name( Text );
	if (ShowObjectSelector(Name))
	{
		m_EditFailHigh.SetWindowText(Name.c_str());
	}
}

void RangeXDialogClass::OnBnClickedWarnlHighIndirect()
{
	m_LastSelected = RangeEnum::ER_WarnHigh;
	CString Text;
	m_EditWarnHigh.GetWindowText(Text);
	SVString Name( Text );
	if (ShowObjectSelector(Name))
	{
		m_EditWarnHigh.SetWindowText(Name.c_str());
	}
}

void RangeXDialogClass::OnBnClickedWarnLowIndirect()
{
	m_LastSelected = RangeEnum::ER_WarnLow;
	CString Text;
	m_EditWarnLow.GetWindowText(Text);
	SVString Name( Text );
	if (ShowObjectSelector(Name))
	{
		m_EditWarnLow.SetWindowText(Name.c_str());
	}
}

void RangeXDialogClass::OnBnClickedFailedLowIndirect()
{
	m_LastSelected = RangeEnum::ER_FailLow;
	CString Text;
	m_EditFailLow.GetWindowText(Text);
	SVString Name( Text );
	if (ShowObjectSelector(Name))
	{
		m_EditFailLow.SetWindowText(Name.c_str());
	}
}

