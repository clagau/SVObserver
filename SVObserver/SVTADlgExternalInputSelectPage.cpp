//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgExternalInputSelectPage
//* .File Name       : $Workfile:   SVTADlgExternalInputSelectPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   16 Dec 2014 17:57:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTADlgExternalInputSelectPage.h"
#include "Definitions/SVUserMessage.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "Definitions/StringTypeDef.h"
#include "SVObserver/SVToolAdjustmentDialogSheetClass.h"
#include "SVOGui\SVExternalToolImageSelectPage.h"
#include "SVTADlgExternalResultPage.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVOGui/BoundValue.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"

#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

/////////////////////////////////////////////////////////////////////////////
// SVTADlgExternalInputSelectPage property page

enum { IDC_INPUT_LIST_TREE = 99 };

enum { MAX_ITEMS_FOR_AUTO_EXPAND = 8 };

//IMPLEMENT_DYNCREATE(SVTADlgExternalInputSelectPage, CPropertyPage)

BEGIN_MESSAGE_MAP(SVTADlgExternalInputSelectPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVTADlgExternalInputSelectPage)
	//}}AFX_MSG_MAP
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_INPUT_LIST_TREE, OnItemChanged)
	ON_NOTIFY(PTN_QUERY_SHOW_BUTTON, IDC_INPUT_LIST_TREE, OnItemQueryShowButton)
	ON_NOTIFY(PTN_ITEMBUTTONCLICK, IDC_INPUT_LIST_TREE, OnItemButtonClick)
	ON_BN_CLICKED(ID_UNDO_CHANGES, OnUndoChanges)
END_MESSAGE_MAP()


SVTADlgExternalInputSelectPage::SVTADlgExternalInputSelectPage(LPCTSTR Title, uint32_t inspectionID, uint32_t toolObjectID, int id)
	: CPropertyPage(id)
	, m_InspectionID(inspectionID)
	, m_ToolObjectID(toolObjectID)
	, m_externalToolTaskController(inspectionID, toolObjectID)
	, m_TaskObjectID(m_externalToolTaskController.getExternalToolTaskObjectId())
	, m_InputValues{ SvOg::BoundValues{ inspectionID, m_TaskObjectID } }
{
	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(m_TaskObjectID, pObject);
	m_psp.pszTitle = Title;
	m_psp.dwFlags |= PSP_USETITLE;

	m_inputValueCount = m_externalToolTaskController.getNumInputValues();
	if (m_inputValueCount > COUNT_OF_INPUT_OUTPUT_IDs)
	{
		m_inputValueCount = COUNT_OF_INPUT_OUTPUT_IDs;
	}

	//{{AFX_DATA_INIT(SVTADlgExternalInputSelectPage)
	//}}AFX_DATA_INIT
}


void SVTADlgExternalInputSelectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTADlgExternalInputSelectPage)
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// SVTADlgExternalInputSelectPage message handlers

BOOL SVTADlgExternalInputSelectPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_InputValues.Init();

	if (m_inputValueCount > 0)
	{
		// Hide the text if we have inputs
		GetDlgItem(IDC_NO_INPUT_TXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INPUT_LIST)->ShowWindow(SW_SHOW);

		// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | PTS_NOTIFY;

		CRect rc;
		// Init the control's size to cover the entire client area
		GetDlgItem(IDC_INPUT_LIST)->GetWindowRect(rc);
		ScreenToClient(rc);
		// Create SVRPropTree control
		m_Tree.Create(dwStyle, rc, this, IDC_INPUT_LIST_TREE);
		m_Tree.SetColumn(m_Tree.GetColumn() * 2);

		SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
		assert(pRoot);
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("External Tool Input"));
		pRoot->SetInfoText(_T(""));

		std::map<std::string, SVRPropertyItem*> mapGroupItems;

		SVRPropertyItem* pGroupItem = nullptr;

		auto inputDefinitions = m_externalToolTaskController.getInputValuesDefinition().inputvaluesdefinition();


		int iItemCount = 0;

		for (int i = 0; i < m_inputValueCount; i++)
		{
			auto type = inputDefinitions[i].type();
			if (type == SvPb::ExDllInterfaceType::TableNames)
			{
				continue;
			}

			pGroupItem = AddGroupToTree(inputDefinitions[i].groupname(), mapGroupItems, pRoot);

			iItemCount++;

			assert(pGroupItem);

			AddItemToTree(inputDefinitions[i], pGroupItem, ID_BASE + i);
		}

		bool bOk = m_Tree.RestoreState(m_externalToolTaskController.getPropTreeState());
		if (!bOk)
		{
			SVRPropertyItem* pChild = pRoot->GetChild();
			while (pChild)
			{
				pChild->Expand(iItemCount <= MAX_ITEMS_FOR_AUTO_EXPAND);
				pChild = pChild->GetSibling();
			}
			pRoot->Expand(true);	// needed for redrawing
		}
	}
	else
	{
		GetDlgItem(IDC_NO_INPUT_TXT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_INPUT_LIST)->ShowWindow(SW_HIDE);
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


SVRPropertyItem* SVTADlgExternalInputSelectPage::AddGroupToTree(const std::string& groupName, std::map<std::string, SVRPropertyItem*>& rMapGroupItems, SVRPropertyItem* pRoot)
{
	std::map<std::string, SVRPropertyItem*>::iterator iterGroup;

	std::string GroupName = groupName;

	SVRPropertyItem* pGroupItem = nullptr;

	if ((iterGroup = rMapGroupItems.find(GroupName)) == rMapGroupItems.end())
	{	// if group does not already exist

		bool bTreeStyle = true;	// false = list-style

		pGroupItem = m_Tree.InsertItem(new SVRPropertyItem(), pRoot);
		pGroupItem->SetCanShrink(bTreeStyle);
		pGroupItem->SetLabelText(GroupName.c_str());
		pGroupItem->SetInfoText(_T(""));
		pGroupItem->Expand();

		pGroupItem->SetBold(true);

		if (!bTreeStyle)
		{
			pGroupItem->SetBackColor(::GetSysColor(COLOR_INACTIVECAPTION));
			pGroupItem->SetForeColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			pGroupItem->SetCanHighlight(false);
		}

		rMapGroupItems[GroupName] = pGroupItem;
	}
	else	// group already exists; use existing
	{
		pGroupItem = iterGroup->second;
	}

	return pGroupItem;
}



void SVTADlgExternalInputSelectPage::AddItemToTree(const SvPb::InputValueDefinition& rDefinition, SVRPropertyItem* pGroupItem, int iID)
{
	SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pGroupItem);
	if (nullptr == pEdit)
	{
		return;
	}

	pEdit->SetCtrlID(iID);

	int LVIndex = rDefinition.linkedvalueindex();

	std::string Label = SvUl::LoadStdString(IDS_OBJECTNAME_INPUT_01 + static_cast<int>(LVIndex));
	Label += " (";
	Label += rDefinition.displayname();
	Label += ")";

	pEdit->SetLabelText(Label.c_str());

	std::string Type;
	switch (rDefinition.vt())
	{
	case VT_BOOL: Type = _T("Bool");
		break;

	case VT_I4:   Type = _T("Long");
		break;

	case VT_R8:   Type = _T("Double");
		break;

	case VT_BSTR: Type = _T("String");
		break;

	case VT_R8 | VT_ARRAY:

		if (rDefinition.type() == SvPb::ExDllInterfaceType::TableArray)
		{
			Type = _T("Table or Table Element");
		}
		else
		{
			Type = _T("Double array");
		}
		break;

	case VT_I4 | VT_ARRAY:
		if (rDefinition.type() == SvPb::ExDllInterfaceType::TableArray)
		{
			Type = _T("Table or Table Element");
		}
		else
		{
			Type = _T("Long Array");
		}
		break;


	default:
		Type = _T("???");
		break;
	}

	std::string Description = rDefinition.helptext();

	Description = _T(" (Type: ") + Type + _T(")  ") + Description;
	pEdit->SetInfoText(Description.c_str());

	std::string Value(m_InputValues.Get<CString>(SvPb::ExternalInputLinkedEId + LVIndex));
	if (Value.empty())
	{
		_variant_t temp = m_InputValues.Get<_variant_t>(SvPb::ExternalInputEId + LVIndex);

		Value = SvUl::VariantToString(temp);
	}

	pEdit->SetItemValue(Value.c_str());
	pEdit->OnRefresh();
}


