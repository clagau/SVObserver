//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetTabView
//* .File Name       : $Workfile:   SVToolSetTabView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.12  $
//* .Check In Date   : $Date:   09 Dec 2014 10:22:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <comdef.h>
#include "SVToolSetTabView.h"
#include "SVObserver.h"
#include "SVImageView.h"
#include "SVIPDoc.h"
#include "SVToolSet.h"
#include "SVTool.h"
#include "SVIODoc.h"
#include "SVMainFrm.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectWriter.h"
#include "SVConfigurationObject.h"
#include "SVUserMessage.h"
#include "SVMessage/SVMessage.h"
#include "SVToolSetTabView.h"
#include "SVIPChildFrm.h"
#include "SVSVIMStateClass.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVOutputObjectList.h"
#include "SVTextEditDialog.h"
#include "SVTaskObjectValueInterface.h"
#include "SVShiftTool.h"
#include "SVShiftToolUtility.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum ToolsetViewUpdateHints
{
	ExpandCollapseHint = SVIPDoc::RefreshView + 1
};

IMPLEMENT_DYNCREATE(SVToolSetTabViewClass, CFormView)

SVToolSetTabViewClass::SVToolSetTabViewClass()
: CFormView(SVToolSetTabViewClass::IDD)
, m_isLabeling(false)
, m_labelingIndex(-1)
{
	//{{AFX_DATA_INIT(SVToolSetTabViewClass)
	//}}AFX_DATA_INIT
}

SVToolSetTabViewClass::~SVToolSetTabViewClass()
{
}

void SVToolSetTabViewClass::SetSelectedTool(const SVGUID& rGuid)
{
	m_toolSetListCtrl.SetSelectedTool(rGuid);
}

SVGUID SVToolSetTabViewClass::GetSelectedTool() const
{
	return m_toolSetListCtrl.GetSelectedTool();
}

ToolListSelectionInfo SVToolSetTabViewClass::GetToolListSelectionInfo() const
{
	return m_toolSetListCtrl.GetToolListSelectionInfo();
}

bool SVToolSetTabViewClass::IsLabelEditing() const
{
	return m_isLabeling;
}

void SVToolSetTabViewClass::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolSetTabViewClass)
	DDX_Control(pDX, IDC_OUTPUT_TREE, m_outputTreeCtrl);
	DDX_Control(pDX, IDC_TOOLSET_TREE, m_toolSetTreeCtrl);
	DDX_Control(pDX, IDC_TOOLSET_LIST, m_toolSetListCtrl);
	DDX_Control(pDX, IDC_TOOLSET_TAB, m_toolSetTabCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVToolSetTabViewClass, CFormView)
	//{{AFX_MSG_MAP(SVToolSetTabViewClass)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TOOLSET_TAB, OnSelchangeToolsetTab)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_NOTIFY(NM_DBLCLK, IDC_TOOLSET_LIST, OnDblclkToolsetList)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TOOLSET_TREE, OnItemexpandedToolsetTree)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_TOOLSET_LIST, OnBeginlabeleditToolsetList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TOOLSET_LIST, OnEndlabeleditToolsetList)
	ON_NOTIFY(NM_CLICK, IDC_TOOLSET_LIST, OnClickToolsetList)
	ON_NOTIFY(NM_RCLICK, IDC_TOOLSET_LIST, OnRightClickToolsetList)
	ON_COMMAND(ID_EDIT_LABEL_ENDS, OnEditLabelEnds)
	ON_COMMAND(ID_SELECTTOOL_TOOLCOMMENT, OnSelectToolComment)
	ON_COMMAND(ID_SELECTTOOL_LEARN, OnSelectToolSetReference)
	ON_COMMAND(ID_SELECTTOOL_NORMALIZE, OnSelectToolNormalize)
	ON_COMMAND(ID_EDIT_NAME, OnEditToolName)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_RUN_ONCE, OnRunOnce)
	//}}AFX_MSG_MAP
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

#ifdef _DEBUG
void SVToolSetTabViewClass::AssertValid() const
{
	CFormView::AssertValid();
}

void SVToolSetTabViewClass::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// SVToolSetTabViewClass message handlers

void SVToolSetTabViewClass::OnSelchangeToolsetTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TC_ITEM tabCtrlItem;
	tabCtrlItem.mask = TCIF_PARAM;

	int cur = m_toolSetTabCtrl.GetCurSel();

	// Hide all but the currently selected window
	for (int i = 0; i < m_toolSetTabCtrl.GetItemCount(); ++i)
	{
		if (i != cur)
		{
			if (m_toolSetTabCtrl.GetItem(i, &tabCtrlItem))
			{
				CWnd* pWnd = GetDlgItem(static_cast<int>(tabCtrlItem.lParam));
				if (pWnd)
				{
					pWnd->ShowWindow(SW_HIDE);
				}
			}
		}
	}

	// Show the Currently Selected Window
	if (m_toolSetTabCtrl.GetItem(cur, &tabCtrlItem))
	{
		CWnd* pWnd = GetDlgItem(static_cast<int>(tabCtrlItem.lParam));
		if (pWnd)
		{
			pWnd->ShowWindow(SW_SHOW);
		}
	}
	*pResult = 0;
}

