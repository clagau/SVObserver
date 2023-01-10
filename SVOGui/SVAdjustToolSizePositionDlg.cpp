//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAdjustToolSizePositionDlg
//* .File Name       : $Workfile:   SVAdjustToolSizePositionDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   17 Sep 2014 08:07:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <limits>
//Moved to precompiled header: #include <array>
//Moved to precompiled header: #include <functional>
#include "SVAdjustToolSizePositionDlg.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma endregion Declarations
namespace SvOg
{
BEGIN_MESSAGE_MAP(SVAdjustToolSizePositionDlg, CDialog)
	//{{AFX_MSG_MAP(SVAdjustToolSizePositionDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_TOP_LEFT_RADIO, OnModeRadio)
	ON_MESSAGE(SvMc::WM_SV_NOTIFY_LBUTTONDOWN, OnNotifyLButtonDown)
	ON_MESSAGE(SvMc::WM_SV_NOTIFY_LBUTTONUP, OnNotifyLButtonUp)
	ON_BN_CLICKED(IDC_MOVETOOL_RADIO, OnModeRadio)
	ON_BN_CLICKED(IDC_ROTATION_RADIO, OnModeRadio)
	ON_BN_CLICKED(IDC_BOTTOM_RIGHT_RADIO, OnModeRadio)
	ON_BN_CLICKED(IDC_FULL_ROI_BTN, OnBnClickedFullROI)
	ON_BN_CLICKED(IDC_BUTTON_PROPAGATE, OnBnClickedPropagate)
	ON_BN_CLICKED(IDC_BUT_EDIT_TOOL, OnBnClickedEditTool)
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_RESULT_LIST, OnItemChanged)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_BN_CLICKED(IDC_CHECK_AUTOFIT, OnCheckAutofit)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

inline bool ApproxEqual(float f1, float f2)
{
	return (fabs(f1 - f2) < FLT_EPSILON);
}

inline bool ApproxEqual(double d1, double d2)
{
	return (fabs(d1 - d2) < DBL_EPSILON);
}

#pragma region Constructor
SVAdjustToolSizePositionDlg::SVAdjustToolSizePositionDlg(uint32_t inspectionId, uint32_t taskObjectId, LPCTSTR Caption, CWnd* pParentWnd)
	: CDialog(SVAdjustToolSizePositionDlg::IDD, pParentWnd)
	, m_ipId(inspectionId)
	, m_Title(Caption)
	, m_ToolSizeHelper(inspectionId, taskObjectId)
{

	m_ToolSizeHelper.StoreExtents(true);
}

SVAdjustToolSizePositionDlg::~SVAdjustToolSizePositionDlg()
{
	::DestroyIcon(m_icoArrowDown);
	::DestroyIcon(m_icoArrowUp);
	::DestroyIcon(m_icoArrowLeft);
	::DestroyIcon(m_icoArrowRight);
	::DestroyIcon(m_icoArrowClockwise);
	::DestroyIcon(m_icoArrowCounterclockwise);
}
#pragma endregion Constructor

#pragma region Protected Methods
void SVAdjustToolSizePositionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVAdjustToolSizePositionDlg)
	DDX_Control(pDX, IDC_UP_BUTTON, m_btnUp);
	DDX_Control(pDX, IDC_RIGHT_BUTTON, m_btnRight);
	DDX_Control(pDX, IDC_LEFT_BUTTON, m_btnLeft);
	DDX_Control(pDX, IDC_DOWN_BUTTON, m_btnDown);
	DDX_Radio(pDX, IDC_TOP_LEFT_RADIO, m_iMode);
	DDX_Control(pDX, IDC_CHECK_AUTOFIT, m_CheckAutoFit);
	//}}AFX_DATA_MAP
}

