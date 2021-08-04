//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogTwoImagePage
//* .File Name       : $Workfile:   SVToolAdjustmentDialogTwoImagePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAdjustmentDialogTwoImagePage.h"
#include "SVMatroxLibrary\SVMatroxSimpleEnums.h"
#include "Definitions/ImageOperatorEnums.h"
#include "Operators\SVImageArithmetic.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
constexpr const char* FirstImageTag = _T("First Image");
constexpr const char* SecondImageTag = _T("Second Image");
constexpr const char* ResultImageTag = _T("Result Image");
constexpr const char* NoImageTag = _T("(No Image Available)"); // maybe move this to the control class?
static const int NumberOfImagesRequired = 2;

enum ImageTabsEnum
{
	FirstImageTab = 0,
	SecondImageTab = 1,
	ResultImageTab = 2
};
struct NoNotification
{
	NoNotification(bool& rNotificationFlag):m_rNotificationFlag(rNotificationFlag)
	{
		m_rNotificationFlag = true;
	}
	~NoNotification()
	{
		m_rNotificationFlag = false;
	}


	bool& m_rNotificationFlag;
};



BOOL SVToolAdjustmentDialogTwoImagePageClass::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		(GetFocus() == GetDlgItem(IDC_EDIT_GAIN) || GetFocus() == GetDlgItem(IDC_EDIT_OFFSET))
		)
	{
		// handle return pressed in edit control
		refresh();
		return TRUE; // this doesn't need processing anymore
	}
	return FALSE; // all other cases still need default processing
}

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogTwoImagePageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogTwoImagePageClass)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelChangeCombo2)
	ON_CBN_SELCHANGE(IDC_OPERATOR_COMBO, OnSelchangeOperatorCombo)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_GAIN, OnCheckEnableGain)
	ON_BN_CLICKED(IDC_CHECK_USE_LUT, OnCheckUseLut)
	ON_BN_CLICKED(IDC_BUTTON_LINK_GAIN, OnBnClickedButtonGain)
	ON_EN_KILLFOCUS(IDC_EDIT_GAIN, OnKillFocusGain)
	ON_BN_CLICKED(IDC_BUTTON_LINK_OFFSET, OnBnClickedButtonOffset)
	ON_EN_KILLFOCUS(IDC_EDIT_OFFSET, OnKillFocusOffset)
	ON_WM_CHAR()

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
SVToolAdjustmentDialogTwoImagePageClass::SVToolAdjustmentDialogTwoImagePageClass(uint32_t inspectionId, uint32_t taskObjectId)
	: CPropertyPage(SVToolAdjustmentDialogTwoImagePageClass::IDD)
	, SvOg::ImageController(inspectionId, taskObjectId, SvPb::SVImageMonoType, false)
	, m_InspectionID(inspectionId)
	, m_TaskObjectID(taskObjectId)
	, m_values {SvOg::BoundValues{ inspectionId, taskObjectId }}
{
}

SVToolAdjustmentDialogTwoImagePageClass::~SVToolAdjustmentDialogTwoImagePageClass()
{}
#pragma endregion Constructor

HRESULT SVToolAdjustmentDialogTwoImagePageClass::SetInspectionData()
{
	NoNotification NoNF(m_IgnoreNotification);
	HRESULT hr = S_OK;

	UpdateData(true); // get data from dialog

	long lOperator = static_cast<long>(m_operatorCtrl.GetItemData(m_operatorCtrl.GetCurSel()));
	m_values.Set<long>(SvPb::ArithmeticOperatorEId, lOperator);
	
	bool UseFloatBuffer = SvOp::SVImageArithmetic::useFloatBuffer(lOperator);
	if (!UseFloatBuffer)
	{
		m_IsGainEnabled = false;
		m_UseLut = false;
	}
	
	m_values.Set<bool>(SvPb::ImageToolEnabledGainId, m_IsGainEnabled);
	m_values.Set<bool>(SvPb::ImageToolUseLutId, m_UseLut);

	hr = m_values.Commit(SvOg::PostAction::doRunOnce | SvOg::PostAction::doReset);
	
	ShowGainAndOffset(lOperator);

	if (SvDef::SVImageOperatorDoubleHeight == lOperator || SvDef::SVImageOperatorFlipVertical == lOperator || SvDef::SVImageOperatorFlipHorizontal == lOperator)
	{
		m_secondAvailableSourceImageListBoxCtl.EnableWindow(false);
		//hide second image tab, because in this mode is only on image aloud.
		m_firstImageCtrl.ShowTab(SecondImageTab, false);
		m_secondImageCtrl.ShowTab(SecondImageTab, false);
	}// end if
	else
	{
		m_secondAvailableSourceImageListBoxCtl.EnableWindow(true);
		//show second image tab
		m_firstImageCtrl.ShowTab(SecondImageTab, true);
		m_secondImageCtrl.ShowTab(SecondImageTab, true);
	}// end else

	UpdateData(false);
	
	return hr;
}