void SVToolSetTabViewClass::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	// Adjust Tab Control
	if (::IsWindow(m_hWnd) && ::IsWindow(m_toolSetTabCtrl.m_hWnd))
	{
		TC_ITEM tabCtrlItem;
		tabCtrlItem.mask = TCIF_PARAM;
		RECT rect1;
		RECT rect2;
		int cur = m_toolSetTabCtrl.GetCurSel();

		m_toolSetTabCtrl.GetWindowRect(&rect2);
		ScreenToClient(&rect2);
		GetClientRect(&rect1);
		rect2.right  = rect1.right - rect2.left;
		rect2.right = max(rect2.right, rect2.left + 1);	// EB 2005 06 21
		rect2.bottom = rect1.bottom - rect2.top;
		rect2.bottom = max(rect2.bottom, rect2.top + 1);	// EB 2005 06 21
		m_toolSetTabCtrl.MoveWindow(&rect2, true);
		m_toolSetTabCtrl.AdjustRect(false, &rect2);
		rect2.right = max(rect2.right, rect2.left + 1);	// EB 2005 06 21
		rect2.bottom = max(rect2.bottom, rect2.top + 1);	// EB 2005 06 21
		for (int i = 0; i < m_toolSetTabCtrl.GetItemCount(); ++i)
		{
			if (m_toolSetTabCtrl.GetItem(i, &tabCtrlItem))
			{
				CWnd* pWnd = GetDlgItem(static_cast<int>(tabCtrlItem.lParam));
				if (pWnd)
				{
					if (i == cur)
					{
						pWnd->MoveWindow(&rect2, true);
					}
					else
					{
						pWnd->MoveWindow(&rect2, false);
					}
				}
			}
		}
	}
}

void SVToolSetTabViewClass::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// Want Pluses and Minuses
	m_toolSetTreeCtrl.SetHasButtonsStyle();

	// Don't Allow editing of labels in Tree 
	m_toolSetTreeCtrl.SetNoEditLabelsStyle();

	SVIPDoc* pCurrentDocument = GetIPDoc();

	if (nullptr != pCurrentDocument)
	{
		TC_ITEM tabCtrlItem;
		tabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
		int count = 0;

		tabCtrlItem.pszText = _T("Tool Set List");;
		tabCtrlItem.lParam	= static_cast<DWORD_PTR>(IDC_TOOLSET_LIST);
		m_toolSetTabCtrl.InsertItem(count++, &tabCtrlItem);

		tabCtrlItem.pszText = _T("Tool Set Tree");
		tabCtrlItem.lParam	= static_cast<DWORD_PTR>(IDC_TOOLSET_TREE);
		m_toolSetTabCtrl.InsertItem(count++, &tabCtrlItem);

		// Set initial tab selection...
		m_toolSetTabCtrl.SetCurSel(0);

		m_outputTreeCtrl.ShowWindow(SW_HIDE); 

		m_toolSetTreeCtrl.ShowWindow(SW_HIDE);
		m_toolSetListCtrl.ShowWindow(SW_SHOW);
	}
}

int SVToolSetTabViewClass::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (-1 == CFormView::OnCreate(lpCreateStruct))
		return -1;

	return 0;
}

void SVToolSetTabViewClass::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST))
	{
		SVIPDoc* pCurrentDocument = GetIPDoc();

		if (nullptr != pCurrentDocument)
		{
			// save the scroll bar positions.
			m_toolSetListCtrl.SaveScrollPos();
			if ((SVIPDoc::RefreshView == lHint) || (S_OK == pCurrentDocument->IsToolSetListUpdated()))
			{
				// Initialize the tool Groupings (for older saved configurations)
				SVToolGrouping& rToolGroupings = pCurrentDocument->GetToolGroupings();
				if (rToolGroupings.empty())
				{
					SVToolSetClass* pToolSet = pCurrentDocument->GetToolSet();
					SVString insertAtEnd;
					for (int i = 0;i < pToolSet->GetSize();i++)
					{
						SVString name = pToolSet->GetAt(i)->GetName();
						rToolGroupings.AddTool(name.c_str(), insertAtEnd.c_str());
					}
				}
				m_toolSetListCtrl.SetTaskObjectList(pCurrentDocument->GetToolSet());
				m_toolSetTreeCtrl.SetTaskObjectList(pCurrentDocument->GetToolSet());

				m_toolSetListCtrl.SetSelectedTool(pCurrentDocument->GetSelectedToolID());
			}
			else if (ExpandCollapseHint == lHint)
			{
				m_toolSetListCtrl.Rebuild();
				m_toolSetTreeCtrl.Rebuild();
			}
			else
			{
				m_toolSetListCtrl.RebuildImages();
				const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
				pCurrentDocument->SetSelectedToolID(rGuid);
			}
			// force the list control to position its scroll bars where they were before the rebuild.
			m_toolSetListCtrl.RestoreScrollPos();
		}
		//CFormView::OnUpdate(pSender, lHint, pHint);   // This call causes flicker
	}
}