BOOL SVAdjustToolSizePositionDlg::OnInitDialog()
{
	m_iMode = MODE_MOVE;	// Default action: Move
	CDialog::OnInitDialog();
	SetWindowText(m_Title.c_str());
	createIcons();
	m_ToolSizeHelper.InitValues();
	bool bShowEditTool = m_ToolSizeHelper.hasAdjustToolSizeParameter();
	bool bShowFullSize = m_ToolSizeHelper.CanResizeToParent();
	bool bAutoSizeEnabled = bShowFullSize && m_ToolSizeHelper.GetAutofitAllowedFlag();

	GetDlgItem(IDC_BUT_EDIT_TOOL)->ShowWindow(bShowEditTool ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_FULL_ROI_BTN)->ShowWindow(bShowFullSize ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_FULL_ROI_BTN)->EnableWindow(!IsFullSize() && IsFullSizeAllowed());

	GetDlgItem(IDC_BUTTON_PROPAGATE)->EnableWindow(UsePropagate());

	if (m_ToolSizeHelper.HasRotation(false) == false)
	{
		GetDlgItem(IDC_ROTATION_RADIO)->ShowWindow(SW_HIDE);	// hide rotation option
	}

	OnModeRadio();

	// Init the control's size to cover the entire client area
	CRect rc;

	// Init the control's size to cover the entire client area
	GetDlgItem(IDC_RESULT_LIST)->GetWindowRect(rc);
	ScreenToClient(rc);
	GetDlgItem(IDC_RESULT_LIST)->DestroyWindow();
	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | PTS_NOTIFY;
	// Create SVRPropTree control
	m_Tree.Create(dwStyle, rc, this, IDC_RESULT_LIST);
	//first column should be 60% of the size and second 40%  
	m_Tree.SetColumn(static_cast<long>(rc.Width() * 0.6));
	m_Tree.ShowInfoText(false);
	BuildTreeFromExtents();

	m_CheckAutoFit.EnableWindow(bAutoSizeEnabled);
	m_CheckAutoFit.SetCheck(IsAutofit() ? BST_CHECKED : BST_UNCHECKED);
	
	GetDlgItem(IDC_FULL_ROI_BTN)->EnableWindow(!IsFullSize() && IsFullSizeAllowed());
	GetDlgItem(IDOK)->EnableWindow(m_hResultFromSetExtent == S_OK);

	return TRUE;  // return TRUE unless you set the focus to a control
}
bool SVAdjustToolSizePositionDlg::IsAutofit()
{
	SvOgu::SizeModes Modes;
	if (!m_ToolSizeHelper.GetToolSizeMode(false, Modes))
	{
		return false;
	}

	auto pred = [&Modes](auto ad)
	{
		return (Modes[ad] == SvDef::TSAutoFit);
	};

	return std::all_of(SvDef::AllToolSizeAdjustEnum, SvDef::AllToolSizeAdjustEnum + SvDef::TSValuesCount, pred);
}
LRESULT SVAdjustToolSizePositionDlg::OnNotifyLButtonDown(WPARAM, LPARAM lParam)
{
	m_pButton = reinterpret_cast <SvMc::SVUpDownButton*> (lParam);

	ButtonAction(m_pButton);

	SetTimer(TIMER_PAUSE, 600, nullptr); //@TODO:  Document why 600.
	return 0;
}

LRESULT SVAdjustToolSizePositionDlg::OnNotifyLButtonUp(WPARAM, LPARAM)
{
	KillTimer(TIMER_PAUSE);
	KillTimer(TIMER_REPEAT);
	m_pButton = nullptr;
	return 0;
}

void SVAdjustToolSizePositionDlg::OnTimer(UINT_CUSTOM nIDEvent)
{
	switch (nIDEvent)
	{
		case TIMER_PAUSE:
			SetTimer(TIMER_REPEAT, 100, nullptr); //@TODO:  Document why 100.
			break;

		case TIMER_REPEAT:
			ButtonAction(m_pButton);
			break;

		default:
			CDialog::OnTimer(nIDEvent);
	}
}

void SVAdjustToolSizePositionDlg::OnModeRadio()
{
	UpdateData();
	if (MODE_ROTATE == m_iMode)
	{
		m_btnDown.SetIcon(nullptr);
		m_btnUp.SetIcon(nullptr);
		m_btnLeft.SetIcon(m_icoArrowClockwise);
		m_btnRight.SetIcon(m_icoArrowCounterclockwise);
	}
	else
	{
		m_btnDown.SetIcon(m_icoArrowDown);
		m_btnUp.SetIcon(m_icoArrowUp);
		m_btnLeft.SetIcon(m_icoArrowLeft);
		m_btnRight.SetIcon(m_icoArrowRight);
	}

	UpdateData(FALSE);
}


