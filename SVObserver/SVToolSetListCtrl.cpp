//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetListCtrl
//* .File Name       : $Workfile:   SVToolSetListCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   19 Dec 2014 04:26:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolSetListCtrl.h"
#include "SVToolGrouping.h"
#include "ToolSetView.h"
#include "ObjectInterfaces\ISVOApp_Helper.h"
#include "TextDefinesSvO.h"
#include "InspectionCommands/CommandFunctionHelper.h"
#include "InspectionCommands/GetPPQObjectName.h"
#include "InspectionCommands\GetTaskObjects.h"
#include "SVCommandLibrary\SVObjectSynchronousCommandTemplate.h"
#include "SVOResource\ConstGlobalSvOr.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "NavigatorElement.h"
#include "ObjectInterfaces/ObjectInfo.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVToolSetListCtrl, CListCtrl)
#pragma endregion Declarations

#pragma region Constructor
SVToolSetListCtrl::SVToolSetListCtrl()
	: CListCtrl()
	, m_iNone(-1)
	, m_iInvalid(-1)
	, m_fullParameterinML(-1)
	, m_iTopIndex(0)
	, m_expandState(-1)
	, m_collapseState(-1)
	, m_ToolSetId(GUID_NULL)
	, m_InspectionId(GUID_NULL)
{
}

SVToolSetListCtrl::~SVToolSetListCtrl()
{
}
#pragma endregion Constructor

void SVToolSetListCtrl::SetSingleSelect()
{
	LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
	style |= LVS_SINGLESEL;
	SetWindowLong(m_hWnd, GWL_STYLE, style);
}

void SVToolSetListCtrl::setObjectIds(const SVGUID& toolsetId, const SVGUID& inspectionId)
{
	m_ToolSetId = toolsetId;
	m_InspectionId = inspectionId;
	int cnt = 0;
	// Get the Header Control
	CHeaderCtrl* pHeaderCtrl = reinterpret_cast<CHeaderCtrl*>(GetDlgItem(0));
	if (pHeaderCtrl)
	{
		// turn off button style
		pHeaderCtrl->ModifyStyle(HDS_BUTTONS, 0, 0);
		cnt = pHeaderCtrl->GetItemCount();
	}

	if (cnt)
	{
		DeleteColumn(0);
	}
	InsertColumn(0, nullptr, LVCFMT_LEFT, -1, -1);

	// Set the column Width
	CRect rect;
	GetClientRect(&rect);
	SetColumnWidth(0, rect.Width());

	Rebuild();
}

ToolSetView* SVToolSetListCtrl::GetView()
{
	return dynamic_cast<ToolSetView*>(GetParent());
}

const ToolSetView* SVToolSetListCtrl::GetView() const
{
	return dynamic_cast<ToolSetView*>(GetParent());
}

