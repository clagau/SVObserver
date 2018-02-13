//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "svobserver.h"
#include "InspectionEngine/SVTaskObject.h"
#include "SVInspectionProcess.h"
#include "SVGuiExtentUpdater.h"
#include "InspectionEngine/SVTool.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVLinearToolClass.h"
#include "InspectionEngine/SVExtentPropertiesInfoStruct.h"
#include "SVMainFrm.h"
#include "ToolSizeAdjustTask.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "InspectionCommands/CommandFunctionHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma endregion Declarations

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(SVAdjustToolSizePositionDlg, CDialog)

inline bool ApproxEqual(float f1, float f2)
{
	return (fabs(f1 - f2) < FLT_EPSILON);
}

inline bool ApproxEqual(double d1, double d2)
{
	return (fabs(d1 - d2) < DBL_EPSILON);
}

#pragma region Constructor
SVAdjustToolSizePositionDlg::SVAdjustToolSizePositionDlg(LPCTSTR Caption, CWnd* pParentWnd, SVTaskObjectClass* pToolTask)
	: CDialog(SVAdjustToolSizePositionDlg::IDD, pParentWnd)
	, m_pToolTask(pToolTask)
	, m_Title(Caption)
{
	assert(m_pToolTask);

	if (nullptr != m_pToolTask)
	{
		if (m_pToolTask->DoesObjectHaveExtents())
		{
			m_pToolTask->GetImageExtent(m_svExtents);
		}
	}
	m_svOriginalExtents = m_svExtents;
	m_iMode = -1;
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
	//}}AFX_DATA_MAP
}

BOOL SVAdjustToolSizePositionDlg::OnInitDialog()
{
	ASSERT(nullptr != m_pToolTask);
	m_iMode = MODE_MOVE;	// Default action: Move
	CDialog::OnInitDialog();

	SetWindowText(m_Title.c_str());

	createIcons();
	SVToolClass* pTool = dynamic_cast<SVToolClass*> (m_pToolTask);
	bool bShow =  (nullptr != pTool) ? pTool->canResizeToParent() : false;

	bool bShowEditTool =  (nullptr !=	ToolSizeAdjustTask::GetToolSizeAdjustTask(m_pToolTask));
	GetDlgItem(IDC_BUT_EDIT_TOOL)->ShowWindow(bShowEditTool ? SW_SHOW : SW_HIDE);

	// If it's a Linear Tool, hide the "Full Image" button if rotation is enabled.
	SVLinearToolClass* lt = dynamic_cast<SVLinearToolClass*> (m_pToolTask);
	if (nullptr != lt)
	{
		bool rotation = lt->GetRotation();
		if (rotation)
		{
			bShow = false;
		}
	}

	GetDlgItem(IDC_FULL_ROI_BTN)->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_FULL_ROI_BTN)->EnableWindow(!IsFullSize() && IsFullSizeAllowed());

	GetDlgItem(IDC_BUTTON_PROPAGATE)->EnableWindow(UsePropagate());


	SVExtentPropertyListType list;
	m_pToolTask->GetFilteredImageExtentPropertyList(list);

	if (std::find(list.begin(), list.end(), SvDef::SVExtentPropertyRotationAngle) == list.end())	// if item is not in filtered list, skip
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

	return TRUE;  // return TRUE unless you set the focus to a control
}

LRESULT SVAdjustToolSizePositionDlg::OnNotifyLButtonDown(WPARAM wParam, LPARAM lParam)
{
	SvMc::SVUpDownButton* pButton = reinterpret_cast <SvMc::SVUpDownButton*> (lParam);
	m_pButton = pButton;

	ButtonAction(m_pButton);

	SetTimer(TIMER_PAUSE, 600, nullptr); //@TODO:  Document why 600.
	return 0;
}