void SVToolSetTabViewClass::OnEditToolName()
{
	CWaitCursor l_cwcMouse;

	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) || !TheSVObserverApp.OkToEdit())
	{
		return;
	}

	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr != pCurrentDocument)
	{
		int index = m_toolSetListCtrl.GetNextItem(-1, LVNI_SELECTED);

		if (-1 != index)
		{
			m_toolSetListCtrl.EditLabel(index);

			// Run the toolset once to update the images/results.
			// 16 Dec 1999 - frb. (99)

			// Post a message to do the run once so that the double click still works
			PostMessage(WM_COMMAND, ID_RUN_ONCE);
		}
	}
}

void SVToolSetTabViewClass::OnClickToolsetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	// Allow Expand/Collapse in run mode
	// Also clicking on the Start or End Group does a run once if allowed.
	if (IsToolsetListCtrlActive())
	{
		NMITEMACTIVATE* pActivate = reinterpret_cast<NMITEMACTIVATE *>(pNMHDR);
		UINT uFlags;
		int item = m_toolSetListCtrl.HitTest(pActivate->ptAction, &uFlags);
		if (!(uFlags & LVHT_ONITEMLABEL) && (uFlags & LVHT_ONITEMSTATEICON | LVHT_ONITEMICON))
		{
			// Toggle Collapse.
			if (item > -1)
			{
				ToggleExpandCollapse(item);
			}
		}
	}
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) || !TheSVObserverApp.OkToEdit())
	{
		return;
	}
	CWaitCursor l_cwcMouse;

	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr != pCurrentDocument)
	{
		pCurrentDocument->SetSelectedToolID(m_toolSetListCtrl.GetSelectedTool());

		// Run the toolset once to update the images/results.
		// 16 Dec 1999 - frb. (99)
		// Post a message to do the run once so that the double click still works
		PostMessage(WM_COMMAND,ID_RUN_ONCE);
	}
}


void SVToolSetTabViewClass::OnRightClickToolsetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	// We need supervisory access to set a tool to be moved & resized after configuration load.
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) || !TheSVObserverApp.OkToEdit())
	{
		return;
	}

	// Get the tool selected from tool list and make it the currently selected tool.
	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr != pCurrentDocument)
	{
		SVToolSetClass* pToolSet = pCurrentDocument->GetToolSet();
		if (nullptr != pToolSet)
		{
			// Get the tool comment...
			const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
			if (!rGuid .empty())
			{
				SVToolClass* pSelectedTool = dynamic_cast<SVToolClass *>(SVObjectManagerClass::Instance().GetObject(rGuid));
				if (nullptr != pSelectedTool)
				{
					POINT l_Point;
					m_toolSetListCtrl.GetSelectedItemScreenPosition(l_Point);
					CMenu l_menu;
					CMenu* l_pPopup;

					BOOL l_bMenuLoaded = false;
					SVShiftTool* pShiftTool = dynamic_cast< SVShiftTool* >(pSelectedTool);
					if (nullptr != pShiftTool)
					{
						long l_shiftMode;
						pShiftTool->m_evoShiftMode.GetValue(l_shiftMode);
						if (l_shiftMode == SV_SHIFT_ENUM::SV_SHIFT_REFERENCE)
						{
							l_bMenuLoaded = l_menu.LoadMenu(IDR_TOOL_LIST_CONTEXT_MENU_SHIFT);
						}
						else
						{
							l_bMenuLoaded = l_menu.LoadMenu(IDR_TOOL_LIST_CONTEXT_MENU);
						}
					}
					else
					{
						l_bMenuLoaded = l_menu.LoadMenu(IDR_TOOL_LIST_CONTEXT_MENU);
					}

					if (TRUE == l_bMenuLoaded)
					{
						if (l_pPopup = l_menu.GetSubMenu(0))
						{
							l_Point.y += 12;
							l_Point.x += 20;
							ClientToScreen(&l_Point);
							l_pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, l_Point.x, l_Point.y, this);
						}
					}
				}
			}
			// Run the toolset once to update the images/results.
			// 16 Dec 1999 - frb. (99)
			pCurrentDocument->RunOnce();
		}
	}
	*pResult = 0;
}

void SVToolSetTabViewClass::OnDblclkToolsetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) || !TheSVObserverApp.OkToEdit())
	{
		return;
	}

	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr != pCurrentDocument)
	{
		const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
		if (!rGuid.empty())
		{
			pCurrentDocument->OnEditTool();
		}
		else
		{
			// Deselect all...
			m_toolSetListCtrl.SetSelectedTool(SVGUID());
		}
	}
	*pResult = 0;
}

void SVToolSetTabViewClass::OnItemexpandedToolsetTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if (TVE_COLLAPSE == pNMTreeView->action)
	{
		m_toolSetTreeCtrl.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	*pResult = 0;
}

