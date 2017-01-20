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
#include "SVExternalToolDetailsSheet.h"
#include "SVToolSet.h"
#include "SVIPDoc.h"
#include "SVExternalTool.h"
#include "SVExternalToolTask.h"
#include "SVInspectionProcess.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "SVPPQObject.h"
#include "SVObjectLibrary\GlobalConst.h"
#include "RootObject.h"
#include "SVOGui/GlobalSelector.h"
#include "SVOGui/PPQSelector.h"
#include "SVOGui/ToolSetItemSelector.h"
#include "SVOGui/NoSelector.h"
#include "TextDefinesSvO.h"
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


SVExternalToolInputSelectPage::SVExternalToolInputSelectPage( LPCTSTR Title, SVExternalToolDetailsSheet* pParent, int id ) 
: CPropertyPage( id )
{
	ASSERT( m_pParentDialog );

	m_pParentDialog = pParent;
	m_pTool = m_pParentDialog->m_pTool;
	m_pTask = m_pParentDialog->m_pTask;
	m_bTabbed = false;
	m_sGroupName = Title;
    m_psp.pszTitle = m_sGroupName.c_str();
    m_psp.dwFlags |= PSP_USETITLE;

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

	if( m_pTask->m_Data.m_lNumInputValues > 0 )
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
		std::map<SVString, SVRPropertyItem*> mapGroupItems;
		std::map<SVString, SVRPropertyItem*>::iterator iterGroup;

		SVRPropertyItem* pGroupItem = nullptr;

		if( m_bTabbed )
		{	// create dummy group for the items; OnOK expects groups to exist
			pGroupItem = m_Tree.InsertItem(new SVRPropertyItem(), pRoot);
			pGroupItem->SetCanShrink(false);
			pGroupItem->SetLabelText(_T("Inputs"));
			pGroupItem->SetInfoText(_T(""));
			pGroupItem->Expand();
		}
		
		int iItemCount = 0;

		for( int i = 0 ; i < m_pTask->m_Data.m_lNumInputValues ; i++ )
		{
			SVStringValueObjectClass& rName = m_pTask->m_Data.m_aInputObjectNames[i];
			SVVariantValueObjectClass& rValue = m_pTask->m_Data.m_aInputObjects[i];
			InputValueDefinitionStruct& rDefinition = m_pTask->m_Data.m_aInputValueDefinitions[i];

			iID++;

			SVString GroupName = SvUl_SF::createSVString( rDefinition.m_bGroup );
			if( !m_bTabbed )
			{
				if ( (iterGroup = mapGroupItems.find(GroupName)) == mapGroupItems.end() )
				{	// if group does not already exist

					bool bTreeStyle = true;	// false = list-style

					pGroupItem = m_Tree.InsertItem(new SVRPropertyItem(), pRoot);
					pGroupItem->SetCanShrink(bTreeStyle);
					pGroupItem->SetLabelText(GroupName.c_str());
					pGroupItem->SetInfoText(_T(""));
					pGroupItem->Expand();

					pGroupItem->SetBold(true);

					if ( !bTreeStyle )
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
			}// end if(!m_bTabbed)

			else	// if tabbed, filter out everything that is not in our tab (group)
			{
				if( GroupName != m_sGroupName)
				{
					continue;	// skip this item; it's not in our group
				}
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
			SVString Temp;
			rName.GetValue(Temp);

			SVString sLabel = SvUl_SF::Format( _T("%s (%s)"), rValue.GetName(), Temp.c_str() );

			pEdit->SetLabelText( sLabel.c_str() );

			SVString Type;
			switch ( rDefinition.m_VT )
			{
				case VT_BOOL: Type = _T("Bool");   break;
				case VT_I4:   Type = _T("Long");   break;
				case VT_R8:   Type = _T("Double"); break;
				case VT_BSTR: Type = _T("String"); break;
				default:      Type = _T("???");    break;
			}

			SVString Description = SvUl_SF::createSVString( rDefinition.m_bHelpText );
			Description = _T(" (Type : ") + Type +_T(")  ") + Description;
			pEdit->SetInfoText( Description.c_str() ) ;

			SVString Value;
			rValue.GetValue( Value );
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
		SVString ObjectName;
		if ( SelectObject(ObjectName, pItem) == IDOK && !ObjectName.empty())
		{
			pItem->SetItemValue( ObjectName.c_str() );
			pItem->OnRefresh();
		}
	}// end if ( pNMPropTree->pItem )
}


// display VO picker dialog and return selection
int SVExternalToolInputSelectPage::SelectObject( SVString& rObjectName, SVRPropertyItem* pItem )
{
	if( nullptr == m_pTool ) { return 0; }

	SVToolSetClass* pToolSet = dynamic_cast<SVToolSetClass*> ( m_pTool->GetAncestor( SVToolSetObjectType ) );

	if( nullptr == pToolSet ) { return 0; }

	SVString InspectionName;
	if( nullptr != pToolSet->GetInspection() )
	{
		InspectionName = pToolSet->GetInspection()->GetName();
	}

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject );
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );

	SvOsl::SelectorOptions BuildOptions( pToolSet->GetInspection()->GetUniqueObjectID(), SV_ARCHIVABLE, pToolSet->GetUniqueObjectID() );
	SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::GlobalSelector, SvOg::PPQSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );

	SVStringSet Items;

	SVString Value;
	pItem->GetItemValue( Value );

	if( !Value.empty() )
	{
		Items.insert( Value );
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems( Items );
	}

	SVString Filter = SvUl_SF::LoadSVString( IDS_FILTER );
	SVString ToolsetOutput = SvUl_SF::LoadSVString( IDS_SELECT_TOOLSET_OUTPUT );
	SVString Title = SvUl_SF::Format( _T("%s - %s"), ToolsetOutput.c_str(), m_pTool->GetName() );

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
				ASSERT( iIndex >= 0 );
				SVVariantValueObjectClass& rValue = m_pTask->m_Data.m_aInputObjects[iIndex];
				int iBucket = 1;

				SVObjectClass* pObject = FindObject(pItem);

				SVString Value;
				pItem->GetItemValue( Value );

				SVInObjectInfoStruct& rInfo = m_pTask->m_Data.m_aInputObjectInfo[iIndex];
				if ( rInfo.GetInputObjectInfo().PObject )
				{	// disconnect existing connection
					rInfo.GetInputObjectInfo().PObject->DisconnectObjectInput(&rInfo);
					rInfo.SetInputObject( nullptr );
				}

				if ( nullptr != pObject )
				{
					rValue.SetValue(iBucket, Value );
					rInfo.SetInputObject( pObject );
					bool bSuccess = false;
					if ( rInfo.GetInputObjectInfo().PObject )
					{
						bSuccess = rInfo.GetInputObjectInfo().PObject->ConnectObjectInput(&rInfo);
					}
					ASSERT( bSuccess );
				}
				else
				{
					rValue.SetType( m_pTask->m_Data.m_aInputValueDefinitions[iIndex].m_VT);
					rValue.SetValueKeepType(iBucket, Value.c_str());
				}

				pItem = pItem->GetSibling();
			}
			pGroup = pGroup->GetSibling();
		}
		m_Tree.SaveState( m_pTask->m_Data.m_PropTreeState );
		CPropertyPage::OnOK();
	}
}

