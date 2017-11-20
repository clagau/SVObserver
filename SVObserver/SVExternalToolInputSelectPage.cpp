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
#include "svobserver.h"
#include "SVGlobal.h"
#include "SVExternalToolInputSelectPage.h"
#include "SVIPDoc.h"
#include "SVExternalToolTask.h"
#include "Definitions/GlobalConst.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "SVPPQObject.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "RootObject.h"
#include "SVOGui/BoundValue.h"
#include "SVOGui/GlobalSelector.h"
#include "SVOGui/PPQSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
#include "SVOGui/NoSelector.h"
#include "TextDefinesSvO.h"
#include "GuiCommands/GetObjectName.h"
#include "GuiCommands/GetInstanceIDByTypeInfo.h"
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

std::string getInputValueTag(int index)
{
	return SvUl::Format(_T("InputValue%d"), index);
}

std::string getInputValueLinkTag(int index)
{
	return SvUl::Format(_T("InputValueLink%d"), index);
}

std::string getInputNameTag(int index)
{
	return SvUl::Format(_T("InputName%d"), index);
}

std::map<std::string, SvOg::BoundValue> createValueMap()
{
	std::map<std::string, SvOg::BoundValue> retValue;
	for (int i = 0; i < COUNT_OF_INPUT_OUTPUT_GUIDs; i++)
	{
		retValue[getInputValueTag(i)] = aInputObjectGUID[i];
		retValue[getInputValueLinkTag(i)] = aInputObject_LinkedGUID[i];
		retValue[getInputNameTag(i)] = aInputObjectNameGuid[i];
	}
	return retValue;
}


SVExternalToolInputSelectPage::SVExternalToolInputSelectPage( LPCTSTR Title, const SVGUID& rInspectionID, const SVGUID& rToolObjectID, const SVGUID& rTaskObjectID, int id )
: CPropertyPage( id )
, m_InspectionID( rInspectionID )
, m_ToolObjectID( rToolObjectID )
, m_TaskObjectID( rTaskObjectID )
, m_Values(SvOg::BoundValues(rInspectionID, rTaskObjectID, createValueMap() ))
{
	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(m_TaskObjectID, pObject);
	m_pTask = dynamic_cast<SVExternalToolTask*>(pObject);
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
	
	DWORD dwStyle;
	CRect rc;

	m_Values.Init();

	if( m_inputValueCount > 0 )
	{
		// Hide the text if we have inputs
		GetDlgItem( IDC_NO_INPUT_TXT )->ShowWindow( SW_HIDE );
		GetDlgItem( IDC_INPUT_LIST )->ShowWindow( SW_SHOW );

		// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
		dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;

		// Init the control's size to cover the entire client area
		GetDlgItem(IDC_INPUT_LIST)->GetWindowRect(rc);
		ScreenToClient(rc);
		// Create SVRPropTree control
		m_Tree.Create(dwStyle, rc, this, IDC_INPUT_LIST_TREE);
		m_Tree.SetColumn(m_Tree.GetColumn() * 2);

		SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
		ASSERT( pRoot );
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T(""));
		pRoot->SetInfoText(_T(""));

		int iID = ID_BASE-1;	// the increment happens before using the value, so subtract one here
		std::map<std::string, SVRPropertyItem*> mapGroupItems;
		std::map<std::string, SVRPropertyItem*>::iterator iterGroup;

		SVRPropertyItem* pGroupItem = nullptr;

		int iItemCount = 0;

		for( int i = 0 ; i < m_inputValueCount; i++ )
		{
			InputValueDefinitionStruct& rDefinition = m_pTask->m_Data.m_aInputValueDefinitions[i];

			iID++;

			std::string GroupName = SvUl::createStdString( rDefinition.m_bGroup );

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

			ASSERT( pGroupItem );

			SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pGroupItem);
			if( nullptr == pEdit)
			{
				break;
			}

			pEdit->SetCtrlID( iID );

			// display name like: "Input 01 (Translation-X)"
			std::string sLabel = SvUl::Format( _T("%s (%s)"), m_Values.GetName(getInputValueTag(i)).c_str(), m_Values.Get<CString>(getInputNameTag(i)) );
			pEdit->SetLabelText( sLabel.c_str() );

			std::string Type;
			switch ( rDefinition.m_VT )
			{
				case VT_BOOL: Type = _T("Bool");   break;
				case VT_I4:   Type = _T("Long");   break;
				case VT_R8:   Type = _T("Double"); break;
				case VT_BSTR: Type = _T("String"); break;
				default:      Type = _T("???");    break;
			}

			std::string Description = SvUl::createStdString( rDefinition.m_bHelpText );
			Description = _T(" (Type : ") + Type +_T(")  ") + Description;
			pEdit->SetInfoText( Description.c_str() ) ;

			std::string Value(m_Values.Get<CString>(getInputValueLinkTag(i)));
			if (Value.empty())
			{
				Value = m_Values.Get<CString>(getInputValueTag(i));
			}
			pEdit->SetItemValue( Value.c_str() );
			pEdit->OnRefresh();
		}

		bool bOk = m_Tree.RestoreState( m_pTask->m_Data.m_PropTreeState );
		if ( !bOk )
		{
			SVRPropertyItem* pChild = pRoot->GetChild();
			while ( pChild )
			{
				pChild->Expand( iItemCount <= MAX_ITEMS_FOR_AUTO_EXPAND );
				pChild = pChild->GetSibling();
			}
			pRoot->Expand( true );	// needed for redrawing
		}
	}
	else
	{
		GetDlgItem( IDC_NO_INPUT_TXT )->ShowWindow( SW_SHOW );
		GetDlgItem( IDC_INPUT_LIST )->ShowWindow( SW_HIDE );
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

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		int iIndex = GetItemIndex(pItem);

		// display value object picker
		std::string ObjectName;
		if ( SelectObject(ObjectName, pItem) == IDOK && !ObjectName.empty())
		{
			pItem->SetItemValue( ObjectName.c_str() );
			pItem->OnRefresh();
		}
	}// end if ( pNMPropTree->pItem )
}


