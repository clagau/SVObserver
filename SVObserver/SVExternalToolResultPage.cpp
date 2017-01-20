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

#pragma region Includes
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
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVExternalToolResultPage, CPropertyPage)

SVExternalToolResultPage::SVExternalToolResultPage( LPCTSTR Title, SVExternalToolDetailsSheet* pParent, int id ) 
	: CPropertyPage(SVExternalToolResultPage::IDD)
{
	m_pParentDialog = pParent;
	m_pTool = m_pParentDialog->m_pTool;
	m_pTask = m_pParentDialog->m_pTask;

	m_sTitle = Title;
    m_psp.pszTitle = m_sTitle.c_str();
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
			SVString Temp;
			rName.GetValue(Temp);

			SVString sLabel = SvUl_SF::Format( _T("%s (%s)"), rValue.GetName(), Temp.c_str());

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

			SVString sDescription = SvUl_SF::Format( _T(" (Type : %s)  %s"), Type.c_str(), SvUl_SF::createSVString( rDefinition.m_bDisplayName ).c_str() );
			pEdit->SetInfoText( sDescription.c_str() ) ;
			pEdit->SetButtonText( _T("Range"));

			SVString sValue;
			rValue.GetValue(sValue);
			pEdit->SetItemValue( sValue.c_str() );
			if( rDefinition.m_VT == VT_BSTR )
			{
				pEdit->ReadOnly();
			}
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
	
		if( m_pTask->m_Data.m_aResultValueDefinitions[iIndex].m_VT == VT_BSTR )
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
		if ( SelectObject(iIndex) == IDOK )
		{
			pItem->SetItemValue( _T("") );
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

		// do validation
		bool bValidated = true;
		// @TODO - add validation logic
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