void SVToolSetTabViewClass::OnBeginlabeleditToolsetList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);

	// Don't allow editing if running.
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) || !TheSVObserverApp.OkToEdit())
	{
		*pResult = true;		// Abandon the label editing.
		return;
	}

	m_labelingIndex = m_toolSetListCtrl.GetNextItem(-1, LVNI_SELECTED);

	// Check for the last item which is the string of 'dashes'.
	if (-1 == m_labelingIndex || !m_toolSetListCtrl.AllowedToEdit())
	{
		*pResult = true;         // Abandon the label editing.
		return;
	}

	// Save the current 'label' in case it needs to be restored.
	CEdit* pEdit = m_toolSetListCtrl.GetEditControl();
	ASSERT_VALID(pEdit);
	pEdit->GetWindowText(m_csLabelSaved);

	m_isLabeling = true;
	SetCapture();            // capture mouse events - 28 Oct 1999 - frb.

	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Validate label text and remove unwanted characters.
void SVToolSetTabViewClass::ValidateLabelText(CString& newText)
{
	// The characters we do not want in the label.
	static const CString csUndesirables = SVEXCLUDECHARS_TOOL_NAME;

	int nCount = csUndesirables.GetLength();
	for (int i = 0;i < nCount;i++)
	{
		TCHAR t = csUndesirables.GetAt(i);

		bool bDone = false;
		while (true != bDone)
		{
			int nIndex = newText.Find(t);
			if (nIndex > -1)
			{
				CString csTemp;
				if (nIndex > 0)
				{
					csTemp = newText.Left(nIndex);
				}
				int nLength = newText.GetLength();
				if (nIndex < (nLength - 1))
				{
					csTemp += newText.Right((nLength - 1) - nIndex);
				}
				newText = csTemp;
			}
			else
			{
				bDone = true;        // Exit while loop
			}
		}
	}

	// Do we have anything left?
	if (newText.GetLength() < 1)
	{
		ASSERT(m_csLabelSaved.GetLength() > 0);
		newText = m_csLabelSaved;
	}
}

void SVToolSetTabViewClass::RenameItem(int item, const CString& oldName, const CString& newName)
{
	// check if it's a group or a tool
	if (m_labelingIndex >= 0 && m_labelingIndex < m_toolSetListCtrl.GetItemCount())
	{
		CString name = m_toolSetListCtrl.GetItemText(m_labelingIndex, 0);
		if (!m_toolSetListCtrl.IsEndListDelimiter(name) && !m_toolSetListCtrl.IsEmptyStringPlaceHolder(name))
		{
			DWORD_PTR itemData = m_toolSetListCtrl.GetItemData(m_labelingIndex);
			if (itemData) // it's a Tool
			{
				SVTaskObjectClass* pTaskObject = reinterpret_cast<SVTaskObjectClass*>(itemData);
				if (pTaskObject)
				{
					TheSVObserverApp.RenameObject(m_csLabelSaved, m_csLabelEdited, pTaskObject->GetUniqueObjectID());
				}
			}
			SVIPDoc* pDoc = GetIPDoc();
			if (pDoc)
			{
				SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
				rGroupings.RenameItem(static_cast<LPCTSTR>(oldName), static_cast<LPCTSTR>(newName));
			}
		}
	}
}

void SVToolSetTabViewClass::OnEditLabelEnds()
{
	// Update the text in the list control item.
	if (m_labelingIndex >= 0 && m_labelingIndex < m_toolSetListCtrl.GetItemCount()) 
	{
		BOOL bResult = m_toolSetListCtrl.SetItemText(m_labelingIndex, 0, m_csLabelEdited);
		ASSERT(bResult);

		// Cause a redraw.
		m_toolSetListCtrl.Update(m_labelingIndex);

		// Broadcast message that the tool name changed if necessary.
		if (m_csLabelEdited != m_csLabelSaved)
		{
			RenameItem(m_labelingIndex, m_csLabelSaved, m_csLabelEdited);
		}
	}
	if (m_isLabeling)
	{
		// Cause edit control to 'end' editing.
		SetFocus();   // Cause edit control to lose 'focus'.
	}

	m_isLabeling = false;
	::ReleaseCapture();     // release the mouse capture
}


void SVToolSetTabViewClass::OnSelectToolComment()
{
	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr != pCurrentDocument)
	{
		SVToolSetClass* pToolSet = pCurrentDocument->GetToolSet();
		if (nullptr != pToolSet)
		{
			const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
			if (!rGuid.empty())
			{
				SVToolClass* pSelectedTool = dynamic_cast<SVToolClass *>(SVObjectManagerClass::Instance().GetObject(rGuid));
				if (nullptr != pSelectedTool)
				{
					// Get the tool comment...
					CString csToolComment;
					SVValueObjectClass* l_pVo = pSelectedTool->GetToolComment(); 
					// bring up tool comment edit.
					HRESULT hr = l_pVo->GetValue(csToolComment);
					if (S_OK == hr )
					{
						SVTextEditDialog l_dlg;
						l_dlg.m_strText = csToolComment;
						INT_PTR l_ret = l_dlg.DoModal();
						if (IDOK == l_ret)
						{
							// Set 
							SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
							pToolSet->GetInspection()->AddInputRequest(pSelectedTool->GetToolComment(), static_cast<LPCTSTR>(l_dlg.m_strText));
							pToolSet->GetInspection()->AddInputRequestMarker();

							pCurrentDocument->RunOnce(pSelectedTool);
						}
					}
				}
			}
		}
	}
}

