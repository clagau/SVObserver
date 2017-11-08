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
#include "GuiCommands\IsValid.h"
#include "GuiCommands\GetErrorMessageList.h"
#include "GuiCommands\GetTaskObjects.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "SVOResource\ConstGlobalSvOr.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
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
, m_iTopIndex(0)
, m_expandState(-1)
, m_collapseState(-1)
, m_ToolSetId(SV_GUID_NULL)
, m_InspectionId(SV_GUID_NULL)
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
	return dynamic_cast< ToolSetView* >( GetParent() );
}

const ToolSetView* SVToolSetListCtrl::GetView() const
{
	return dynamic_cast< ToolSetView* >( GetParent() );
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

		typedef SvCmd::GetTaskObjects Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(m_ToolSetId);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionId, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_taskList = commandPtr->TaskObjects();
		}

		int itemNo = 0;
		bool bCollapsed = false;
		int indent = 0;
		for (SVToolGrouping::const_iterator it = groupings.begin(); it != groupings.end();++it)
		{
			if (it->second.m_type == ToolGroupData::StartOfGroup)
			{
				bCollapsed = it->second.m_bCollapsed;
				itemNo = InsertStartGroup(itemNo, it->first.c_str(), bCollapsed);
				indent = 1;
			}
			else if (it->second.m_type == ToolGroupData::EndOfGroup)
			{
				itemNo = InsertEndGroup(itemNo, it->first.c_str(), bCollapsed);
				bCollapsed = false;
				indent = 0;
			}
			else
			{
				int index = -1;
				for (int i=0; i < m_taskList.size(); i++)
				{
					if (!m_taskList[i].first.empty() && 0 == m_taskList[i].first.compare(it->first.c_str()))
					{
						index = i;
						break;
					}
				}

				if (-1 < index)
				{
					itemNo = InsertTool(itemNo, index, bCollapsed, indent);
				}
			}
		}
		if (!GetItemCount())
		{
			InsertEmptyString(itemNo);
		}
		else
		{
			AddEndDelimiter();
		}
		RebuildImages();
	}
}

int SVToolSetListCtrl::InsertStartGroup(int itemNo, const std::string& rStartName, bool bCollapsed)
{
	int index = itemNo;
	int img = (bCollapsed) ? m_expandState : m_collapseState;
	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM;
	item.iItem = index;
	item.iSubItem = 0;
	item.iImage = img;
	item.iIndent = 0;
	item.lParam = -1;  //no Tool entry
	index = InsertItem( &item );
	SetItemText( index, 0, rStartName.c_str() );

	index++;

	return index;
}

int SVToolSetListCtrl::InsertEndGroup(int itemNo, const std::string& rEndName, bool bCollapsed)
{
	int index = itemNo;
	if (!rEndName.empty() && !bCollapsed)
	{
		LVITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM;
		item.iItem = index;
		item.iSubItem = 0;
		item.iIndent = 1;
		item.lParam = -1; //no Tool entry
		item.iImage = m_iNone;
		index = InsertItem( &item );
		SetItemText( index, 0, rEndName.c_str() );
	}
	index++;

	return index;
}

int SVToolSetListCtrl::InsertTool(int itemNo, int listIndex, bool bCollapsed, int indent)
{
	int index = itemNo;
	if (!bCollapsed)
	{
		int img = m_iNone;
		bool bValid = isToolValid(m_taskList[listIndex].second); 

		if (!bValid)
		{
			img = m_iInvalid;
		}

		LVITEM item;
		memset(&item, 0, sizeof(item));
		item.iItem = index;
		item.iSubItem = 0;
		item.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM;
		item.iIndent = indent;
		item.lParam = listIndex; //Tool, index in the m_taskList.
		item.iImage = img;
		index = InsertItem(&item);
		SetItemText(index, 0, m_taskList[listIndex].first.c_str());
		index++;
	}
	return index;
}

void SVToolSetListCtrl::AddEndDelimiter()
{
	int itemNo = GetItemCount();
	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM;
	item.iItem = itemNo;
	item.iSubItem = 0;
	item.iIndent = 0;
	item.lParam = -1; //no Tool entry
	item.iImage = m_iNone;

	int index = InsertItem(&item);
	SetItemText(index, 0, SvO::EndListDelimiter);
}

void SVToolSetListCtrl::InsertEmptyString(int itemNo)
{
	std::string Empty = SvUl::LoadStdString(IDS_EMPTY_STRING);
	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM;
	item.iItem = itemNo;
	item.iSubItem = 0;
	item.iImage = m_iNone;
	item.iIndent = 0;
	item.lParam = -1; //no Tool entry
	int index = InsertItem(&item);
	SetItemText(index, 0, Empty.c_str());
}

bool SVToolSetListCtrl::IsEndListDelimiter( const std::string& rName ) const
{
	return (rName == SvO::EndListDelimiter);
}

bool SVToolSetListCtrl::IsEmptyStringPlaceHolder( const std::string& rName ) const
{
	std::string EmptyString = SvUl::LoadStdString(IDS_EMPTY_STRING);
	return (rName == EmptyString);
}

