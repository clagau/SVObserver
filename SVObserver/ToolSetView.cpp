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
#include "Tools/SVTool.h"
#include "SVIODoc.h"
#include "SVMainFrm.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVConfigurationObject.h"
#include "SVMessage/SVMessage.h"
#include "SVIPChildFrm.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVOGui/SVTextEditDialog.h"
#include "Tools/LoopTool.h"
#include "Tools/SVShiftTool.h"
#include "SVShiftToolUtility.h"
#include "Definitions/GlobalConst.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#include "SVInspectionProcess.h"
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

IMPLEMENT_DYNCREATE(ToolSetView, CFormView)
#pragma endregion Declarations

#pragma region Constructor
ToolSetView::ToolSetView()
	: CFormView(ToolSetView::IDD)
	, m_isLabeling(false)
	, m_labelingIndex(-1)
	, m_showDuplicateNameMessage(false)
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


PtrNavigatorElement ToolSetView::GetSelectedNavigatorElement(int* ptrSelectedIndex) const
{
	return m_toolSetListCtrl.GetSelectedNavigatorElement(ptrSelectedIndex);
}
PtrNavigatorElement ToolSetView::GetNavigatorElement(int item) const
{
	return m_toolSetListCtrl.GetNavigatorElement(item);
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

	if (::IsWindow(m_hWnd) && ::IsWindow(m_toolSetListCtrl.m_hWnd))
	{
		RECT rect1;
		RECT rect2;

		m_toolSetListCtrl.GetWindowRect(&rect2);
		ScreenToClient(&rect2);
		GetClientRect(&rect1);
		rect2.right = rect1.right - rect2.left;
		rect2.right = std::max(rect2.right, rect2.left + 1);
		rect2.bottom = rect1.bottom - rect2.top;
		rect2.bottom = std::max(rect2.bottom, rect2.top + 1);
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
			if (SVIPDoc::RefreshDelete == lHint)
			{
				int index = getListCtrl().GetNextItem(-1, LVNI_SELECTED);
				index--;
				getListCtrl().Rebuild();
				// Select previous..
				if (index >= 0 && index < getListCtrl().GetItemCount())
				{

					getListCtrl().SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
					getListCtrl().EnsureVisible(index, true);
				}
			}
			else if ((SVIPDoc::RefreshView == lHint) || (S_OK == pCurrentDocument->IsToolSetListUpdated()))
			{
			

				SVGUID selectedToolID = GetSelectedTool();
				m_toolSetListCtrl.setObjectIds(pCurrentDocument->GetToolSet()->GetUniqueObjectID(), pCurrentDocument->GetToolSet()->GetInspection()->GetUniqueObjectID());
				SetSelectedTool(selectedToolID);
				m_toolSetListCtrl.RebuildImages();
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
	if (nullptr == pCurrentDocument)
	{
		return;
	}
	SVToolSetClass* pToolSet = pCurrentDocument->GetToolSet();
	if (nullptr == pToolSet)
	{
		return;
	}

	bool hasComment(true);
	int SelectedListIndex(-1);
	auto NavElement = GetSelectedNavigatorElement(&SelectedListIndex);
	if (!NavElement)
	{
		return;
	}
	std::string Selection(NavElement->m_DisplayName);
	switch (NavElement->m_Type)
	{

		case NavElementType::StartGrouping:
			break;
		case NavElementType::EndGrouping:
			break;
		case NavElementType::LoopTool:
			break;
		case NavElementType::Tool:
			break;
		case NavElementType::SubTool:
			break;
		case NavElementType::EndDelimiterToolSet:
		case NavElementType::EndDelimiterLoopTool:
		case NavElementType::Empty:
			Selection.clear();
			hasComment = false;
			break;
	}
	SVGUID  SelectedGuid(NavElement->m_Guid);
	POINT l_Point;
	BOOL l_bMenuLoaded = false;
	m_toolSetListCtrl.GetSelectedItemScreenPosition(l_Point);
	CMenu l_menu;
	bool bRemoveAdjustToolPos = false;

	// Get the tool comment...
	if (!SelectedGuid.empty())
	{
		SvTo::SVToolClass* pSelectedTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(SelectedGuid));
		if (nullptr != pSelectedTool)
		{
			SvTo::SVShiftTool* pShiftTool = dynamic_cast<SvTo::SVShiftTool*> (pSelectedTool);
			if (nullptr != pShiftTool)
			{
				long l_shiftMode;
				pShiftTool->m_evoShiftMode.GetValue(l_shiftMode);
				if (l_shiftMode == SvTo::SV_SHIFT_ENUM::SV_SHIFT_REFERENCE)
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
			if (!pSelectedTool->DoesObjectHaveExtents())
			{
				bRemoveAdjustToolPos = true;
			}
		}
	}
	else
	{
		if (-1 != SelectedListIndex)
		{
			l_bMenuLoaded = l_menu.LoadMenu(IDR_TOOL_LIST_CONTEXT_MENU1);
			// Remove Tool Comment menu item if Group is Not Selected 
			if (hasComment == false)
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
		if (CMenu* l_pPopup = l_menu.GetSubMenu(0))
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
	*pResult = 0;
}

void ToolSetView::OnDblClkToolSetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	//check if on black icon clicked and if display the error message instead of TA-dialog
	const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
	if (!rGuid.empty())
	{
		NMITEMACTIVATE* pActivate = reinterpret_cast<NMITEMACTIVATE *>(pNMHDR);
		UINT uFlags;
		/*int item =*/ m_toolSetListCtrl.HitTest(pActivate->ptAction, &uFlags);
		if ((uFlags & LVHT_ONITEMICON))
		{
			//display error box and return else go further with TA dialog.
			if (m_toolSetListCtrl.displayErrorBox(rGuid))
			{
				*pResult = 0;
				return;
			}
		}
	}

	if (enterSelectedEntry())
	{
		*pResult = 0;
	}
}

void ToolSetView::OnBeginLabelEditToolSetList(NMHDR* pNMHDR, LRESULT* pResult)
{
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

	if (m_LabelSaved.empty())
	{
		// Save the current 'label' in case it needs to be restored.
		CEdit* pEdit = m_toolSetListCtrl.GetEditControl();
		ASSERT_VALID(pEdit);
		CString Text;
		pEdit->GetWindowText(Text);
		m_LabelSaved = Text;
		m_isLabeling = true;

		SetCapture();
	}

	*pResult = 0;
}

// Validate label text and remove unwanted characters.
void ToolSetView::ValidateLabelText(std::string& rNewText)
{
	// strip leading and trailing spaces
	SvUl::Trim(rNewText);

	SvUl::RemoveCharacters(rNewText, SvDef::cExcludeCharsToolIpName);

	if (rNewText.empty())
	{
		assert(m_LabelSaved.size() > 0);
		rNewText = m_LabelSaved;
	}
}
void ToolSetView::RenameItem()
{
	if (m_LabelEdited == m_LabelSaved)
		return;
	auto NavElement = GetNavigatorElement(m_labelingIndex);

	if (!NavElement)
	{
		return;
	}
	SVGUID toolId(NavElement->m_Guid);
	bool renameGrouping(false);
	switch (NavElement->m_Type)
	{

		case NavElementType::SubTool:
		case NavElementType::SubLoopTool:
			if (GUID_NULL != toolId) // it's a Tool
			{
				TheSVObserverApp.OnObjectRenamed(m_LabelSaved, toolId);
				renameGrouping = false;
			}
			break;
		case NavElementType::LoopTool:
		case NavElementType::Tool:
			if (GUID_NULL != toolId) // it's a Tool
			{
				TheSVObserverApp.OnObjectRenamed(m_LabelSaved, toolId);
				renameGrouping = true;
			}
			break;
		case NavElementType::EndGrouping:
		case NavElementType::StartGrouping:
			renameGrouping = true;
			break;
	}
	if (renameGrouping)
	{
		SVIPDoc* pDoc = GetIPDoc();
		if (pDoc)
		{
			SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
			rGroupings.RenameItem(m_LabelSaved, m_LabelEdited);
		}
	}

}

void ToolSetView::OnEditLabelEnds()
{
	// Update the text in the list control item.
	if (m_labelingIndex >= 0 && m_labelingIndex < m_toolSetListCtrl.GetItemCount())
	{
		BOOL bResult = m_toolSetListCtrl.SetItemText(m_labelingIndex, 0, m_LabelEdited.c_str());
		ASSERT(bResult);

		// Cause a redraw.
		m_toolSetListCtrl.Update(m_labelingIndex);

		RenameItem();
	}
	if (m_isLabeling)
	{
		// Cause edit control to 'end' editing.
		SetFocus(); // Cause edit control to lose 'focus'.
	}
	m_isLabeling = false;
	::ReleaseCapture();
	if (m_showDuplicateNameMessage)
	{
		m_showDuplicateNameMessage = false;
		bool bDoReEdit = ShowDuplicateNameMessage(m_duplicateName);
		if (bDoReEdit)
		{
			PostMessage(WM_COMMAND, ID_EDIT_NAME);
		}
	}
	m_duplicateName.clear();
	m_LabelSaved.clear();
}

bool ToolSetView::EditToolGroupingComment(const std::string& groupingName)
{
	bool bRetVal = false;
	SVIPDoc* pDoc = GetIPDoc();
	if (!pDoc)
	{
		return false;
	}
	SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
	if (rGroupings.IsStartTag(groupingName) || rGroupings.IsEndTag(groupingName))
	{
		bRetVal = true;
		SvOg::SVTextEditDialog dlg(rGroupings.GetComment(groupingName).c_str());
		INT_PTR rc = dlg.DoModal();
		if (IDOK == rc)
		{
			rGroupings.SetComment(groupingName, dlg.getText().GetString());
		}
	}
	return bRetVal;
}

bool ToolSetView::IsLoopToolSelected() const
{
	int selectedItem(-1);
	auto NavElement = GetSelectedNavigatorElement(&selectedItem);

	if (!NavElement)
	{
		return false;
	}
	switch (NavElement->m_Type)
	{
		case NavElementType::SubTool:
		case NavElementType::SubLoopTool:
		case NavElementType::EndDelimiterLoopTool:
			return true;
	}
	return false;
}


void ToolSetView::OnSelectComment()
{

	int selectedItem(-1);
	auto NavElement = GetSelectedNavigatorElement(&selectedItem);

	if (!NavElement)
	{
		return;
	}
	SVGUID toolId(NavElement->m_Guid);
	switch (NavElement->m_Type)
	{
		case NavElementType::SubLoopTool:
		case NavElementType::SubTool:
		case NavElementType::LoopTool:
		case NavElementType::Tool:
			EditToolComment(toolId);
			break;
		case  NavElementType::EndGrouping:
		case NavElementType::StartGrouping:
			EditToolGroupingComment(NavElement->m_DisplayName);
			break;
	}

}

void ToolSetView::EditToolComment(SVGUID& rToolGuid)
{
	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr == pCurrentDocument)
	{
		return;
	}
	SVToolSetClass* pToolSet = pCurrentDocument->GetToolSet();
	if (nullptr == pToolSet)
	{
		return;
	}
	if (!rToolGuid.empty())
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pToolSet->GetInspection());

		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
		Controller Values {SvOg::BoundValues{ pInspection->GetUniqueObjectID(), rToolGuid }};
		Values.Init();

		// Get the tool comment...
		std::string ToolComment = Values.Get<CString>(SVToolCommentTypeObjectGuid);
		SvOg::SVTextEditDialog Dlg(ToolComment.c_str());
		if (IDOK == Dlg.DoModal())
		{
			Values.Set<CString>(SVToolCommentTypeObjectGuid, Dlg.getText());
			SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
			Values.Commit();
		}
	}

}