SVObjectClass* SVExternalToolInputSelectPage::FindObject(SVRPropertyItem* pItem)
{
	SVObjectClass* pObject = nullptr;

	SVString CompleteObjectName;
	SVString Name;
	pItem->GetItemValue(Name);
	
	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*> ( m_pTool->GetAncestor( SVInspectionObjectType ) );
	ASSERT( pInspection );

	//If pointer is a nullptr then name is empty
	if( nullptr != pInspection )
	{ 
		CompleteObjectName = pInspection->GetCompleteName(); 
	}
	SVString ToolSetName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVTOOLSET );

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
	SVString Value;
	pItem->GetItemValue(Value);
	

	VARTYPE vt = static_cast<VARTYPE>(m_pTask->m_Data.m_aInputValueDefinitions[iIndex].m_VT);
	_variant_t  vtItem( Value.c_str() );
	_variant_t  vtNew;

	hr = ::VariantChangeType( &vtNew, &vtItem, VARIANT_ALPHABOOL, vt );

	if ( S_OK != hr )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10048 ); 
	}
	else
	{
		// CALL DLL TO VALIDATE RANGE
		hr = m_pTask->m_dll.ValidateValueParameter( m_pTask->GetUniqueObjectID(), (long) iIndex, vtNew );
		if ( S_OK != hr )
		{
			_bstr_t bMessage;
			m_pTask->GetDLLMessageString(hr, bMessage.GetAddress());
			SVString Message = SvUl_SF::createSVString( bMessage);
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, Message.c_str(), SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10049 ); 
		}
	}

	return hr;
}

int SVExternalToolInputSelectPage::GetItemIndex(SVRPropertyItem* pItem)
{
	ASSERT( pItem );
	return pItem->GetCtrlID() - ID_BASE;
}

