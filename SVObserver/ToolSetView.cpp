//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ToolSetView
//* .File Name       : $Workfile:   ToolSetView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.15  $
//* .Check In Date   : $Date:   12 Feb 2015 15:38:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <comdef.h>
#include "ToolSetView.h"
#include "SVObserver.h"
#include "SVIPDoc.h"
#include "SVToolSet.h"
#include "SVTool.h"
#include "SVIODoc.h"
#include "SVMainFrm.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectWriter.h"
#include "SVConfigurationObject.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "SVMessage/SVMessage.h"
#include "SVIPChildFrm.h"
#include "SVSVIMStateClass.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVOutputObjectList.h"
#include "SVOGui/SVTextEditDialog.h"
#include "SVTaskObjectValueInterface.h"
#include "SVShiftTool.h"
#include "SVShiftToolUtility.h"
#include "TextDefinesSvO.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum ToolsetViewUpdateHints
{
	ExpandCollapseHint = SVIPDoc::RefreshView + 1
};

IMPLEMENT_DYNCREATE( ToolSetView, CFormView )
#pragma endregion Declarations

#pragma region Constructor
ToolSetView::ToolSetView()
: CFormView(ToolSetView::IDD)
, m_isLabeling(false)
, m_labelingIndex(-1)
{
	//{{AFX_DATA_INIT(ToolSetView)
	//}}AFX_DATA_INIT
}

ToolSetView::~ToolSetView()
{
}
#pragma endregion Constructor

void ToolSetView::SetSelectedTool(const SVGUID& rGuid)
{
	m_toolSetListCtrl.SetSelectedTool(rGuid);
}

SVGUID ToolSetView::GetSelectedTool() const
{
	return m_toolSetListCtrl.GetSelectedTool();
}

ToolListSelectionInfo ToolSetView::GetToolListSelectionInfo() const
{
	return m_toolSetListCtrl.GetToolListSelectionInfo();
}

bool ToolSetView::IsLabelEditing() const
{
	return m_isLabeling;
}

void ToolSetView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ToolSetView)
	DDX_Control(pDX, IDC_TOOLSET_LIST, m_toolSetListCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ToolSetView, CFormView)
	//{{AFX_MSG_MAP(ToolSetView)
	//ON_NOTIFY(TCN_SELCHANGE, IDC_TOOLSET_TAB, OnSelchangeToolsetTab)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_NOTIFY(NM_DBLCLK, IDC_TOOLSET_LIST, OnDblClkToolSetList)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_TOOLSET_LIST, OnBeginLabelEditToolSetList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TOOLSET_LIST, OnEndLabelEditToolSetList)
	ON_NOTIFY(NM_CLICK, IDC_TOOLSET_LIST, OnClickToolSetList)
	ON_NOTIFY(NM_RCLICK, IDC_TOOLSET_LIST, OnRightClickToolSetList)
	ON_COMMAND(ID_EDIT_LABEL_ENDS, OnEditLabelEnds)
	ON_COMMAND(ID_SELECTTOOL_TOOLCOMMENT, OnSelectComment)
	ON_COMMAND(ID_SELECTTOOL_LEARN, OnSelectToolSetReference)
	ON_COMMAND(ID_SELECTTOOL_NORMALIZE, OnSelectToolNormalize)
	ON_COMMAND(ID_EDIT_NAME, OnEditToolName)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_RUN_ONCE, OnRunOnce)
	//}}AFX_MSG_MAP
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

#ifdef _DEBUG
void ToolSetView::AssertValid() const
{
	CFormView::AssertValid();
}

void ToolSetView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

void ToolSetView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if ( ::IsWindow( m_hWnd ) && ::IsWindow( m_toolSetListCtrl.m_hWnd ) )
	{
		RECT rect1;
		RECT rect2;

		m_toolSetListCtrl.GetWindowRect(&rect2);
		ScreenToClient(&rect2);
		GetClientRect(&rect1);
		rect2.right = rect1.right - rect2.left;
		rect2.right = max(rect2.right, rect2.left + 1);
		rect2.bottom = rect1.bottom - rect2.top;
		rect2.bottom = max(rect2.bottom, rect2.top + 1);
		m_toolSetListCtrl.MoveWindow(&rect2, true);
	}
}

