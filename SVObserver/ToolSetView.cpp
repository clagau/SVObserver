//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVIPSplitterFrame.h"
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
#include "SVOGui/DataController.h"
#include "SVInspectionProcess.h"
#include "SVUtilityLibrary/SVClock.h"
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

namespace
{
std::pair<bool, bool> determineRequiredMenuModifications(uint32_t navigatorObjectID)
{
	bool isShiftToolWithReference = false;
	bool hasRoi = true;

	if (navigatorObjectID > SvDef::InvalidObjectId)
	{
		SvTo::SVToolClass* pSelectedTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(navigatorObjectID));
		if (nullptr != pSelectedTool)
		{
			SvTo::SVShiftTool* pShiftTool = dynamic_cast<SvTo::SVShiftTool*> (pSelectedTool);
			if (nullptr != pShiftTool)
			{
				long shiftMode;
				pShiftTool->m_evoShiftMode.GetValue(shiftMode);
				if (shiftMode == SvTo::SV_SHIFT_ENUM::SV_SHIFT_REFERENCE)
				{
					isShiftToolWithReference = true;
				}
			}
			if (!pSelectedTool->DoesObjectHaveExtents() || SvPb::TransformationToolClassId == pSelectedTool->GetClassID())
			{
				hasRoi = false;
			}
		}
	}

	return {isShiftToolWithReference, hasRoi};
}
}


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

void ToolSetView::SetSelectedToolId(uint32_t toolId)
{
	m_toolSetListCtrl.SetSelectedToolId(toolId);
}

std::vector<uint32_t> ToolSetView::GetAllSelectedToolIds() const
{
	return m_toolSetListCtrl.GetAllSelectedToolIds();
}


NavigatorIndexAndToolId ToolSetView::Get1stSelIndexAndId() const
{
	NavigatorIndexAndToolId niati = m_toolSetListCtrl.Get1stSelIndexAndId();

#if defined (TRACE_THEM_ALL) || defined (TRACE_TOOLSET)
	uint32_t id = niati.second;
	static uint32_t s_lastId = 0;
	if (s_lastId != id)
	{
		s_lastId = id;
		std::string outputText {_T("ToolSetView: selected: ")};
		outputText += std::to_string(id) + '\n';
		::OutputDebugString(outputText.c_str());
	}
#endif

	return niati;
}

NavigatorIndexAndElement ToolSetView::Get1stSelIndexAndElement() const
{
	auto niaev = m_toolSetListCtrl.GetSelectedNavigatorIndexAndElementVector();

	if (niaev.empty())
	{
		return {-1, nullptr};
	}
	
	return niaev[0];
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

	SVIPDoc* pCurrentDocument = GetIPDoc();

	if (nullptr != pCurrentDocument)
	{
		m_toolSetListCtrl.ShowWindow(SW_SHOW);
	}

	m_toolSetListCtrl.SetMultipleSelect(); // SVO-1213: multiple selection for List control allowed
}

int ToolSetView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (-1 == CFormView::OnCreate(lpCreateStruct)) { return -1; }

	return 0;
}

