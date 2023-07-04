//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVStatusLibrary/SvimState.h"
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
	DDX_Control(pDX, IDC_SELECTED_REMOTE_LIST, m_selectedInputCtrl);
	DDX_Control(pDX, IDC_AVAILABLE_REMOTE_LIST, m_availableInputCtrl);
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
	Log_Assert( m_pSheet );

	for(const auto& pEntry : m_pSheet->m_pPPQ->GetUsedInputs())
	{
		if(pEntry->m_ObjectType != IO_REMOTE_INPUT ) { continue; }

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
		if(pEntry->m_PPQIndex == -1 )
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

	StrCurPos.Format( "Current PPQ\r\nPosition : %d", m_pSheet->m_lCurrentPosition + 1 );
	GetDlgItem(IDC_ADD_REMOTE_BUTTON)->EnableWindow(m_pSheet->OkToAdd());

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVPPQEntryDialogRemotePageClass::OnAddButton()
{
	Log_Assert( m_pSheet );
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
	}// end if

	UpdateData( FALSE );
}

void SVPPQEntryDialogRemotePageClass::OnRemoveButton()
{
	Log_Assert( m_pSheet );
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
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(pIOEntry->m_IOId);
			if (nullptr != pObject)
			{
				std::string name {pObject->GetName()};
				m_availableInputCtrl.AddString(name.c_str());
				m_AvailableItems[name] = pIOEntry;
			}
		}
	}// end if

	UpdateData( FALSE );
}

void SVPPQEntryDialogRemotePageClass::OnOK()
{
	UpdateData( TRUE );
	Log_Assert( m_pSheet );

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
		}// end if
	}// end for

	SvimState::AddState( SV_STATE_MODIFIED );

	CPropertyPage::OnOK();
}// end OnOK

BOOL SVPPQEntryDialogRemotePageClass::OnApply()
{
	// Set is taken flag...
	m_bIsTaken = ( m_selectedInputCtrl.GetCount() > 0 );

	return CPropertyPage::OnApply();
}