void SVToolAdjustmentDialogTwoImagePageClass::refresh()
{
	HRESULT hres = SetInspectionData();
	if (hres != S_OK)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		//@todo error handling
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, "Werte konnten nicht gesetzt werden!", SvStl::SourceFileParams(StdMessageParams), 0);
	}
	

	setImages();
}

#pragma region Protected Methods
void SVToolAdjustmentDialogTwoImagePageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogTwoImagePageClass)
	DDX_Control(pDX, IDC_OPERATOR_COMBO, m_operatorCtrl);
	DDX_Control(pDX, IDC_SECOND_IMAGE, m_secondImageCtrl);
	DDX_Control(pDX, IDC_FIRST_IMAGE, m_firstImageCtrl);
	DDX_Control(pDX, IDC_COMBO1, m_firstAvailableSourceImageListBoxCtl);
	DDX_Control(pDX, IDC_COMBO2, m_secondAvailableSourceImageListBoxCtl);
	DDX_Check(pDX, IDC_CHECK_ENABLE_GAIN, m_IsGainEnabled);
	DDX_Check(pDX, IDC_CHECK_USE_LUT, m_UseLut);
	DDX_Control(pDX, IDC_EDIT_GAIN, m_EditGainValue);
	DDX_Control(pDX, IDC_EDIT_OFFSET, m_EditOffsetValue);
	DDX_Control(pDX, IDC_BUTTON_LINK_GAIN, m_ButtonGainLink);
	DDX_Control(pDX, IDC_BUTTON_LINK_OFFSET, m_ButtonOffsetLink);
	

	//}}AFX_DATA_MAP
}

