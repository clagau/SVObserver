//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQEntryDialogRemotePageClass
//* .File Name       : $Workfile:   SVPPQEntryDialogRemotePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   28 Feb 2014 08:31:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPPQEntryDialogRemotePage.h"
#include "SVPPQEntryDialog.h"
#include "SVPPQObject.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#pragma endregion Includes

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVPPQEntryDialogRemotePageClass, CPropertyPage)

SVPPQEntryDialogRemotePageClass::SVPPQEntryDialogRemotePageClass() : CPropertyPage(SVPPQEntryDialogRemotePageClass::IDD)
{
}
	
void SVPPQEntryDialogRemotePageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPPQEntryDialogRemotePageClass)
	DDX_Control(pDX, IDC_SELECTED_REMOTE_LIST, selectedInputListCtrl);
	DDX_Control(pDX, IDC_AVAILABLE_REMOTE_LIST, availableInputListCtrl);
	DDX_Text(pDX, IDC_CURRENT_POSITION, StrCurPos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPPQEntryDialogRemotePageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVPPQEntryDialogRemotePageClass)
	ON_BN_CLICKED(IDC_ADD_REMOTE_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_REMOVE_REMOTE_BUTTON, OnRemoveButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPPQEntryDialogRemotePageClass message handlers

BOOL SVPPQEntryDialogRemotePageClass::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	assert( m_pSheet );

	for(const auto& pEntry : m_pSheet->m_pPPQ->GetAllInputs())
	{
		if(pEntry->m_ObjectType != IO_REMOTE_INPUT ) { continue; }

		// Fill selected input list box...
		if(pEntry->m_PPQIndex == m_pSheet->m_lCurrentPosition )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(pEntry->m_IOId);

			int nIndex = selectedInputListCtrl.AddString( l_pObject->GetName() );
			m_SelectedItems[nIndex] = pEntry;
		}// end if

		// Fill available input list box...
		if(pEntry->m_PPQIndex == -1 )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(pEntry->m_IOId);

			int nIndex = availableInputListCtrl.AddString( l_pObject->GetName() );
			m_AvailableItems[nIndex] = pEntry;
		}// end if
	}// end for

	StrCurPos.Format( "Current PPQ\r\nPosition : %d", m_pSheet->m_lCurrentPosition + 1 );
	GetDlgItem(IDC_ADD_REMOTE_BUTTON)->EnableWindow(m_pSheet->OkToAdd());

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVPPQEntryDialogRemotePageClass::OnAddButton()
{
	assert( m_pSheet );
	UpdateData( TRUE );

	int index = availableInputListCtrl.GetCurSel();
	if( m_pSheet && index >= 0 )
	{
		SVIOEntryHostStructPtr pIOEntry;
		const auto iter = m_AvailableItems.find(index);
		if (m_AvailableItems.end() != iter)
		{
			pIOEntry = iter->second;
			m_AvailableItems.erase(iter);
		}
		availableInputListCtrl.DeleteString( index );

		if(nullptr != pIOEntry)
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			index = selectedInputListCtrl.AddString( l_pObject->GetName() );
			m_SelectedItems[index] = pIOEntry;
		}
	}// end if

	UpdateData( FALSE );
}

void SVPPQEntryDialogRemotePageClass::OnRemoveButton()
{
	assert( m_pSheet );
	UpdateData( TRUE );

	int index = selectedInputListCtrl.GetCurSel();
	if( m_pSheet && index >= 0 )
	{
		SVIOEntryHostStructPtr pIOEntry;
		const auto iter = m_SelectedItems.find(index);
		if (m_SelectedItems.end() != iter)
		{
			pIOEntry = iter->second;
			m_SelectedItems.erase(iter);
		}
		selectedInputListCtrl.DeleteString( index );

		if(nullptr != pIOEntry)
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			index = availableInputListCtrl.AddString( l_pObject->GetName() );
			m_AvailableItems[index] = pIOEntry;
		}
	}// end if

	UpdateData( FALSE );
}

void SVPPQEntryDialogRemotePageClass::OnOK()
{
	UpdateData( TRUE );
	assert( m_pSheet );

	m_bIsTaken = ( selectedInputListCtrl.GetCount() > 0 );

	// Check selected input list ( remove deselected items )...
	for( int k = selectedInputListCtrl.GetCount() - 1; k >= 0; -- k )
	{
		SVIOEntryHostStructPtr pIOEntry;
		const auto iter = m_SelectedItems.find(k);
		if (m_SelectedItems.end() != iter)
		{
			pIOEntry = iter->second;
			m_SelectedItems.erase(iter);
		}
		selectedInputListCtrl.DeleteString( k );

		if(nullptr != pIOEntry)
		{
			pIOEntry->m_Enabled = TRUE;
			pIOEntry->m_PPQIndex = m_pSheet->m_lCurrentPosition;
		}
	}

	// And now add new items...( remainder in selected list box control )
	for( int i = availableInputListCtrl.GetCount() - 1; i >= 0;  -- i )
	{
		SVIOEntryHostStructPtr pIOEntry;
		const auto iter = m_AvailableItems.find(i);
		if (m_AvailableItems.end() != iter)
		{
			pIOEntry = iter->second;
			m_AvailableItems.erase(iter);
		}
		availableInputListCtrl.DeleteString( i );

		if(nullptr != pIOEntry)
		{
			pIOEntry->m_Enabled = FALSE;
			pIOEntry->m_PPQIndex = -1;
		}// end if
	}// end for

	SVSVIMStateClass::AddState( SV_STATE_MODIFIED );

	CPropertyPage::OnOK();
}// end OnOK

BOOL SVPPQEntryDialogRemotePageClass::OnApply()
{
	// Set is taken flag...
	m_bIsTaken = ( selectedInputListCtrl.GetCount() > 0 );

	return CPropertyPage::OnApply();
}