void SVTADlgExternalInputSelectPage::OnUndoChanges()
{
	CWnd *pParent = GetParent();

	if (nullptr != pParent)
	{
		pParent->PostMessage(SV_REMOVE_PAGES_FOR_TESTED_DLL);

		pParent->PostMessage(SV_ADD_PAGES_FOR_TESTED_DLL);

		pParent->PostMessage(PSM_SETCURSEL, c_indexOfInputValuePage, 0);
	}
}

void SVTADlgExternalInputSelectPage::OnItemQueryShowButton(NMHDR* , LRESULT* plResult)
{
	*plResult = TRUE;	// always show button
}

void SVTADlgExternalInputSelectPage::OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	*plResult = TRUE;

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;

	if (pNMPropTree->pItem)
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;

		// display value object picker
		std::string ObjectName;
		if (SelectObject(ObjectName, pItem) == IDOK && !ObjectName.empty())
		{
			pItem->SetItemValue(ObjectName.c_str());
			pItem->OnRefresh();
		}
	}// end if ( pNMPropTree->pItem )
}

// display VO picker dialog and return selection
int SVTADlgExternalInputSelectPage::SelectObject(std::string& rObjectName, SVRPropertyItem* pItem)
{
	
	//const  SvOp::InputValueDefinition* pDef = GetInputValueDefinitionPtr(pItem);
	auto pDef = this->GetInputDefinitionPtr(pItem);
	bool isTable(false);
	if (nullptr == pDef)
	{
		assert(false);
		return IDCANCEL;
	}
	isTable = (pDef->type() == SvPb::ExDllInterfaceType::TableArray);
	
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	if (!isTable)
	{
		SvPb::ObjectSelectorType type = (pDef->vt() & VT_BSTR) == 0 ? SvPb::allNumberValueObjects : SvPb::allValueObjects;
		*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::cameraObject, SvPb::SearchArea::ppqItems, SvPb::SearchArea::toolsetItems},
			m_InspectionID, SvPb::archivable, SvDef::InvalidObjectId, true, type, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed, m_ToolObjectID);
		SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	}
	else
	{
		*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
			{ SvPb::SearchArea::toolsetItems },
			m_InspectionID, SvPb::taskObject, SvDef::InvalidObjectId, true, SvPb::tableObjects, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed, m_ToolObjectID);
		SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	}

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject);
	if (responseCmd.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
	}

	std::string value;
	pItem->GetItemValue(value);

	if (!value.empty())
	{
		SvDef::StringSet checkedItems;
		checkedItems.insert(value);
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems(checkedItems);
	}

	std::string Filter = SvUl::LoadStdString(IDS_FILTER);
	std::string ToolsetOutput = SvUl::LoadStdString(IDS_SELECT_TOOLSET_OUTPUT);
	std::string Title = SvUl::Format(_T("%s - %s"), ToolsetOutput.c_str(), GetName(m_ToolObjectID).c_str());

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog(Title.c_str(), ToolsetOutput.c_str(), Filter.c_str(), this);

	if (IDOK == Result)
	{
		SVObjectReference objectRef {SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult()};
		rObjectName = objectRef.GetObjectNameBeforeObjectType(SvPb::SVInspectionObjectType, true);
	}

	return static_cast<int>(Result);
}

void SVTADlgExternalInputSelectPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
	*plResult = S_OK;

	SVRPropertyItem* pItem = pNMPropTree->pItem;

	if (pItem)
	{
		auto pDef = this->GetInputDefinitionPtr(pItem);

		if (!pDef)
		{
			*plResult = S_FALSE;
			return;
		}

		SvPb::ExDllInterfaceType type = pDef->type();

		int index = GetItemIndex(pItem);
		assert(index >= 0 && index < m_inputValueCount);

		_variant_t newVal;
		pItem->GetItemValue(newVal);

		std::string Name;
		pItem->GetItemValue(Name);

		if (false == m_externalToolTaskController.validateValueParameterWrapper(m_TaskObjectID, index, newVal, type))
		{
			*plResult = S_FALSE;
		}
	}
}