int SVToolSetListCtrl::InsertSubTools(int itemNo, int indent, const GUID& rGuidToolId)
{
	SvPb::InspectionCmdMsgs Request, Response;
	SvPb::TaskObjectListRequest*  pTaskObjectListRequest = Request.mutable_taskobjectlistrequest();
	SvPb::SetGuidInProtoBytes(pTaskObjectListRequest->mutable_taskobjectid(), rGuidToolId);
	SvCmd::InspectionCommandsSynchronous(m_InspectionId, &Request, &Response);
	SvOi::ObjectInfoVector  ObjectInfos;
	SvCmd::ResponseToObjectInfo(Response, ObjectInfos);

	for (auto &ObjectInfo : ObjectInfos)
	{
		if (ObjectInfo.ObjectType == SvDef::SVToolObjectType)
		{
			auto pNavElement = std::make_shared<NavigatorElement>(ObjectInfo.DisplayName.c_str());
			pNavElement->m_Collapsed = false;
			pNavElement->m_OwnerGuid = rGuidToolId;
			pNavElement->m_Guid = ObjectInfo.guid;
			pNavElement->m_Valid = ObjectInfo.isValid;
			pNavElement->m_Type = ObjectInfo.ObjectSubType == SvDef::LoopToolObjectType ?
				NavElementType::SubLoopTool : NavElementType::SubTool;
			itemNo = InsertElement(itemNo, indent, pNavElement);
			//if loopTool would be allowed in a LoopTool this has to be insert here.
		}
	}
	itemNo = InsertDelimiter(itemNo, indent, NavElementType::EndDelimiterLoopTool, rGuidToolId);
	return itemNo;
}
void SVToolSetListCtrl::Rebuild()
{
	ToolSetView* pView = GetView();

	if (pView)
	{
		const SVToolGrouping& groupings = pView->GetToolGroupings();
		if (nullptr == m_ImageList.GetSafeHandle())
		{
			CreateImageLists();
		}
		DeleteAllItems();
		m_NavigatorElementVector.clear();

		SvPb::InspectionCmdMsgs Request, Response;
		SvPb::TaskObjectListRequest*  pTaskObjectListRequest = Request.mutable_taskobjectlistrequest();
		SvPb::SetGuidInProtoBytes(pTaskObjectListRequest->mutable_taskobjectid(), m_ToolSetId);
		SvCmd::InspectionCommandsSynchronous(m_InspectionId, &Request, &Response);
		SvOi::ObjectInfoVector  ToolSetInfos;
		SvCmd::ResponseToObjectInfo(Response, ToolSetInfos);


		int itemNo {0};
		bool bGroupingCollapsed {false};
		int indent {0};

		for (SVToolGrouping::const_iterator GroupingIt = groupings.begin(); GroupingIt != groupings.end(); ++GroupingIt)
		{
			auto pNavElement = std::make_shared<NavigatorElement>(GroupingIt->first.c_str());
			switch (GroupingIt->second.m_type)
			{
				case ToolGroupData::StartOfGroup:
				{
					pNavElement->m_Type = NavElementType::StartGrouping;
					pNavElement->m_Comment = GroupingIt->second.m_comment;
					pNavElement->m_Collapsed = GroupingIt->second.m_bCollapsed;
					itemNo = InsertElement(itemNo, 0, pNavElement);
					bGroupingCollapsed = GroupingIt->second.m_bCollapsed;
					indent = 1;
					break;
				}
				case ToolGroupData::EndOfGroup:
				{
					pNavElement->m_Type = NavElementType::EndGrouping;
					pNavElement->m_Comment = GroupingIt->second.m_comment;
					pNavElement->m_Collapsed = false;
					if (bGroupingCollapsed == false)
					{
						itemNo = InsertElement(itemNo, indent, pNavElement);
					}
					bGroupingCollapsed = false;
					indent = 0;
					break;
				}

				case  ToolGroupData::Tool:
				{
					if (bGroupingCollapsed == true)
						continue;
					auto ToolSetIt = std::find_if(ToolSetInfos.begin(), ToolSetInfos.end(),
						[&](SvOi::ObjectInfo& objInfo)
					{
						return objInfo.DisplayName == pNavElement->m_DisplayName;
					});
					if (ToolSetIt == ToolSetInfos.end() || ToolSetIt->ObjectType != SvDef::SVToolObjectType)
					{
						//object kein tool oder nicht im toolset
						continue;
					}

					if (ToolSetIt->ObjectSubType == SvDef::LoopToolObjectType)
					{
						pNavElement->m_Type = NavElementType::LoopTool;
					}
					else
					{
						pNavElement->m_Type = NavElementType::Tool;
					}
					pNavElement->m_Collapsed = GroupingIt->second.m_bCollapsed;
					pNavElement->m_Valid = ToolSetIt->isValid;
					pNavElement->m_Guid = ToolSetIt->guid;
					itemNo = InsertElement(itemNo, indent, pNavElement);
					if (pNavElement->m_Type == NavElementType::LoopTool&& pNavElement->m_Collapsed == false)
					{
						indent++;
						itemNo = InsertSubTools(itemNo, indent, pNavElement->m_Guid);
						indent--;
					}
				}
			}
		}
		if (!GetItemCount())
		{
			itemNo = InsertDelimiter(itemNo, 0, NavElementType::Empty, m_ToolSetId);
		}
		else
		{
			itemNo = InsertDelimiter(itemNo, 0, NavElementType::EndDelimiterToolSet, m_ToolSetId);
		}
	}
}

