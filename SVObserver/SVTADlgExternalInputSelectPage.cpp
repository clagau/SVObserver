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
#include "Operators/SVExternalToolTask.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/SVUserMessage.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "SVPPQObject.h"
#include "Definitions/StringTypeDef.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVOGui\SVExternalToolImageSelectPage.h"
#include "SVTADlgExternalResultPage.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVOGui/BoundValue.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
#include "Operators/TableObject.h"
#include "Operators/SVDLLToolDefinitionStructs.h"
#include "Operators/SVDLLToolLoadLibraryClass.h"

#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

/////////////////////////////////////////////////////////////////////////////
// SVExternalToolInputSelectPage property page

enum { IDC_INPUT_LIST_TREE = 99 };

enum { MAX_ITEMS_FOR_AUTO_EXPAND = 8 };

//IMPLEMENT_DYNCREATE(SVExternalToolInputSelectPage, CPropertyPage)

BEGIN_MESSAGE_MAP(SVExternalToolInputSelectPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVExternalToolInputSelectPage)
	//}}AFX_MSG_MAP
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_INPUT_LIST_TREE, OnItemChanged)
	ON_NOTIFY(PTN_QUERY_SHOW_BUTTON, IDC_INPUT_LIST_TREE, OnItemQueryShowButton)
	ON_NOTIFY(PTN_ITEMBUTTONCLICK, IDC_INPUT_LIST_TREE, OnItemButtonClick)
	ON_BN_CLICKED(ID_UNDO_CHANGES, OnUndoChanges)
END_MESSAGE_MAP()


SVExternalToolInputSelectPage::SVExternalToolInputSelectPage(LPCTSTR Title, uint32_t inspectionID, uint32_t toolObjectID, uint32_t taskObjectID, int id)
	: CPropertyPage(id)
	, m_InspectionID(inspectionID)
	, m_ToolObjectID(toolObjectID)
	, m_TaskObjectID(taskObjectID)
	, m_Values {SvOg::BoundValues{ inspectionID, taskObjectID }}
{
	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(m_TaskObjectID, pObject);
	m_pTask = dynamic_cast<SvOp::SVExternalToolTask*> (pObject);
	assert(m_pTask);
	m_psp.pszTitle = Title;
	m_psp.dwFlags |= PSP_USETITLE;
	m_inputValueCount = m_pTask->m_Data.m_lNumInputValues;
	if (m_inputValueCount > COUNT_OF_INPUT_OUTPUT_IDs)
	{
		m_inputValueCount = COUNT_OF_INPUT_OUTPUT_IDs;
	}

	//{{AFX_DATA_INIT(SVExternalToolInputSelectPage)
	//}}AFX_DATA_INIT
}

SVExternalToolInputSelectPage::~SVExternalToolInputSelectPage()
{
}

void SVExternalToolInputSelectPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVExternalToolInputSelectPage)
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// SVExternalToolInputSelectPage message handlers

BOOL SVExternalToolInputSelectPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_Values.Init();

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
		std::map<std::string, SVRPropertyItem*>::iterator iterGroup;

		SVRPropertyItem* pGroupItem = nullptr;

		int iItemCount = 0;

		for (int i = 0; i < m_inputValueCount; i++)
		{
			SvOp::InputValueDefinition& rDefinition = m_pTask->m_Data.m_InputDefinitions[i];
			if (rDefinition.getType() == SvOp::ExDllInterfaceType::TableNames)
			{
				continue;
			}

			int iID = ID_BASE + i;

			std::string GroupName = rDefinition.getGroup();

			if ((iterGroup = mapGroupItems.find(GroupName)) == mapGroupItems.end())
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

				mapGroupItems[GroupName] = pGroupItem;
			}
			else	// group already exists; use existing
			{
				pGroupItem = iterGroup->second;
			}

			iItemCount++;

			assert(pGroupItem);

			SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pGroupItem);
			if (nullptr == pEdit)
			{
				break;
			}

			pEdit->SetCtrlID(iID);

			int LVIndex = rDefinition.getLinkedValueIndex();


			std::string  ObjectName = m_Values.GetName(SvPb::ExternalInputEId + LVIndex).c_str();
			std::string sLabel = SvUl::LoadStdString(IDS_OBJECTNAME_INPUT_01 + static_cast<int>(LVIndex));
				sLabel += " (";
				sLabel += rDefinition.getDisplayName();
				sLabel += ")";

			
			pEdit->SetLabelText(sLabel.c_str());

			std::string strType;
			switch (rDefinition.getVt())
			{
				case VT_BOOL: strType = _T("Bool");   break;
				case VT_I4:   strType = _T("Long");   break;
				case VT_R8:   strType = _T("Double"); break;
				case VT_BSTR: strType = _T("String"); break;
				case VT_R8 | VT_ARRAY:

					if (rDefinition.getType() == SvOp::ExDllInterfaceType::TableArray)
					{
						strType = _T("Table or Table Element");
					}
					else
					{
						strType = _T("Double array");
					}
					break;

				case VT_I4 | VT_ARRAY:
					if (rDefinition.getType() == SvOp::ExDllInterfaceType::TableArray)
					{
						strType = _T("Table or Table Element");
					}
					else
					{
						strType = _T("Long Array");
					}
					break;


				default:      strType = _T("???");    break;
			}

			std::string Description = rDefinition.getHelpText();
			Description = _T(" (Type: ") + strType + _T(")  ") + Description;
			pEdit->SetInfoText(Description.c_str());

			std::string Value(m_Values.Get<CString>(SvPb::ExternalInputLinkedEId + LVIndex));
			if (Value.empty())
			{
				_variant_t temp = m_Values.Get<_variant_t>(SvPb::ExternalInputEId + LVIndex);

				Value = SvUl::VariantToString(temp);

			}
			pEdit->SetItemValue(Value.c_str());
			pEdit->OnRefresh();
		}

		bool bOk = m_Tree.RestoreState(m_pTask->m_Data.m_PropTreeState);
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

