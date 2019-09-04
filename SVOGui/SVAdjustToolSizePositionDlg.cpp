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
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVObjectLibrary/SVClsids.h"
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
SVAdjustToolSizePositionDlg::SVAdjustToolSizePositionDlg(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, LPCTSTR Caption, CWnd* pParentWnd)
	: CDialog(SVAdjustToolSizePositionDlg::IDD, pParentWnd)
	, m_toolTaskId(rTaskObjectID)
	, m_ipId(rInspectionID)
	, m_Title(Caption)
	, m_Values{ SvOg::BoundValues{ rInspectionID, rTaskObjectID } }
{
	SvPb::InspectionCmdMsgs requestMessage, responseMessage;
	auto* pRequest = requestMessage.mutable_getextentparameterrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_toolTaskId);
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_getextentparameterresponse())
	{
		m_extents = responseMessage.getextentparameterresponse().parameters();
		m_translationType = responseMessage.getextentparameterresponse().translationtype();
	}
	m_originalExtents = m_extents;
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
	m_iMode = MODE_MOVE;	// Default action: Move
	CDialog::OnInitDialog();
	m_Values.Init();

	SetWindowText(m_Title.c_str());

	createIcons();
	bool bShow = false;
	bool bShowEditTool = false;

	SvPb::InspectionCmdMsgs requestMessage, responseMessage;
	auto* pTsaParamRequest = requestMessage.mutable_gettoolsizeadjustparameterrequest();
	SvPb::SetGuidInProtoBytes(pTsaParamRequest->mutable_objectid(), m_toolTaskId);
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_gettoolsizeadjustparameterresponse())
	{
		auto responseData = responseMessage.gettoolsizeadjustparameterresponse();
		//m_isFullSizeAllowed = responseData.isfullsizeallowed();
		//m_isAdjustSizeAllowed = responseData.isadjustsizeallowed();
		//m_isAdjustPositionAllowed = responseData.isadjustpositionallowed();
		m_autoSizeEnabled = responseData.enableautosize();
		bShow = responseData.canresizetoparent();
		bShowEditTool = true;
	}

	// If it's a Linear Tool, hide the "Full Image" button if rotation is enabled.
	auto* pParamRequest = requestMessage.mutable_getobjectparametersrequest();
	SvPb::SetGuidInProtoBytes(pParamRequest->mutable_objectid(), m_toolTaskId);
	hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_getobjectparametersresponse())
	{
		if (SvPb::SVLinearToolObjectType == responseMessage.getobjectparametersresponse().typeinfo().subtype())
		{
			bool rotation = m_Values.Get<bool>(SVLinearToolUseRotationGuid);
			if (rotation)
			{
				bShow = false;
			}
		}
	}

	GetDlgItem(IDC_BUT_EDIT_TOOL)->ShowWindow(bShowEditTool ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_FULL_ROI_BTN)->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_FULL_ROI_BTN)->EnableWindow(!IsFullSize() && IsFullSizeAllowed());

	GetDlgItem(IDC_BUTTON_PROPAGATE)->EnableWindow(UsePropagate());

	auto iterFindRot = std::find_if(m_extents.begin(), m_extents.end(), [](const auto& rItem) { return SvPb::SVExtentPropertyRotationAngle == rItem.type(); });
	if (m_extents.end() == iterFindRot || iterFindRot->filteredoutflag() )
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
	m_pButton = reinterpret_cast <SvMc::SVUpDownButton*> (lParam);

	ButtonAction(m_pButton);

	SetTimer(TIMER_PAUSE, 600, nullptr); //@TODO:  Document why 600.
	return 0;
}

LRESULT SVAdjustToolSizePositionDlg::OnNotifyLButtonUp(WPARAM wParam, LPARAM lParam)
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

void SVAdjustToolSizePositionDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = reinterpret_cast<LPNMPROPTREE>(pNotifyStruct);
	*plResult = S_OK;
	if (pNMPropTree->pItem)
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;

		// do validation
		SvPb::SVExtentPropertyEnum eProperty = static_cast<SvPb::SVExtentPropertyEnum> (pItem->GetCtrlID() - ID_BASE);
		std::string Value;
		pItem->GetItemValue(Value);
		double dValue = atof(Value.c_str());
		double dOldValue = SvCmd::getValueForProperties<double>(m_extents, eProperty);
		if (!ApproxEqual(dOldValue, dValue))
		{
			SvPb::InspectionCmdMsgs requestMessage, responseMessage;
			auto* pRequest = requestMessage.mutable_setextentparameterrequest();
			SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_toolTaskId);
			pRequest->mutable_setproperty()->set_propertyflag(eProperty);
			pRequest->mutable_setproperty()->set_value(dValue);
			HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
			
			pItem->SetItemValue(SvUl::AsString(dValue).c_str());
			pItem->OnRefresh();

			if (S_OK == hr && responseMessage.has_setextentparameterresponse())
			{
				m_extents = responseMessage.setextentparameterresponse().parameters();
				m_translationType = responseMessage.getextentparameterresponse().translationtype();
			}
			FillTreeFromExtents();
		}
	}
}

