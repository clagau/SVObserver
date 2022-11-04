//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVPPQEntryDialogDigInPage.h"
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

IMPLEMENT_DYNCREATE(SVPPQEntryDialogDigInPageClass, CPropertyPage)

SVPPQEntryDialogDigInPageClass::SVPPQEntryDialogDigInPageClass() : CPropertyPage(SVPPQEntryDialogDigInPageClass::IDD)
{
}

void SVPPQEntryDialogDigInPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPPQEntryDialogDigInPageClass)
	DDX_Control(pDX, IDC_SELECTED_LIST, m_selectedInputCtrl);
	DDX_Control(pDX, IDC_AVAILABLE_LIST, m_availableInputCtrl);
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
	assert( m_pSheet );

	for(const auto& pEntry : m_pSheet->m_pPPQ->GetUsedInputs())
	{
		if( pEntry->m_ObjectType != IO_DIGITAL_INPUT )
		{
			continue;
		}

		// Fill selected input list box...
		if(pEntry->m_PPQIndex == m_pSheet->m_lCurrentPosition )
		{
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(pEntry->m_IOId);
			if (nullptr != pObject)
			{
				std::string name {pObject->GetName()};
				m_selectedInputCtrl.AddString(name.c_str());
				m_SelectedItems[name] = pEntry;
			}
		}// end if

		// Fill available input list box...
		if(pEntry->m_PPQIndex == -1)
		{
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(pEntry->m_IOId);
			if (nullptr != pObject)
			{
				std::string name {pObject->GetName()};
				m_availableInputCtrl.AddString(name.c_str());
				m_AvailableItems[name] = pEntry;
			}
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
	assert( m_pSheet );
	UpdateData( TRUE );

	int index = m_availableInputCtrl.GetCurSel();
	if( m_pSheet && index >= 0 )
	{
		CString entryName;
		m_availableInputCtrl.GetText(index, entryName);
		SVIOEntryHostStructPtr pIOEntry;
		const auto iter = m_AvailableItems.find(std::string(entryName));
		if (m_AvailableItems.end() != iter)
		{
			pIOEntry = iter->second;
			m_AvailableItems.erase(iter);
		}
		m_availableInputCtrl.DeleteString(index);

		if(nullptr != pIOEntry)
		{
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(pIOEntry->m_IOId);
			if (nullptr != pObject)
			{
				std::string name {pObject->GetName()};
				m_selectedInputCtrl.AddString(name.c_str());
				m_SelectedItems[name] = pIOEntry;
			}
		}
	}

	UpdateData( FALSE );
}

void SVPPQEntryDialogDigInPageClass::OnRemoveButton() 
{
	assert( m_pSheet );
	UpdateData( TRUE );

	int index = m_selectedInputCtrl.GetCurSel();
	if( m_pSheet && index >= 0 )
	{
		CString entryName;
		m_selectedInputCtrl.GetText(index, entryName);
		SVIOEntryHostStructPtr pIOEntry;
		const auto iter = m_SelectedItems.find(std::string(entryName));
		if (m_SelectedItems.end() != iter)
		{
			pIOEntry = iter->second;
			m_SelectedItems.erase(iter);
		}
		m_selectedInputCtrl.DeleteString(index);

		if(nullptr != pIOEntry)
		{
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId );
			if(nullptr != pObject)
			{
				std::string name{pObject->GetName()};
				m_availableInputCtrl.AddString(name.c_str());
				m_AvailableItems[name] = pIOEntry;
			}
		}
	}

	UpdateData( FALSE );
}

void SVPPQEntryDialogDigInPageClass::OnOK() 
{
	UpdateData( TRUE );
	assert( m_pSheet );

	m_bIsTaken = ( m_selectedInputCtrl.GetCount() > 0 );

	// Check selected input list ( remove deselected items )...
	for( int i = m_selectedInputCtrl.GetCount() - 1; i >= 0; -- i )
	{
		CString entryName;
		m_selectedInputCtrl.GetText(i, entryName);
		SVIOEntryHostStructPtr pIOEntry;
		const auto iter = m_SelectedItems.find(std::string(entryName));
		if (m_SelectedItems.end() != iter)
		{
			pIOEntry = iter->second;
			m_SelectedItems.erase(iter);
		}
		m_selectedInputCtrl.DeleteString(i);

		if(nullptr != pIOEntry)
		{
			pIOEntry->m_Enabled = true;
			pIOEntry->m_PPQIndex = m_pSheet->m_lCurrentPosition;
		}
	}
	
	// And now add new items...( remainder in selected list box control )
	for( int i = m_availableInputCtrl.GetCount() - 1; i >= 0;  -- i )
	{
		CString entryName;
		m_availableInputCtrl.GetText(i, entryName);
		SVIOEntryHostStructPtr pIOEntry;
		const auto iter = m_AvailableItems.find(std::string(entryName));
		if (m_AvailableItems.end() != iter)
		{
			pIOEntry = iter->second;
			m_AvailableItems.erase(iter);
		}
		m_availableInputCtrl.DeleteString(i);

		if(nullptr != pIOEntry)
		{
			pIOEntry->m_Enabled = false;
			pIOEntry->m_PPQIndex = -1;
		}
	}

	SVSVIMStateClass::AddState( SV_STATE_MODIFIED );

	CPropertyPage::OnOK();
}

BOOL SVPPQEntryDialogDigInPageClass::OnApply() 
{
	// Set is taken flag...
	m_bIsTaken = ( m_selectedInputCtrl.GetCount() > 0 );
	
	return CPropertyPage::OnApply();
}