void ToolSetView::OnSelectToolSetReference()
{
	const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass *>(SVObjectManagerClass::Instance().GetObject(rGuid));

	if (nullptr != pTool)
	{
		SvTo::SVShiftTool* pShiftTool = dynamic_cast<SvTo::SVShiftTool *>(pTool);

		if (nullptr != pShiftTool)
		{
			SVShiftToolUtility::SetToolSetReference(pShiftTool);
		}
	}
}

void ToolSetView::OnSelectToolNormalize()
{
	const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
	if (!rGuid.empty())
	{
		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass *>(SVObjectManagerClass::Instance().GetObject(rGuid));
		if (nullptr != pTool)
		{
			SvTo::SVShiftTool* pShiftTool = dynamic_cast<SvTo::SVShiftTool *>(pTool);

			if (nullptr != pShiftTool)
			{
				SVShiftToolUtility::SetToolNormalize(pShiftTool);
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

bool ToolSetView::ShowDuplicateNameMessage(const std::string& rName) const
{
	SvDef::StringVector msgList;
	msgList.push_back(rName);
	SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
	INT_PTR res = Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RenameError_DuplicateName, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10221, GUID_NULL, MB_RETRYCANCEL);
	return (IDRETRY == res);
}

void ToolSetView::OnEndLabelEditToolSetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_showDuplicateNameMessage = false;
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);

	m_isLabeling = false;
	*pResult = 0;

	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr == pCurrentDocument)
	{
		return;
	}
	SVToolSetClass* pToolSet = pCurrentDocument->GetToolSet();
	if (nullptr == pToolSet)
	{
		return;
	}
	CEdit* pEdit = m_toolSetListCtrl.GetEditControl();
	if (nullptr == pEdit)
	{
		return;
	}
	int SelectedListIndex(-1);
	auto NavElement = GetSelectedNavigatorElement(&SelectedListIndex);
	if (!NavElement)
	{
		return;
	}
	std::string Selection(NavElement->m_DisplayName);
	SVGUID  SelectedTool(NavElement->m_Guid);
	SVGUID  SelectedLoopTool(NavElement->m_OwnerGuid);
	bool RenameSubTool(false);
	CString Text;
	pEdit->GetWindowText(Text);
	std::string NewText(Text);


	// Validate the text and remove unwanted characters.
	ValidateLabelText(NewText);
	if (m_LabelSaved != NewText) // In case it was renamed to the same name as before renaming
	{
		SvTo::LoopTool* pLoopTool(nullptr);
		SvTo::SVToolClass* pTool(nullptr);
		bool IsNameOk(false);
		bool IsSubTool(false);
		switch (NavElement->m_Type)
		{
			case NavElementType::StartGrouping:
			case NavElementType::EndGrouping:
			{
				if (!CheckName(NewText, m_LabelSaved.c_str()))
				{
					m_duplicateName = NewText;
					m_showDuplicateNameMessage = true;
					NewText = m_LabelSaved; // reset it back to original
				}
				break;
			}
			case NavElementType::SubTool:
			case NavElementType::SubLoopTool:
				IsSubTool = true;
				pLoopTool = dynamic_cast<SvTo::LoopTool*> (SVObjectManagerClass::Instance().GetObject(SelectedLoopTool));
			case NavElementType::LoopTool:
			case NavElementType::Tool:
				pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(SelectedTool));
				if (nullptr == pTool)
				{
					return;
				}

				if (IsSubTool && pLoopTool)
				{
					IsNameOk = pLoopTool->IsNameUnique(NewText.c_str(), m_LabelSaved.c_str());
				}
				else if (!IsSubTool)
				{
					IsNameOk = CheckName(NewText, m_LabelSaved.c_str());
				}
				if (IsNameOk)
				{
					pTool->SetName(NewText.c_str());
				}
				else
				{
					m_duplicateName = NewText;
					m_showDuplicateNameMessage = true;
					NewText = m_LabelSaved; // reset it back to original
				}
				break;
			default:
				return;
		}


	}

	// Save the edited and validated label for the callback message handler.
	m_LabelEdited = NewText;

	// Post call back message for OnEditLabelEnds();
	PostMessage(WM_COMMAND, ID_EDIT_LABEL_ENDS);

	//m_labelingIndex = -1;
	pCurrentDocument->SetModifiedFlag();
	pCurrentDocument->UpdateAllViews(nullptr);

	// Update result io view, too...
	SVIODoc* pIODoc = TheSVObserverApp.GetIODoc();
	if (nullptr != pIODoc)
	{
		pIODoc->UpdateAllViews(nullptr);
	}
	*pResult = true;

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
	} while (nullptr == l_pSplitterFrame && nullptr != l_pcWnd);

	if (nullptr != l_pSplitterFrame)
	{
		l_pSplitterFrame->SetViewSize(this, Size);
	}
}