std::map< SvPb::SVExtentPropertyEnum, double> SVAdjustToolSizePositionDlg::GetExtentsFromTree()
{
	std::map< SvPb::SVExtentPropertyEnum, double> mymap;
	
	if (nullptr == m_pRoot)
	{
		return mymap;
	}

	SVRPropertyItem* pNext = m_pRoot->GetChild();
	while (pNext)
	{

		SvPb::SVExtentPropertyEnum eProperty = static_cast<SvPb::SVExtentPropertyEnum> (pNext->GetCtrlID() - ID_BASE);
		std::string Value;
		pNext->GetItemValue(Value);
		double dValue = atof(Value.c_str());
		mymap[eProperty] = dValue;
		SVRPropertyItem* pNextSibling = pNext->GetSibling();
		if (pNextSibling)
		{
			pNext = pNextSibling;
		}
		else
		{
			pNext = pNext->GetChild();
		}
	}
	return mymap;
}
void SVAdjustToolSizePositionDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = reinterpret_cast<LPNMPROPTREE>(pNotifyStruct);
	*plResult = S_OK;
	if (pNMPropTree->pItem)
	{
		
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		SvPb::SVExtentPropertyEnum eProperty = static_cast<SvPb::SVExtentPropertyEnum> (pItem->GetCtrlID() - ID_BASE);
		std::string Value;
		pItem->GetItemValue(Value);
		if (m_PreviousTreeEntries[eProperty] == Value)
		{
			return;
		}

	}
	
	*plResult = S_OK;
	std::map< SvPb::SVExtentPropertyEnum, double>  Extends = GetExtentsFromTree();
	m_hResultFromSetExtent =  m_ToolSizeHelper.SetExtentsMap(Extends);
	FillTreeFromExtents(false);


}


void SVAdjustToolSizePositionDlg::OnOK()
{
	//check if extents have change.  if they change set SV_STATE_MODIFIED
	bool hasChanged = m_ToolSizeHelper.HasChanged(true);


	if (hasChanged)
	{	//This AddState is not clean separated, because it is not possible this value to set from GUI2.0
		//But the modified topic should be reviewed for GUI2.0 anyway.
		SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
		SvCmd::RunOnceSynchronous(m_ipId);
	}

	OnBnClickedPropagate();
	CDialog::OnOK();
}

void SVAdjustToolSizePositionDlg::OnCancel()
{
	m_ToolSizeHelper.RestoreExtents();

	OnBnClickedPropagate();
	CDialog::OnCancel();
}

void SVAdjustToolSizePositionDlg::OnBnClickedFullROI()
{
	if (m_ToolSizeHelper.SetExtentToParent())
	{
		FillTreeFromExtents(true);
		m_Tree.Invalidate();
	}
}
void SVAdjustToolSizePositionDlg::OnBnClickedPropagate()
{
	m_ToolSizeHelper.Propagate();
}

void SVAdjustToolSizePositionDlg::OnBnClickedEditTool()
{
	CWnd* pFrame = AfxGetMainWnd();
	if (nullptr != pFrame)
	{
		pFrame->PostMessage(WM_COMMAND, ID_EDIT_EDITTOOLTAB1, 0);
	}

	OnOK();
}
#pragma endregion Protected Methods

#pragma region Private Methods
HRESULT SVAdjustToolSizePositionDlg::ButtonAction(SvMc::SVUpDownButton* pButton)
{
	UpdateData();

	static SvPb::SVExtentLocationPropertyEnum aActions[2][4] =	// rows are buttons (4), columns are modes (2)
	{// TOP_LEFT, BOTTOM_RIGHT, MOVE, ROTATE
		{SvPb::SVExtentLocationPropertyTop, SvPb::SVExtentLocationPropertyBottom, SvPb::SVExtentLocationPropertyCenter, SvPb::SVExtentLocationPropertyUnknown },	// btnUp & btnDown
		{SvPb::SVExtentLocationPropertyLeft, SvPb::SVExtentLocationPropertyRight, SvPb::SVExtentLocationPropertyCenter, SvPb::SVExtentLocationPropertyRotate },	// btnLeft & btnRight
	};
	enum { BTN_UP_DOWN, BTN_LEFT_RIGHT };
	int dx = 0;
	int dy = 0;
	SvPb::SVExtentLocationPropertyEnum eAction = SvPb::SVExtentLocationPropertyUnknown;

	SvPb::SVExtentDirectionsEnum Direction = SvPb::SVExtentDirectionBoth;
	if (pButton == &m_btnUp)
	{
		eAction = aActions[BTN_UP_DOWN][m_iMode];
		dy = -1;
		Direction = SvPb::SVExtentDirectionVertical;
	}
	else if (pButton == &m_btnDown)
	{
		eAction = aActions[BTN_UP_DOWN][m_iMode];
		dy = 1;
		Direction = SvPb::SVExtentDirectionVertical;
	}
	else if (pButton == &m_btnLeft)
	{
		eAction = aActions[BTN_LEFT_RIGHT][m_iMode];
		dx = -1;
		Direction = SvPb::SVExtentDirectionHorizontal;
	}
	else if (pButton == &m_btnRight)
	{
		eAction = aActions[BTN_LEFT_RIGHT][m_iMode];
		dx = 1;
		Direction = SvPb::SVExtentDirectionHorizontal;
	}


	bool bAllowedAction = m_ToolSizeHelper.Isallowedlocation(eAction, Direction);
	HRESULT hr {S_OK};
	if (bAllowedAction && eAction == SvPb::SVExtentLocationPropertyRotate)
	{
		hr = AdjustToolAngle(dx);
	}
	else if (bAllowedAction)
	{
		hr = AdjustTool(eAction, dx, dy);
	}

	FillTreeFromExtents(true);
	m_Tree.Invalidate();
	return hr;
}