LRESULT SVAdjustToolSizePositionDlg::OnNotifyLButtonUp(WPARAM wParam, LPARAM lParam)
{
	SvMc::SVUpDownButton* pButton = reinterpret_cast <SvMc::SVUpDownButton*> (lParam);
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

void SVAdjustToolSizePositionDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = reinterpret_cast<LPNMPROPTREE>(pNotifyStruct);
	*plResult = S_OK;
	if (pNMPropTree->pItem)
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;

		// do validation
		SvDef::SVExtentPropertyEnum eProperty = static_cast<SvDef::SVExtentPropertyEnum> (pItem->GetCtrlID() - ID_BASE);
		std::string Value;
		pItem->GetItemValue(Value);
		double dValue = atof(Value.c_str());

		m_pToolTask->GetImageExtent(m_svExtents);

		double dOldValue;
		m_svExtents.GetExtentProperty(eProperty, dOldValue);
		if (!ApproxEqual(dOldValue, dValue))
		{
			m_svExtents.SetExtentProperty(eProperty, dValue);

			m_svExtents.UpdateData();

			m_svExtents.GetExtentProperty(eProperty, dValue);
			pItem->SetItemValue(SvUl::AsString(dValue).c_str());
			pItem->OnRefresh();

			HRESULT hr = SVGuiExtentUpdater::SetImageExtent(m_pToolTask, m_svExtents, ResetMode_Tool);
			m_pToolTask->GetImageExtent(m_svExtents);
			FillTreeFromExtents();
		}
	}
}

void SVAdjustToolSizePositionDlg::OnOK()
{
	//check if extents have change.  if they change set SV_STATE_MODIFIED
	if (m_svOriginalExtents != m_svExtents)
	{
		SVSVIMStateClass::AddState(SV_STATE_MODIFIED);

		if (nullptr != m_pToolTask && nullptr != m_pToolTask->GetInspection())
		{
			const SVGUID& rInspectionId = m_pToolTask->GetInspection()->GetUniqueObjectID();
			SvPB::InspectionRunOnceRequest requestMessage;

			SvPB::SetGuidInProtoBytes(requestMessage.mutable_inspectionid(), rInspectionId);
			SvCmd::InspectionCommandsSynchronous(rInspectionId, &requestMessage, nullptr);
		}
	}
	CDialog::OnOK();
}

void SVAdjustToolSizePositionDlg::OnCancel()
{
	SVGuiExtentUpdater::SetImageExtent(m_pToolTask, m_svOriginalExtents, ResetMode_Tool);
	CDialog::OnCancel();
}

void SVAdjustToolSizePositionDlg::OnBnClickedFullROI()
{
	HRESULT hr = SVGuiExtentUpdater::SetImageExtentToParent(m_pToolTask, ResetMode_Tool);

	if (S_OK == hr)
	{
		m_pToolTask->GetImageExtent(m_svExtents);

		FillTreeFromExtents();
		m_Tree.Invalidate();
	}
}
void SVAdjustToolSizePositionDlg::OnBnClickedPropagate()
{
	HRESULT hr = SVGuiExtentUpdater::ForwardSizeAndPosition(m_pToolTask, ResetMode_ToolList);
}

