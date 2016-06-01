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
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVPPQEntryDialogRemotePage.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVPPQEntryDialog.h"
#include "SVSVIMStateClass.h"
#include "SVRemoteInputObject.h"
#include "SVInfoStructs.h"
#include "SVPPQObject.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
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

SVPPQEntryDialogRemotePageClass::SVPPQEntryDialogRemotePageClass()
: CPropertyPage(SVPPQEntryDialogRemotePageClass::IDD)
, m_AvailableItems( boost::bind( &( CListBox::GetItemData ), &availableInputListCtrl, _1 ) , boost::bind( &( CListBox::SetItemData ), &availableInputListCtrl, _1, _2 ) )
, m_SelectedItems( boost::bind( &( CListBox::GetItemData ), &selectedInputListCtrl, _1 ) , boost::bind( &( CListBox::SetItemData ), &selectedInputListCtrl, _1, _2 ) )
{
	//{{AFX_DATA_INIT(SVPPQEntryDialogRemotePageClass)
	StrCurPos = _T("");
	//}}AFX_DATA_INIT
    m_bIsTaken = FALSE;
}

SVPPQEntryDialogRemotePageClass::~SVPPQEntryDialogRemotePageClass()
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
	ASSERT( m_pSheet );

	long lSize;
	int j;
	int nIndex;
	SVIOEntryHostStructPtrList ppIOEntries;
	SVIOEntryHostStructPtr pIOEntry;

	// Get list of available inputs
	if( !m_pSheet->m_pPPQ->GetAllInputs( ppIOEntries ) )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17042_ErrorGettingInputs );
		DebugBreak();
	}

	lSize = static_cast<long>(ppIOEntries.size());

	for( j = 0; j < lSize; ++ j )
	{
		pIOEntry = ppIOEntries[j];

		if( pIOEntry->m_ObjectType != IO_REMOTE_INPUT ) { continue; }

		// Fill selected input list box...
		if( pIOEntry->m_PPQIndex == m_pSheet->m_lCurrentPosition )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			nIndex = selectedInputListCtrl.AddString( l_pObject->GetName() );
			m_SelectedItems.SetItemData( nIndex, pIOEntry );
		}// end if

		// Fill available input list box...
		if( pIOEntry->m_PPQIndex == -1 )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			nIndex = availableInputListCtrl.AddString( l_pObject->GetName() );
			m_AvailableItems.SetItemData( nIndex, pIOEntry );
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
	ASSERT( m_pSheet );
	UpdateData( TRUE );

	int index = availableInputListCtrl.GetCurSel();
	if( m_pSheet && index >= 0 )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_AvailableItems.GetItemData( index );

		if( l_Iter != m_AvailableItems.end() )
		{
			pIOEntry = l_Iter->second;
		}

		m_AvailableItems.ClearItemData( index );
		availableInputListCtrl.DeleteString( index );

		if( !( pIOEntry.empty() ) )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			index = selectedInputListCtrl.AddString( l_pObject->GetName() );
			m_SelectedItems.SetItemData( index, pIOEntry );
		}
	}// end if

	UpdateData( FALSE );
}

void SVPPQEntryDialogRemotePageClass::OnRemoveButton()
{
	ASSERT( m_pSheet );
	UpdateData( TRUE );

	int index = selectedInputListCtrl.GetCurSel();
	if( m_pSheet && index >= 0 )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_SelectedItems.GetItemData( index );

		if( l_Iter != m_SelectedItems.end() )
		{
			pIOEntry = l_Iter->second;
		}

		m_SelectedItems.ClearItemData( index );
		selectedInputListCtrl.DeleteString( index );

		if( !( pIOEntry.empty() ) )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			index = availableInputListCtrl.AddString( l_pObject->GetName() );
			m_AvailableItems.SetItemData( index, pIOEntry );
		}
	}// end if

	UpdateData( FALSE );
}

void SVPPQEntryDialogRemotePageClass::OnOK()
{
	UpdateData( TRUE );
	ASSERT( m_pSheet );

	m_bIsTaken = ( selectedInputListCtrl.GetCount() > 0 );

	// Check selected input list ( remove deselected items )...
	for( int k = selectedInputListCtrl.GetCount() - 1; k >= 0; -- k )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_SelectedItems.GetItemData( k );

		if( l_Iter != m_SelectedItems.end() )
		{
			pIOEntry = l_Iter->second;
		}

		m_SelectedItems.ClearItemData( k );
		selectedInputListCtrl.DeleteString( k );

		if( !( pIOEntry.empty() ) )
		{
			pIOEntry->m_Enabled = TRUE;
			pIOEntry->m_PPQIndex = m_pSheet->m_lCurrentPosition;
		}
	}

	// And now add new items...( remainder in selected list box control )
	for( int i = availableInputListCtrl.GetCount() - 1; i >= 0;  -- i )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_AvailableItems.GetItemData( i );

		if( l_Iter != m_AvailableItems.end() )
		{
			pIOEntry = l_Iter->second;
		}

		m_AvailableItems.ClearItemData( i );
		availableInputListCtrl.DeleteString( i );

		if( !( pIOEntry.empty() ) )
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