int SVToolSetListCtrl::InsertElement(int itemNo, int Indend, PtrNavigatorElement& rpNaviElement)
{
	int img = m_iNone;
	switch (rpNaviElement->m_Type)
	{
		case NavElementType::StartGrouping:
			img = (rpNaviElement->m_Collapsed) ? m_expandState : m_collapseState;
			break;
		case NavElementType::EndGrouping:
			if (rpNaviElement->m_DisplayName.empty())
			{
				return itemNo;////don t show empty end group 
			}
			break;
		case NavElementType::LoopTool:
			if (rpNaviElement->m_Valid)
			{
				img = (rpNaviElement->m_Collapsed) ? m_expandStateLoopToolValid : m_collapseStateLoopToolValid;
			}
			else
			{
				img = (rpNaviElement->m_Collapsed) ? m_expandStateLoopToolInvalid : m_collapseStateLoopToolInvalid;
			}

			break;
		case NavElementType::Tool:
		case NavElementType::SubTool:
			img = (rpNaviElement->m_Valid) ? m_iNone : m_iInvalid;
			break;
	}
	m_NavigatorElementVector.push_back(rpNaviElement);
	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM;
	item.iItem = itemNo;
	item.iSubItem = 0;
	item.iImage = img;
	item.iIndent = Indend;
	item.lParam = LPARAM(m_NavigatorElementVector.size() - 1);//Index ;
	int index = InsertItem(&item);
	SetItemText(index, 0, rpNaviElement->m_DisplayName.c_str());

	return ++index;
}


int  SVToolSetListCtrl::InsertDelimiter(int itemNo, int Indend, NavElementType type, const GUID& rOwnerGuid)
{

	PtrNavigatorElement pNavElement;
	std::string Delimiter(SvO::EndListDelimiter);
	std::string LoopToolDelimiter(SvO::LoopToolDelimiter);
	std::string Empty = SvUl::LoadStdString(IDS_EMPTY_STRING);


	switch (type)
	{
		case NavElementType::Empty:
			pNavElement = std::make_shared<NavigatorElement>(Empty.c_str());
			pNavElement->m_Type = NavElementType::Empty;
			Indend = 0;
			break;
		case NavElementType::EndDelimiterLoopTool:
			pNavElement = std::make_shared<NavigatorElement>(LoopToolDelimiter.c_str());
			pNavElement->m_Type = NavElementType::EndDelimiterLoopTool;
			pNavElement->m_OwnerGuid = rOwnerGuid;
			break;
		case NavElementType::EndDelimiterToolSet:
			pNavElement = std::make_shared<NavigatorElement>(Delimiter.c_str());
			pNavElement->m_Type = NavElementType::EndDelimiterToolSet;
			Indend = 0;
			break;
		default:
			return itemNo;
			break;
	}
	if (!pNavElement)
		return itemNo;

	m_NavigatorElementVector.push_back(pNavElement);


	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM;
	item.iItem = itemNo;
	item.iSubItem = 0;
	item.iIndent = Indend;
	item.lParam = LPARAM(m_NavigatorElementVector.size() - 1);//Index 
	item.iImage = m_iNone;
	int index = InsertItem(&item);
	SetItemText(index, 0, pNavElement->m_DisplayName.c_str());

	return ++index;
}

bool SVToolSetListCtrl::displayErrorBox(const SVGUID& rGuid) const
{

	SvPb::InspectionCmdMsgs Request, Response;
	SvPb::GetMessageListRequest* pGetMessageListRequest = Request.mutable_getmessagelistrequest();
	SvPb::SetGuidInProtoBytes(pGetMessageListRequest->mutable_objectid(), rGuid);
	HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionId, &Request, &Response);
	SvStl::MessageContainerVector messageList;
	if (hr == S_OK && Response.has_getmessagelistresponse())
	{
		messageList = SvCmd::setMessageContainerFromMessagePB(Response.getmessagelistresponse().messages());
	}

	if (0 < messageList.size())
	{
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(messageList[0].getMessage());
		return true;
	}
	return false;
}