bool ToolSetView::SetParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	CSize l_Size;
	_variant_t svVariant;
	bool bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CELL_HEIGHT, htiParent, svVariant);
	if (bOk)
	{
		l_Size.cy = svVariant;
	}

	if (bOk)
	{
		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CELL_WIDTH, htiParent, svVariant);
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

bool ToolSetView::CheckParameters(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	CSize l_Size;
	_variant_t svVariant;
	CRect l_cRect;
	GetWindowRect(l_cRect);

	bool bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CELL_HEIGHT, htiParent, svVariant);
	if (bOk)
	{
		l_Size.cy = svVariant;
	}

	if (bOk)
	{
		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CELL_WIDTH, htiParent, svVariant);
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

void ToolSetView::GetParameters(SvOi::IObjectWriter& rWriter)
{
	CRect l_cRect;

	GetWindowRect(l_cRect);

	_variant_t svVariant;

	svVariant = l_cRect.Height();
	rWriter.WriteAttribute(SvXml::CTAG_CELL_HEIGHT, svVariant);
	svVariant.Clear();

	svVariant = l_cRect.Width();
	rWriter.WriteAttribute(SvXml::CTAG_CELL_WIDTH, svVariant);
	svVariant.Clear();
}

SVToolGrouping& ToolSetView::GetToolGroupings() 
{
	return GetIPDoc()->GetToolGroupings();
}