void ToolSetView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// Only Single selection for List control
	m_toolSetListCtrl.SetSingleSelect();

	SVIPDoc* pCurrentDocument = GetIPDoc();

	if (nullptr != pCurrentDocument)
	{
		m_toolSetListCtrl.ShowWindow(SW_SHOW);
	}
}

int ToolSetView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (-1 == CFormView::OnCreate(lpCreateStruct)) { return -1; }

	return 0;
}

void ToolSetView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
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

				SVGUID selectedToolID = GetSelectedTool();
				m_toolSetListCtrl.setObjectIds(pCurrentDocument->GetToolSet()->GetUniqueObjectID(), pCurrentDocument->GetToolSet()->GetInspection()->GetUniqueObjectID());
				SetSelectedTool(selectedToolID);
			}
			else if (ExpandCollapseHint == lHint)
			{
				m_toolSetListCtrl.Rebuild();
			}
			else
			{
				m_toolSetListCtrl.RebuildImages();
			}
			// force the list control to position its scroll bars where they were before the rebuild.
			m_toolSetListCtrl.RestoreScrollPos();
		}
	}
}

void ToolSetView::OnEditToolName()
{
	CWaitCursor l_cwcMouse;

	if (!SvOi::isOkToEdit())
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

void ToolSetView::OnClickToolSetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	// Allow Expand/Collapse in run mode
	// Also clicking on the Start or End Group does a run once if allowed.

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

	if (!SvOi::isOkToEdit())
	{
		return;
	}
	CWaitCursor l_cwcMouse;

	// Run the toolset once to update the images/results.
	// 16 Dec 1999 - frb. (99)
	// Post a message to do the run once so that the double click still works
	PostMessage(WM_COMMAND, ID_RUN_ONCE);
}

void ToolSetView::OnRightClickToolSetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (!SvOi::isOkToEdit())
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
			POINT l_Point;
			BOOL l_bMenuLoaded = false;
			m_toolSetListCtrl.GetSelectedItemScreenPosition(l_Point);
			CMenu l_menu;
			CMenu* l_pPopup;
			bool bRemoveAdjustToolPos = false;

			// Get the tool comment...
			const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
			if (!rGuid .empty())
			{
				SVToolClass* pSelectedTool = dynamic_cast<SVToolClass *>(SVObjectManagerClass::Instance().GetObject(rGuid));
				if (nullptr != pSelectedTool)
				{
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
					//if the selected tool does not have an ROI remove the Adjust Tool Position menu item
					if ( !pSelectedTool->DoesObjectHaveExtents() )
					{
						bRemoveAdjustToolPos = true;
					}
				}
			}
			else
			{
				const ToolListSelectionInfo& info = GetToolListSelectionInfo();
				if (-1 != info.m_listIndex)
				{
					l_bMenuLoaded = l_menu.LoadMenu(IDR_TOOL_LIST_CONTEXT_MENU1);
					// Remove Tool Comment menu item if Group is Not Selected 
					CString name = m_toolSetListCtrl.GetItemText(info.m_listIndex, 0);
					if (m_toolSetListCtrl.IsEndListDelimiter(name) || m_toolSetListCtrl.IsEmptyStringPlaceHolder(name))
					{
						l_menu.RemoveMenu(ID_SELECTTOOL_TOOLCOMMENT, MF_BYCOMMAND);
					}
				}
			}

			if (bRemoveAdjustToolPos)
			{
				l_menu.RemoveMenu(ID_EDIT_ADJUSTTOOLPOSITION, MF_BYCOMMAND);
			}

			if (TRUE == l_bMenuLoaded)
			{
				if (l_pPopup = l_menu.GetSubMenu(0))
				{
					l_Point.y += 12;
					l_Point.x += 20;
					ClientToScreen(&l_Point);
					l_pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, l_Point.x, l_Point.y, AfxGetMainWnd());
				}
			}
			// Run the toolset once to update the images/results.
			// 16 Dec 1999 - frb. (99)
			pCurrentDocument->RunOnce();
		}
	}
	*pResult = 0;
}

