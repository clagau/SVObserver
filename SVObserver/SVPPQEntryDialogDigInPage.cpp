//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQEntryDialogDigInPage
//* .File Name       : $Workfile:   SVPPQEntryDialogDigInPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   28 Feb 2014 08:31:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVPPQEntryDialogDigInPage.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVPPQEntryDialog.h"
#include "SVSVIMStateClass.h"
#include "SVDigitalInputObject.h"
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

IMPLEMENT_DYNCREATE(SVPPQEntryDialogDigInPageClass, CPropertyPage)

SVPPQEntryDialogDigInPageClass::SVPPQEntryDialogDigInPageClass()
: CPropertyPage(SVPPQEntryDialogDigInPageClass::IDD)
, m_AvailableItems( boost::bind( &( CListBox::GetItemData ), &m_AvailableInputCtrl, _1 ) , boost::bind( &( CListBox::SetItemData ), &m_AvailableInputCtrl, _1, _2 ) )
, m_SelectedItems( boost::bind( &( CListBox::GetItemData ), &m_SelectedInputCtrl, _1 ) , boost::bind( &( CListBox::SetItemData ), &m_SelectedInputCtrl, _1, _2 ) )
{
	//{{AFX_DATA_INIT(SVPPQEntryDialogDigInPageClass)
	m_CurrentPos = _T("");
	//}}AFX_DATA_INIT
	m_bIsTaken = FALSE;
}

SVPPQEntryDialogDigInPageClass::~SVPPQEntryDialogDigInPageClass()
{
}

void SVPPQEntryDialogDigInPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPPQEntryDialogDigInPageClass)
	DDX_Control(pDX, IDC_SELECTED_LIST, m_SelectedInputCtrl);
	DDX_Control(pDX, IDC_AVAILABLE_LIST, m_AvailableInputCtrl);
	DDX_Text(pDX, IDC_CURRENT_POSITION, m_CurrentPos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPPQEntryDialogDigInPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVPPQEntryDialogDigInPageClass)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPPQEntryDialogDigInPageClass message handlers

BOOL SVPPQEntryDialogDigInPageClass::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	ASSERT( m_pSheet );

	long lSize;
	int j;
	int nIndex;
	SVIOEntryHostStructPtrVector ppIOEntries;
	SVIOEntryHostStructPtr pIOEntry;

	// Get list of available inputs
	if( !m_pSheet->m_pPPQ->GetAllInputs( ppIOEntries ) )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17041_ErrorGettingInputs );
		DebugBreak();
	}

	lSize = static_cast<long>(ppIOEntries.size());

	for( j = 0; j < lSize; ++ j )
	{
		pIOEntry = ppIOEntries[j];

		if( pIOEntry->m_ObjectType != IO_DIGITAL_INPUT )
			continue;

		// Fill selected input list box...
		if( pIOEntry->m_PPQIndex == m_pSheet->m_lCurrentPosition )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			nIndex = m_SelectedInputCtrl.AddString( l_pObject->GetName() );
			m_SelectedItems.SetItemData( nIndex, pIOEntry );
		}// end if

		// Fill available input list box...
		if( pIOEntry->m_PPQIndex == -1 )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			nIndex = m_AvailableInputCtrl.AddString( l_pObject->GetName() );
			m_AvailableItems.SetItemData( nIndex, pIOEntry );
		}// end if

	}// end for

	m_CurrentPos.Format( "Current PPQ\r\nPosition : %d", m_pSheet->m_lCurrentPosition + 1 );

	GetDlgItem(IDC_ADD_BUTTON)->EnableWindow(m_pSheet->OkToAdd());

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVPPQEntryDialogDigInPageClass::OnAddButton() 
{
	ASSERT( m_pSheet );
	UpdateData( TRUE );

	int index = m_AvailableInputCtrl.GetCurSel();
	if( m_pSheet && index >= 0 )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_AvailableItems.GetItemData( index );

		if( l_Iter != m_AvailableItems.end() )
		{
			pIOEntry = l_Iter->second;
		}

		m_AvailableItems.ClearItemData( index );
		m_AvailableInputCtrl.DeleteString( index );

		if( !( pIOEntry.empty() ) )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			index = m_SelectedInputCtrl.AddString( l_pObject->GetName() );
			m_SelectedItems.SetItemData( index, pIOEntry );
		}
	}

	UpdateData( FALSE );
}

void SVPPQEntryDialogDigInPageClass::OnRemoveButton() 
{
	ASSERT( m_pSheet );
	UpdateData( TRUE );

	int index = m_SelectedInputCtrl.GetCurSel();
	if( m_pSheet && index >= 0 )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_SelectedItems.GetItemData( index );

		if( l_Iter != m_SelectedItems.end() )
		{
			pIOEntry = l_Iter->second;
		}

		m_SelectedItems.ClearItemData( index );
		m_SelectedInputCtrl.DeleteString( index );

		if( !( pIOEntry.empty() ) )
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );

			index = m_AvailableInputCtrl.AddString( l_pObject->GetName() );
			m_AvailableItems.SetItemData( index, pIOEntry );
		}
	}

	UpdateData( FALSE );
}

void SVPPQEntryDialogDigInPageClass::OnOK() 
{
	UpdateData( TRUE );
	ASSERT( m_pSheet );

	m_bIsTaken = ( m_SelectedInputCtrl.GetCount() > 0 );

	// Check selected input list ( remove deselected items )...
	for( int k = m_SelectedInputCtrl.GetCount() - 1; k >= 0; -- k )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_SelectedItems.GetItemData( k );

		if( l_Iter != m_SelectedItems.end() )
		{
			pIOEntry = l_Iter->second;
		}

		m_SelectedItems.ClearItemData( k );
		m_SelectedInputCtrl.DeleteString( k );

		if( !( pIOEntry.empty() ) )
		{
			pIOEntry->m_Enabled = TRUE;
			pIOEntry->m_PPQIndex = m_pSheet->m_lCurrentPosition;
		}
	}
	
	// And now add new items...( remainder in selected list box control )
	for( int i = m_AvailableInputCtrl.GetCount() - 1; i >= 0;  -- i )
	{
		SVIOEntryHostStructPtr pIOEntry;
		SVDataItemManager::iterator l_Iter = m_AvailableItems.GetItemData( i );

		if( l_Iter != m_AvailableItems.end() )
		{
			pIOEntry = l_Iter->second;
		}

		m_AvailableItems.ClearItemData( i );
		m_AvailableInputCtrl.DeleteString( i );

		if( !( pIOEntry.empty() ) )
		{
			pIOEntry->m_Enabled = FALSE;
			pIOEntry->m_PPQIndex = -1;
		}
	}

	SVSVIMStateClass::AddState( SV_STATE_MODIFIED );

	CPropertyPage::OnOK();
}

BOOL SVPPQEntryDialogDigInPageClass::OnApply() 
{
	// Set is taken flag...
	m_bIsTaken = ( m_SelectedInputCtrl.GetCount() > 0 );
	
	return CPropertyPage::OnApply();
}