BOOL SVToolAdjustmentDialogTwoImagePageClass::OnInitDialog()
{
	
	NoNotification NoNF(m_IgnoreNotification); 
	CPropertyPage::OnInitDialog();
	
	
	Init();
	m_values.Init();

	m_GainWidget = std::make_unique<LinkedValueWidgetHelper>(m_EditGainValue, m_ButtonGainLink, m_InspectionID, m_TaskObjectID, SvPb::ImageToolGainId, &m_values, ObjectSelectorData {m_TaskObjectID});
	m_OffsetWidget = std::make_unique<LinkedValueWidgetHelper>(m_EditOffsetValue, m_ButtonOffsetLink, m_InspectionID, m_TaskObjectID, SvPb::ImageToolOffsetId, &m_values, ObjectSelectorData {m_TaskObjectID});
	const SvUl::NameObjectIdList& rAvailableImageList = GetAvailableImageList();
	RetreiveCurrentlySelectedImageNames();
	RetreiveResultImageNames();

	m_firstAvailableSourceImageListBoxCtl.Init(rAvailableImageList, m_firstImageName, NoImageTag);
	m_secondAvailableSourceImageListBoxCtl.Init(rAvailableImageList, m_secondImageName, NoImageTag);

	m_firstImageCtrl.AddTab(FirstImageTag);
	m_firstImageCtrl.AddTab(SecondImageTag);
	m_firstImageCtrl.AddTab(ResultImageTag);
	m_secondImageCtrl.AddTab(FirstImageTag);
	m_secondImageCtrl.AddTab(SecondImageTag);
	m_secondImageCtrl.AddTab(ResultImageTag);
	m_secondImageCtrl.SelectTab(1);
	setImages();

	// Fill Arithmetic Combo...
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("AND")), static_cast<DWORD_PTR>(SVImageAnd));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("OR")), static_cast<DWORD_PTR>(SVImageOr));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("XOR")), static_cast<DWORD_PTR>(SVImageXOr));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("NOR")), static_cast<DWORD_PTR>(SVImageNor));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("XNOR")), static_cast<DWORD_PTR>(SVImageXNor));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("NAND")), static_cast<DWORD_PTR>(SVImageNand));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("ADD")), static_cast<DWORD_PTR>(SVImageAddSaturation));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("SUB")), static_cast<DWORD_PTR>(SVImageSubSaturation));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("SUB ABS")), static_cast<DWORD_PTR>(SVImageSubAbsSaturation));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("MULT")), static_cast<DWORD_PTR>(SVImageMulSaturation));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("DIV")), static_cast<DWORD_PTR>(SVImageDiv));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("MIN")), static_cast<DWORD_PTR>(SVImageMin));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("MAX")), static_cast<DWORD_PTR>(SVImageMax));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("PASS")), static_cast<DWORD_PTR>(SVImagePass));

	// Special Image Operator... ( not defined by MIL ! )
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("AVERAGE")), static_cast<DWORD_PTR>(SvDef::SVImageOperatorAverage));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("DOUBLE HEIGHT")), static_cast<DWORD_PTR>(SvDef::SVImageOperatorDoubleHeight));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("FLIP VERTICAL")), static_cast<DWORD_PTR>(SvDef::SVImageOperatorFlipVertical));
	m_operatorCtrl.SetItemData(m_operatorCtrl.AddString(_T("FLIP HORIZONTAL")), static_cast<DWORD_PTR>(SvDef::SVImageOperatorFlipHorizontal));

	// Get Current Arithmetic Operator...

	long lOperator = m_values.Get<long>(SvPb::ArithmeticOperatorEId);

	for (int i = 0; i < m_operatorCtrl.GetCount(); ++i)
	{
		if (lOperator == static_cast<long>(m_operatorCtrl.GetItemData(i)))
		{
			m_operatorCtrl.SetCurSel(i); // Set Selected Operator...
			break;
		}
	}
	
	m_IsGainEnabled = m_values.Get<BOOL>(SvPb::ImageToolEnabledGainId);;
	m_UseLut = m_values.Get<BOOL>(SvPb::ImageToolUseLutId);
	
	
	ShowGainAndOffset(lOperator);
	UpdateData(false); // set data to dialog
	
	return true;
}

void SVToolAdjustmentDialogTwoImagePageClass::OnSelchangeCombo1()
{
	UpdateData(true); // get data from dialog

	CString Name;
	m_firstAvailableSourceImageListBoxCtl.GetLBText(m_firstAvailableSourceImageListBoxCtl.GetCurSel(), Name);
	if (!Name.IsEmpty() && Name != NoImageTag)
	{
		m_firstImageName = Name;
		ConnectToImage(m_firstInputName, m_firstImageName);
		refresh();
	}
}

void SVToolAdjustmentDialogTwoImagePageClass::OnSelChangeCombo2()
{
	UpdateData(true); // get data from dialog

	CString Name;
	m_secondAvailableSourceImageListBoxCtl.GetLBText(m_secondAvailableSourceImageListBoxCtl.GetCurSel(), Name);
	if (!Name.IsEmpty() && Name != NoImageTag)
	{
		m_secondImageName = Name;
		ConnectToImage(m_secondInputName, m_secondImageName);
		refresh();
	}
}