void SVExternalToolInputSelectPage::OnUndoChanges()
{
	CWnd *pParent = GetParent();

	if (nullptr != pParent)
	{
		pParent->PostMessage(SV_REMOVE_PAGES_FOR_TESTED_DLL);

		pParent->PostMessage(SV_ADD_PAGES_FOR_TESTED_DLL);

		pParent->PostMessage(PSM_SETCURSEL, c_indexOfInputValuePage, 0);
	}
}

void SVExternalToolInputSelectPage::OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	*plResult = TRUE;	// always show button
}

void SVExternalToolInputSelectPage::OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult)
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
int SVExternalToolInputSelectPage::SelectObject(std::string& rObjectName, SVRPropertyItem* pItem)
{
	
	const  SvOp::InputValueDefinition* pDef = GetInputValueDefinitionPtr(pItem);
	bool isTable(false);
	if (pDef)
	{
		isTable = (pDef->getType() == SvOp::ExDllInterfaceType::TableArray);
	}
	
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	if (!isTable)
	{
		*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::ObjectSelectorType::globalConstantItems, SvPb::ObjectSelectorType::cameraObject, SvPb::ObjectSelectorType::ppqItems, SvPb::ObjectSelectorType::toolsetItems},
			m_InspectionID, SvPb::archivable, SvDef::InvalidObjectId, true);
		SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	}
	else
	{
		*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::ObjectSelectorType::tableObjects},
			m_InspectionID, SvPb::archivable, SvDef::InvalidObjectId, true);
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

void SVExternalToolInputSelectPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
	*plResult = S_OK;

	if (pNMPropTree->pItem)
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		int iIndex = GetItemIndex(pItem);
		assert(iIndex >= 0);

		// do validation
		bool bValidated = true;

		SVObjectClass* pObject = FindObject(pItem);

		if (pObject)
		{
			// selected a value object as input;
			bValidated = ValidateValueObject(pObject, m_pTask->m_Data.m_InputDefinitions[iIndex]);
		}
		else
		{
			bValidated = S_OK == ValidateItem(pItem);
		}

		if (!bValidated)
		{
			*plResult = S_FALSE;
		}

	}// end if ( pNMPropTree->pItem )

}

// Loops through Tree Items to fill existing SVInputObjectInfo array (if input is another VO) and/or SVValueObjects with 
// constant values (if input is not another VO)
void SVExternalToolInputSelectPage::OnOK()
{
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
					int LVIndex = m_pTask->m_Data.m_InputDefinitions[iIndex].getLinkedValueIndex();
					std::string Value;
					pItem->GetItemValue(Value);
					bool done {false};
					_variant_t  array;
					switch (m_pTask->m_Data.m_InputDefinitions[iIndex].getVt())
					{
						case VT_ARRAY | VT_R8:
						{
							if (SvUl::StringToSafeArray<double>(Value, array) > 0)
							{
								m_Values.Set<_variant_t>(SvPb::ExternalInputEId + LVIndex, array);
								done = true;
							}
							break;
						}
						case VT_ARRAY | VT_I4:
						{
							if (SvUl::StringToSafeArray<long>(Value, array) > 0)
							{
								m_Values.Set<_variant_t>(SvPb::ExternalInputEId + LVIndex, array);
								done = true;
							}
							break;
						}
					}
					if (!done)
					{
						m_Values.Set<CString>(SvPb::ExternalInputEId + LVIndex, Value.c_str());
					}
				}

				pItem = pItem->GetSibling();
			}
			pGroup = pGroup->GetSibling();
		}
		m_Tree.SaveState(m_pTask->m_Data.m_PropTreeState);
		m_Values.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);
		CPropertyPage::OnOK();
	}
}