void SVToolSetTabViewClass::OnSelectToolSetReference()
{
	const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
	SVToolClass* pTool = dynamic_cast<SVToolClass *>(SVObjectManagerClass::Instance().GetObject(rGuid));

	if (nullptr != pTool)
	{
		SVShiftTool* l_pShiftTool = dynamic_cast<SVShiftTool *>(pTool);

		if (nullptr != l_pShiftTool)
		{
			SVShiftToolUtility::SetToolSetReference(l_pShiftTool);
		}
	}
}

void SVToolSetTabViewClass::OnSelectToolNormalize()
{
	const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
	if (!rGuid.empty())
	{
		SVToolClass* pTool = dynamic_cast<SVToolClass *>(SVObjectManagerClass::Instance().GetObject(rGuid));
		if (nullptr != pTool)
		{
			SVShiftTool* l_pShiftTool = dynamic_cast<SVShiftTool *>(pTool);

			if (nullptr != l_pShiftTool)
			{
				SVShiftToolUtility::SetToolNormalize(l_pShiftTool);
			}
		}
	}
}

void SVToolSetTabViewClass::OnRunOnce()
{
	CWaitCursor l_cwcMouse;

	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST))
	{
		return;
	}

	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr != pCurrentDocument)
	{
		//
		// Run the toolset once to update the images/results.
		// 16 Dec 1999 - frb. (99)
		//
		pCurrentDocument->RunOnce();
	}
}

static bool ShowDuplicateNameMessage(const CString& rName)
{
	CString msg("A duplicate name was found for the item being renamed\n");
	msg += rName;
	int rc = AfxMessageBox(msg, MB_RETRYCANCEL);
	return (IDRETRY == rc);
}

void SVToolSetTabViewClass::OnEndlabeleditToolsetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);

	SVIPDoc* pCurrentDocument = GetIPDoc();

	if (nullptr != pCurrentDocument)
	{
		m_isLabeling = false;
		const SVGUID& rGuid = pCurrentDocument->GetSelectedToolID();
		CEdit* pEdit = m_toolSetListCtrl.GetEditControl();
		if (nullptr != pEdit)
		{
			CString newText;
			pEdit->GetWindowText(newText);

			// Validate the text and remove unwanted characters.
			ValidateLabelText(newText);
			bool bDoReEdit = false;
			if (m_csLabelSaved != newText) // In case it was renamed to the same name as before renaming
			{
				if (!rGuid.empty()) // renaming a Tool
				{
					SVToolSetClass* pToolSet = pCurrentDocument->GetToolSet();
					SVToolClass* pTool = dynamic_cast<SVToolClass*>(SVObjectManagerClass::Instance().GetObject(rGuid));
					if (nullptr != pToolSet && nullptr != pTool)
					{
						// Check for duplicate name and show message if Duplicate
						bool bNameOk = CheckName(newText);
						if (bNameOk)
						{
							pTool->SetName(newText);
						}
						else
						{
							
							bDoReEdit = ShowDuplicateNameMessage(newText);
							newText = m_csLabelSaved; // reset it back to original
						}
					}
				}
				else // renaming a Group label
				{
					if (!CheckName(newText))
					{
						bDoReEdit = ShowDuplicateNameMessage(newText);
						newText = m_csLabelSaved; // reset it back to original
					}
				}
			}

			// Save the edited and validated label for the callback message handler.
			m_csLabelEdited = newText;

			// Post call back message for OnEditLabelEnds();
			PostMessage(WM_COMMAND, ID_EDIT_LABEL_ENDS);

			//m_labelingIndex = -1;
			pCurrentDocument->SetModifiedFlag();
			pCurrentDocument->UpdateAllViews(NULL);

			// Update result io view, too...
			SVIODoc* pIODoc = TheSVObserverApp.GetIODoc();
			if (nullptr != pIODoc)
			{
				pIODoc->UpdateAllViews(nullptr);
			}
			*pResult = true;
			if (bDoReEdit)
			{
				PostMessage(WM_COMMAND, ID_EDIT_NAME);
			}
		}
	}
	else
	{
		m_isLabeling = false;
		*pResult = 0;
	}
}

void SVToolSetTabViewClass::OnDraw(CDC* pDC)
{
	CFormView::OnDraw(pDC);

	// Get the Current page and refresh it
	TC_ITEM tabCtrlItem;
	tabCtrlItem.mask = TCIF_PARAM;

	int cur = m_toolSetTabCtrl.GetCurSel();

	if (m_toolSetTabCtrl.GetItem(cur, &tabCtrlItem))
	{
		if (IDC_TOOLSET_LIST == tabCtrlItem.lParam)
		{
			m_toolSetListCtrl.BringWindowToTop();
		}
		else if (IDC_TOOLSET_TREE == tabCtrlItem.lParam)
		{
			m_toolSetTreeCtrl.BringWindowToTop();
		}
	}
}