void SVToolAdjustmentDialogTwoImagePageClass::OnSelchangeOperatorCombo()
{
	refresh();
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVToolAdjustmentDialogTwoImagePageClass::setImages()
{
	RetreiveCurrentlySelectedImageNames();

	IPictureDisp* pFirstImage = GetImage(m_firstImageName);
	IPictureDisp* pSecondImage = GetImage(m_secondImageName);
	m_firstImageCtrl.setImage(pFirstImage, FirstImageTab);
	m_firstImageCtrl.setImage(pSecondImage, SecondImageTab);
	m_secondImageCtrl.setImage(pFirstImage, FirstImageTab);
	m_secondImageCtrl.setImage(pSecondImage, SecondImageTab);

	IPictureDisp* pResultImage = GetImage(m_resultImageID);
	if (pResultImage)
	{
		m_firstImageCtrl.setImage(pResultImage, ResultImageTab);
		m_secondImageCtrl.setImage(pResultImage, ResultImageTab);
		//display result image tab
		m_firstImageCtrl.ShowTab(ResultImageTab, true);
		m_secondImageCtrl.ShowTab(ResultImageTab, true);
	}
	else
	{
		//hide result image tab
		m_firstImageCtrl.ShowTab(ResultImageTab, false);
		m_secondImageCtrl.ShowTab(ResultImageTab, false);
	}

	m_firstImageCtrl.Refresh();
	m_secondImageCtrl.Refresh();
}

void SVToolAdjustmentDialogTwoImagePageClass::RetreiveCurrentlySelectedImageNames()
{
	const SvUl::InputNameObjectIdPairList& rImageList = GetInputImageList(SvDef::InvalidObjectId, NumberOfImagesRequired);

	// This requires that the input name sorts in descending natural order
	// and that the images we are concerned with are first in the list
	if (rImageList.size() && rImageList.size() > 1)
	{
		SvUl::InputNameObjectIdPairList::const_iterator it = rImageList.begin();
		m_firstInputName = it->first;
		m_firstImageName = it->second.first;
		++it;
		m_secondInputName = it->first;
		m_secondImageName = it->second.first;
	}
}

void SVToolAdjustmentDialogTwoImagePageClass::RetreiveResultImageNames()
{
	const SvUl::NameObjectIdList& rImageList = GetResultImages();

	SvUl::NameObjectIdList::const_iterator it = rImageList.begin();
	if (it != rImageList.end())
	{
		m_resultImageName = it->first;
		m_resultImageID = it->second;
	}
}

void SVToolAdjustmentDialogTwoImagePageClass::ShowGainAndOffset(long op)
{
	
	bool UseFloatBuffer = SvOp::SVImageArithmetic::useFloatBuffer(op);
	CWnd* pWnd(nullptr);

	
	if (nullptr != (pWnd = GetDlgItem(IDC_CHECK_ENABLE_GAIN)))
	{
		pWnd->EnableWindow(UseFloatBuffer);
		pWnd->ShowWindow(UseFloatBuffer ? SW_SHOW : SW_HIDE);
		
	}

	int command = m_IsGainEnabled ? SW_SHOW : SW_HIDE;
	std::vector<int> controlIds {IDC_BUTTON_LINK_GAIN, IDC_CHECK_USE_LUT, IDC_STATIC_GAIN,
		IDC_STATIC_OFFSET, IDC_EDIT_OFFSET, IDC_EDIT_GAIN, IDC_BUTTON_LINK_OFFSET,};
	for (auto id : controlIds)
	{
		if (nullptr != (pWnd = GetDlgItem(id)))
		{
			pWnd->ShowWindow(command);
		}
	}
}


void SVToolAdjustmentDialogTwoImagePageClass::OnCheckEnableGain()
{
	refresh();
}

void SVToolAdjustmentDialogTwoImagePageClass::OnCheckUseLut()
{
	refresh();
}
void SVToolAdjustmentDialogTwoImagePageClass::OnBnClickedButtonGain()
{
	m_GainWidget->OnButton();
	refresh();
}
void SVToolAdjustmentDialogTwoImagePageClass::OnBnClickedButtonOffset()
{
	m_OffsetWidget->OnButton();
	refresh();
}

void SVToolAdjustmentDialogTwoImagePageClass::OnKillFocusGain()
{
	if (m_IgnoreNotification)
	{
		return;
	}
	m_GainWidget->EditboxToValue();
	refresh();

}
void SVToolAdjustmentDialogTwoImagePageClass::OnKillFocusOffset()
{
	if (m_IgnoreNotification)
	{
		return;
	}
	m_OffsetWidget->EditboxToValue();
	refresh();
}

void SVToolAdjustmentDialogTwoImagePageClass::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	
	if(nChar == VK_RETURN && 
		(GetFocus() == GetDlgItem(IDC_EDIT_GAIN) || GetFocus() == GetDlgItem(IDC_EDIT_OFFSET)))
	{
		refresh();
	}
	else
		CPropertyPage::OnChar(nChar, nRepCnt, nFlags);
}
	
#pragma endregion Private Methods
} //namespace SvOg