bool ToolSetView::ToolSetListHasChanged()
{
	
	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (!pCurrentDocument)
	{
		return false;
	}

	SVToolSet* pToolSet = pCurrentDocument ->GetToolSet();
	if (!pToolSet)
	{
		return false;
	}
	if (pToolSet->GetLastListUpdateTimestamp() > GetTimeStamp())
	{
		return true;
	}	

	
	int toolgroupSize= GetToolGroupings().GetNumberOfVisibleItems();
	int listctrlSize = static_cast<int> (m_toolSetListCtrl.GetItemCount());

	if (toolgroupSize >  listctrlSize - 1)
	{
		
		//@TODO[MEC][10.10][13.10.2020] only the case of additonal entries in Toolgroupinglist  will be cured by a rebuild.
		//@TODO[MEC][10.10][13.10.2020]  number of looptool subtools is not in toolgroupsize  therfore we check only if Toolgroupsize is to big
		ASSERT((toolgroupSize >  listctrlSize - 1));
		return true;
	}

	return false;
}
void ToolSetView::OnUpdate(CView* , LPARAM lHint, CObject* )
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
				getListCtrl().Rebuild(true);
				// Select previous..
				if (index >= 0 && index < getListCtrl().GetItemCount())
				{

					getListCtrl().SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
					getListCtrl().EnsureVisible(index, true);
				}
			}
			else if (ExpandCollapseHint == lHint)
			{
				m_toolSetListCtrl.Rebuild(false);
			}
			else if ((SVIPDoc::RefreshView == lHint) || ToolSetListHasChanged())
			{
				auto [selectedPos, selectedToolID] = Get1stSelIndexAndId();

				if (pCurrentDocument->GetToolSet() == 0 || pCurrentDocument->GetToolSet()->GetInspection() == 0)
				{
					return;
				}
				m_toolSetListCtrl.setObjectIds(pCurrentDocument->GetToolSet()->getObjectId(), pCurrentDocument->GetToolSet()->GetInspection()->getObjectId(), SVIPDoc::RefreshView == lHint);

				SetSelectedToolId(selectedToolID);

				if (selectedToolID == 0 && selectedPos >-1 && m_toolSetListCtrl.GetItemCount() > selectedPos)
				{
					//no tool selected but grouping or end of looptool
					m_toolSetListCtrl.SetItemState(selectedPos, LVIS_SELECTED, LVIS_SELECTED);
				}

				m_toolSetListCtrl.RebuildImages();
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

	m_toolSetListCtrl.DeselectContentOfSelectedItems();

	// Run the toolset once to update the images/results.
	// 16 Dec 1999 - frb. (99)
	// Post a message to do the run once so that the double click still works
	PostMessage(WM_COMMAND, ID_RUN_ONCE);
}

void ToolSetView::OnRightClickToolSetList(NMHDR* , LRESULT* pResult)
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
	SVToolSet* pToolSet = pCurrentDocument->GetToolSet();
	if (nullptr == pToolSet)
	{
		return;
	}

	NavigatorIndexAndElementVector niaev = m_toolSetListCtrl.GetSelectedNavigatorIndexAndElementVector();

	if (true == niaev.empty())
	{
		return;
	}

	loadAndAdaptMenu(niaev);

	// Run the toolset once to update the images/results.
	// 16 Dec 1999 - frb. (99)
	pCurrentDocument->RunOnce();
	*pResult = 0;
}