bool  SVAdjustToolSizePositionDlg::AdjustTool(SvPb::SVExtentLocationPropertyEnum eAction, int dx, int dy)
{

	return m_ToolSizeHelper.AdjustTool(eAction, dx, dy);

}

bool SVAdjustToolSizePositionDlg::AdjustToolAngle(double dDAngle)
{

	return m_ToolSizeHelper.AdjustToolAngle(true, dDAngle);


}

void SVAdjustToolSizePositionDlg::BuildTreeFromExtents()
{
	m_pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	//pRoot = m_Tree.GetRootItem(); This is a differnt Item!
	
	Log_Assert(m_pRoot);
	m_pRoot->SetCanShrink(false);
	m_pRoot->SetInfoText(_T(""));
	m_pRoot->HideItem();
	m_pRoot->SetHeight(2);

	FillTreeFromExtents(m_pRoot, true,true);

	SVRPropertyItem* pChild = m_pRoot->GetChild();
	while (pChild)
	{
		pChild->Expand(TRUE);
		pChild = pChild->GetSibling();
	}
	m_pRoot->Expand(true);	// needed for redrawing
}

void SVAdjustToolSizePositionDlg::FillTreeFromExtents(bool overwrite)
{
	FillTreeFromExtents(m_pRoot, false, overwrite);

	GetDlgItem(IDC_FULL_ROI_BTN)->EnableWindow(!IsFullSize() && IsFullSizeAllowed());
	GetDlgItem(IDOK)->EnableWindow(m_hResultFromSetExtent == S_OK);
}

bool  SVAdjustToolSizePositionDlg::IsReadonly(const ::SvPb::ExtentParameter& item, const SvOgu::SizeModes& Modes ) const
{
	bool bReadonly(false);
	if (m_ToolSizeHelper.GetAutoSizeEnabled() != SvPb::EnableNone)
	{
		bReadonly = item.issetbyreset();
	}
	
	try
	{
		if ((item.type() & SvPb::SVExtentPropertyEnum::SVExtentPropertyHeight) && Modes.at(SvDef::TSHeight) != SvDef::TSNone)
		{
			bReadonly = true;
		}
		else if ((item.type() & SvPb::SVExtentPropertyEnum::SVExtentPropertyWidth) && Modes.at(SvDef::TSWidth) != SvDef::TSNone)
		{
			bReadonly = true;
		}
		else if ((item.type() & SvPb::SVExtentPropertyEnum::SVExtentPropertyPositionPointX) && Modes.at(SvDef::TSPositionX) != SvDef::TSNone)
		{
			bReadonly = true;
		}
		else if ((item.type() & SvPb::SVExtentPropertyEnum::SVExtentPropertyPositionPointY) && Modes.at(SvDef::TSPositionY) != SvDef::TSNone)
		{
			bReadonly = true;
		}
	}
	catch (std::out_of_range&  )
	{
		bReadonly = false;
	}

	return bReadonly;
}

