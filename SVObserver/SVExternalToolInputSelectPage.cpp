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

// SVExternalToolInputSelectPage.cpp : implementation file
//
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


SVExternalToolInputSelectPage::SVExternalToolInputSelectPage(const CString& sTitle, SVExternalToolDetailsSheet* pParent, int id ) 
: CPropertyPage( id )
{
	ASSERT( m_pParentDialog );

	m_pParentDialog = pParent;
	m_pTool = m_pParentDialog->m_pTool;
	m_pTask = m_pParentDialog->m_pTask;
	m_bTabbed = false;
	m_sGroupName = sTitle;
    m_psp.pszTitle = m_sGroupName;
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
		std::map<CString, SVRPropertyItem*> mapGroupItems;
		std::map<CString, SVRPropertyItem*>::iterator iterGroup;

		SVRPropertyItem* pGroupItem = NULL;

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

			CString sGroup(rDefinition.bstrGroup);
			if( !m_bTabbed )
			{
				if ( (iterGroup = mapGroupItems.find(sGroup)) == mapGroupItems.end() )
				{	// if group does not already exist

					bool bTreeStyle = true;	// false = list-style

					pGroupItem = m_Tree.InsertItem(new SVRPropertyItem(), pRoot);
					pGroupItem->SetCanShrink(bTreeStyle);
					pGroupItem->SetLabelText(sGroup);
					pGroupItem->SetInfoText(_T(""));
					pGroupItem->Expand();

					pGroupItem->SetBold(true);

					if ( !bTreeStyle )
					{
						pGroupItem->SetBackColor(::GetSysColor(COLOR_INACTIVECAPTION));
						pGroupItem->SetForeColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
						pGroupItem->SetCanHighlight(false);
					}

					mapGroupItems[sGroup] = pGroupItem;
				}
				else	// group already exists; use existing
				{
					pGroupItem = iterGroup->second;
				}
			}// end if(!m_bTabbed)

			else	// if tabbed, filter out everything that is not in our tab (group)
			{
				if( sGroup != m_sGroupName)
				{
					continue;	// skip this item; it's not in our group
				}
			}

			iItemCount++;

			ASSERT( pGroupItem );

			SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pGroupItem);
			if (!pEdit)
				break;

			pEdit->SetCtrlID( iID );

			// display name like: "Input 01 (Translation-X)"
			CString l_Temp;
			rName.GetValue(l_Temp);

			CString sLabel = rValue.GetName();
			sLabel += _T(" (");
			sLabel += l_Temp;
			sLabel += _T(")");

			pEdit->SetLabelText( sLabel );

			CString strType;
			switch ( rDefinition.lVT )
			{
				case VT_BOOL: strType = _T("Bool");   break;
				case VT_I4:   strType = _T("Long");   break;
				case VT_R8:   strType = _T("Double"); break;
				case VT_BSTR: strType = _T("String"); break;
				default:      strType = _T("???");    break;
			}

			CString sDescription;
			sDescription = _T(" (Type : ") + strType +_T(")  ") + CString(rDefinition.bstrHelpText);
			pEdit->SetInfoText( sDescription ) ;

			CString sValue;
			rValue.GetValue(sValue);
			pEdit->SetItemValue( sValue );
			pEdit->OnRefresh();
		}// end for( int i = 0 ; i < m_pTask->m_Data.m_iNumInputValues ; i++ )

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
		CString sObjectName;
		if ( SelectObject(sObjectName, pItem) == IDOK && !sObjectName.IsEmpty())
		{
			pItem->SetItemValue( sObjectName );
			pItem->OnRefresh();
		}
	}// end if ( pNMPropTree->pItem )
}