// display VO picker dialog and return selection
int SVExternalToolInputSelectPage::SelectObject( std::string& rObjectName, SVRPropertyItem* pItem )
{
	std::string InspectionName = GetName(m_InspectionID);

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, std::string( _T("") ) );

	SvOsl::SelectorOptions BuildOptions( m_InspectionID, SvDef::SV_ARCHIVABLE, GetToolSetGUID() );
	SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::GlobalSelector, SvOg::PPQSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );

	SvDef::StringSet Items;

	std::string Value;
	pItem->GetItemValue( Value );

	if( !Value.empty() )
	{
		Items.insert( Value );
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems( Items );
	}

	std::string Filter = SvUl::LoadStdString( IDS_FILTER );
	std::string ToolsetOutput = SvUl::LoadStdString( IDS_SELECT_TOOLSET_OUTPUT );
	std::string Title = SvUl::Format( _T("%s - %s"), ToolsetOutput.c_str(),GetName(m_ToolObjectID) );

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), ToolsetOutput.c_str(), Filter.c_str(), this );

	if( IDOK == Result )
	{
		rObjectName = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getDisplayLocation().c_str();
	}

	return static_cast<int>( Result );
}

void SVExternalToolInputSelectPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;
	*plResult = S_OK;
	
	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		int iIndex = GetItemIndex(pItem);
		ASSERT( iIndex >= 0 );

		// do validation
		bool bValidated = true;

		SVObjectClass* pObject = FindObject(pItem);
		
		if( pObject )
		{// selected a value object as input; no validation??
			
		}
		else
		{
			bValidated = S_OK == ValidateItem(pItem);
		}

		if ( !bValidated )
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

	if( m_Tree.GetRootItem() && nullptr != m_Tree.GetRootItem()->GetChild() )
	{
		pGroup = m_Tree.GetRootItem()->GetChild()->GetChild();
		while ( pGroup )
		{
			SVRPropertyItem* pItem = nullptr;
			pItem = pGroup->GetChild();
			while ( pItem )
			{
				int iIndex = GetItemIndex(pItem);
				ASSERT( iIndex >= 0 && iIndex < m_inputValueCount );
				if (iIndex >= 0 && iIndex < m_inputValueCount)
				{
					SVObjectClass* pObject = FindObject(pItem);

					std::string Value;
					pItem->GetItemValue(Value);
					m_Values.Set<CString>(getInputValueTag(iIndex), Value.c_str());
				}

				pItem = pItem->GetSibling();
			}
			pGroup = pGroup->GetSibling();
		}
		m_Tree.SaveState( m_pTask->m_Data.m_PropTreeState );
		m_Values.Commit(true);
		CPropertyPage::OnOK();
	}
}