void SVAdjustToolSizePositionDlg::FillTreeFromExtents(SVRPropertyItem* pRoot, bool shouldCreate, bool overwrite)
{
	Log_Assert(pRoot);
	m_ToolSizeHelper.InitValues();
	if (overwrite)
	{
		m_hResultFromSetExtent = m_ToolSizeHelper.CheckExtents();
	}
	SvOgu::SizeModes Modes;
	m_ToolSizeHelper.GetToolSizeMode(false, Modes);

	for (auto item : m_ToolSizeHelper.GetExtents(false))
	{
		if ((0 == (m_iPropertyFilter & item.type())) || item.filteredoutflag())
		{
			continue;
		}

		SVRPropertyItemEdit* pEdit = nullptr;
		if (shouldCreate)
		{
			pEdit = dynamic_cast<SVRPropertyItemEdit*>(m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot));
		}
		else
		{
			pEdit = dynamic_cast<SVRPropertyItemEdit*> (m_Tree.FindItem(ID_BASE + (int)item.type()));
		}
		Log_Assert(pEdit);

		if (nullptr != pEdit)
		{
		

			double ExtentValue = item.value();
			double DisplayedValue = ExtentValue;
			bool isequal {true};
			bool validString {true};
			if (shouldCreate == false && overwrite == false)
			{
				std::string strValue;
				pEdit->GetItemValue(strValue);
				try
				{
					DisplayedValue = std::stod(strValue);
				}
				catch (std::exception )
				{
					validString = false;
				}

				isequal = ApproxEqual(DisplayedValue, ExtentValue);
			}
			std::string LabelText;
			if (!validString ||  isequal == false )
			{
				//char BlackDot[] = {'\x95', ' ', '\0'};
				//LabelText = std::string(BlackDot) + item.name().c_str();
				pEdit->SetBackColor(0xcccccc);//grey
				LabelText = item.name().c_str();

			}
			else
			{
				pEdit->SetBackColor(GetSysColor(COLOR_WINDOW));
				LabelText = item.name().c_str();
			}

			pEdit->SetCtrlID(ID_BASE + (int)item.type());
			pEdit->SetLabelText(LabelText.c_str());
			pEdit->SetBold(false);
			pEdit->SetHeight(16);

			std::string Value {"???"};
			if (validString)
			{
				if ((item.type() & SvPb::SVExtentPropertyEnum::SVExtentPropertyNoDecimalPlaces) != 0)
				{
					Value = std::format(_T("{}"), static_cast<int> (DisplayedValue));

				}
				else if ((item.type() & SvPb::SVExtentPropertyEnum::SVExtentProperty2DecimalPlaces) != 0)
				{
					Value = std::format(_T("{:.2f}"), DisplayedValue);
				}
				else
				{
					Value = SvUl::AsString(DisplayedValue);
				}
			}
			
			if (IsReadonly(item, Modes))
			{
				pEdit->SetForeColor(::GetSysColor(COLOR_INACTIVECAPTION));
				pEdit->ReadOnly(true);
			}
			else
			{
				pEdit->SetForeColor(::GetSysColor(COLOR_WINDOWTEXT));
				pEdit->ReadOnly(false);
			}
			pEdit->SetItemValue(Value.c_str());
			pEdit->OnRefresh();
			m_PreviousTreeEntries[item.type()] = Value;
		}
	}
}

bool SVAdjustToolSizePositionDlg::UsePropagate()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_usepropagatesizeandpositionrequest();
	pRequest->set_objectid(m_ipId);

	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_usepropagatesizeandpositionresponse())
	{
		return responseCmd.usepropagatesizeandpositionresponse().isused();
	}
	return false;
}

bool SVAdjustToolSizePositionDlg::IsFullSizeAllowed()
{


	return m_ToolSizeHelper.IsFullSizeAllowed(true);
}

bool SVAdjustToolSizePositionDlg::IsFullSize()
{
	return m_ToolSizeHelper.IsFullSize(false);
}

void SVAdjustToolSizePositionDlg::createIcons()
{
	HINSTANCE hInstance = ::AfxGetResourceHandle();
	m_icoArrowDown = static_cast<HICON> (::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ARROW_DOWN), IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR));
	m_icoArrowUp = static_cast<HICON> (::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ARROW_UP), IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR));
	m_icoArrowLeft = static_cast<HICON> (::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ARROW_LEFT), IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR));
	m_icoArrowRight = static_cast<HICON> (::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ARROW_RIGHT), IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR));
	m_icoArrowClockwise = static_cast<HICON> (::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ARROW_CLOCKWISE), IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR));
	m_icoArrowCounterclockwise = static_cast<HICON> (::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ARROW_COUNTERCLOCKWISE), IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR));
}
void SVAdjustToolSizePositionDlg::OnCheckAutofit()
{
	UpdateData(FALSE);
	if (m_CheckAutoFit.GetCheck() == BST_CHECKED)
	{
		m_ToolSizeHelper.SetAllToolSizeMode(SvDef::TSAutoFit, true);
		m_ToolSizeHelper.SetExtentToParent();
	}
	else if (m_CheckAutoFit.GetCheck() == BST_UNCHECKED)
	{
		m_ToolSizeHelper.SetAllToolSizeMode(SvDef::TSNone, true);
	}
	FillTreeFromExtents(true);
	m_Tree.Invalidate();

}
#pragma endregion Private Methods
} //namespace SvOg