void SVAdjustToolSizePositionDlg::OnOK()
{
	//check if extents have change.  if they change set SV_STATE_MODIFIED
	bool hasChanged = false;
	if (m_originalExtents.size() == m_extents.size())
	{
		for (int i = 0; i < m_extents.size(); i++)
		{
			if (m_extents[i].value() != m_originalExtents[i].value() || m_extents[i].type() != m_originalExtents[i].type())
			{
				hasChanged = true;
				break;
			}
		}
	}
	else
	{
		hasChanged = true;
	}
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
	SvPb::InspectionCmdMsgs requestMessage, responseMessage;
	auto* pRequest = requestMessage.mutable_setextentparameterrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_toolTaskId);
	pRequest->mutable_extentlist()->mutable_extentlist()->MergeFrom(m_originalExtents);
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	assert(S_OK == hr);

	OnBnClickedPropagate();
	CDialog::OnCancel();
}

void SVAdjustToolSizePositionDlg::OnBnClickedFullROI()
{
	SvPb::InspectionCmdMsgs requestMessage, responseMessage;
	auto* pRequest = requestMessage.mutable_setextentparameterrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_toolTaskId);
	pRequest->set_settoparent(true);
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_setextentparameterresponse())
	{
		m_extents = responseMessage.setextentparameterresponse().parameters();
		m_translationType = responseMessage.getextentparameterresponse().translationtype();

		FillTreeFromExtents();
		m_Tree.Invalidate();
	}
}
void SVAdjustToolSizePositionDlg::OnBnClickedPropagate()
{
	SvPb::InspectionCmdMsgs Request, Response;
	SvPb::PropagateSizeAndPositionRequest* pPropagateRequest = Request.mutable_propagatesizeandpositionrequest();
	SvPb::SetGuidInProtoBytes(pPropagateRequest->mutable_objectid(), m_ipId);
	SvCmd::InspectionCommands(m_ipId, Request, &Response);
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


	bool bAllowedAction = false;
	SvPb::InspectionCmdMsgs Request, Response;
	SvPb::IsAllowedLocationRequest* pAllowedLocationRequest = Request.mutable_isallowedlocationrequest();
	SvPb::SetGuidInProtoBytes(pAllowedLocationRequest->mutable_objectid(), m_toolTaskId);
	pAllowedLocationRequest->set_location(eAction);
	pAllowedLocationRequest->set_direction(Direction);
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, Request, &Response);
	if (S_OK == hr && Response.has_isallowedlocationresponse())
	{
		bAllowedAction = Response.isallowedlocationresponse().isallowed();
	}
	if (bAllowedAction && eAction == SvPb::SVExtentLocationPropertyRotate)
	{
		hr = AdjustToolAngle(dx);
	}
	else if (bAllowedAction)
	{
		hr = AdjustTool(eAction, dx, dy);
	}

	FillTreeFromExtents();
	m_Tree.Invalidate();
	return hr;
}

HRESULT SVAdjustToolSizePositionDlg::AdjustTool(SvPb::SVExtentLocationPropertyEnum eAction, int dx, int dy)
{
	SvPb::InspectionCmdMsgs requestMessage, responseMessage;
	auto* pRequest = requestMessage.mutable_setextentparameterrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_toolTaskId);
	auto* pUpdateParam = pRequest->mutable_updatefromoutputspace();
	pUpdateParam->set_action(eAction);
	pUpdateParam->set_dx(dx);
	pUpdateParam->set_dy(dy);
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_setextentparameterresponse())
	{
		m_extents = responseMessage.setextentparameterresponse().parameters();
		m_translationType = responseMessage.getextentparameterresponse().translationtype();
	}

	return hr;
}

HRESULT SVAdjustToolSizePositionDlg::AdjustToolAngle(double dDAngle)
{
	double dCurrentAngle = SvCmd::getValueForProperties<double>(m_extents, SvPb::SVExtentPropertyRotationAngle);

	if (SvPb::SVExtentTranslationPolarUnwrap == m_translationType)
	{
		dCurrentAngle += dDAngle;
	}
	else
	{
		dCurrentAngle -= dDAngle;
	}

	SvPb::InspectionCmdMsgs requestMessage, responseMessage;
	auto* pRequest = requestMessage.mutable_setextentparameterrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_toolTaskId);
	pRequest->mutable_setproperty()->set_propertyflag(SvPb::SVExtentPropertyRotationAngle);
	pRequest->mutable_setproperty()->set_value(dCurrentAngle);
	HRESULT result = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);

	if (S_OK == result && responseMessage.has_setextentparameterresponse())
	{
		m_extents = responseMessage.setextentparameterresponse().parameters();
		m_translationType = responseMessage.getextentparameterresponse().translationtype();
	}

	return result;
}