bool SVToolSetListCtrl::isToolValid(const SVGUID& tool) const
{
	bool isToolValid = false;
	SvPb::InspectionCmdMsgs Request, Response;
	SvPb::IsValidRequest* pIsValidRequest = Request.mutable_isvalidrequest();
	SvPb::SetGuidInProtoBytes(pIsValidRequest->mutable_objectid(), tool);

	HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionId, &Request, &Response);
	if (S_OK == hr && Response.has_isvalidresponse())
	{
		isToolValid = Response.isvalidresponse().isvalid();
	}
	return isToolValid;
}

void SVToolSetListCtrl::RebuildImages()
{
	//	This function is not efficient because we have a lot 
	// function call to isToolValid
	if (!m_ImageList.GetSafeHandle())
	{
		CreateImageLists();
	}

	int ItemCount = GetItemCount();

	SvCmd::GetPPQObjectName cmd(m_InspectionId);
	std::string ppqName;
	if (S_OK == cmd.Execute())
	{
		ppqName = cmd.GetName();
	}

	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_IMAGE;
	for (int i = 0; i < ItemCount; i++)
	{
		int img = m_iNone;
		item.iItem = i;
		auto NavElement = GetNavigatorElement(i);
		if (!NavElement)
		{
			break;
		}
		bool isLoopTool(false);
		switch (NavElement->m_Type)
		{
			case NavElementType::LoopTool:
			case NavElementType::SubLoopTool:
				isLoopTool = true;
				//fall thru;
			case NavElementType::SubTool:
			case NavElementType::Tool:
			{
				GUID guid(NavElement->m_Guid);
				if (GUID_NULL != guid)
				{
					NavElement->m_Valid = isToolValid(guid);
					if (isLoopTool)
					{
						if (NavElement->m_Collapsed)
						{
							img = NavElement->m_Valid ? m_expandStateLoopToolValid : m_expandStateLoopToolInvalid;
						}
						else
						{
							img = NavElement->m_Valid ? m_collapseStateLoopToolValid : m_collapseStateLoopToolInvalid;
						}
					}
					else
					{
						if (NavElement->m_Valid)
						{
							auto* pView = GetView();
							if (0 < ppqName.size() && nullptr != pView && pView->areParametersInMonitorList(ppqName.c_str(), guid))
							{
								img = m_fullParameterinML;
							}
						}
						else
						{
							img = m_iInvalid;
						}
					}


					item.iImage = img;
					SetItem(&item);
				}

				break;
			}
			default:
				continue;
		}
	}
}
// OnKeyDown added to handle down arrow and up arrow, tool selection and scroll bars. 
void SVToolSetListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool l_bUpdate = false;
	int l_iNext = 0;
	int l_iSelected = GetNextItem(-1, LVNI_SELECTED);

	if (VK_DOWN == nChar)
	{
		int iCount = GetItemCount();
		l_iNext = l_iSelected + 1;
		if (l_iNext < iCount)
		{
			l_bUpdate = true;
		}
	}
	else if (VK_UP == nChar)
	{
		l_iNext = l_iSelected - 1;
		if (l_iNext >= 0)
		{
			l_bUpdate = true;
		}
	}
	else
	{
		// Get Selected Item and determine if it's a Group Node
		POSITION pos = GetFirstSelectedItemPosition();
		if (pos)
		{
			int item = GetNextSelectedItem(pos);
			if (item >= 0)
			{
				if (VK_ADD == nChar || VK_RIGHT == nChar)
				{
					ExpandItem(item);
					l_bUpdate = true;
					l_iNext = l_iSelected;
				}
				else if (VK_SUBTRACT == nChar || VK_LEFT == nChar)
				{
					// Collapse
					CollapseItem(item);
					l_bUpdate = true;
					l_iNext = l_iSelected;
				}
			}
		}
	}

	if (l_bUpdate)
	{
		ToolSetView* l_pParent = GetView();
		if (l_pParent)
		{
			SetItemState(l_iSelected, 0, LVIS_SELECTED); // un-select
			SetItemState(l_iNext, LVIS_SELECTED, LVIS_SELECTED); // select
			EnsureVisible(l_iNext, false);

			if (SvOi::isOkToEdit())
			{
				l_pParent->PostMessage(WM_COMMAND, ID_RUN_ONCE);
			}
		}
	}
}