void SVAdjustToolSizePositionDlg::OnBnClickedEditTool()
{
	SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>(AfxGetMainWnd());
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

	static SvDef::SVExtentLocationPropertyEnum aActions[2][4] =	// rows are buttons (4), columns are modes (2)
	{// TOP_LEFT, BOTTOM_RIGHT, MOVE, ROTATE
		{ SvDef::SVExtentLocationPropertyTop, SvDef::SVExtentLocationPropertyBottom, SvDef::SVExtentLocationPropertyCenter, SvDef::SVExtentLocationPropertyUnknown },	// btnUp & btnDown
		{ SvDef::SVExtentLocationPropertyLeft, SvDef::SVExtentLocationPropertyRight, SvDef::SVExtentLocationPropertyCenter, SvDef::SVExtentLocationPropertyRotate },	// btnLeft & btnRight
	};
	enum { BTN_UP_DOWN, BTN_LEFT_RIGHT };
	int dx = 0;
	int dy = 0;
	SvDef::SVExtentLocationPropertyEnum eAction = SvDef::SVExtentLocationPropertyUnknown;

	SvDef::SVExtentDirectionsEnum Direction = SvDef::SVExtentDirectionBoth;
	if (pButton == &m_btnUp)
	{
		eAction = aActions[BTN_UP_DOWN][m_iMode];
		dy = -1;
		Direction = SvDef::SVExtentDirectionVertical;
	}
	else if (pButton == &m_btnDown)
	{
		eAction = aActions[BTN_UP_DOWN][m_iMode];
		dy = 1;
		Direction = SvDef::SVExtentDirectionVertical;
	}
	else if (pButton == &m_btnLeft)
	{
		eAction = aActions[BTN_LEFT_RIGHT][m_iMode];
		dx = -1;
		Direction = SvDef::SVExtentDirectionHorizontal;
	}
	else if (pButton == &m_btnRight)
	{
		eAction = aActions[BTN_LEFT_RIGHT][m_iMode];
		dx = 1;
		Direction = SvDef::SVExtentDirectionHorizontal;
	}


	bool bAllowedAction(true);
	SVToolClass *pToolClass(nullptr);

	pToolClass = dynamic_cast<SVToolClass*>(m_pToolTask);


	if (nullptr != pToolClass)
	{
		bAllowedAction = pToolClass->IsAllowedLocation(eAction, Direction);
	}


	HRESULT hr = S_OK;
	if (bAllowedAction && eAction == SvDef::SVExtentLocationPropertyRotate)
	{
		hr = AdjustToolAngle(dx);
	}
	else if (bAllowedAction)
	{
		hr = AdjustTool(eAction, dx, dy);
	}

	m_pToolTask->GetImageExtent(m_svExtents);
	FillTreeFromExtents();
	m_Tree.Invalidate();
	return hr;
}

HRESULT SVAdjustToolSizePositionDlg::AdjustTool(SvDef::SVExtentLocationPropertyEnum eAction, int dx, int dy)
{
	HRESULT hr = S_OK;

	SVImageExtentClass l_pExtents;

	hr = m_pToolTask->GetImageExtent(l_pExtents);

	if (S_OK == hr)
	{
		hr = l_pExtents.UpdateFromOutputSpace(eAction, static_cast<long>(dx), static_cast<long>(dy));
	}

	if (S_OK == hr)
	{
		hr = SVGuiExtentUpdater::SetImageExtent(m_pToolTask, l_pExtents, ResetMode_Tool);
	}

	return hr;
}

HRESULT SVAdjustToolSizePositionDlg::AdjustToolAngle(double dDAngle)
{
	HRESULT hr = S_OK;
	SVImageExtentClass l_Extents;
	double dCurrentAngle = 0.0;
	hr = m_pToolTask->GetImageExtent(l_Extents);
	hr = l_Extents.GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dCurrentAngle);
	if (l_Extents.GetTranslation() == SvDef::SVExtentTranslationPolarUnwrap)
	{
		dCurrentAngle += dDAngle;
	}
	else
	{
		dCurrentAngle -= dDAngle;
	}

	if (S_OK == hr)
	{
		hr = l_Extents.SetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dCurrentAngle);
		if (S_OK == hr)
		{
			hr = SVGuiExtentUpdater::SetImageExtent(m_pToolTask, l_Extents, ResetMode_Tool);
			m_svExtents = l_Extents;
		}
	}
	return hr;
}