void ToolSetView::loadAndAdaptMenu(const NavigatorIndexAndElementVector& rNiaev)
{
	BOOL bMenuLoaded = false;
	CMenu menu;

	if (std::none_of(rNiaev.cbegin(), rNiaev.cend(), [](auto& rObject){return rObject.second->m_navigatorObjectId < 1; }))
	{	//i.e. only "proper" tools are selected
		switch (rNiaev.size()) //number of selected ToolSetViewEntries
		{
			case 0:
				bMenuLoaded = menu.LoadMenu(IDR_TOOL_LIST_CONTEXT_MENU_EMPTY);
			case 1:
			{
				auto [isShiftToolWithReference, hasRoi] = determineRequiredMenuModifications(rNiaev[0].second->m_navigatorObjectId);

				bMenuLoaded = menu.LoadMenu(isShiftToolWithReference ? IDR_TOOL_LIST_CONTEXT_MENU_SHIFT : IDR_TOOL_LIST_CONTEXT_MENU);
				if (false == hasRoi)
				{
					menu.RemoveMenu(ID_EDIT_ADJUSTTOOLPOSITION, MF_BYCOMMAND); ///< if the selected tool does not have an ROI: remove the Adjust Tool Position menu item
				}
				break;
			}
			default:
				bMenuLoaded = menu.LoadMenu(IDR_TOOL_LIST_CONTEXT_MENU_MULTI);
		}
	}
	else
	{
		if (1 == rNiaev.size())
		{
			bMenuLoaded = menu.LoadMenu(IDR_TOOL_LIST_CONTEXT_MENU1);
			// Remove tool comment menu item unless group is selected 
			if (rNiaev[0].second->mayHaveComment() == false)
			{
				menu.RemoveMenu(ID_SELECTTOOL_TOOLCOMMENT, MF_BYCOMMAND);
			}
		}
		else
		{
			bMenuLoaded = menu.LoadMenu(IDR_TOOL_LIST_CONTEXT_MENU_EMPTY);
		}
	}

	if (FALSE == bMenuLoaded)
	{
		return;
	}

	POINT point;
	m_toolSetListCtrl.GetSelectedItemScreenPosition(point);
	if (CMenu* pPopup = menu.GetSubMenu(0))
	{
		point.y += 12;
		point.x += 20;
		ClientToScreen(&point);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
}

void ToolSetView::OnDblClkToolSetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	//check if on black icon clicked and if display the error message instead of TA-dialog
	uint32_t toolId = m_toolSetListCtrl.Get1stSelIndexAndId().second;
	if (SvDef::InvalidObjectId != toolId)
	{
		NMITEMACTIVATE* pActivate = reinterpret_cast<NMITEMACTIVATE *>(pNMHDR);
		UINT uFlags;
		/*int item =*/ m_toolSetListCtrl.HitTest(pActivate->ptAction, &uFlags);
		if ((uFlags & LVHT_ONITEMICON))
		{
			//display error box and return else go further with TA dialog.
			if (m_toolSetListCtrl.displayErrorBox(toolId))
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

void ToolSetView::OnBeginLabelEditToolSetList(NMHDR*, LRESULT* pResult)
{
	// Don't allow editing if running.
	if (!SvOi::isOkToEdit())
	{
		*pResult = true; // Abandon the label editing.
		return;
	}

	m_labelingIndex = m_toolSetListCtrl.GetNextItem(-1, LVNI_SELECTED);

	// Check for the last item which is the string of 'dashes'.
	if (-1 == m_labelingIndex || !m_toolSetListCtrl.mayBeEdited())
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
	rNewText = SvUl::Trim(rNewText.c_str());

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
	auto pNavElement = GetNavigatorElement(m_labelingIndex);

	if (!pNavElement)
	{
		return;
	}
	
	SVIPDoc* pDoc = GetIPDoc();
	if (!pDoc)
	{
		return;
		
	}
	SVToolGrouping& rGroupings = pDoc->GetToolGroupings();
	

	uint32_t toolId(pNavElement->m_navigatorObjectId);
	
	switch (pNavElement->m_Type)
	{

		case NavElementType::SubTool:
		case NavElementType::SubLoopTool:
			if (SvDef::InvalidObjectId != toolId) // it's a Tool
			{
				TheSVObserverApp().OnObjectRenamed(m_LabelSaved, toolId);
				pNavElement->m_DisplayName = m_LabelEdited;
			}
			break;
		case NavElementType::GroupTool:
		case NavElementType::LoopTool:
		case NavElementType::Tool:
			if (SvDef::InvalidObjectId != toolId) // it's a Tool
			{
				rGroupings.RenameItem(m_LabelSaved, m_LabelEdited);
				TheSVObserverApp().OnObjectRenamed(m_LabelSaved, toolId);
				pNavElement->m_DisplayName = m_LabelEdited;
				
			}
			break;
		case NavElementType::EndGrouping:
		case NavElementType::StartGrouping:
			rGroupings.RenameItem(m_LabelSaved, m_LabelEdited);
			pNavElement->m_DisplayName = m_LabelEdited;
			break;
	}
	

}

void ToolSetView::OnEditLabelEnds()
{
	// Update the text in the list control item.
	if (m_labelingIndex >= 0 && m_labelingIndex < m_toolSetListCtrl.GetItemCount())
	{
		BOOL bResult = m_toolSetListCtrl.SetItemText(m_labelingIndex, 0, m_LabelEdited.c_str());
		assert(bResult); UNREFERENCED_PARAMETER(bResult);

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

bool ToolSetView::IsSubToolSelected() const
{
	PtrNavigatorElement  pNavElement = Get1stSelIndexAndElement().second;

	if (!pNavElement)
	{
		return false;
	}
	switch (pNavElement->m_Type)
	{
		case NavElementType::SubTool:
		case NavElementType::SubLoopTool:
		case NavElementType::EndDelimiterTool:
			return true;
	}
	return false;
}


void ToolSetView::OnSelectComment()
{

	PtrNavigatorElement  pNavElement = Get1stSelIndexAndElement().second;

	if (!pNavElement)
	{
		return;
	}
	uint32_t toolId(pNavElement->m_navigatorObjectId);
	switch (pNavElement->m_Type)
	{
		case NavElementType::SubLoopTool:
		case NavElementType::SubTool:
		case NavElementType::GroupTool:
		case NavElementType::LoopTool:
		case NavElementType::Tool:
			EditToolComment(toolId);
			break;
		case NavElementType::EndGrouping:
		case NavElementType::StartGrouping:
			EditToolGroupingComment(pNavElement->m_DisplayName);
			break;
	}

}

void ToolSetView::EditToolComment(uint32_t toolId)
{
	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr == pCurrentDocument)
	{
		return;
	}
	SVToolSet* pToolSet = pCurrentDocument->GetToolSet();
	if (nullptr == pToolSet)
	{
		return;
	}
	if (SvDef::InvalidObjectId != toolId)
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pToolSet->GetInspection());

		SvOg::ValueController Values {SvOg::BoundValues{ pInspection->getObjectId(), toolId }};
		Values.Init();

		// Get the tool comment...
		std::string ToolComment = Values.Get<CString>(SvPb::ToolCommentTypeEId).GetString();
		SvOg::SVTextEditDialog Dlg(ToolComment.c_str());
		if (IDOK == Dlg.DoModal())
		{
			Values.Set<CString>(SvPb::ToolCommentTypeEId, Dlg.getText());
			SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
			Values.Commit();
		}
	}

}

void ToolSetView::OnSelectToolSetReference()
{
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass *>(SVObjectManagerClass::Instance().GetObject(m_toolSetListCtrl.Get1stSelIndexAndId().second));

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
	uint32_t toolId = m_toolSetListCtrl.Get1stSelIndexAndId().second;
	if (SvDef::InvalidObjectId != toolId)
	{
		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass *>(SVObjectManagerClass::Instance().GetObject(toolId));
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
	SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
	INT_PTR res = Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RenameError_DuplicateName, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10221, SvDef::InvalidObjectId, MB_RETRYCANCEL);
	return (IDRETRY == res);
}

void ToolSetView::OnEndLabelEditToolSetList(NMHDR*, LRESULT* pResult)
{
	m_showDuplicateNameMessage = false;
	m_isLabeling = false;
	*pResult = 0;

	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr == pCurrentDocument)
	{
		return;
	}
	SVToolSet* pToolSet = pCurrentDocument->GetToolSet();
	if (nullptr == pToolSet)
	{
		return;
	}
	CEdit* pEdit = m_toolSetListCtrl.GetEditControl();
	if (nullptr == pEdit)
	{
		return;
	}

	PtrNavigatorElement  pNavElement = Get1stSelIndexAndElement().second;

	if (!pNavElement)
	{
		return;
	}

	CString Text;
	pEdit->GetWindowText(Text);
	std::string NewText(Text);


	// Validate the text and remove unwanted characters.
	ValidateLabelText(NewText);
	if (m_LabelSaved != NewText) // In case it was renamed to the same name as before renaming
	{
		SvTo::SVToolClass* pLoopGroupTool(nullptr);
		SvTo::SVToolClass* pTool(nullptr);
		bool IsNameOk(false);
		bool IsSubTool(false);
		switch (pNavElement->m_Type)
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
				pLoopGroupTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(pNavElement->m_OwnerId));
				if (nullptr == pLoopGroupTool || (SvPb::GroupToolObjectType != pLoopGroupTool->GetObjectSubType() && SvPb::LoopToolObjectType != pLoopGroupTool->GetObjectSubType()))
				{
					pLoopGroupTool = nullptr;
				}
				[[fallthrough]];
			case NavElementType::LoopTool:
			case NavElementType::GroupTool:
			case NavElementType::Tool:
				pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(pNavElement->m_navigatorObjectId));
				if (nullptr == pTool)
				{
					return;
				}

				if (IsSubTool && pLoopGroupTool)
				{
					IsNameOk = pLoopGroupTool->IsNameUnique(NewText.c_str(), m_LabelSaved.c_str());
				}
				else if (!IsSubTool)
				{
					IsNameOk = CheckName(NewText, m_LabelSaved.c_str());
				}
				if (IsNameOk)
				{
					//@TODO[MEC][10.10][13.10.2020]  here the tool is renamed, would be the correct position to
					// rename Grouping list.
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
	SVIODoc* pIODoc = GetTheIODoc();
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
		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_CELL_WIDTH, htiParent, svVariant);
	}

	if (bOk)
	{
		l_Size.cx = svVariant;
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
		std::string Name = m_toolSetListCtrl.GetItemText(item, 0).GetString();

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
	PtrNavigatorElement pNavElement = Get1stSelIndexAndElement().second;

	if (!pNavElement)
	{
		return false;
	}
	std::string ItemName(pNavElement->m_DisplayName);
	switch (pNavElement->m_Type)
	{
		case NavElementType::SubTool:
		case NavElementType::SubLoopTool:
		case NavElementType::EndDelimiterTool:
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
	if (!TheSVObserverApp().OkToEdit() || IsLabelEditing())
	{
		return false;
	}
	SVIPDoc* pCurrentDocument = GetIPDoc();
	if (nullptr == pCurrentDocument)
	{
		return false;
	}

	PtrNavigatorElement pNavElement = Get1stSelIndexAndElement().second;

	if (!pNavElement)
	{
		return false;
	}
	uint32_t toolId(pNavElement->m_navigatorObjectId);

	switch (pNavElement->m_Type)
	{

		case NavElementType::SubLoopTool:
		case NavElementType::SubTool:
		case NavElementType::GroupTool:
		case NavElementType::LoopTool:
		case NavElementType::Tool:
			if (SvDef::InvalidObjectId != toolId)
			{
				pCurrentDocument->OnEditTool();
				SetSelectedToolId(toolId);
			}
			break;
		case NavElementType::EndGrouping:
		case NavElementType::StartGrouping:
			EditToolGroupingComment(pNavElement->m_DisplayName);
			break;
		default:
			// Deselect all...
			m_toolSetListCtrl.SetSelectedToolId(SvDef::InvalidObjectId);
			break;
	}
	return true;
}

bool ToolSetView::hasCurrentToolErrors()
{
	uint32_t toolId = m_toolSetListCtrl.Get1stSelIndexAndId().second;
	if (SvDef::InvalidObjectId != toolId)
	{
		return !m_toolSetListCtrl.isToolValid(toolId);
	}
	return false;
}

void ToolSetView::displayFirstCurrentToolError()
{
	uint32_t toolId = m_toolSetListCtrl.Get1stSelIndexAndId().second;
	if (SvDef::InvalidObjectId != toolId)
	{
		m_toolSetListCtrl.displayErrorBox(toolId);
	}
}

bool ToolSetView::isAddParameter2MonitorListPossible(LPCTSTR ppqName) const
{
	uint32_t toolId = m_toolSetListCtrl.Get1stSelIndexAndId().second;
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && SvDef::InvalidObjectId != toolId)
	{
		return pConfig->isAddParameter2MonitorListPossible(ppqName, toolId);
	}
	return false;
}

