//******************************************************************************
//* COPYRIGHT (c) 2004 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgExternalInputSelectPage
//* .File Name       : $Workfile:   TADlgExternalInputSelectPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   16 Dec 2014 17:57:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgExternalInputSelectPage.h"
#include "Definitions/SVUserMessage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVOGui/TADlgExternalToolImageSelectPage.h"
#include "TADlgExternalResultPage.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
#include "SVOGuiUtility/BoundValue.h"
#include "SVOGuiUtility/LinkedValue.h"
#include "SVOGuiUtility/LinkedValueSelectorDialog.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

/////////////////////////////////////////////////////////////////////////////
// TADlgExternalInputSelectPage property page

enum { IDC_INPUT_LIST_TREE = 99 };

enum { MAX_ITEMS_FOR_AUTO_EXPAND = 8 };

LPCTSTR getTypeString(const SvPb::InputValueDefinition& rDefinition)
{
	switch (rDefinition.vt())
	{
	case VT_BOOL: 
		return _T("Bool");

	case VT_I4:   
		return _T("Long");

	case VT_R8:   
		return _T("Double");

	case VT_BSTR: 
		return _T("String");

	case VT_R8 | VT_ARRAY:
		if (rDefinition.type() == SvPb::ExDllInterfaceType::TableArray)
		{
			return _T("Table or Table Element");
		}
		else
		{
			return _T("Double array");
		}

	case VT_I4 | VT_ARRAY:
		if (rDefinition.type() == SvPb::ExDllInterfaceType::TableArray)
		{
			return _T("Table or Table Element");
		}
		else
		{
			return _T("Long Array");
		}

	default:
		return _T("???");
	}
}

//IMPLEMENT_DYNCREATE(TADlgExternalInputSelectPage, CPropertyPage)

BEGIN_MESSAGE_MAP(TADlgExternalInputSelectPage, CPropertyPage)
	//{{AFX_MSG_MAP(TADlgExternalInputSelectPage)
	//}}AFX_MSG_MAP
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_INPUT_LIST_TREE, OnItemChanged)
	ON_NOTIFY(PTN_QUERY_SHOW_BUTTON, IDC_INPUT_LIST_TREE, OnItemQueryShowButton)
	ON_NOTIFY(PTN_ITEMBUTTONCLICK, IDC_INPUT_LIST_TREE, OnItemButtonClick)
	ON_BN_CLICKED(ID_UNDO_CHANGES, OnUndoChanges)
	ON_WM_SIZE()
END_MESSAGE_MAP()


TADlgExternalInputSelectPage::TADlgExternalInputSelectPage(LPCTSTR Title, uint32_t inspectionID, uint32_t toolObjectID, SvOgu::ExternalToolTaskController& rExternalToolTaskController)
	: CPropertyPage(IDD)
	, m_InspectionID(inspectionID)
	, m_ToolObjectID(toolObjectID)
	, m_rExternalToolTaskController(rExternalToolTaskController)
	, m_TaskObjectID(m_rExternalToolTaskController.getExternalToolTaskObjectId())
	, m_InputValues{ SvOgu::BoundValues{ inspectionID, m_TaskObjectID } }
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

	//{{AFX_DATA_INIT(TADlgExternalInputSelectPage)
	//}}AFX_DATA_INIT
}


void TADlgExternalInputSelectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADlgExternalInputSelectPage)
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// TADlgExternalInputSelectPage message handlers

BOOL TADlgExternalInputSelectPage::OnInitDialog()
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
		Log_Assert(pRoot);
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
			Log_Assert(pGroupItem);

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

	if (IsDynamicLayoutEnabled())
	{
		CMFCDynamicLayout* dynamicLayout = GetDynamicLayout();
		if (!dynamicLayout->HasItem(m_Tree.m_hWnd))
		{
			dynamicLayout->AddItem(m_Tree.m_hWnd, CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeHorizontalAndVertical(100, 100));
		}
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

SVRPropertyItem* TADlgExternalInputSelectPage::AddGroupToTree(const std::string& groupName, std::map<std::string, SVRPropertyItem*>& rMapGroupItems, SVRPropertyItem* pRoot)
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



void TADlgExternalInputSelectPage::AddItemToTree(const SvPb::InputValueDefinition& rDefinition, SVRPropertyItem* pGroupItem, int iID)
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

	std::string Description = std::string {" (Type: "} + getTypeString(rDefinition) + _T(")  ") + rDefinition.helptext();
	pEdit->SetInfoText(Description.c_str());

	setValueColumn(SvPb::ExternalInputEId + LVIndex, rDefinition.type(), *pEdit);
}


void TADlgExternalInputSelectPage::OnUndoChanges()
{
	CWnd *pParent = GetParent();

	if (nullptr != pParent)
	{
		pParent->PostMessage(SV_ADAPT_TO_UNTESTED_DLL);

		pParent->PostMessage(SV_ADAPT_TO_TESTED_DLL);

		pParent->PostMessage(SV_SELECT_INPUT_VALUE_PAGE);
	}
}

void TADlgExternalInputSelectPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);
	// TODO THB: Find a way to resize that
}