void ToolSetView::OnSetFocus(CWnd* pOldWnd)
{
	CFormView::OnSetFocus(pOldWnd);

	SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>(AfxGetMainWnd());

	if (nullptr != pFrame)
	{
		SVIPSplitterFrame* pSplitterFrame = dynamic_cast<SVIPSplitterFrame*>(pFrame->GetActiveFrame());

		if (nullptr != pSplitterFrame)
		{
			pSplitterFrame->RefreshAllSplitters();
		}
	}
}

void ToolSetView::HandleExpandCollapse(const std::string& rName, bool bCollapse)
{
	SVIPDoc* pDoc = GetIPDoc();
	if (nullptr != pDoc)
	{
		SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
		bool bState = rGroupings.IsCollapsed(rName.c_str());
		if (bState != bCollapse)
		{
			rGroupings.Collapse(rName.c_str(), bCollapse);
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
		std::string Name = m_toolSetListCtrl.GetItemText(item, 0);

		bool bState = rGroupings.IsCollapsed(Name.c_str());
		if (rGroupings.Collapse(Name.c_str(), bState ? false : true))
		{
			OnUpdate(this, ExpandCollapseHint, nullptr);
		}
	}
}

bool ToolSetView::IsEndToolGroupAllowed() const
{
	bool bRetVal = false;
	SVIPDoc* pDoc = GetIPDoc();
	if (nullptr == pDoc)
	{
		return false;
	}
	int selectedItem(-1);
	auto NavElement = GetSelectedNavigatorElement(&selectedItem);

	if (!NavElement)
	{
		return false;
	}
	std::string ItemName(NavElement->m_DisplayName);
	switch (NavElement->m_Type)
	{
		case NavElementType::SubTool:
		case NavElementType::SubLoopTool:
		case NavElementType::EndDelimiterLoopTool:
			return false;
			break;
		case NavElementType::EndDelimiterToolSet:
			ItemName.clear();
			break;
		default:
			break;

	}

	const SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
	const std::string& rName = rGroupings.FindCandidateStartGroup(ItemName);
	if (!rName.empty())
	{
		bRetVal = true;
	}

	return bRetVal;
}

bool ToolSetView::enterSelectedEntry()
{
	if (!TheSVObserverApp.OkToEdit() || IsLabelEditing())
	{
		return false;
	}
	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr == pCurrentDocument)
	{
		return false;
	}

	int selectedItem(-1);
	auto NavElement = GetSelectedNavigatorElement(&selectedItem);
	if (!NavElement)
	{
		return false;
	}
	SVGUID toolId(NavElement->m_Guid);

	switch (NavElement->m_Type)
	{

		case NavElementType::SubLoopTool:
		case NavElementType::SubTool:
		case NavElementType::LoopTool:
		case NavElementType::Tool:
			if (!toolId.empty())
			{
				pCurrentDocument->OnEditTool();
				SetSelectedTool(toolId);
			}
			break;
		case  NavElementType::EndGrouping:
		case NavElementType::StartGrouping:
			EditToolGroupingComment(NavElement->m_DisplayName);
			break;
		default:
			// Deselect all...
			m_toolSetListCtrl.SetSelectedTool(SVGUID());
			break;
	}
	return true;
}

