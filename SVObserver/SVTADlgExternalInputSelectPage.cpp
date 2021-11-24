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
#include "SVOGui\SVTADlgExternalToolImageSelectPage.h"
#include "SVTADlgExternalResultPage.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVOGui/BoundValue.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
#include "SVOGui/LinkedValue.h"
#include "SVOGui/LinkedValueSelectorDialog.h"
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

std::pair<SvOg::LinkedValueSelectorTypesEnum, std::string> getTypeData(const SvPb::InputValueDefinition& rDefinition)
{
	std::pair<SvOg::LinkedValueSelectorTypesEnum, std::string> result = { SvOg::LinkedValueSelectorTypesEnum::All, "" };
	
	switch (rDefinition.vt())
	{
	case VT_BOOL: result.second = _T("Bool");
		break;

	case VT_I4:   result.second = _T("Long");
		break;

	case VT_R8:   result.second = _T("Double");
		break;

	case VT_BSTR: 
		result.first = SvOg::LinkedValueSelectorTypesEnum::DirectIndirect;
		result.second = _T("String");
		break;

	case VT_R8 | VT_ARRAY:
		if (rDefinition.type() == SvPb::ExDllInterfaceType::TableArray)
		{
			result.first = SvOg::LinkedValueSelectorTypesEnum::Indirect;
			result.second = _T("Table or Table Element");
		}
		else
		{
			result.first = SvOg::LinkedValueSelectorTypesEnum::DirectIndirect;
			result.second = _T("Double array");
		}
		break;

	case VT_I4 | VT_ARRAY:
		if (rDefinition.type() == SvPb::ExDllInterfaceType::TableArray)
		{
			result.first = SvOg::LinkedValueSelectorTypesEnum::Indirect;
			result.second = _T("Table or Table Element");
		}
		else
		{
			result.first = SvOg::LinkedValueSelectorTypesEnum::DirectIndirect;
			result.second = _T("Long Array");
		}
		break;


	default:
		result.first = SvOg::LinkedValueSelectorTypesEnum::None;
		result.second = _T("???");
		break;
	}

	return result;
}

//IMPLEMENT_DYNCREATE(SVTADlgExternalInputSelectPage, CPropertyPage)

BEGIN_MESSAGE_MAP(SVTADlgExternalInputSelectPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVTADlgExternalInputSelectPage)
	//}}AFX_MSG_MAP
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_INPUT_LIST_TREE, OnItemChanged)
	ON_NOTIFY(PTN_QUERY_SHOW_BUTTON, IDC_INPUT_LIST_TREE, OnItemQueryShowButton)
	ON_NOTIFY(PTN_ITEMBUTTONCLICK, IDC_INPUT_LIST_TREE, OnItemButtonClick)
	ON_BN_CLICKED(ID_UNDO_CHANGES, OnUndoChanges)
END_MESSAGE_MAP()


SVTADlgExternalInputSelectPage::SVTADlgExternalInputSelectPage(LPCTSTR Title, uint32_t inspectionID, uint32_t toolObjectID, ExternalToolTaskController& rExternalToolTaskController)
	: CPropertyPage(IDD)
	, m_InspectionID(inspectionID)
	, m_ToolObjectID(toolObjectID)
	, m_rExternalToolTaskController(rExternalToolTaskController)
	, m_TaskObjectID(m_rExternalToolTaskController.getExternalToolTaskObjectId())
	, m_InputValues{ SvOg::BoundValues{ inspectionID, m_TaskObjectID } }
{
	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(m_TaskObjectID, pObject);
	m_psp.pszTitle = Title;
	m_psp.dwFlags |= PSP_USETITLE;

	m_inputValueCount = m_rExternalToolTaskController.getNumInputValues();
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

		auto inputDefinitions = m_rExternalToolTaskController.getInputValuesDefinition().inputvaluesdefinition();


		int iItemCount = 0;

		for (int i = 0; i < m_inputValueCount; i++)
		{
			auto type = inputDefinitions[i].type();
			if (type == SvPb::ExDllInterfaceType::TableNames)
			{
				continue;
			}

			pGroupItem = AddGroupToTree(inputDefinitions[i].groupname(), mapGroupItems, pRoot);
			assert(pGroupItem);

			AddItemToTree(inputDefinitions[i], pGroupItem, ID_BASE + i);
			++iItemCount;
		}

		bool bOk = m_Tree.RestoreState(m_rExternalToolTaskController.getPropTreeState());
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
	SVRPropertyItemEdit* pEdit = dynamic_cast<SVRPropertyItemEdit*>(m_Tree.InsertItem(new SVRPropertyItemEdit(), pGroupItem));
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

	auto [objectSelectType, typeString] = getTypeData(rDefinition);

	std::string Description = rDefinition.helptext();

	Description = _T(" (Type: ") + typeString + _T(")  ") + Description;
	pEdit->SetInfoText(Description.c_str());

	setValueColumn(SvPb::ExternalInputEId + LVIndex, objectSelectType, *pEdit);
}


