//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVOResource\ConstGlobalSvOr.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "NavigatorElement.h"
#include "ObjectInterfaces/ObjectInfo.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageTextEnum.h"
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
{
}

SVToolSetListCtrl::~SVToolSetListCtrl()
{
}
#pragma endregion Constructor

// cppcheck-suppress unusedFunction
void SVToolSetListCtrl::SetSingleSelect()
{
	LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
	style |= LVS_SINGLESEL;
	SetWindowLong(m_hWnd, GWL_STYLE, style);
}

void SVToolSetListCtrl::SetMultipleSelect()
{
	LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
	style &= ~LVS_SINGLESEL;
	SetWindowLong(m_hWnd, GWL_STYLE, style);
}

void SVToolSetListCtrl::setObjectIds(uint32_t toolsetId, uint32_t inspectionId, bool correctGrouping)
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

	Rebuild(correctGrouping);
}

ToolSetView* SVToolSetListCtrl::GetView()
{
	return dynamic_cast<ToolSetView*>(GetParent());
}

const ToolSetView* SVToolSetListCtrl::GetView() const
{
	return dynamic_cast<ToolSetView*>(GetParent());
}

int SVToolSetListCtrl::InsertSubTools(int itemNo, int indent, uint32_t toolId)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_taskobjectlistrequest();
	pRequest->set_taskobjectid(toolId);

	SvCmd::InspectionCommands(m_InspectionId, requestCmd, &responseCmd);
	SvOi::ObjectInfoVector  ObjectInfos;
	SvCmd::ResponseToObjectInfo(responseCmd, ObjectInfos);
	for (auto &ObjectInfo : ObjectInfos)
	{
		if (ObjectInfo.ObjectType == SvPb::SVToolObjectType)
		{
			auto pNavElement = std::make_shared<NavigatorElement>(ObjectInfo.DisplayName.c_str());
			pNavElement->m_Collapsed = false;
			pNavElement->m_OwnerId = toolId;
			pNavElement->m_navigatorObjectId = ObjectInfo.m_objectId;
			pNavElement->m_Valid = ObjectInfo.isValid;
			pNavElement->m_Type = ObjectInfo.ObjectSubType == SvPb::LoopToolObjectType ?
				NavElementType::SubLoopTool : NavElementType::SubTool;
			itemNo = InsertElement(itemNo, indent, pNavElement);
			//if loopTool would be allowed in a LoopTool this has to be insert here.
		}
	}
	itemNo = InsertDelimiter(itemNo, indent, NavElementType::EndDelimiterTool, toolId);
	return itemNo;
}

void SVToolSetListCtrl::Rebuild( bool checkGrouping)
{
	ToolSetView* pView = GetView();

	if (pView)
	{
		SVToolGrouping& groupings = pView->GetToolGroupings();
		if (nullptr == m_ImageList.GetSafeHandle())
		{
			CreateImageLists();
		}
		DeleteAllItems();
		m_NavigatorElementVector.clear();

		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		SvPb::TaskObjectListRequest*  pTaskObjectListRequest = requestCmd.mutable_taskobjectlistrequest();
		pTaskObjectListRequest->set_taskobjectid(m_ToolSetId);

		SvCmd::InspectionCommands(m_InspectionId, requestCmd, &responseCmd);
		SvOi::ObjectInfoVector  ToolSetInfos;
		SvCmd::ResponseToObjectInfo(responseCmd, ToolSetInfos);

		int nchanged(0);
		if (checkGrouping && groupings.Correct(ToolSetInfos, nchanged))
		{
			std::vector<std::string>  ssv;
			ssv.push_back(std::to_string(nchanged));
			TRACE1("Correct groupings add : %i  Items",nchanged );
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Correct_Grouping_Add_S_Items, ssv, SvStl::SourceFileParams(StdMessageParams));
		}

		int itemNo {0};
		bool bGroupingCollapsed {false};
		int indent {0};
		

		for (SVToolGrouping::iterator GroupingIt = groupings.begin(); GroupingIt != groupings.end(); ++GroupingIt)
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
						[&](const SvOi::ObjectInfo& rObjInfo)
					{
						return rObjInfo.DisplayName == pNavElement->m_DisplayName;
					});
					if (ToolSetIt == ToolSetInfos.end() || ToolSetIt->ObjectType != SvPb::SVToolObjectType)
					{
						//object kein tool oder nicht im toolset
						
						continue;
					}

					switch (ToolSetIt->ObjectSubType)
					{
					case SvPb::LoopToolObjectType:
						pNavElement->m_Type = NavElementType::LoopTool;
						break;
					case SvPb::GroupToolObjectType:
						pNavElement->m_Type = NavElementType::GroupTool;
						break;					
					default:
						pNavElement->m_Type = NavElementType::Tool;
						pNavElement->m_OwnerId = m_ToolSetId;
						break;
					}
					pNavElement->m_Collapsed = GroupingIt->second.m_bCollapsed;
					pNavElement->m_Valid = ToolSetIt->isValid;
					pNavElement->m_navigatorObjectId = ToolSetIt->m_objectId;
					itemNo = InsertElement(itemNo, indent, pNavElement);
					if ((pNavElement->m_Type == NavElementType::LoopTool || pNavElement->m_Type == NavElementType::GroupTool) && pNavElement->m_Collapsed == false)
					{
						indent++;
						itemNo = InsertSubTools(itemNo, indent, pNavElement->m_navigatorObjectId);
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

		pView->RefreshTimestamp();
	}
}

