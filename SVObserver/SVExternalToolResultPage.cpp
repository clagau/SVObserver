//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolResultPage
//* .File Name       : $Workfile:   SVExternalToolResultPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Jul 2014 18:51:30  $
//******************************************************************************
// SVExternalToolResultPage.cpp : implementation file
//


#include "stdafx.h"
#include "svobserver.h"
#include "SVExternalToolResultPage.h"
#include "SVGlobal.h"
#include "SVExternalToolDetailsSheet.h"
#include "SVToolSet.h"
#include "SVIPDoc.h"
#include "SVExternalTool.h"
#include "SVExternalToolTask.h"
#include "SVInspectionProcess.h"
#include "SVResult.h"
#include "SVSetupDialogManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVExternalToolResultPage property page

IMPLEMENT_DYNCREATE(SVExternalToolResultPage, CPropertyPage)

SVExternalToolResultPage::SVExternalToolResultPage(const CString& sTitle, SVExternalToolDetailsSheet* pParent, int id ) 
	: CPropertyPage(SVExternalToolResultPage::IDD)
{
	m_pParentDialog = pParent;
	m_pTool = m_pParentDialog->m_pTool;
	m_pTask = m_pParentDialog->m_pTask;

	m_sTitle = sTitle;
    m_psp.pszTitle = m_sTitle;
    m_psp.dwFlags |= PSP_USETITLE;

	//{{AFX_DATA_INIT(SVExternalToolResultPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVExternalToolResultPage::~SVExternalToolResultPage()
{
}

void SVExternalToolResultPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVExternalToolResultPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVExternalToolResultPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVExternalToolResultPage)
	//}}AFX_MSG_MAP
    ON_NOTIFY(PTN_QUERY_SHOW_BUTTON, IDC_RESULT_LIST, OnItemQueryShowButton)
    ON_NOTIFY(PTN_ITEMBUTTONCLICK, IDC_RESULT_LIST, OnItemButtonClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVExternalToolResultPage message handlers

BOOL SVExternalToolResultPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( m_pTask->m_Data.m_lNumResultValues > 0 )
	{
		GetDlgItem( IDC_NO_RESULT_TXT )->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_RESULT_LIST )->ShowWindow(SW_SHOW);

		DWORD dwStyle;
		CRect rc;

		// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
		dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;

		// Init the control's size to cover the entire client area
		GetDlgItem(IDC_RESULT_LIST)->GetWindowRect(rc);
		ScreenToClient(rc);
		// Create SVRPropTree control
		m_Tree.Create(dwStyle, rc, this, IDC_RESULT_LIST);
		m_Tree.SetColumn(m_Tree.GetColumn() * 2);

		SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
		ASSERT( pRoot );
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("Results"));
		pRoot->SetInfoText(_T(""));

		int iID = ID_BASE-1;	// the increment happens before using the value, so subtract one here

	//	SVRPropertyItem* pGroupItem = NULL;

		
		for( int i = 0 ; i < m_pTask->m_Data.m_lNumResultValues ; i++ )
		{
			SVStringValueObjectClass& rName = m_pTask->m_Data.m_aResultObjectNames[i];  
			SVVariantValueObjectClass& rValue = m_pTask->m_Data.m_aResultObjects[i];  
			ResultValueDefinitionStruct& rDefinition = m_pTask->m_Data.m_aResultValueDefinitions[i]; 


			SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot);
			if (!pEdit)
				break;

			iID++;
			pEdit->SetCtrlID( iID );

			// display name like: "Result 01 ( )"
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
			sDescription = _T(" (Type : ") + strType +_T(")  ") + CString(rDefinition.bstrDisplayName);
			pEdit->SetInfoText( sDescription ) ;
			pEdit->SetButtonText( _T("Range"));

			CString sValue;
			rValue.GetValue(sValue);
			pEdit->SetItemValue( sValue );
			if( rDefinition.lVT == VT_BSTR )
				pEdit->ReadOnly();
			pEdit->OnRefresh();
		}// end for( int i = 0 ; i < m_pTask->m_Data.m_iNumResultValues ; i++ )

		SVRPropertyItem* pChild = pRoot->GetChild();
		while ( pChild )
		{
			pChild->Expand( TRUE );
			pChild = pChild->GetSibling();
		}
		pRoot->Expand( true );	// needed for redrawing
	}
	else
	{
		GetDlgItem( IDC_NO_RESULT_TXT )->ShowWindow( SW_SHOW );
		GetDlgItem( IDC_RESULT_LIST )->ShowWindow( SW_HIDE );

	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void SVExternalToolResultPage::OnItemQueryShowButton(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		int iIndex = GetItemIndex(pItem);
	
		if( m_pTask->m_Data.m_aResultValueDefinitions[iIndex].lVT == VT_BSTR )
		{
			*plResult = FALSE;	// Do not show button for a string, No Range available.
		}
		else
		{
			*plResult = TRUE;	// Show button for other types.
		}
	}
}

void SVExternalToolResultPage::OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	*plResult = TRUE;

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;
		int iIndex = GetItemIndex(pItem);

		// display value object picker
		CString sObjectName;
		if ( SelectObject(iIndex) == IDOK )
		{
			pItem->SetItemValue( sObjectName );
			pItem->OnRefresh();
		}
	}// end if ( pNMPropTree->pItem )
}


// display VO picker dialog and return selection
int SVExternalToolResultPage::SelectObject(int iIndex)
{

	SVVariantResultClass* pResult = dynamic_cast< SVVariantResultClass*>(m_pTask->GetResultRangeObject(iIndex)) ;

	if( pResult )
	{
		SVSetupDialogManager::Instance().SetupDialog( pResult->GetClassID(), pResult->GetUniqueObjectID(), this->m_pParentDialog );
	}

	return 0;
}

void SVExternalToolResultPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
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

		//SVObjectClass* pObject = FindObject(pItem);
		
		//if( pObject )
		{// selected a value object as input; no validation??
			
		}
		//else
		{
			//bValidated = ValidateItem(pItem) == S_OK;
		}

		if ( !bValidated )
		{
			*plResult = S_FALSE;
		}

	}// end if ( pNMPropTree->pItem )

}

// Loops through Tree Items to fill existing SVResultObjectInfo array (if input is another VO) and/or SVValueObjects with 
// constant values (if input is not another VO)
void SVExternalToolResultPage::OnOK() 
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
				SVVariantValueObjectClass& rValue = m_pTask->m_Data.m_aResultObjects[iIndex];
				int iBucket = rValue.GetLastSetIndex();


				pItem = pItem->GetSibling();
			}
			pGroup = pGroup->GetSibling();
		}

		CPropertyPage::OnOK();
	}
}


int SVExternalToolResultPage::GetItemIndex(SVRPropertyItem* pItem)
{
	ASSERT( pItem );
	return pItem->GetCtrlID() - ID_BASE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVExternalToolResultPage.cpp_v  $
 * 
 *    Rev 1.1   17 Jul 2014 18:51:30   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:32:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jul 2012 16:52:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Dec 2010 08:05:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Aug 2006 16:28:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  556
 * SCR Title:  Fix external tool problems
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Only display normal controls if there items to be displayed, otherwise show a message stating there are no items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Jul 2004 12:32:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