// Used to save scroll position before rebuilding list.
void SVToolSetListCtrl::SaveScrollPos()
{
	m_iTopIndex = GetTopIndex();
}

// Used in conjunction with SaveScrollPos to restore 
// scroll bars after a rebuild.
void SVToolSetListCtrl::RestoreScrollPos()
{
	if (m_iTopIndex > 0)
	{
		EnsureVisible(GetItemCount() - 1, false);
		EnsureVisible(m_iTopIndex, false);
	}
}

PtrNavigatorElement SVToolSetListCtrl::GetNavigatorElement(int index) const
{
	LVITEM item;
	memset(&item, '\0', sizeof(LVITEM));
	item.mask = LVIF_PARAM;
	item.iItem = index;

	if (0 <= index &&  index < GetItemCount() && GetItem(&item))
	{
		int NavIndex = static_cast<int>(item.lParam); //index of the m_NavigatorElementVector
		if (0 <= NavIndex && m_NavigatorElementVector.size() > NavIndex)
		{
			return m_NavigatorElementVector[NavIndex];
		}
	}
	return PtrNavigatorElement(nullptr);
}

PtrNavigatorElement SVToolSetListCtrl::GetSelectedNavigatorElement(int* pSelectedElement) const
{
	int index = GetNextItem(-1, LVNI_SELECTED);
	if (pSelectedElement)
	{
		*pSelectedElement = index;
	}
	return GetNavigatorElement(index);
}

SVGUID SVToolSetListCtrl::GetSelectedTool() const
{
	auto element = GetSelectedNavigatorElement();
	if (!element)
	{
		return GUID_NULL;
	}
	else
	{
		return element->m_Guid;
	}
}

bool SVToolSetListCtrl::AllowedToEdit() const
{
	auto element = GetSelectedNavigatorElement();
	if (!element)
	{
		return false;
	}

	switch (element->m_Type)
	{
		case NavElementType::StartGrouping:
		case NavElementType::EndGrouping:
		case NavElementType::SubTool:
		case NavElementType::Tool:
		case NavElementType::LoopTool:
			return true;
			break;
		default:
			return false;
	}
	return false;
}

void SVToolSetListCtrl::EnsureOneIsSelected()
{
	int ItemCount = GetItemCount();
	if (ItemCount > 0 && GetSelectedCount() == 0)
	{
		SetItemState(ItemCount - 1, LVIS_SELECTED, LVIS_SELECTED);
		Invalidate();
	}
}
void SVToolSetListCtrl::SetSelectedTool(const SVGUID& rGuid)
{
	for (int i = 0; i < GetItemCount(); ++i)
	{
		LVITEM item;
		memset(&item, '\0', sizeof(LVITEM));
		item.mask = LVIF_PARAM;
		item.iItem = i;

		if (GetItem(&item))
		{
			int index = static_cast<int>(item.lParam); //index of the m_taskList, else -1
			if (-1 < index && m_NavigatorElementVector.size() > index)
			{
				if (!rGuid.empty() && rGuid == m_NavigatorElementVector[index]->m_Guid)
				{
					SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
				}
				else
				{
					SetItemState(i, 0, LVIS_SELECTED);
				}
			}
		}
	}
	Invalidate(); // cause WM_PAINT to refresh list control.
}



void SVToolSetListCtrl::GetSelectedItemScreenPosition(POINT& rPoint) const
{
	int index = GetNextItem(-1, LVNI_SELECTED);
	GetItemPosition(index, &rPoint);
}