void SVAdjustToolSizePositionDlg::BuildTreeFromExtents()
{
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	assert(pRoot);
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
	assert(pRoot);
	
	for (auto item : m_extents)
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
		assert(pEdit);

		if (nullptr != pEdit)
		{
			double dValue = item.value();

			pEdit->SetCtrlID(ID_BASE + (int)item.type());
			pEdit->SetLabelText(item.name().c_str());
			pEdit->SetBold(false);
			pEdit->SetHeight(16);

			std::string Value;
			if ((item.type() & SvPb::SVExtentPropertyEnum::SVExtentPropertyNoDecimalPlaces) != 0)
			{
				Value = SvUl::Format(_T("%d"), static_cast<int> (dValue));
			}
			else if ((item.type() & SvPb::SVExtentPropertyEnum::SVExtentProperty2DecimalPlaces) != 0)
			{
				Value = SvUl::Format(_T("%.2f"), dValue);
			}
			else
			{
				Value = SvUl::AsString(dValue);
			}

			bool bReadonly(false);
			if (m_autoSizeEnabled != SvPb::EnableNone)
			{
				bReadonly = item.issetbyreset();
			}

			if (bReadonly)
			{
				pEdit->SetForeColor(::GetSysColor(COLOR_INACTIVECAPTION));
				pEdit->ReadOnly(true);
			}
			pEdit->SetItemValue(Value.c_str());
			pEdit->OnRefresh();
		}
	}
}

bool SVAdjustToolSizePositionDlg::UsePropagate()
{
	SvPb::InspectionCmdMsgs Request, Response;
	SvPb::UsePropagateSizeAndPositionRequest* pPropagateRequest = Request.mutable_usepropagatesizeandpositionrequest();
	SvPb::SetGuidInProtoBytes(pPropagateRequest->mutable_objectid(), m_ipId);
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, Request, &Response);
	if (S_OK == hr && Response.has_usepropagatesizeandpositionresponse())
	{
		return Response.usepropagatesizeandpositionresponse().isused();
	}
	return false;
}

bool SVAdjustToolSizePositionDlg::IsFullSizeAllowed()
{
	bool bAllowFullsize(true);
	if (m_autoSizeEnabled == SvPb::EnableNone)
	{
		bAllowFullsize = false;
	}

	std::tr1::array<SvPb::SVExtentPropertyEnum, 4> PropArray = {SvPb::SVExtentPropertyWidth, SvPb::SVExtentPropertyHeight, SvPb::SVExtentPropertyPositionPointX, SvPb::SVExtentPropertyPositionPointY};
	std::for_each(PropArray.begin(), PropArray.end(), [&](SvPb::SVExtentPropertyEnum p)
	{
		if (bAllowFullsize)
		{
			auto iter = find_if(m_extents.begin(), m_extents.end(), [p](auto item) { return p == item.type(); });
			if (m_extents.end() != iter && iter->issetbyreset())
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
	bool bFull{false};
	long width = SvCmd::getValueForProperties<long>(m_extents, SvPb::SVExtentPropertyOutputWidth);
	long height = SvCmd::getValueForProperties<long>(m_extents, SvPb::SVExtentPropertyOutputHeight);
	long top = SvCmd::getValueForProperties<long>(m_extents, SvPb::SVExtentPropertyPositionPointY);
	long left = SvCmd::getValueForProperties<long>(m_extents, SvPb::SVExtentPropertyPositionPointX);

	SvPb::InspectionCmdMsgs requestMessage, responseMessage;
	auto* pRequest = requestMessage.mutable_getextentparameterrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_toolTaskId);
	pRequest->set_shouldfromparent(true);
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_getextentparameterresponse())
	{
		auto parentExtent = responseMessage.getextentparameterresponse().parameters();
		long parentWidth = SvCmd::getValueForProperties<long>(parentExtent, SvPb::SVExtentPropertyOutputWidth);
		long parentHeight = SvCmd::getValueForProperties<long>(parentExtent, SvPb::SVExtentPropertyOutputHeight);
		long parentTop = SvCmd::getValueForProperties<long>(parentExtent, SvPb::SVExtentPropertyPositionPointY);
		long parentLeft = SvCmd::getValueForProperties<long>(parentExtent, SvPb::SVExtentPropertyPositionPointX);
		if (parentWidth == width
			&& parentHeight == height
			&& parentTop == top
			&& parentLeft == left)
		{
			bFull = true;
		}
	}
	return bFull;
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
} //namespace SvOg