bool SVToolSetListCtrl::displayErrorBox(const SVGUID& rGuid) const
{
	typedef SvCmd::GetErrorMessageList Command;
	typedef SVSharedPtr<Command> CommandPtr;
	CommandPtr commandPtr = new Command(rGuid);
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionId, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		const SvStl::MessageContainerVector& messageList = commandPtr->GetMessageList();
		if (0 < messageList.size())
		{
			SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
			Exception.setMessage( messageList[0].getMessage() );
			return true;
		}
	}
	return false;
}

bool SVToolSetListCtrl::isToolValid(const SVGUID& tool) const
{
	bool isToolValid = false;
	typedef SvCmd::IsValid Command;
	typedef SVSharedPtr<Command> CommandPtr;
	CommandPtr commandPtr = new Command(tool);
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionId, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		isToolValid = commandPtr->isValid();
	}
	return isToolValid;
}

bool SVToolSetListCtrl::IsStartGrouping(int index, bool& bState) const
{
	bool bRetVal = false;
	const ToolSetView* pView = GetView();
	if (pView)
	{
		std::string Name = GetItemText(index, 0);
		const SVToolGrouping& groups = pView->GetToolGroupings();
		bRetVal = groups.IsStartTag(Name.c_str(), bState);
	}
	return bRetVal;
}

void SVToolSetListCtrl::RebuildImages()
{
	if (!m_ImageList.GetSafeHandle())
	{
		CreateImageLists();
	}

	int l_iCount = GetItemCount();

	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_IMAGE;
	for (int i = 0;i < l_iCount; i++)
	{
		int img = m_iNone;
		item.iItem = i;	
		SVGUID toolId = getToolGuid(i);

		if (SV_GUID_NULL != toolId)
		{
			bool bValid = isToolValid(toolId);
			
			if (!bValid)
			{
				img  = m_iInvalid;
			}
		}
		else
		{
			// check for Group (Start or End)
			// if start Group add +/- depending on collapsed state
			bool bState;
			if (IsStartGrouping(i, bState))
			{
				if (bState) 
				{
					// show + (plus)
					img = m_expandState;
				}
				else
				{
					// show - (minus)
					img = m_collapseState;
				}
			}
		}
		item.iImage = img;
		SetItem(&item); 
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

			if ( SvOi::isOkToEdit() )
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
		EnsureVisible(GetItemCount() -1, false);
		EnsureVisible(m_iTopIndex, false);
	}
}

SVGUID SVToolSetListCtrl::getToolGuid(int index) const
{
	SVGUID guid = SV_GUID_NULL;
	if (-1 < index )
	{
		LVITEM item;
		memset(&item, '\0', sizeof(LVITEM));
		item.mask = LVIF_PARAM;
		item.iItem = index;

		if (GetItem(&item))
		{
			int listIndex = static_cast<int>(item.lParam); //index of the m_taskList, else -1
			if (-1 < listIndex && m_taskList.size() > listIndex)
			{
				guid = m_taskList[listIndex].second;
			}
		}
	}
	return guid;
}

SVGUID SVToolSetListCtrl::GetSelectedTool() const
{
	SVGUID guid = SV_GUID_NULL;
	int index = GetNextItem(-1, LVNI_SELECTED);
	
	{
		guid = getToolGuid(index);
	}
	return guid;
}

bool SVToolSetListCtrl::AllowedToEdit() const
{
	bool bRetVal = false;
	int index = GetNextItem(-1, LVNI_SELECTED);
	if (index != -1)
	{
		std::string Text = GetItemText(index, 0);
		if (!IsEndListDelimiter( Text ) && !IsEmptyStringPlaceHolder( Text ))
		{
			bRetVal = true;
		}
	}
	return bRetVal;
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
			if ( -1 < index && m_taskList.size() > index )
			{
				if (!rGuid.empty() && rGuid == m_taskList[index].second)
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

ToolListSelectionInfo SVToolSetListCtrl::GetToolListSelectionInfo() const
{
	std::string itemText; 
	int listIndex = GetNextItem(-1, LVNI_SELECTED);
	if (-1 != listIndex)
	{
		itemText = GetItemText(listIndex, 0);
	}
	if (IsEndListDelimiter( itemText ) || IsEmptyStringPlaceHolder( itemText ))
	{
		itemText.clear();
	}
	return ToolListSelectionInfo( listIndex, itemText );
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
		if( VK_RETURN == pMsg->wParam )
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
	m_ImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 4, 1 );

	CWinApp* pApp = AfxGetApp();
	if (pApp)
	{
		m_iNone = m_ImageList.Add(pApp->LoadIcon(IDI_STATUS_NONE));
		m_iInvalid = m_ImageList.Add(pApp->LoadIcon(IDI_STATUS_BLACK));
		m_collapseState = m_ImageList.Add(pApp->LoadIcon(IDI_COLLAPSE));
		m_expandState = m_ImageList.Add(pApp->LoadIcon(IDI_EXPAND));
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
		pView->HandleExpandCollapse( Name, true );
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
		pView->HandleExpandCollapse( Name, false );
	}
}