BEGIN_MESSAGE_MAP(SVToolSetListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(SVToolSetListCtrl)
	ON_WM_WINDOWPOSCHANGED()
	ON_NOTIFY(HDN_BEGINTRACKA, 0, OnBeginTrack)
	ON_NOTIFY(HDN_BEGINTRACKW, 0, OnBeginTrack)
	//}}AFX_MSG_MAP
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void SVToolSetListCtrl::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CListCtrl::OnWindowPosChanged(lpwndpos);

	if (!(lpwndpos->flags & SWP_NOSIZE))
	{
		CRect rect;
		GetClientRect(&rect);
		SetColumnWidth(0, rect.Width());
	}
}

void SVToolSetListCtrl::OnBeginTrack(NMHDR* pNMHDR, LRESULT* pResult)
{
	switch (pNMHDR->code)
	{
		// This is due to a bug in NT where the code is not translated
		// to ANSI/UNICODE properly (in NT - UNICODE is always sent)
		case HDN_BEGINTRACKW: // fall through...
		case HDN_BEGINTRACKA: // fall through...
		case HDN_DIVIDERDBLCLICKA: // fall through...
		case HDN_DIVIDERDBLCLICKW: // fall through...
		{
			*pResult = true; // disable tracking
		}
		break;

		default:
		{
			*pResult = 0;
		}
		break;
	}
}

BOOL SVToolSetListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam)
		{
			return GetView()->enterSelectedEntry();
		}
		// When an item is being edited make sure the edit control receives certain important key strokes
		// MFC has been designed in such a way that the parent window gets a chance at the messages using the PreTranslateMessage() virtual function. 
		// If the parent window is a CDialog or a CFormView, the ESC and the return key are handled by the parent and does not reach the control. 
		// To allow our control to process some important messages we need to override the PreTranslateMessage() function. 
		// In the code below, we allow the control key combinations to go through to the control. 
		// This allows copy, cut and paste using the control key combinations.
		if (GetEditControl() &&
			(VK_DELETE == pMsg->wParam ||
			VK_F2 == pMsg->wParam ||
			VK_RETURN == pMsg->wParam ||
			VK_ESCAPE == pMsg->wParam ||
			GetKeyState(VK_CONTROL)))
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return true; // DO NOT process further
		}
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}

void SVToolSetListCtrl::CreateImageLists()
{
	m_ImageList.Create(SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 4, 1);

	CWinApp* pApp = AfxGetApp();
	if (pApp)
	{
		m_iNone = m_ImageList.Add(pApp->LoadIcon(IDI_STATUS_NONE));
		m_iInvalid = m_ImageList.Add(pApp->LoadIcon(IDI_STATUS_BLACK));
		m_fullParameterinML = m_ImageList.Add(pApp->LoadIcon(IDI_HMI_ICON));
		m_collapseState = m_ImageList.Add(pApp->LoadIcon(IDI_COLLAPSE));
		m_expandState = m_ImageList.Add(pApp->LoadIcon(IDI_EXPAND));
		m_expandStateLoopToolValid = m_expandState;
		m_collapseStateLoopToolValid = m_collapseState;
		m_expandStateLoopToolInvalid = m_expandState;
		m_collapseStateLoopToolInvalid = m_collapseState;
	}
	SetImageList(&m_ImageList, LVSIL_SMALL);
}

void SVToolSetListCtrl::CollapseItem(int item)
{
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = item;
	lvItem.iSubItem = 0;

	GetItem(&lvItem);
	std::string Name = GetItemText(item, 0);
	// Send to View...
	ToolSetView* pView = GetView();
	if (pView)
	{
		pView->HandleExpandCollapse(Name, true);
	}
}

void SVToolSetListCtrl::ExpandItem(int item)
{
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = item;
	lvItem.iSubItem = 0;

	GetItem(&lvItem);
	std::string Name = GetItemText(item, 0);
	// Send to View...
	ToolSetView* pView = GetView();
	if (pView)
	{
		pView->HandleExpandCollapse(Name, false);
	}
}