// Loops through Tree Items to fill existing SVInputObjectInfo array (if input is another VO) and/or SVValueObjects with 
// constant values (if input is not another VO)
void SVTADlgExternalInputSelectPage::OnOK()
{
	auto inputDefinitions = m_externalToolTaskController.getInputValuesDefinition().inputvaluesdefinition();
	SVRPropertyItem* pGroup = nullptr;
	if (m_Tree.GetRootItem() && nullptr != m_Tree.GetRootItem()->GetChild())
	{
		pGroup = m_Tree.GetRootItem()->GetChild()->GetChild();
		while (pGroup)
		{
			SVRPropertyItem* pItem = pGroup->GetChild();
			while (pItem)
			{
				int iIndex = GetItemIndex(pItem);
				assert(iIndex >= 0 && iIndex < m_inputValueCount);
				if (iIndex >= 0 && iIndex < m_inputValueCount)
				{
					int LVIndex = inputDefinitions[iIndex].linkedvalueindex();
					std::string Value;
					pItem->GetItemValue(Value);
					bool done {false};
					_variant_t  array;
					SvPb::ExDllInterfaceType  type = inputDefinitions[iIndex].type();
					if (type == SvPb::ExDllInterfaceType::Array)
					{
					switch (inputDefinitions[iIndex].vt())
					{
						case VT_ARRAY | VT_R8:
						{
							if (SvUl::StringToSafeArray<double>(Value, array) >= 0)
							{
								m_InputValues.Set<_variant_t>(SvPb::ExternalInputEId + LVIndex, array);
								done = true;
							}
							break;
						}
						case VT_ARRAY | VT_I4:
						{
							if (SvUl::StringToSafeArray<long>(Value, array) >= 0)
							{
								m_InputValues.Set<_variant_t>(SvPb::ExternalInputEId + LVIndex, array);
								done = true;
							}
							break;
						}
						}
					}
					if (!done)
					{
						m_InputValues.Set<CString>(SvPb::ExternalInputEId + LVIndex, Value.c_str());
					}
				}

				pItem = pItem->GetSibling();
			}
			pGroup = pGroup->GetSibling();
		}
		std::map<std::string, bool> propTreeState;
		m_Tree.SaveState(propTreeState);
		m_externalToolTaskController.setPropTreeState(propTreeState);

		m_InputValues.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);
		CPropertyPage::OnOK();
	}
}


const std::unique_ptr<SvPb::InputValueDefinition> SVTADlgExternalInputSelectPage::GetInputDefinitionPtr(SVRPropertyItem* pItem)
{
	int iIndex = GetItemIndex(pItem);
	auto inputDefinitions = m_externalToolTaskController.getInputValuesDefinition().inputvaluesdefinition();
	if (iIndex >= 0 && inputDefinitions.size() > iIndex)
	{
		return  std::make_unique< SvPb::InputValueDefinition>(inputDefinitions[iIndex]);
	}
	else
	{
		return nullptr;
	}
}

int SVTADlgExternalInputSelectPage::GetItemIndex(SVRPropertyItem* pItem)
{
	assert(pItem);
	return pItem->GetCtrlID() - ID_BASE;
}

std::string SVTADlgExternalInputSelectPage::GetName(uint32_t id) const
{
	std::string inspectionName;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
	pRequest->set_objectid(id);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
	{
		inspectionName = responseCmd.getobjectparametersresponse().name();
	}
	return inspectionName;
}


BOOL SVTADlgExternalInputSelectPage::OnKillActive()
{
	// Since the ValueController used here share the same data with the one used on another page (see SVTADlgExternalInputSelectPage),
	// we need to avoid unwanted interactions between the two. Therefore ensure a Commit() w/o call for each controller by page leave.
	m_InputValues.Commit(SvOg::PostAction::doNothing);

	return CPropertyPage::OnKillActive();
}