void ToolSetView::OnDblClkToolSetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	if ( enterSelectedEntry() )
	{
		*pResult = 0;
	}
}

void ToolSetView::OnBeginLabelEditToolSetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);

	// Don't allow editing if running.
	if (!SvOi::isOkToEdit())
	{
		*pResult = true; // Abandon the label editing.
		return;
	}

	m_labelingIndex = m_toolSetListCtrl.GetNextItem(-1, LVNI_SELECTED);

	// Check for the last item which is the string of 'dashes'.
	if (-1 == m_labelingIndex || !m_toolSetListCtrl.AllowedToEdit())
	{
		*pResult = true; // Abandon the label editing.
		return;
	}

	// Save the current 'label' in case it needs to be restored.
	CEdit* pEdit = m_toolSetListCtrl.GetEditControl();
	ASSERT_VALID(pEdit);
	pEdit->GetWindowText(m_csLabelSaved);

	m_isLabeling = true;
	SetCapture(); // capture mouse events - 28 Oct 1999 - frb.

	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Validate label text and remove unwanted characters.
void ToolSetView::ValidateLabelText(CString& newText)
{
	// strip leading and trailing spaces
	newText.Trim();

	SVString newName( newText );

	SvUl_SF::RemoveCharacters( newName, SvO::SVEXCLUDECHARS_TOOL_IP_NAME );

	if( 0 < newName.size() )
	{
		newText = newName.c_str();
	}
	else
	{
		ASSERT(m_csLabelSaved.GetLength() > 0);
		newText = m_csLabelSaved;
	}
}

void ToolSetView::RenameItem(int item, const CString& oldName, const CString& newName)
{
	// check if it's a group or a tool
	if (m_labelingIndex >= 0 && m_labelingIndex < m_toolSetListCtrl.GetItemCount())
	{
		CString name = m_toolSetListCtrl.GetItemText(m_labelingIndex, 0);
		if (!m_toolSetListCtrl.IsEndListDelimiter(name) && !m_toolSetListCtrl.IsEmptyStringPlaceHolder(name))
		{
			SVGUID toolId = m_toolSetListCtrl.getToolGuid(m_labelingIndex);
			if (SVInvalidGUID != toolId) // it's a Tool
			{
				TheSVObserverApp.RenameObject(SVString(m_csLabelSaved), SVString(m_csLabelEdited), toolId);
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

void ToolSetView::OnEditLabelEnds()
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
		SetFocus(); // Cause edit control to lose 'focus'.
	}

	m_isLabeling = false;
	::ReleaseCapture(); // release the mouse capture
}

bool ToolSetView::EditToolGroupingComment()
{
	bool bRetVal  = false;
	const ToolListSelectionInfo& info = m_toolSetListCtrl.GetToolListSelectionInfo();
	if (!m_toolSetListCtrl.IsEndListDelimiter(info.m_selection) && 
		!m_toolSetListCtrl.IsEmptyStringPlaceHolder(info.m_selection))
	{
		SVIPDoc* pDoc = GetIPDoc();
		if (pDoc)
		{
			SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
			if (rGroupings.IsStartTag(info.m_selection.GetString()) || rGroupings.IsEndTag(info.m_selection.GetString()))
			{
				bRetVal = true;
				SvOg::SVTextEditDialog dlg( rGroupings.GetComment(info.m_selection.GetString()).c_str() );
				INT_PTR rc = dlg.DoModal();
				if (IDOK == rc)
				{
					rGroupings.SetComment(info.m_selection.GetString(), dlg.getText().GetString());
				}
			}
		}
	}
	return bRetVal;
}

void ToolSetView::OnSelectComment()
{
	const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
	if (rGuid.empty())
	{
		EditToolGroupingComment();
	}
	else
	{
		OnSelectToolComment();
	}
}

void ToolSetView::OnSelectToolComment()
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
						SvOg::SVTextEditDialog Dlg( csToolComment );
						INT_PTR Result = Dlg.DoModal();
						if( IDOK == Result )
						{
							// Set 
							SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
							pToolSet->GetInspection()->AddInputRequest(pSelectedTool->GetToolComment(), static_cast<LPCTSTR>( Dlg.getText() ));
							pToolSet->GetInspection()->AddInputRequestMarker();

							pCurrentDocument->RunOnce(pSelectedTool);
						}
					}
				}
			}
		}
	}
}