void SVAdjustToolSizePositionDlg::BuildTreeFromExtents()
{
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	ASSERT(pRoot);
	pRoot->SetCanShrink(false);
	pRoot->SetInfoText(_T(""));
	pRoot->HideItem();
	pRoot->SetHeight(2);  //@TODO:  Document why 2 is used here.

	FillTreeFromExtents(pRoot, true);

	SVRPropertyItem* pChild = pRoot->GetChild();
	while (pChild)
	{
		pChild->Expand(TRUE);
		pChild = pChild->GetSibling();
	}
	pRoot->Expand(true);	// needed for redrawing
}

void SVAdjustToolSizePositionDlg::FillTreeFromExtents()
{
	SVRPropertyItem* pRoot = m_Tree.GetRootItem();
	FillTreeFromExtents(pRoot, false);

	GetDlgItem(IDC_FULL_ROI_BTN)->EnableWindow(!IsFullSize() && IsFullSizeAllowed());
}

void SVAdjustToolSizePositionDlg::FillTreeFromExtents(SVRPropertyItem* pRoot, bool shouldCreate)
{
	ASSERT(pRoot);
	ASSERT(m_pToolTask);
	SVExtentPropertyListType list;
	HRESULT hr = m_pToolTask->GetFilteredImageExtentPropertyList(list);
	SVExtentPropertyMapType map;
	m_svExtents.GetExtentPropertyList(static_cast<SvDef::SVExtentPropertyEnum> (m_iPropertyFilter), map);

	for (SVExtentPropertyMapType::const_iterator iter = map.begin(); iter != map.end(); ++iter)
	{
		if (std::find(list.begin(), list.end(), iter->first) == list.end())	// if item is not in filtered list, skip
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
			pEdit = dynamic_cast<SVRPropertyItemEdit*> (m_Tree.FindItem(ID_BASE + (int)iter->first));
		}
		ASSERT(pEdit);

		if (nullptr != pEdit)
		{
			std::string Name = iter->second;
			double dValue = 0.0;
			m_svExtents.GetExtentProperty(iter->first, dValue);

			pEdit->SetCtrlID(ID_BASE + (int)iter->first);
			pEdit->SetLabelText(Name.c_str());
			pEdit->SetBold(false);
			pEdit->SetHeight(16);

			std::string Value;
			if ((iter->first & SvDef::SVExtentPropertyEnum::SVExtentPropertyNoDecimalPlaces) != 0)
			{
				Value = SvUl::Format(_T("%d"), static_cast<int> (dValue));
			}
			else if ((iter->first & SvDef::SVExtentPropertyEnum::SVExtentProperty2DecimalPlaces) != 0)
			{
				Value = SvUl::Format(_T("%.2f"), dValue);
			}
			else
			{
				Value = SvUl::AsString(dValue);
			}

			bool bReadonly(false);
			SVExtentPropertyInfoStruct info;
			SVToolClass *pTool = dynamic_cast<SVToolClass*> (m_pToolTask);
			if (nullptr != pTool && (S_OK == pTool->GetPropertyInfo(iter->first, info)))
			{

				if (pTool->GetAutoSizeEnabled() != EnableNone)
				{
					bReadonly = info.bSetByReset;
				}
				bReadonly = bReadonly ||    info.bFormula;

			}

			if (bReadonly)
			{
				pEdit->SetForeColor(::GetSysColor(COLOR_INACTIVECAPTION));
				pEdit->ReadOnly(true);
			}
			pEdit->SetItemValue(Value.c_str());
			pEdit->OnRefresh();
		}
	}//end for( iter = map.begin(); iter != map.end(); iter++ )
}

bool SVAdjustToolSizePositionDlg::UsePropagate()
{

	SVInspectionProcess *pInspection(nullptr);
	SVToolClass *pTool = dynamic_cast<SVToolClass*> (m_pToolTask);
	int count(0);
	if (nullptr != pTool)
	{
		pInspection = dynamic_cast<SVInspectionProcess*>(pTool->GetInspection());
	}
	if (nullptr != pInspection)
	{
		pInspection->LoopOverTools((SVInspectionProcess::pToolFunc) ToolSizeAdjustTask::UseSizeAdjust, count);
	}
	return (count > 0);
}