void TADlgExternalInputSelectPage::OnItemQueryShowButton(NMHDR* , LRESULT* plResult)
{
	*plResult = TRUE;	// always show button
}

void TADlgExternalInputSelectPage::OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	*plResult = TRUE;

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;

	if (pNMPropTree->pItem)
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;

		auto* pEditItem = dynamic_cast<SVRPropertyItemEdit*>(pItem);

		// display value object picker
		Log_Assert(pEditItem);
		if (nullptr != pEditItem)
		{
			//set focus away from this control, so that changed values will be set, before SelectObject is called.
			SetFocus();
			SelectObject(*pEditItem);
		}
	}// end if ( pNMPropTree->pItem )
}

// display VO picker dialog and return selection
void TADlgExternalInputSelectPage::SelectObject(SVRPropertyItemEdit& rItem)
{
	auto pDef = GetInputDefinitionPtr(&rItem);
	if (pDef)
	{
		VARTYPE vtType = (SvPb::ExDllInterfaceType::TableArray == pDef->type()) ? VT_EMPTY : static_cast<VARTYPE>(pDef->vt());
		SvPb::EmbeddedIdEnum eId = SvPb::ExternalInputEId + pDef->linkedvalueindex();
		SvOgu::LinkedValueSelectorDialog dlg(m_InspectionID, m_InputValues.GetObjectID(eId), 
			m_InputValues.GetName(eId),
			m_InputValues.Get<SvOgu::LinkedValueData>(eId),
			vtType);
		if (IDOK == dlg.DoModal())
		{
			m_InputValues.Set(eId, dlg.getData());
			setValueColumn(eId, pDef->type(), rItem);
		}
	}
}

void TADlgExternalInputSelectPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
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

void TADlgExternalInputSelectPage::OnOK()
{
	updateInputValuesFromPropertyTree();
	CPropertyPage::OnOK();
}

// Loops through Tree Items to fill existing SVInputObjectInfo array (if input is another VO) and/or SVValueObjects with 
// constant values (if input is not another VO)
void TADlgExternalInputSelectPage::updateInputValuesFromPropertyTree()
{
	if (m_Tree.GetRootItem() && nullptr != m_Tree.GetRootItem()->GetChild())
	{
		std::map<std::string, bool> propTreeState;
		m_Tree.SaveState(propTreeState);
		m_rExternalToolTaskController.setPropTreeState(propTreeState);

		m_InputValues.Commit(SvOgu::PostAction::doNothing);
	}
}


const std::unique_ptr<SvPb::InputValueDefinition> TADlgExternalInputSelectPage::GetInputDefinitionPtr(SVRPropertyItem* pItem)
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

int TADlgExternalInputSelectPage::GetItemIndex(SVRPropertyItem* pItem)
{
	Log_Assert(pItem);
	return pItem->GetCtrlID() - ID_BASE;
}

std::string TADlgExternalInputSelectPage::GetName(uint32_t id) const
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


BOOL TADlgExternalInputSelectPage::OnKillActive()
{
	updateInputValuesFromPropertyTree();

	return CPropertyPage::OnKillActive();
}

void TADlgExternalInputSelectPage::setValueColumn(SvPb::EmbeddedIdEnum eId, SvPb::ExDllInterfaceType type, SVRPropertyItemEdit& rEdit)
{
	auto data = m_InputValues.Get<SvOgu::LinkedValueData>(eId);
	bool isReadOnly{ true };
	CString valueString;
	switch (type)
	{
		case SvPb::TableArray:
		case SvPb::TableNames:
		case SvPb::Invalid:
			valueString = SvCmd::getDottedName(m_InspectionID, data.m_indirectIdName).c_str();
			break;
		case SvPb::Scalar:
		case SvPb::Array:
		default:
		valueString = SvUl::VariantToString(data.m_Value).c_str();
		isReadOnly = (SvPb::LinkedSelectedOption::DirectValue != data.m_selectedOption);
			break;
	}

	rEdit.SetBackColorReadOnly(false, SvDef::WhiteSmoke);
	rEdit.SetItemValue(valueString);
	rEdit.OnRefresh();
	rEdit.ReadOnly(isReadOnly, true);
}

bool TADlgExternalInputSelectPage::setStringToData(SVRPropertyItem& rItem)
{
	auto pDef = GetInputDefinitionPtr(&rItem);

	int index = GetItemIndex(&rItem);
	Log_Assert(index >= 0 && index < m_inputValueCount);
	if (!pDef || index < 0 || index >= m_inputValueCount)
	{
		return false;
	}

	SvPb::ExDllInterfaceType type = pDef->type();
	SvPb::EmbeddedIdEnum eId = SvPb::ExternalInputEId + pDef->linkedvalueindex();
	auto data = m_InputValues.Get<SvOgu::LinkedValueData>(eId);
	if (SvPb::LinkedSelectedOption::DirectValue == data.m_selectedOption)
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
			bool isOk = SvOgu::LinkedValueSelectorDialog::createVariantFromString(static_cast<VARTYPE>(pDef->vt()), std::string {Name}, data.m_directValue);
			if (isOk)
			{
				data.m_Value = data.m_directValue;
				m_InputValues.Set<SvOgu::LinkedValueData>(eId, data);
			}
		}
	}
	return true;
}