SVObjectClass* SVExternalToolInputSelectPage::FindObject(SVRPropertyItem* pItem)
{
	SVObjectClass* pObject = nullptr;

	std::string CompleteObjectName = GetName(m_InspectionID);
	std::string Name;
	pItem->GetItemValue(Name);
	
	std::string ToolSetName = SvUl::LoadStdString( IDS_CLASSNAME_SVTOOLSET );

	// if object name starts with tool set, inspection name must be added
	// else it must not be added, because it can be another object (e.g. "PPQ_1.Length" or "Environment.Mode.IsRun")
	if( 0 == Name.find(ToolSetName) )
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
	SVObjectManagerClass::Instance().GetObjectByDottedName( CompleteObjectName, pObject );


	return pObject;
}

HRESULT SVExternalToolInputSelectPage::ValidateItem(SVRPropertyItem* pItem)
{
	HRESULT hr = S_OK;

	// VALIDATE TYPE

	int iIndex = GetItemIndex(pItem);
	std::string Value;
	pItem->GetItemValue(Value);
	

	VARTYPE vt = static_cast<VARTYPE>(m_pTask->m_Data.m_aInputValueDefinitions[iIndex].m_VT);
	_variant_t  vtItem( Value.c_str() );
	_variant_t  vtNew;

	hr = ::VariantChangeType( &vtNew, &vtItem, VARIANT_ALPHABOOL, vt );

	if ( S_OK != hr )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10048 ); 
	}
	else
	{
		// CALL DLL TO VALIDATE RANGE
		hr = m_pTask->m_dll.ValidateValueParameter(m_TaskObjectID, (long) iIndex, vtNew );
		if ( S_OK != hr )
		{
			_bstr_t bMessage;
			m_pTask->GetDLLMessageString(hr, bMessage.GetAddress());
			std::string Message = SvUl::createStdString( bMessage);
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, Message.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10049 ); 
		}
	}

	return hr;
}

int SVExternalToolInputSelectPage::GetItemIndex(SVRPropertyItem* pItem)
{
	ASSERT( pItem );
	return pItem->GetCtrlID() - ID_BASE;
}

std::string SVExternalToolInputSelectPage::GetName(const SVGUID& guid) const
{
	std::string inspectionName;
	typedef SvCmd::GetObjectName Command;
	typedef SVSharedPtr<Command> CommandPtr;

	CommandPtr commandPtr(new Command(guid));
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		inspectionName = commandPtr->GetName();
	}
	return inspectionName;
}

GUID SVExternalToolInputSelectPage::GetToolSetGUID() const
{
	GUID toolsetGUID = GUID_NULL;

	typedef SvCmd::GetInstanceIDByTypeInfo Command;
	typedef SVSharedPtr<Command> CommandPtr;

	SvDef::SVObjectTypeInfoStruct info(SvDef::SVToolSetObjectType);
	CommandPtr commandPtr = CommandPtr(new Command(m_InspectionID, info));
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		toolsetGUID = commandPtr->GetInstanceID();
	}

	return toolsetGUID;
}