bool ToolSetView::isRemoveParameter2MonitorListPossible(LPCTSTR ppqName) const
{
	uint32_t toolId = m_toolSetListCtrl.Get1stSelIndexAndId().second;
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && SvDef::InvalidObjectId != toolId)
	{
		return pConfig->isRemoveParameter2MonitorListPossible(ppqName, toolId);
	}
	return false;
}

bool ToolSetView::areParametersInMonitorList(LPCTSTR ppqName, uint32_t toolId) const
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && SvDef::InvalidObjectId != toolId)
	{
		return pConfig->areParametersInMonitorList(ppqName, toolId);
	}
	return false;
}

void ToolSetView::addParameter2MonitorList(LPCTSTR ppqName)
{
	uint32_t toolId = m_toolSetListCtrl.Get1stSelIndexAndId().second;
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && SvDef::InvalidObjectId != toolId)
	{
		SvStl::MessageContainerVector messages = pConfig->addParameter2MonitorList(ppqName, toolId);
		if (messages.size())
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(messages[0].getMessage());
		}
	}
}

void ToolSetView::removeParameter2MonitorList(LPCTSTR ppqName)
{
	uint32_t toolId = m_toolSetListCtrl.Get1stSelIndexAndId().second;
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig && SvDef::InvalidObjectId != toolId)
	{
		pConfig->removeParameter2MonitorList(ppqName, toolId);
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

void ToolSetView::RefreshTimestamp()
{
	m_timestamp = SvUl::GetTimeStamp();
}

