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
#include "CameraLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error
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
		SVStringArray messageList;
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

	CString title = m_RangeClassHelper.GetOwnerName();
	SetWindowText( title );

	// Ensure the title is readable
	CRect rect;
	CSize size;
	GetWindowRect(&rect);
	CDC* pDC = GetDC();
	if(nullptr != pDC )
	{
		size = pDC->GetTextExtent(title);
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
	bool res = false;
	CString csText;

	try
	{
		m_EditFailHigh.GetWindowText(csText);
		m_RangeClassHelper.SetInternalData(SvOi::Tid_FailHigh, csText);
		m_EditWarnHigh.GetWindowText(csText);
		m_RangeClassHelper.SetInternalData(SvOi::Tid_WarnHigh, csText);
		m_EditFailLow.GetWindowText(csText);
		m_RangeClassHelper.SetInternalData(SvOi::Tid_FailLow, csText);
		m_EditWarnLow.GetWindowText(csText);
		m_RangeClassHelper.SetInternalData(SvOi::Tid_WarnLow, csText);
		res = true;
	}
	catch ( const SvStl::MessageContainer& rSvE )
	{
		//Now that we have caught the exception we would like to display it
		SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
		Exception.setMessage( rSvE.getMessage() );
	}

	return res;
}

bool RangeXDialogClass::ShowObjectSelector(CString& name)
{
	bool retValue = m_RangeClassHelper.FillObjectSelector();

	if ( !retValue )
	{
		return retValue;
	}

	if(name.GetLength() > 0)
	{
		SVStringSet nameSet;
		nameSet.insert(SVString(name));
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems(nameSet);
	}

	HINSTANCE resHandle = AfxGetResourceHandle();
	CString Title = m_RangeClassHelper.GetOwnerName();
	Title += _T(": ");
	Title += RangeEnum::ERange2String(resHandle, m_LastSelected).c_str();

	CString mainTabTitle;
	mainTabTitle.LoadString( IDS_SELECT_TOOLSET_OUTPUT );
	CString FilterTab;
	FilterTab.LoadString( IDS_FILTER );

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title, mainTabTitle, FilterTab, this );

	if( IDOK == Result )
	{
		name = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getLocation().c_str();
		return true;
	}

	return false;
}

// @TODO:  The next 4 methods are very similar.  Consider refactoring to call a common method.  Otherwise, clean up the camelcase and result checking in all the methods.
void RangeXDialogClass::OnBnClickedFailHighIndirect()
{
	m_LastSelected = RangeEnum::ER_FailHigh;
	CString csText;
	m_EditFailHigh.GetWindowText(csText); // @TODO:  Should check to see if GetWindowText worked before using the value it returned.
	if (ShowObjectSelector(csText))
	{
		m_EditFailHigh.SetWindowText(csText);
	}
}

void RangeXDialogClass::OnBnClickedWarnlHighIndirect()
{
	m_LastSelected = RangeEnum::ER_WarnHigh;
	CString csText;
	m_EditWarnHigh.GetWindowText(csText);
	if (ShowObjectSelector(csText) )
	{
		m_EditWarnHigh.SetWindowText(csText);
	}
}

void RangeXDialogClass::OnBnClickedWarnLowIndirect()
{
	m_LastSelected = RangeEnum::ER_WarnLow;
	CString csText;
	m_EditWarnLow.GetWindowText(csText);
	if (ShowObjectSelector(csText) )
	{
		m_EditWarnLow.SetWindowText(csText);
	}
}

void RangeXDialogClass::OnBnClickedFailedLowIndirect()
{
	m_LastSelected = RangeEnum::ER_FailLow;
	CString csText;
	m_EditFailLow.GetWindowText(csText);
	if (ShowObjectSelector(csText) )
	{
		m_EditFailLow.SetWindowText(csText);
	}
}