bool SVAdjustToolSizePositionDlg::IsFullSizeAllowed()
{
	SVToolClass *pTool = dynamic_cast<SVToolClass*> (m_pToolTask);
	bool bAllowFullsize(true);
	if (!pTool)
	{
		bAllowFullsize = false;
	}
	if (bAllowFullsize && pTool->GetAutoSizeEnabled() == EnableNone)
	{
		bAllowFullsize = false;
	}


	SVExtentPropertyInfoStruct info;
	std::tr1::array<SvDef::SVExtentPropertyEnum, 4> PropArray = {SvDef::SVExtentPropertyWidth, SvDef::SVExtentPropertyHeight, SvDef::SVExtentPropertyPositionPointX, SvDef::SVExtentPropertyPositionPointY};
	std::for_each(PropArray.begin(), PropArray.end(), [&](SvDef::SVExtentPropertyEnum p)
	{
		if (bAllowFullsize && S_OK == pTool->GetPropertyInfo(p, info))
		{
			if (info.bSetByReset)
			{
				bAllowFullsize = false;
			}
		}

	}
	);


	return bAllowFullsize;
}

bool SVAdjustToolSizePositionDlg::IsFullSize()
{
	bool l_bFull = false;
	long lWidth = 0;
	long lHeight = 0;
	long lTop = 0;
	long lLeft = 0;
	HRESULT hr1 = m_svExtents.GetExtentProperty(SvDef::SVExtentPropertyOutputWidth, lWidth);
	HRESULT hr2 = m_svExtents.GetExtentProperty(SvDef::SVExtentPropertyOutputHeight, lHeight);
	HRESULT hr3 = m_svExtents.GetExtentProperty(SvDef::SVExtentPropertyPositionPointX, lLeft);
	HRESULT hr4 = m_svExtents.GetExtentProperty(SvDef::SVExtentPropertyPositionPointY, lTop);

	m_pToolTask->GetImageExtent(m_svExtents);
	SVImageExtentClass l_ParentExtent;
	SVToolClass* pTool = dynamic_cast<SVToolClass*>(m_pToolTask->GetTool());
	if (pTool && S_OK == pTool->GetParentExtent(l_ParentExtent))
	{
		long l_lParentWidth = 0;
		long l_lParentHeight = 0;
		long l_lParentTop = 0;
		long l_lParentLeft = 0;
		l_ParentExtent.GetExtentProperty(SvDef::SVExtentPropertyOutputWidth, l_lParentWidth);
		l_ParentExtent.GetExtentProperty(SvDef::SVExtentPropertyOutputHeight, l_lParentHeight);
		l_ParentExtent.GetExtentProperty(SvDef::SVExtentPropertyPositionPointX, l_lParentTop);
		l_ParentExtent.GetExtentProperty(SvDef::SVExtentPropertyPositionPointY, l_lParentLeft);
		if (l_lParentWidth == lWidth
			&& l_lParentHeight == lHeight
			&& l_lParentTop == l_lParentTop
			&& l_lParentLeft == l_lParentLeft)
		{
			l_bFull = true;
		}
	}
	return l_bFull;
}

void SVAdjustToolSizePositionDlg::createIcons()
{
	HINSTANCE hInstance = ::AfxGetResourceHandle();
	m_icoArrowDown = ::LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_ARROW_DOWN),
		IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);

	m_icoArrowUp = ::LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_ARROW_UP),
		IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);

	m_icoArrowLeft = ::LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_ARROW_LEFT),
		IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);

	m_icoArrowRight = ::LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_ARROW_RIGHT),
		IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);

	m_icoArrowClockwise = ::LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_ARROW_CLOCKWISE),
		IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);

	m_icoArrowCounterclockwise = ::LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_ARROW_COUNTERCLOCKWISE),
		IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);
}
#pragma endregion Private Methods