// display VO picker dialog and return selection
int SVExternalToolInputSelectPage::SelectObject( CString& rObjectName, SVRPropertyItem* pItem )
{
	CString ObjectName;

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

	CString Value;
	pItem->GetItemValue( Value );

	SVString ItemName( Value );
	if( !ItemName.empty() )
	{
		Items.insert( ItemName );
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems( Items );
	}

	CString Title;
	CString ToolsetOutput;
	CString Filter;
	ToolsetOutput.LoadString( IDS_SELECT_TOOLSET_OUTPUT );
	Title.Format( _T("%s - %s"), ToolsetOutput, m_pTool->GetName() );
	Filter.LoadString( IDS_FILTER );

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title, ToolsetOutput, Filter, this );

	if( IDOK == Result )
	{
		ObjectName = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getDisplayLocation().c_str();
	}

	rObjectName = ObjectName;

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

		CString sValue;
		pItem->GetItemValue(sValue);

		// do validation
		bool bValidated = true;

		SVObjectClass* pObject = FindObject(pItem);
		
		if( pObject )
		{// selected a value object as input; no validation??
			
		}
		else
		{
			bValidated = ValidateItem(pItem) == S_OK;
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

	SVRPropertyItem* pGroup = NULL;

	if( m_Tree.GetRootItem() && m_Tree.GetRootItem()->GetChild() != NULL )
	{
		pGroup = m_Tree.GetRootItem()->GetChild()->GetChild();
		while ( pGroup )
		{
			SVRPropertyItem* pItem = NULL;
			pItem = pGroup->GetChild();
			while ( pItem )
			{
				int iIndex = GetItemIndex(pItem);
				ASSERT( iIndex >= 0 );
				SVVariantValueObjectClass& rValue = m_pTask->m_Data.m_aInputObjects[iIndex];
				int iBucket = 1;

				SVObjectClass* pObject = FindObject(pItem);

				CString strTmp;
				pItem->GetItemValue(strTmp);

				SVInObjectInfoStruct& rInfo = m_pTask->m_Data.m_aInputObjectInfo[iIndex];
				if ( rInfo.GetInputObjectInfo().PObject )
				{	// disconnect existing connection
					BOOL bSuccess = (::SVSendMessage( rInfo.GetInputObjectInfo().PObject, SVM_DISCONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(&rInfo), NULL ) == SVMR_SUCCESS);
					rInfo.SetInputObject( NULL );
				}

				if ( pObject )
				{
					rValue.SetValue(iBucket, strTmp);
					rInfo.SetInputObject( pObject );
					BOOL bSuccess = (::SVSendMessage( rInfo.GetInputObjectInfo().PObject, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(&rInfo), NULL ) == SVMR_SUCCESS);
					ASSERT( bSuccess );
				}
				else
				{
					rValue.SetType( m_pTask->m_Data.m_aInputValueDefinitions[iIndex].lVT);
					rValue.SetValueKeepType(iBucket, strTmp);
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
	SVObjectClass* pObject = NULL;

	CString strName;
	pItem->GetItemValue(strName);
	
	SVToolSetClass* pToolSet = dynamic_cast<SVToolSetClass*> ( m_pTool->GetAncestor( SVToolSetObjectType ) );
	ASSERT( pToolSet );

	//MZA: change function to find object from inspection child object to anz dotted name
	SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( strName ), pObject );


	return pObject;
}

HRESULT SVExternalToolInputSelectPage::ValidateItem(SVRPropertyItem* pItem)
{
	HRESULT hr = S_OK;

	// VALIDATE TYPE

	int iIndex = GetItemIndex(pItem);
	CString strValue;
	pItem->GetItemValue(strValue);
	

	VARTYPE vt = static_cast<VARTYPE>(m_pTask->m_Data.m_aInputValueDefinitions[iIndex].lVT);
	_variant_t  vtItem( strValue );
	_variant_t  vtNew;

	hr = ::VariantChangeType( &vtNew, &vtItem, VARIANT_ALPHABOOL, vt );


	if ( hr != S_OK )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_InvalidData, StdMessageParams, SvOi::Err_10048 ); 
	}
	else
	{
		// CALL DLL TO VALIDATE RANGE
		hr = m_pTask->m_dll.ValidateValueParameter( m_pTask->GetUniqueObjectID(), (long) iIndex, vtNew );
		if ( hr != S_OK )
		{
			BSTR bstrMessage = NULL;
			m_pTask->m_dll.GetMessageString(hr, &bstrMessage);
			CString sMessage(bstrMessage);
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, sMessage, StdMessageParams, SvOi::Err_10049 ); 
			::SysFreeString(bstrMessage);
		}
	}

	return hr;
}

int SVExternalToolInputSelectPage::GetItemIndex(SVRPropertyItem* pItem)
{
	ASSERT( pItem );
	return pItem->GetCtrlID() - ID_BASE;
}

