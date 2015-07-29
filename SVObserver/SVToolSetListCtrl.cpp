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
#include "SVTool.h"
#include "ToolSetView.h"
#include "SVObserver.h"
#include "SVIPDoc.h"
#include "SVSVIMStateClass.h"
#include "TextDefinesSvO.h"
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
, m_pToolSet(nullptr)
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

void SVToolSetListCtrl::SetTaskObjectList(SVTaskObjectListClass* pTaskObjectList)
{
	m_pToolSet = pTaskObjectList;
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
	InsertColumn(0, m_pToolSet->GetName(), LVCFMT_LEFT, -1, -1);

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

static SVToolClass* FindTool(const SVTaskObjectListClass* pToolSet, const CString& name)
{
	SVToolClass* pTool;
	for (int i = 0; i < pToolSet->GetSize(); i++)
	{
		pTool = dynamic_cast<SVToolClass *>(pToolSet->GetAt(i));
		if (pTool && pTool->GetName() == name)
		{
			return pTool;
		}
	}
	return nullptr;
}

void SVToolSetListCtrl::Rebuild()
{
	ToolSetView* pView = GetView();

	if (pView)
	{
		SVIPDoc* pDoc = pView->GetIPDoc();
		if (pDoc)
		{
			const SVToolGrouping& groupings = pDoc->GetToolGroupings();
			if (nullptr == m_ImageList.GetSafeHandle())
			{
				CreateImageLists();
			}

			DeleteAllItems();

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
					SVToolClass* pTool = FindTool(m_pToolSet, it->first.c_str());
					if (pTool)
					{
						itemNo = InsertTool(itemNo, pTool, bCollapsed, indent);
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
}

int SVToolSetListCtrl::InsertStartGroup(int itemNo, const CString& startName, bool bCollapsed)
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
	item.lParam = 0;
	index = InsertItem(&item);
	SetItemText(index, 0, startName);

	index++;

	return index;
}

int SVToolSetListCtrl::InsertEndGroup(int itemNo, const CString& endName, bool bCollapsed)
{
	int index = itemNo;
	if (!endName.IsEmpty() && !bCollapsed)
	{
		LVITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM;
		item.iItem = index;
		item.iSubItem = 0;
		item.iIndent = 1;
		item.lParam = 0;
		item.iImage = m_iNone;
		index = InsertItem(&item);
		SetItemText(index, 0, endName);
	}
	index++;

	return index;
}

int SVToolSetListCtrl::InsertTool(int itemNo, SVToolClass* pTool, bool bCollapsed, int indent)
{
	int index = itemNo;
	if (pTool && !bCollapsed)
	{
		int img = m_iNone;
		if (!pTool->IsValid())
		{
			img = m_iInvalid;
		}

		LVITEM item;
		memset(&item, 0, sizeof(item));
		item.iItem = index;
		item.iSubItem = 0;
		item.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM;
		item.iIndent = indent;
		item.lParam = reinterpret_cast<DWORD_PTR>(pTool);
		item.iImage = img;
		index = InsertItem(&item);
		SetItemText(index, 0, pTool->GetName());
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
	item.lParam = 0;
	item.iImage = m_iNone;

	int index = InsertItem(&item);
	SetItemText(index, 0, SvO::EndListDelimiter);
}

void SVToolSetListCtrl::InsertEmptyString(int itemNo)
{
	CString strEmpty;
	strEmpty.LoadString(IDS_EMPTY_STRING);
	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM;
	item.iItem = itemNo;
	item.iSubItem = 0;
	item.iImage = m_iNone;
	item.iIndent = 0;
	item.lParam = 0;
	int index = InsertItem(&item);
	SetItemText(index, 0, strEmpty);
}

bool SVToolSetListCtrl::IsEndListDelimiter(const CString& text) const
{
	return (text == SvO::EndListDelimiter);
}

bool SVToolSetListCtrl::IsEmptyStringPlaceHolder(const CString& text) const
{
	CString emptyString;
	emptyString.LoadString(NULL, IDS_EMPTY_STRING);
	return (text == emptyString);
}

bool SVToolSetListCtrl::IsStartGrouping(int index, bool& bState) const
{
	bool bRetVal = false;
	const ToolSetView* pView = GetView();
	if (pView)
	{
		SVIPDoc* pDoc = pView->GetIPDoc();
		if (pDoc)
		{
			CString name = GetItemText(index, 0);
			const SVToolGrouping& groups = pDoc->GetToolGroupings();
			bRetVal = groups.IsStartTag(name.GetString(), bState);
		}
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
		SVToolClass* pTool = nullptr;
		int img = m_iNone;
		item.iItem = i;	
		DWORD_PTR userData = GetItemData(i);

		if (userData)
		{
			try
			{
				pTool = dynamic_cast<SVToolClass *>(reinterpret_cast<SVTaskObjectListClass *>(userData));
			}
			catch ( ... )
			{
				pTool = nullptr;
			}
	
			if (nullptr != pTool)
			{
				if (!pTool->IsValid())
				{
					img  = m_iInvalid;
				}
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
				if (VK_ADD == nChar)
				{
					ExpandItem(item);
				}
				else if (VK_SUBTRACT == nChar)
				{
					// Collapse
					CollapseItem(item);
				}
			}
		}
	}
	if (l_bUpdate)
	{
		ToolSetView* l_pParent = static_cast< ToolSetView* >( GetParent() );
		if (l_pParent)
		{
			SVIPDoc* pCurrentDocument = dynamic_cast<SVIPDoc*>(l_pParent->GetDocument());
			if (pCurrentDocument)
			{
				SetItemState(l_iSelected, 0, LVIS_SELECTED); // un-select
				// SEJ - 999 move up or down in the list
//				SetCurrentSelection(l_iNext); // Select.
				SetItemState(l_iNext, LVIS_SELECTED, 0); // select
				EnsureVisible(l_iNext, false);

				const SVGUID& rGuid = GetSelectedTool();
				pCurrentDocument->SetSelectedToolID(rGuid);

				if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && TheSVObserverApp.OkToEdit())
				{
					l_pParent->PostMessage(WM_COMMAND, ID_RUN_ONCE);
				}
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

SVGUID SVToolSetListCtrl::GetSelectedTool() const
{
	SVGUID guid;
	int index = GetNextItem(-1, LVNI_SELECTED);
	if (index != -1)
	{
		LVITEM item;
		memset(&item, '\0', sizeof(LVITEM));
		item.mask = LVIF_PARAM;
		item.iItem = index;

		if (GetItem(&item))
		{
			if (item.lParam)
			{
				SVObjectClass* pObject = reinterpret_cast<SVObjectClass *>(item.lParam);
				if (pObject)
				{
					guid = pObject->GetUniqueObjectID();
				}
			}
		}
	}
	return guid;
}

bool SVToolSetListCtrl::AllowedToEdit() const
{
	bool bRetVal = false;
	int index = GetNextItem(-1, LVNI_SELECTED);
	if (index != -1)
	{
		CString text = GetItemText(index, 0);
		if (!IsEndListDelimiter(text) && !IsEmptyStringPlaceHolder(text))
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
			if (item.lParam)
			{
				SVTaskObjectClass* pObject = reinterpret_cast<SVTaskObjectClass*>(item.lParam);

				if (!rGuid.empty() && nullptr != pObject && rGuid == pObject->GetUniqueObjectID())
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
	int toolListIndex = m_pToolSet->GetSize();
	CString itemText; 
	int listIndex = GetNextItem(-1, LVNI_SELECTED);
	if (-1 != listIndex)
	{
		itemText = GetItemText(listIndex, 0);
	}
	if (IsEndListDelimiter(itemText) || IsEmptyStringPlaceHolder(itemText))
	{
		itemText.Empty();
	}
	return ToolListSelectionInfo(listIndex, itemText);
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
		// When an item is being edited make sure the edit control receives certain important key strokes
		// MFC has been designed in such a way that the parent window gets a chance at the messages using the PreTranslateMessage() virtual function. 
		// If the parent window is a CDialog or a CFormView, the ESC and the return key are handled by the parent and does not reach the control. 
		// To allow our control to process some important messages we need to override the PreTranslateMessage() function. 
		// In the code below, we allow the control key combinations to go through to the control. 
		// This allows copy, cut and paste using the control key combinations.
		if (GetEditControl() && 
			(VK_DELETE == pMsg->wParam ||
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
	m_ImageList.Create(16, 16, TRUE, 2, 2);

	CWinApp* pApp = AfxGetApp();
	if (pApp)
	{
		m_iNone = m_ImageList.Add(pApp->LoadIcon(IDI_STATUS_NONE));
		m_iInvalid = m_ImageList.Add(pApp->LoadIcon(IDI_STATUS_BLACK));
		m_collapseState = m_ImageList.Add(pApp->LoadIcon(IDI_COLLAPSE));
		m_expandState = m_ImageList.Add(pApp->LoadIcon(IDI_EXPAND));
	}
	SetImageList(&m_ImageList, LVSIL_NORMAL);
	SetImageList(&m_ImageList, LVSIL_SMALL);
}

void SVToolSetListCtrl::CollapseItem(int item)
{
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = item;
	lvItem.iSubItem = 0;

	GetItem(&lvItem);
	CString name = GetItemText(item, 0);
	// Send to View...
	ToolSetView* pView = dynamic_cast< ToolSetView* >( GetParent() );
	if (pView)
	{
		pView->HandleExpandCollapse(name, true);
	}
}

void SVToolSetListCtrl::ExpandItem(int item)
{
	LVITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = item;
	lvItem.iSubItem = 0;

	GetItem(&lvItem);
	CString name = GetItemText(item, 0);
	// Send to View...
	ToolSetView* pView = dynamic_cast< ToolSetView* >( GetParent() );
	if (pView)
	{
		pView->HandleExpandCollapse(name, false);
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolSetListCtrl.cpp_v  $
 * 
 *    Rev 1.6   19 Dec 2014 04:26:38   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed Tool list control to select only one item
 * Context menus for Copy and Paste
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Jun 2014 13:21:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added InsertEmptyString method.
 * Revised InsertTool method for indenting.
 * Removed unused imageList icons.
 * Revised to indent Tools in a group and the End Group.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Jun 2014 09:32:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revsied SetSelectedTool method to check for valid SVTaskObject pointer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Jun 2014 16:46:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support Tool Groupings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2014 14:51:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SetItemData to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 16:29:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.13   08 May 2013 10:49:04   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  834
   SCR Title:  Modify tool set list to maintain position when tools are deleted
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added functions to support selected tool with keyboard and scroll bars.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 15:47:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   02 Jul 2012 17:50:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   29 Jun 2010 14:08:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source to change object validity test and casting changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   01 Jun 2010 11:10:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   18 Aug 2005 12:06:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  506
   SCR Title:  Update Tool Set List Tab to include an indicator when a tool is invalid
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update RebuildImages to handle invalid tool vs empty pockets in the list control.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   17 Aug 2005 15:05:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update RebuildImages to get data from list control instead of Task Object List array.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   17 Aug 2005 14:58:00   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     fix problem in iterating the list
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   17 Aug 2005 13:36:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated the status of a null tool to invalid.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   17 Aug 2005 13:33:40   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added try/catch around a dynamic_cast to prevent a crash.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   16 Aug 2005 15:25:36   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  506
   SCR Title:  Update Tool Set List Tab to include an indicator when a tool is invalid
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added functionality to paint tool status Icons in front of the tool names.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   22 Apr 2003 16:56:02   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   20 Nov 2002 14:42:18   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed OLE Automation
   
   /////////////////////////////////////////////////////////////////////////////////////
*/