int SVToolSetListCtrl::InsertElement(int itemNo, int indent, PtrNavigatorElement& rpNaviElement)
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
				return itemNo;////don't show empty end group 
			}
			break;
		case NavElementType::GroupTool:
		case NavElementType::LoopTool:
			if (rpNaviElement->m_Valid)
			{
				img = (rpNaviElement->m_Collapsed) ? m_expandState : m_collapseState;
			}
			else
			{
				img = (rpNaviElement->m_Collapsed) ? m_expandStateInvalid : m_collapseStateInvalid;
			}

			break;
		case NavElementType::Tool:
		case NavElementType::SubTool:
			img = (rpNaviElement->m_Valid) ? m_iNone : m_iInvalid;
			break;
	}

	rpNaviElement->m_indent = indent;
	
	m_NavigatorElementVector.push_back(rpNaviElement);
	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM;
	item.iItem = itemNo;
	item.iSubItem = 0;
	item.iImage = img;
	item.iIndent = indent;
	item.lParam = LPARAM(m_NavigatorElementVector.size() - 1);//Index ;
	int index = InsertItem(&item);
	SetItemText(index, 0, rpNaviElement->m_DisplayName.c_str());

	return ++index;
}


int  SVToolSetListCtrl::InsertDelimiter(int itemNo, int indent, NavElementType type, uint32_t ownerId)
{

	PtrNavigatorElement pDelimiter;
	std::string Delimiter(SvO::EndListDelimiter);
	std::string LoopToolDelimiter(SvO::LoopToolDelimiter);
	std::string Empty = SvUl::LoadStdString(IDS_EMPTY_STRING);


	switch (type)
	{
		case NavElementType::Empty:
			pDelimiter = std::make_shared<NavigatorElement>(Empty.c_str());
			pDelimiter->m_Type = NavElementType::Empty;
			pDelimiter->m_OwnerId = ownerId;
			indent = 0;
			break;
		case NavElementType::EndDelimiterTool:
			pDelimiter = std::make_shared<NavigatorElement>(LoopToolDelimiter.c_str());
			pDelimiter->m_Type = NavElementType::EndDelimiterTool;
			pDelimiter->m_OwnerId = ownerId;
			break;
		case NavElementType::EndDelimiterToolSet:
			pDelimiter = std::make_shared<NavigatorElement>(Delimiter.c_str());
			pDelimiter->m_Type = NavElementType::EndDelimiterToolSet;
			pDelimiter->m_OwnerId = ownerId;
			indent = 0;
			break;
		default:
			return itemNo;
			break;
	}
	if (!pDelimiter)
		return itemNo;

	pDelimiter->m_indent = indent;
	m_NavigatorElementVector.push_back(pDelimiter);


	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM;
	item.iItem = itemNo;
	item.iSubItem = 0;
	item.iIndent = indent;
	item.lParam = LPARAM(m_NavigatorElementVector.size() - 1);//Index 
	item.iImage = m_iNone;
	int index = InsertItem(&item);
	SetItemText(index, 0, pDelimiter->m_DisplayName.c_str());

	return ++index;
}

