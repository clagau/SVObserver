//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolInputSelectPage
//* .File Name       : $Workfile:   SVExternalToolInputSelectPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   03 Oct 2013 13:31:04  $
//******************************************************************************

// SVExternalToolInputSelectPage.cpp : implementation file
//

#include "stdafx.h"
#include "svobserver.h"
#include "SVGlobal.h"
#include "SVExternalToolInputSelectPage.h"
#include "SVExternalToolDetailsSheet.h"
#include "SVToolSet.h"
#include "SVIPDoc.h"
#include "SVExternalTool.h"
#include "SVExternalToolTask.h"
#include "SVToolsetOutputSelectionDialog.h"
#include "SVInspectionProcess.h"
#include "../SVObjectLibrary/SVObjectManagerClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
int SVExternalToolInputSelectPage::SelectObject(CString& rsObjectName, SVRPropertyItem* pItem )
{
	CString sObjectName;

	SVToolsetOutputSelectionDialog dlg;
	
	SVToolSetClass* pToolSet = dynamic_cast<SVToolSetClass*> ( m_pTool->GetAncestor( SVToolSetObjectType ) );
	ASSERT( pToolSet );
	dlg.PTaskObjectList = pToolSet;
//	dlg.uAttributesDesired = SV_SELECTABLE_FOR_EQUATION;
	dlg.uAttributesDesired = SV_ARCHIVABLE;

	SVObjectReferenceVector selectedInputs;
	
	SVObjectReference input(FindObject(pItem));
	selectedInputs.push_back(input);

	dlg.m_treeOutputList.SetSelection(selectedInputs);

	UINT_PTR nResult = dlg.DoModal();
	if( nResult == IDOK )
	{
		CString strTmp = dlg.m_sSelectedOutputName;
		if(!strTmp.IsEmpty())
		{
			// @PPH removed to get rid of the inspection name followed by a dot
			// sObjectName = pToolSet->GetInspection()->GetName() + CString(_T(".")) + strTmp;
			sObjectName = strTmp;
		}
	}

	rsObjectName = sObjectName;

	return static_cast<int>(nResult);
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
					BOOL bSuccess = ::SVSendMessage( rInfo.GetInputObjectInfo().PObject, SVM_DISCONNECT_OBJECT_INPUT, ( DWORD )&rInfo, NULL );
					rInfo.SetInputObject( NULL );
				}

				if ( pObject )
				{
					rValue.SetValue(iBucket, strTmp);
					rInfo.SetInputObject( pObject );
					BOOL bSuccess = ::SVSendMessage( rInfo.GetInputObjectInfo().PObject, SVM_CONNECT_OBJECT_INPUT, ( DWORD )&rInfo, NULL );
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
		//MessageBeep(MB_ICONASTERISK);
		CString sMessage(_T("Invalid Data"));
		AfxMessageBox(sMessage, MB_OK );
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
			AfxMessageBox(sMessage, MB_OK );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVExternalToolInputSelectPage.cpp_v  $
 * 
 *    Rev 1.3   03 Oct 2013 13:31:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   64 bit platform types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2013 12:05:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Sep 2013 12:37:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  863
 * SCR Title:  Fix Bug in External Tool Inputs with inspection name.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   - Change SelectObject - Remove Inspection name
 * - FindObject - Change to use GetObjectByDottedName(...
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:32:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   19 Jun 2012 12:44:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   27 Feb 2012 10:08:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  753
 * SCR Title:  Fix input selector for the External Tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issue with the input selector. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   08 Dec 2010 08:05:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   05 Nov 2010 11:02:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Jun 2007 12:46:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Aug 2006 16:28:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  556
 * SCR Title:  Fix external tool problems
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Only display normal controls if there items to be displayed, otherwise show a message stating there are no items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   25 Jul 2006 13:08:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  572
 * SCR Title:  Fix External Tool input picker to all the ability to select stings as inputs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed the attribute so that you are able to select strings as inputs to the External Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 14:07:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Aug 2004 15:13:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed OnItemButtonClick to update if an item was selected
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Jul 2004 12:32:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