void SVTADlgExternalInputSelectPage::OnUndoChanges()
{
	CWnd *pParent = GetParent();

	if (nullptr != pParent)
	{
		pParent->PostMessage(SV_ADAPT_TO_UNTESTED_DLL);

		pParent->PostMessage(SV_ADAPT_TO_TESTED_DLL);

		pParent->PostMessage(SV_SELECT_INPUT_VALUE_PAGE);
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

		auto* pEditItem = dynamic_cast<SVRPropertyItemEdit*>(pItem);

		// display value object picker
		assert(pEditItem);
		if (nullptr != pEditItem)
		{
			//set focus away from this control, so that changed values will be set, before SelectObject is called.
			SetFocus();
			SelectObject(*pEditItem);
		}
	}// end if ( pNMPropTree->pItem )
}

// display VO picker dialog and return selection
void SVTADlgExternalInputSelectPage::SelectObject(SVRPropertyItemEdit& rItem)
{
	auto pDef = GetInputDefinitionPtr(&rItem);
	if (pDef)
	{
		SvOg::ObjectSelectorData objSelectorData;
		auto vtType = static_cast<VARTYPE>(pDef->vt());
		switch (pDef->type())
		{
			case SvPb::ExDllInterfaceType::TableArray:
				objSelectorData.m_type = SvPb::tableObjects;
				objSelectorData.m_attribute = SvPb::taskObject;
				objSelectorData.m_searchArea = {SvPb::SearchArea::toolsetItems};
				vtType = VT_EMPTY;
				break;
			case SvPb::ExDllInterfaceType::Array:
				objSelectorData.m_type = (pDef->vt() & VT_BSTR) == 0 ? SvPb::allNumberValueObjects : SvPb::allValueObjects;
				objSelectorData.m_attribute = SvPb::archivable;
				objSelectorData.m_searchArea = {SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::cameraObject, SvPb::SearchArea::ppqItems, SvPb::SearchArea::toolsetItems};
				objSelectorData.m_wholeArray = true;
				break;
			default:
				objSelectorData.m_type = (pDef->vt() & VT_BSTR) == 0 ? SvPb::allNumberValueObjects : SvPb::allValueObjects;
				objSelectorData.m_attribute = SvPb::archivable;
				objSelectorData.m_searchArea = {SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::cameraObject, SvPb::SearchArea::ppqItems, SvPb::SearchArea::toolsetItems};
				objSelectorData.m_wholeArray = false;
				break;
		}

		objSelectorData.m_excludeSameLineageVector = { m_ToolObjectID };
		objSelectorData.m_stopAtId = m_ToolObjectID;

		auto [objectSelectType, _] = getTypeData(*pDef);
		SvPb::EmbeddedIdEnum eId = SvPb::ExternalInputEId + pDef->linkedvalueindex();
		SvOg::LinkedValueSelectorDialog dlg(m_InspectionID, m_InputValues.GetObjectID(eId), 
			m_InputValues.GetName(eId),
			m_InputValues.Get<SvOg::LinkedValueData>(eId),
			vtType,
			objSelectorData, nullptr, objectSelectType);
		if (IDOK == dlg.DoModal())
		{
			m_InputValues.Set(eId, dlg.getData());
			setValueColumn(eId, objectSelectType, rItem);
		}
	}
}

void SVTADlgExternalInputSelectPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
	SVRPropertyItem* pItem = pNMPropTree->pItem;

	if (pItem)
	{
		if (setStringToData(*pItem))
		{
			*plResult = S_OK;
			return;
		}
	}
	*plResult = S_FALSE;
}

void SVTADlgExternalInputSelectPage::OnOK()
{
	updateInputValuesFromPropertyTree();
	CPropertyPage::OnOK();
}

// Loops through Tree Items to fill existing SVInputObjectInfo array (if input is another VO) and/or SVValueObjects with 
// constant values (if input is not another VO)
void SVTADlgExternalInputSelectPage::updateInputValuesFromPropertyTree()
{
	if (m_Tree.GetRootItem() && nullptr != m_Tree.GetRootItem()->GetChild())
	{
		std::map<std::string, bool> propTreeState;
		m_Tree.SaveState(propTreeState);
		m_rExternalToolTaskController.setPropTreeState(propTreeState);

		m_InputValues.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);
	}
}


const std::unique_ptr<SvPb::InputValueDefinition> SVTADlgExternalInputSelectPage::GetInputDefinitionPtr(SVRPropertyItem* pItem)
{
	int iIndex = GetItemIndex(pItem);
	auto inputDefinitions = m_rExternalToolTaskController.getInputValuesDefinition().inputvaluesdefinition();
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
	updateInputValuesFromPropertyTree();

	return CPropertyPage::OnKillActive();
}

void SVTADlgExternalInputSelectPage::setValueColumn(SvPb::EmbeddedIdEnum eId, SvOg::LinkedValueSelectorTypesEnum selectorTypes, SVRPropertyItemEdit& rEdit)
{
	auto data = m_InputValues.Get<SvOg::LinkedValueData>(eId);
	bool isReadOnly{ true };
	CString valueString;
	if (SvOg::LinkedValueSelectorTypesEnum::All == selectorTypes || SvOg::LinkedValueSelectorTypesEnum::DirectIndirect == selectorTypes)
	{
		valueString = SvUl::VariantToString(data.m_Value).c_str();
		isReadOnly = (SvPb::LinkedSelectedType::DirectValue != data.m_type);
	}
	else
	{
		valueString = SvCmd::getDottedName(m_InspectionID, data.m_indirectIdName).c_str();
	}

	rEdit.SetItemValue(valueString);
	rEdit.OnRefresh();
	rEdit.ReadOnly(isReadOnly, true);
}

bool SVTADlgExternalInputSelectPage::setStringToData(SVRPropertyItem& rItem)
{
	auto pDef = GetInputDefinitionPtr(&rItem);

	int index = GetItemIndex(&rItem);
	assert(index >= 0 && index < m_inputValueCount);
	if (!pDef || index < 0 || index >= m_inputValueCount)
	{
		return false;
	}

	SvPb::ExDllInterfaceType type = pDef->type();
	SvPb::EmbeddedIdEnum eId = SvPb::ExternalInputEId + pDef->linkedvalueindex();
	auto data = m_InputValues.Get<SvOg::LinkedValueData>(eId);
	if (SvPb::LinkedSelectedType::DirectValue == data.m_type)
	{
		_variant_t newVal;
		rItem.GetItemValue(newVal);

		std::string Name;
		rItem.GetItemValue(Name);

		if (false == m_rExternalToolTaskController.validateValueParameterWrapper(m_TaskObjectID, index, newVal, type))
		{
			return false;
		}
		else
		{
			bool isOk = SvOg::LinkedValueSelectorDialog::createVariantFromString(static_cast<VARTYPE>(pDef->vt()), std::string {Name}, data.m_directValue);
			if (isOk)
			{
				data.m_Value = data.m_directValue;
				m_InputValues.Set<SvOg::LinkedValueData>(eId, data);
			}
		}
	}
	return true;
}