bool SVToolSetListCtrl::displayErrorBox(uint32_t toolId) const
{

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getmessagelistrequest();
	pRequest->set_objectid(toolId);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionId, requestCmd, &responseCmd);
	SvStl::MessageContainerVector messageList;
	if (hr == S_OK && responseCmd.has_standardresponse())
	{
		messageList = SvPb::convertProtobufToMessageVector(responseCmd.standardresponse().errormessages());
	}

	if (0 < messageList.size())
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(messageList[0].getMessage());
		return true;
	}
	return false;
}

bool SVToolSetListCtrl::isToolValid(uint32_t tool) const
{
	bool isToolValid = false;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
	pRequest->set_objectid(tool);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
	{
		isToolValid = responseCmd.getobjectparametersresponse().isvalid();
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
	std::string ppqName;

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	SvPb::GetPPQNameRequest* pPPQNameRequest = requestCmd.mutable_getppqnamerequest();
	pPPQNameRequest->set_inspectionid(m_InspectionId);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getppqnameresponse())
	{
		ppqName = responseCmd.getppqnameresponse().ppqname();
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
		bool isLoopGroupTool(false);
		switch (NavElement->m_Type)
		{
			case NavElementType::GroupTool:
			case NavElementType::LoopTool:
			case NavElementType::SubLoopTool:
				isLoopGroupTool = true;
				[[fallthrough]];
			case NavElementType::SubTool:
			case NavElementType::Tool:
			{
				uint32_t id(NavElement->m_navigatorObjectId);
				if (SvDef::InvalidObjectId != id)
				{
					NavElement->m_Valid = isToolValid(id);
					if (isLoopGroupTool)
					{
						if (NavElement->m_Collapsed)
						{
							img = NavElement->m_Valid ? m_expandState : m_expandStateInvalid;
						}
						else
						{
							img = NavElement->m_Valid ? m_collapseState : m_collapseStateInvalid;
						}
					}
					else
					{
						if (NavElement->m_Valid)
						{
							auto* pView = GetView();
							if (0 < ppqName.size() && nullptr != pView && pView->areParametersInMonitorList(ppqName.c_str(), id))
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
void SVToolSetListCtrl::OnKeyDown(UINT nChar, UINT, UINT)
{
	bool bUpdate = false;
	int iNext = 0;
	int iSelected = GetNextItem(-1, LVNI_SELECTED);

	if (VK_DOWN == nChar)
	{
		int iCount = GetItemCount();
		iNext = iSelected + 1;
		if (iNext < iCount)
		{
			bUpdate = true;
		}
	}
	else if (VK_UP == nChar)
	{
		iNext = iSelected - 1;
		if (iNext >= 0)
		{
			bUpdate = true;
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
					bUpdate = true;
					iNext = iSelected;
				}
				else if (VK_SUBTRACT == nChar || VK_LEFT == nChar)
				{
					// Collapse
					CollapseItem(item);
					bUpdate = true;
					iNext = iSelected;
				}
			}
		}
	}

	if (bUpdate)
	{
		ToolSetView* pParent = GetView();
		if (pParent)
		{
			SetItemState(iSelected, 0, LVIS_SELECTED); // deselect
			SetItemState(iNext, LVIS_SELECTED, LVIS_SELECTED); // select
			EnsureVisible(iNext, false);

			if (SvOi::isOkToEdit())
			{
				pParent->PostMessage(WM_COMMAND, ID_RUN_ONCE);
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


int SVToolSetListCtrl::GetNavigatorElementIndentation(int index) const
{
	auto pElement = GetNavigatorElement(index);

	if (nullptr == pElement)
	{
		return cUndefinedIndentationValue;
	}

	return pElement->m_indent;
}


std::vector<PtrNavigatorElement> SVToolSetListCtrl::GetSelectedNavigatorElements() const
{
	std::vector<PtrNavigatorElement> selectedElements;

	int index = -1;

	while (true)
	{
		index = GetNextItem(index, LVNI_SELECTED);
		if (index < 0)
		{
			break;
		}
		selectedElements.push_back(GetNavigatorElement(index));
	}

	return selectedElements;
}


std::vector<uint32_t> SVToolSetListCtrl::GetAllSelectedToolIds() const
{
	std::vector<uint32_t> toolIds;
	auto selectedElements = GetSelectedNavigatorElements();

	for (auto pElement : selectedElements)
	{
		if (pElement)
		{
			if(pElement->m_navigatorObjectId)
			{
				toolIds.push_back(pElement->m_navigatorObjectId);
			}
		}
	}

	return toolIds;
}


PtrNavigatorElement SVToolSetListCtrl::Get1stSelNavigatorElement() const
{
	auto elements = GetSelectedNavigatorElements();

	if (elements.empty())
	{
		return nullptr;
	}

	return elements[0];
}


int SVToolSetListCtrl::Get1stSelIndex() const
{
	return GetNextItem(-1, LVNI_SELECTED);
}


uint32_t SVToolSetListCtrl::Get1stSelToolId() const
{
	auto elements = GetSelectedNavigatorElements();

	if (elements.empty())
	{
		return SvDef::InvalidObjectId;
	}

	auto element = elements[0];

	if (nullptr == element)
	{
		return SvDef::InvalidObjectId;
	}
	else
	{
		return element->m_navigatorObjectId;
	}
}


void SVToolSetListCtrl::UndoSubSelectionsAllItems()
{
	int index = -1;
	while (index = GetNextItem(index, LVNI_SELECTED), index > -1)
	{
		UndoSubSelections(index);
	}
}


void SVToolSetListCtrl::UndoSubSelections(int index)
{
	auto indentation = GetNavigatorElementIndentation(index);

	if (cUndefinedIndentationValue == indentation)
		return;

	while (GetNavigatorElementIndentation(++index) > indentation)
	{
		SetItemState(index, 0, LVIS_SELECTED); // deselect
	}
}


bool SVToolSetListCtrl::mayBeEdited() const
{
	auto element = Get1stSelNavigatorElement();

	if (!element)
	{
		return false;
	}

	switch (element->m_Type)
	{
		case NavElementType::StartGrouping:
		case NavElementType::EndGrouping:
		case NavElementType::SubTool:
		case NavElementType::GroupTool:
		case NavElementType::Tool:
		case NavElementType::LoopTool:
			return true;
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

void SVToolSetListCtrl::selectTool(uint32_t toolId)
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
				if (SvDef::InvalidObjectId != toolId && toolId == m_NavigatorElementVector[index]->m_navigatorObjectId)
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
	m_ImageList.Create(SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 6, 1);

	CWinApp* pApp = AfxGetApp();
	if (pApp)
	{
		m_iNone = m_ImageList.Add(pApp->LoadIcon(IDI_STATUS_NONE));
		m_iInvalid = m_ImageList.Add(pApp->LoadIcon(IDI_STATUS_BLACK));
		m_fullParameterinML = m_ImageList.Add(pApp->LoadIcon(IDI_HMI_ICON));
		m_collapseState = m_ImageList.Add(pApp->LoadIcon(IDI_COLLAPSE));
		m_expandState = m_ImageList.Add(pApp->LoadIcon(IDI_EXPAND));
		m_expandStateInvalid = m_ImageList.Add(pApp->LoadIcon(IDI_EXPAND_INVALID));
		m_collapseStateInvalid = m_ImageList.Add(pApp->LoadIcon(IDI_COLLAPSE_INVALID));
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
	std::string Name = GetItemText(item, 0).GetString();
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
	std::string Name = GetItemText(item, 0).GetString();
	// Send to View...
	ToolSetView* pView = GetView();
	if (pView)
	{
		pView->HandleExpandCollapse(Name, false);
	}
}