bool ToolSetView::hasCurrentToolErrors()
{
	const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
	if (!rGuid.empty())
	{
		return !m_toolSetListCtrl.isToolValid(rGuid);
	}
	return false;
}

void ToolSetView::displayFirstCurrentToolError()
{
	const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
	if (!rGuid.empty())
	{
		m_toolSetListCtrl.displayErrorBox(rGuid);
	}
}

bool ToolSetView::isAddParameter2MonitorListPossible(LPCTSTR ppqName) const
{
	const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && !rGuid.empty())
	{
		return pConfig->isAddParameter2MonitorListPossible(ppqName, rGuid);
	}
	return false;
}

bool ToolSetView::isRemoveParameter2MonitorListPossible(LPCTSTR ppqName) const
{
	const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && !rGuid.empty())
	{
		return pConfig->isRemoveParameter2MonitorListPossible(ppqName, rGuid);
	}
	return false;
}

bool ToolSetView::areParametersInMonitorList(LPCTSTR ppqName, const SVGUID& rToolId) const
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && !rToolId.empty())
	{
		return pConfig->areParametersInMonitorList(ppqName, rToolId);
	}
	return false;
}

void ToolSetView::addParameter2MonitorList(LPCTSTR ppqName)
{
	const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && !rGuid.empty())
	{
		SvStl::MessageContainerVector messages = pConfig->addParameter2MonitorList(ppqName, rGuid);
		if (messages.size())
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(messages[0].getMessage());
		}
	}
}

void ToolSetView::removeParameter2MonitorList(LPCTSTR ppqName)
{
	const SVGUID& rGuid = m_toolSetListCtrl.GetSelectedTool();
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && !rGuid.empty())
	{
		pConfig->removeParameter2MonitorList(ppqName, rGuid);
	}
}

bool ToolSetView::CheckName(const std::string& rName, LPCTSTR pExclude) const
{
	bool bNameOk = true;
	SVIPDoc* pDoc = GetIPDoc();
	if (nullptr != pDoc)
	{
		const SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
		bNameOk = rGroupings.IsNameUnique(rName, pExclude);
	}
	return bNameOk;
}