SVObjectClass* SVExternalToolInputSelectPage::FindObject(SVRPropertyItem* pItem)
{
	SVObjectClass* pObject = nullptr;
	std::string CompleteObjectName = GetName(m_InspectionID);
	std::string Name;
	pItem->GetItemValue(Name);
	std::string ToolSetName = SvUl::LoadStdString(IDS_CLASSNAME_SVTOOLSET);
	// if object name starts with tool set, inspection name must be added
	// else it must not be added, because it can be another object (e.g. "PPQ_1.Length" or "Environment.Mode.IsRun")
	if (0 == Name.compare(0, ToolSetName.length(), ToolSetName))
	{
		// Inspection name plus object name.
		CompleteObjectName += "." + Name;
	}
	else
	{
		// Object name is already complete.
		CompleteObjectName = Name;
	}

	//MZA: change function to find object from inspection child object to anz dotted name
	SVObjectManagerClass::Instance().GetObjectByDottedName(CompleteObjectName, pObject);
	return pObject;
}
bool SVExternalToolInputSelectPage::ValidateValueObject(SVObjectClass* pObject, const SvOp::InputValueDefinition& rInputedef )
{
	bool res{ false };
	if (rInputedef.getType() == SvOp::ExDllInterfaceType::TableArray)
	{
		SvOp::TableObject* pTableObject = dynamic_cast<SvOp::TableObject*>(pObject);
		res = (pTableObject != nullptr);
	}
	else
	{
		SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*>(pObject);
		res = (pValueObject != nullptr);
		if (res)
		{
			DWORD type = pValueObject->GetType();

			SvVol::SVVariantValueObjectClass* pVariant = dynamic_cast<SvVol::SVVariantValueObjectClass*>(pObject);
			if (pVariant)
			{
				type |= pVariant->GetValueType();
				type |= pVariant->GetDefaultType();
			}

			switch (rInputedef.getVt())
			{
			case VT_ARRAY | VT_R8:
				if (type != VT_R8) //allow not array objects
				{
					res = false;
				}
				break;
			case VT_ARRAY | VT_I4:
				if (type != VT_I4)
				{
					res = false;
				}
				break;
			}
		}
	}
	if (!res)
	{
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10048);
	}
	return res;
}



const SvOp::InputValueDefinition* SVExternalToolInputSelectPage::GetInputValueDefinitionPtr(SVRPropertyItem* pItem)
{
	int iIndex = GetItemIndex(pItem);
	if (m_pTask && iIndex >= 0 && m_pTask->m_Data.m_InputDefinitions.size() > iIndex)
	{
		return  &(m_pTask->m_Data.m_InputDefinitions[iIndex]);
	}
	else
	{
		return nullptr;
	}

	

}

HRESULT SVExternalToolInputSelectPage::ValidateItem(SVRPropertyItem* pItem)
{

	int iIndex = GetItemIndex(pItem);
	std::string Value;
	pItem->GetItemValue(Value);
	_variant_t  vtItem(Value.c_str());
	_variant_t  vtNew;

	const SvOp::InputValueDefinition* pDef = GetInputValueDefinitionPtr(pItem);
	if (!pDef)
		return E_FAIL;

	VARTYPE vt = static_cast<VARTYPE>(pDef->getVt());
	SvOp::ExDllInterfaceType type = pDef->getType();
	HRESULT  hr = E_FAIL;
	if (type == SvOp::ExDllInterfaceType::Array)
	{
		//@todo[mec] allow arrays of size 1
		if (vt == (VT_ARRAY | VT_R8))
		{
			if (SvUl::StringToSafeArray<double>(Value, vtNew) > 0)
			{
				hr = S_OK;
			}

		}
		else if (vt == (VT_ARRAY | VT_I4))
		{
			if (SvUl::StringToSafeArray<long>(Value, vtNew) > 0)
			{
				hr = S_OK;
			}

		}
	}
	else if (type == SvOp::ExDllInterfaceType::Scalar)
	{
		hr = ::VariantChangeTypeEx(&vtNew, &vtItem, SvDef::LCID_USA, VARIANT_ALPHABOOL, vt);
	}
	else if (type == SvOp::ExDllInterfaceType::TableArray)
	{
		if (Value.empty())
		{
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}


	if (S_OK != hr)
	{
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10048);
	}
	else
	{
		// CALL DLL TO VALIDATE RANGE
		hr = m_pTask->m_dll.ValidateValueParameter(m_TaskObjectID, (long)iIndex, vtNew);
		if (S_OK != hr)
		{
			_bstr_t bMessage;
			m_pTask->GetDLLMessageString(hr, bMessage.GetAddress());
			std::string Message = SvUl::createStdString(bMessage);
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, Message.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10049);
		}
	}

	return hr;
}

int SVExternalToolInputSelectPage::GetItemIndex(SVRPropertyItem* pItem)
{
	assert(pItem);
	return pItem->GetCtrlID() - ID_BASE;
}

std::string SVExternalToolInputSelectPage::GetName(uint32_t id) const
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