void SVToolSetTabViewClass::OnLButtonDown(UINT nFlags, CPoint point)
{
	// If we are editing a label, we have the mouse 'captured'.
	// If a mouse event occurs we need to end the editing.
	if (m_isLabeling)
	{
		// Cause edit control to 'end' editing.
		SetFocus();   // Cause edit control to lose 'focus'.
	}
	CFormView::OnLButtonDown(nFlags, point);
}

void SVToolSetTabViewClass::SetViewSize(CSize &Size)
{
	CWnd *l_pcWnd = this;
	SVIPSplitterFrame *l_pSplitterFrame = nullptr;

	do
	{
		l_pcWnd = l_pcWnd->GetParent();

		if (nullptr != l_pcWnd)
		{
			l_pSplitterFrame = dynamic_cast<SVIPSplitterFrame*>(l_pcWnd);
		}
	}
	while (nullptr == l_pSplitterFrame && nullptr != l_pcWnd);

	if (nullptr != l_pSplitterFrame)
	{
		l_pSplitterFrame->SetViewSize(this, Size);
	}
}

BOOL SVToolSetTabViewClass::SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	BOOL bOk = false;

	CSize l_Size;
	_variant_t svVariant;

	bOk = SVNavigateTreeClass::GetItem(rTree, CTAG_CELL_HEIGHT, htiParent, svVariant);
	if (bOk)
	{
		l_Size.cy = svVariant;
	}

	if (bOk)
	{
		bOk = SVNavigateTreeClass::GetItem(rTree, CTAG_CELL_WIDTH, htiParent, svVariant);
		if (bOk)
		{
			l_Size.cx = svVariant;
		}
	}

	if (bOk)
	{
		SetViewSize(l_Size);
	}
	return bOk;
}

BOOL SVToolSetTabViewClass::CheckParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	BOOL bOk = false;
	CSize l_Size;
	_variant_t svVariant;

	CRect l_cRect;
	GetWindowRect(l_cRect);

	bOk = SVNavigateTreeClass::GetItem(rTree, CTAG_CELL_HEIGHT, htiParent, svVariant);
	if (bOk)
	{
		l_Size.cy = svVariant;
	}

	if (bOk)
	{
		bOk = SVNavigateTreeClass::GetItem(rTree, CTAG_CELL_WIDTH, htiParent, svVariant);
		if (bOk)
		{
			l_Size.cx = svVariant;
		}
	}

	// If the Width of this view is less than 10, Windows somehow gets into a mad
	// message loop using up 50% processor utilization.
	// So we force the minimum width to be at least 10 pixels to prevent this problem.
	l_Size.cx = std::max<long>(10, l_Size.cx);
	if (bOk)
	{
		if (l_Size.cx != l_cRect.Width() || l_Size.cy != l_cRect.Height())
		{
			SetViewSize(l_Size);

			GetWindowRect(l_cRect);
		}
	}
	return bOk;
}

SVIPDoc* SVToolSetTabViewClass::GetIPDoc() const
{
	return dynamic_cast<SVIPDoc*>(GetDocument());
}

BOOL SVToolSetTabViewClass::GetParameters(SVObjectWriter& rWriter)
{
	BOOL bOk = true;

	CRect l_cRect;

	GetWindowRect(l_cRect);

	_variant_t svVariant;

	svVariant = l_cRect.Height();
	rWriter.WriteAttribute(CTAG_CELL_HEIGHT, svVariant);
	svVariant.Clear();

	svVariant = l_cRect.Width();
	rWriter.WriteAttribute(CTAG_CELL_WIDTH, svVariant);
	svVariant.Clear();

	return bOk;
}

void SVToolSetTabViewClass::OnSetFocus(CWnd* pOldWnd)
{
	CFormView::OnSetFocus(pOldWnd);

	SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );

	if(nullptr != pFrame)
	{
		SVIPSplitterFrame* pSplitterFrame = dynamic_cast< SVIPSplitterFrame*>(pFrame->GetActiveFrame());

		if(nullptr != pSplitterFrame)
		{
			pSplitterFrame->RefreshAllSplitters();
		}
		pFrame->EnableZoomToolbar(false);
	}
}

void SVToolSetTabViewClass::HandleExpandCollapse(const CString& name, bool bCollapse)
{
	SVIPDoc* pDoc = GetIPDoc();
	if (nullptr != pDoc)
	{
		SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
		bool bState = rGroupings.IsCollapsed(name.GetString());
		if (bState != bCollapse)
		{
			rGroupings.Collapse(name.GetString(), bCollapse);
			OnUpdate(this, ExpandCollapseHint, nullptr);
		}
	}
}