void ToolSetView::OnSelectToolSetReference()
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

void ToolSetView::OnSelectToolNormalize()
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

void ToolSetView::OnRunOnce()
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

bool ToolSetView::ShowDuplicateNameMessage(const CString& rName) const
{
	CString msg("A duplicate name was found for the item being renamed\n");
	msg += rName;
	SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
	INT_PTR rc = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, msg, StdMessageParams, SvOi::Err_10221, NULL, nullptr, MB_RETRYCANCEL );
	return (IDRETRY == rc);
}

void ToolSetView::OnEndLabelEditToolSetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);

	SVIPDoc* pCurrentDocument = GetIPDoc();

	if (nullptr != pCurrentDocument)
	{
		m_isLabeling = false;
		const SVGUID& rGuid = GetSelectedTool();
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
						bool bNameOk = CheckName(newText, m_csLabelSaved.GetString());
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
					if (!CheckName(newText, m_csLabelSaved.GetString()))
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

void ToolSetView::OnLButtonDown(UINT nFlags, CPoint point)
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

void ToolSetView::SetViewSize(CSize &Size)
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

BOOL ToolSetView::SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	BOOL bOk = false;

	CSize l_Size;
	_variant_t svVariant;

	bOk = SVNavigateTree::GetItem(rTree, CTAG_CELL_HEIGHT, htiParent, svVariant);
	if (bOk)
	{
		l_Size.cy = svVariant;
	}

	if (bOk)
	{
		bOk = SVNavigateTree::GetItem(rTree, CTAG_CELL_WIDTH, htiParent, svVariant);
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

BOOL ToolSetView::CheckParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	BOOL bOk = false;
	CSize l_Size;
	_variant_t svVariant;

	CRect l_cRect;
	GetWindowRect(l_cRect);

	bOk = SVNavigateTree::GetItem(rTree, CTAG_CELL_HEIGHT, htiParent, svVariant);
	if (bOk)
	{
		l_Size.cy = svVariant;
	}

	if (bOk)
	{
		bOk = SVNavigateTree::GetItem(rTree, CTAG_CELL_WIDTH, htiParent, svVariant);
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

SVIPDoc* ToolSetView::GetIPDoc() const
{
	return dynamic_cast<SVIPDoc*>(GetDocument());
}

BOOL ToolSetView::GetParameters(SVObjectWriter& rWriter)
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

const SVToolGrouping& ToolSetView::GetToolGroupings() const
{
	return GetIPDoc()->GetToolGroupings();
}

void ToolSetView::OnSetFocus(CWnd* pOldWnd)
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

void ToolSetView::HandleExpandCollapse(const CString& name, bool bCollapse)
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

void ToolSetView::ToggleExpandCollapse(int item)
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

bool ToolSetView::IsEndToolGroupAllowed() const
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

bool ToolSetView::enterSelectedEntry()
{
	if ( TheSVObserverApp.OkToEdit() && !IsLabelEditing() )
	{
	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr != pCurrentDocument)
	{
		const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
		if (!rGuid.empty())
		{
			pCurrentDocument->OnEditTool();
			SetSelectedTool(rGuid);
		}
		else
		{
			// check if tool group is selected
			if (!EditToolGroupingComment())
			{
				// Deselect all...
				m_toolSetListCtrl.SetSelectedTool(SVGUID());
			}
		}
	}
		return true;
	}
	else
	{
		return false;
	}
}

bool ToolSetView::CheckName(const CString& name , LPCTSTR pExclude) const
{
	bool bNameOk = true;
	SVIPDoc* pDoc = GetIPDoc();
	if (nullptr != pDoc)
	{
		const SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
		bNameOk = rGroupings.IsNameUnique(static_cast<LPCTSTR>(name),pExclude );
	}
	return bNameOk;
}

