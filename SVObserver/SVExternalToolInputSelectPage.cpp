//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolInputSelectPage
//* .File Name       : $Workfile:   SVExternalToolInputSelectPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   16 Dec 2014 17:57:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVExternalToolInputSelectPage.h"
#include "SVIPDoc.h"
#include "Operators/SVExternalToolTask.h"
#include "Definitions/GlobalConst.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "SVPPQObject.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVOGui/BoundValue.h"
#include "TextDefinesSvO.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
#include "Operators/TableObject.h"
#include "Operators/SVDLLToolDefinitionStructs.h"

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
END_MESSAGE_MAP()


SVExternalToolInputSelectPage::SVExternalToolInputSelectPage(LPCTSTR Title, const SVGUID& rInspectionID, const SVGUID& rToolObjectID, const SVGUID& rTaskObjectID, int id)
	: CPropertyPage(id)
	, m_InspectionID(rInspectionID)
	, m_ToolObjectID(rToolObjectID)
	, m_TaskObjectID(rTaskObjectID)
	, m_Values {SvOg::BoundValues{ rInspectionID, rTaskObjectID }}
{
	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(m_TaskObjectID, pObject);
	m_pTask = dynamic_cast<SvOp::SVExternalToolTask*> (pObject);
	ASSERT(m_pTask);
	m_psp.pszTitle = Title;
	m_psp.dwFlags |= PSP_USETITLE;
	m_inputValueCount = m_pTask->m_Data.m_lNumInputValues;
	if (m_inputValueCount > COUNT_OF_INPUT_OUTPUT_GUIDs)
	{
		m_inputValueCount = COUNT_OF_INPUT_OUTPUT_GUIDs;
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
		ASSERT(pRoot);
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T(""));
		pRoot->SetInfoText(_T(""));

		int iID = ID_BASE - 1;	// the increment happens before using the value, so subtract one here
		std::map<std::string, SVRPropertyItem*> mapGroupItems;
		std::map<std::string, SVRPropertyItem*>::iterator iterGroup;

		SVRPropertyItem* pGroupItem = nullptr;

		int iItemCount = 0;

		for (int i = 0; i < m_inputValueCount; i++)
		{
			SvOp::InputValueDefinition& rDefinition = m_pTask->m_Data.m_aInputValueDefinitions[i];
			if (rDefinition.getType() == SvOp::ExDllInterfaceType::TableNames)
			{
				continue;
			}

			iID++;

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

			ASSERT(pGroupItem);

			SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pGroupItem);
			if (nullptr == pEdit)
			{
				break;
			}

			pEdit->SetCtrlID(iID);

			int Index = rDefinition.getLinkedValueIndex();
			// display name like: "Input 01 (Translation-X)"
			std::string sLabel = SvUl::Format(_T("%s (%s)"), m_Values.GetName(aInputObjectGUID[Index]).c_str(), m_Values.Get<CString>(aInputObjectNameGuid[Index]));
			pEdit->SetLabelText(sLabel.c_str());

			std::string strType;
			switch (rDefinition.GetVt())
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
			Description = _T(" (Type : ") + strType + _T(")  ") + Description;
			pEdit->SetInfoText(Description.c_str());

			std::string Value(m_Values.Get<CString>(aInputObject_LinkedGUID[Index]));
			if (Value.empty())
			{
				_variant_t temp = m_Values.Get<_variant_t>(aInputObjectGUID[Index]);

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
		SvPb::InspectionCmdMsgs request, response;
	*request.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
	{SvPb::ObjectSelectorType::globalConstantItems, SvPb::ObjectSelectorType::ppqItems, SvPb::ObjectSelectorType::toolsetItems},
		m_InspectionID, SvPb::archivable,GUID_NULL,true);
	SvCmd::InspectionCommands(m_InspectionID, request, &response);

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject);
	if (response.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(response.getobjectselectoritemsresponse().tree());
	}
	//@Todo[MEC][8.20] [12.07.2019] EXTERNAL TOOL
	// Enable Table Objects in Object Selector 
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
		ASSERT(iIndex >= 0);

		// do validation
		bool bValidated = true;

		SVObjectClass* pObject = FindObject(pItem);

		if (pObject)
		{
			// selected a value object as input;
			bValidated = ValidateValueObject(pObject, iIndex);
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
				ASSERT(iIndex >= 0 && iIndex < m_inputValueCount);
				if (iIndex >= 0 && iIndex < m_inputValueCount)
				{
					std::string Value;
					pItem->GetItemValue(Value);
					bool done {false};
					_variant_t  array;
					switch (m_pTask->m_Data.m_aInputValueDefinitions[iIndex].GetVt())
					{
						case VT_ARRAY | VT_R8:
						{
							if (SvUl::StringToSafeArray<double>(Value, array) > 0)
							{
								m_Values.Set<_variant_t>(aInputObjectGUID[iIndex], array);
								done = true;
							}
							break;
						}
						case VT_ARRAY | VT_I4:
						{
							if (SvUl::StringToSafeArray<long>(Value, array) > 0)
							{
								m_Values.Set<_variant_t>(aInputObjectGUID[iIndex], array);
								done = true;
							}
							break;
						}
					}
					if (!done)
					{
						m_Values.Set<CString>(aInputObjectGUID[iIndex], Value.c_str());
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
	if (0 == Name.find(ToolSetName))
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

bool SVExternalToolInputSelectPage::ValidateValueObject(SVObjectClass* pObject, int iIndex)
{
	bool res {false};
	if (m_pTask->m_Data.m_aInputValueDefinitions[iIndex].getType() == SvOp::ExDllInterfaceType::TableArray)
	{
		SvOp::TableObject* pTableObject = dynamic_cast<SvOp::TableObject*>(pObject);
		res=   (pTableObject != nullptr);
	}
	else
	{
		SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*>(pObject);
		res = (pValueObject != nullptr);
		if (res)
		{
			switch (m_pTask->m_Data.m_aInputValueDefinitions[iIndex].GetVt())
			{
				case VT_ARRAY | VT_R8:
					if (!pValueObject->isArray() || pValueObject->GetType() != VT_R8)
						res = false;
					break;
				case VT_ARRAY | VT_I4:
					if (!pValueObject->isArray() || pValueObject->GetType() != VT_I4)
						res = false;
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
	
	for (int i = 0; i < m_pTask->m_Data.m_aInputValueDefinitions.size(); i++)
	{
		if(m_pTask->m_Data.m_aInputValueDefinitions[i].getLinkedValueIndex() == iIndex)
			return &(m_pTask->m_Data.m_aInputValueDefinitions[i]);
	}
	return nullptr;
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

	VARTYPE vt = static_cast<VARTYPE>(pDef->GetVt());
	SvOp::ExDllInterfaceType type = pDef->getType();
	HRESULT  hr = E_FAIL;
	if (type == SvOp::ExDllInterfaceType::Array)
	{
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
	ASSERT(pItem);
	return pItem->GetCtrlID() - ID_BASE;
}

std::string SVExternalToolInputSelectPage::GetName(const SVGUID& guid) const
{
	std::string inspectionName;
	SvPb::InspectionCmdMsgs request, response;
	SvPb::GetObjectParametersRequest* pGetObjectNameRequest = request.mutable_getobjectparametersrequest();

	SvPb::SetGuidInProtoBytes(pGetObjectNameRequest->mutable_objectid(), guid);
	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
	if (S_OK == hr && response.has_getobjectparametersresponse())
	{
		inspectionName = response.getobjectparametersresponse().name();
	}
	return inspectionName;
}