void SVToolSetTabViewClass::ToggleExpandCollapse(int item)
{
	SVIPDoc* pDoc = GetIPDoc();
	if (nullptr != pDoc)
	{
		LVITEM lvItem;
		lvItem.mask = LVIF_PARAM;
		lvItem.iItem = item;
		lvItem.iSubItem = 0;
		SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
		m_toolSetListCtrl.GetItem(&lvItem);
		CString name = m_toolSetListCtrl.GetItemText(item, 0);

		bool bState = rGroupings.IsCollapsed(name.GetString());
		if (rGroupings.Collapse(name.GetString(), bState ? false : true))
		{
			OnUpdate(this, ExpandCollapseHint, nullptr);
		}
	}
}

bool SVToolSetTabViewClass::IsEndToolGroupAllowed() const
{
	bool bRetVal = false;
	SVIPDoc* pDoc = GetIPDoc();
	if (nullptr != pDoc)
	{
		CString itemName;
		int item = -1;
		// If there is a unmatched Start Group before the insertion point, End Group is allowed
		POSITION pos = m_toolSetListCtrl.GetFirstSelectedItemPosition();
		if (0 != pos)
		{
			item = m_toolSetListCtrl.GetNextSelectedItem(pos);
			itemName = m_toolSetListCtrl.GetItemText(item, 0);
			if (m_toolSetListCtrl.IsEndListDelimiter(itemName))
			{
				itemName.Empty();
			}
		}
		const SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
		const SVString& name = rGroupings.FindCandidateStartGroup(static_cast<LPCTSTR>(itemName));
		if (!name.empty())
		{
			bRetVal = true;
		}
	}
	return bRetVal;
}

bool SVToolSetTabViewClass::CheckName(const CString& name) const
{
	bool bNameOk = true;
	SVIPDoc* pDoc = GetIPDoc();
	if (nullptr != pDoc)
	{
		const SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
		bNameOk = rGroupings.IsNameUnique(static_cast<LPCTSTR>(name));
	}
	return bNameOk;
}

bool SVToolSetTabViewClass::IsToolsetListCtrlActive() const
{
	bool bRetVal = false;
	TC_ITEM tabCtrlItem;
	tabCtrlItem.mask = TCIF_PARAM;

	int cur = m_toolSetTabCtrl.GetCurSel();

	if (m_toolSetTabCtrl.GetItem(cur, &tabCtrlItem))
	{
		if (IDC_TOOLSET_LIST == static_cast<int>(tabCtrlItem.lParam))
		{
			bRetVal = true;
		}
	}
	return bRetVal;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolSetTabView.cpp_v  $
 * 
 *    Rev 1.12   09 Dec 2014 10:22:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed functions that support Operator Move.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Nov 2014 16:46:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  969
 * SCR Title:  SVObserver Should Check the Name of Group Tools When Naming Other Tools
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed IsGroupNameUnique method.
 * Added CheckName method.
 * Added ShowDuplicateNameMessage function.
 * Revised OnEndlabeleditToolsetList to check for unique name and prompt with Retry/Canel message box if not..
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   14 Aug 2014 16:18:38   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   blue frame around active panel new function OnSetFocus
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Jul 2014 14:19:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised makeGroupnameUnique to call SVToolGrouping MakeNameUnique
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   26 Jun 2014 09:51:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revsied OnUpdate to correct an issue where the toolset is displayed in reverse order whenthere are no tool groupings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Jun 2014 18:32:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed FindLoneStartGroup method.
 * Revised IsEndGroupAllowed method to correct an issue with not finding the proper start/end group pairing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   12 Jun 2014 16:46:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support Tool Groupings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 Feb 2014 12:19:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  854
 * SCR Title:  Add HotKey (F2) to Edit a Tool's Name
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Add logic to OnEditLabelEnds to end lable edit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   11 Nov 2013 07:39:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed tool comment from SVStringValueObjectClass to SVStaticStringValueObjectClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Sep 2013 14:51:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  854
 * SCR Title:  Add HotKey (F2) to Edit a Tool's Name
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added OnEditToolName method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2013 19:46:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.63   14 May 2013 13:36:08   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  834
   SCR Title:  Modify tool set list to maintain position when tools are deleted
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added logic to Update to use RefreshView hint.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.1   13 May 2013 16:29:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.62   08 May 2013 10:50:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  834
 * SCR Title:  Modify tool set list to maintain position when tools are deleted
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnUpdate to maintain scrollbar positions in tool list control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 12:26:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.61   18 Feb 2013 13:21:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated inspection input info structs from using a CString as the data element to a variant.  Updated all methods that call the inspection interface from CString to variant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.60   10 Jan 2013 15:16:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Merged 6.01 branch code into main development code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.59   07 Jan 2013 15:08:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed the menu for the Shift Tool modes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.58   02 Jan 2013 15:45:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed OnSelectToolLearn to OnSelectToolSetReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.57   07 Dec 2012 10:38:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new menu options for the new Shift Tool - Learn & Normalize
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.56   11 Oct 2012 10:59:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information based on the navigate tree class moving to the XML library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.55   28 Sep 2012 14:49:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated saving operation to use the object writer instead of the XML tree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.54   25 Sep 2012 15:28:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.53   18 Sep 2012 18:32:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.52   04 Sep 2012 15:37:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51.1.4   09 Jan 2013 12:47:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the SetReference & SetNormalize to use the SVShiftToolUtility
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51.1.3   08 Jan 2013 16:07:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated code to fix issues with selected tool and add tool filtering.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51.1.2   07 Jan 2013 15:01:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the way the menu gets brought in for the Shift Tool.  It will have the standard menu for Absolute and No Shift modes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51.1.1   02 Jan 2013 15:14:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed OnSelectToolLearn to OnSelectToolSetReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51.1.0   05 Dec 2012 12:27:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new menu options for the new Shift Tool - Learn & Normalize
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51   08 Aug 2012 15:44:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added method to handle the is movable check.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.50   07 Aug 2012 16:18:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated views to remove flicker.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.49   03 Aug 2012 11:14:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added additional logic to have two context menus for tools with or without OperatorMove option.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.48   01 Aug 2012 12:41:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.47   30 Jul 2012 14:27:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added message map entry for set tool for operator move.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.46   09 Jul 2012 14:20:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.45   05 Jul 2012 14:31:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added context menu item to edit tool comment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44   02 Jul 2012 17:50:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.43   19 Jun 2012 14:25:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   13 Mar 2012 14:00:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  745
 * SCR Title:  Fix contention issue during load process and display
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated formatting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.41   16 Jan 2012 15:42:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added tool adjustment functionality for the new remote input tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   16 Sep 2011 16:23:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   30 Mar 2011 15:29:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use STL min/max functions instead of MFC macros.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   27 Jan 2011 12:03:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   13 Dec 2010 11:31:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   08 Dec 2010 13:45:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   05 Nov 2010 14:58:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove the un-necessary IP View Control from the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   05 Nov 2010 11:13:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   01 Jun 2010 11:10:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   16 Dec 2009 13:20:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   17 Nov 2008 10:54:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed constant in max( statement from 10 to 10L for VC8 Compile.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   20 Oct 2005 10:15:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed potential crash caused by C-style casts
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   24 Aug 2005 09:46:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed the use of SVIPDoc::SingleModeRunLoop and replaced it with SVInspectionProcess::RunOnce.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   10 Aug 2005 12:44:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   02 Aug 2005 13:56:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OKToEdit() to make menu security more consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   01 Aug 2005 08:40:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Access Points for "Editing the tool set" to make their behavior more consistant
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   25 Jul 2005 11:36:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added check to toolsettabbed view to check for edit mode instead of prompting user for security.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   22 Jul 2005 13:56:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed code to deselect tools when going into different modes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   19 Jul 2005 11:27:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed security from right click "tool selected for operator move" which is an edit mode function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   27 Jun 2005 15:05:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed text literal to SVEXCLUDECHARS_***
 * cleaned up file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   27 Jun 2005 09:59:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  497
 * SCR Title:  Remove Freeze Set from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed Show/Hide Connection to Freeze Set.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   27 Jun 2005 09:57:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  497
 * SCR Title:  Remove Freeze Set from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed freeze set tab and access to freeze set.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   24 Jun 2005 11:26:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  496
 * SCR Title:  Fix Load Configuration display problem
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed OnSize to avoid the continuous WM_PAINTs / high processor utilization bug when view size is very small
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   21 Jun 2005 13:17:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New SVSecurity access added.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   08 Apr 2005 14:11:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  481
 * SCR Title:  Fix Bug when loading Config ( causes 50% processor usage )
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Made change to ToolSetTabViewClass::CheckParameters to limit the width to a minimum of 10 pixels.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   17 Feb 2005 14:01:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new Extents.
 * Added CheckParameters(
 * Removed GetViewSize(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   17 Jun 2003 15:18:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added prototype code for using a map in the inspection to lookup objects that are used in the input request queue. It will be finalized after further testing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   22 Apr 2003 17:07:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   05 Feb 2003 17:30:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Made modifications to the display logic to display the current source and result images indexes, respectively. This should make it work correctly regardless of whether it is online or performing RunOnce/Regression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   10 Jan 2003 14:02:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to how the images are displayed while running and while doing a RunOnce operation to use the image that is specified in the SVProductInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   20 Nov 2002 14:42:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added SetParameters & GetParameters
 * Remove OLE Dispatch Map
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   21 Feb 2002 09:46:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  233
 * SCR Title:  Unable to select a tool to edit by double clicking when toolset is large on .5
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed ::OnClickToolsetList to not call SingleRunModeLoop. Instead it posts a message, ID_RUN_ONCE, and SingleRunModeLoop is called in its handler, OnRunOnce
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   12 Nov 2001 10:22:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  238
 * SCR Title:  Security Issue: Some users without the privlegy can execute "Move Tools".
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved short-cut check on priviledge in OnClickToolsetList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
