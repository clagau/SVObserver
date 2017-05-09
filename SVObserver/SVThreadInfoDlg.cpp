//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadInfoDlg
//* .File Name       : $Workfile:   SVThreadInfoDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   19 Dec 2014 06:50:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVThreadInfoDlg.h"
#include "afxdialogex.h"
#include "SVSystemLibrary/SVThreadManager.h"
#include "SVOCore/SVSVIMStateClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

// SVThreadInfoDlg dialog

IMPLEMENT_DYNAMIC(SVThreadInfoDlg, CDialog)

enum 
{
	IDC_PROPERTYTREE = 100,
	PROP_THREADS_BASE = 101,
};


SVThreadInfoDlg::SVThreadInfoDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SVThreadInfoDlg::IDD, pParent)
	,m_IdIndex(0)
	,m_DisplayState(0)
	, m_bManagerEnable(FALSE)
{
	
}

SVThreadInfoDlg::~SVThreadInfoDlg()
{
}

void SVThreadInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_THREAD_MANAGER_ENABLE, m_bManagerEnable);
}


BEGIN_MESSAGE_MAP(SVThreadInfoDlg, CDialog)
    ON_NOTIFY(PTN_ITEMBUTTONCLICK, IDC_PROPERTYTREE, OnItemButtonClick)
    ON_NOTIFY(PTN_ITEMCHANGED, IDC_PROPERTYTREE, OnItemChanged)
	ON_BN_CLICKED(IDC_SAVE, &OnBnClickedSave)
	ON_BN_CLICKED(IDC_THREAD_MANAGER_ENABLE, &OnEnableCheck)
END_MESSAGE_MAP()


BOOL SVThreadInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Setup Prop tree
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;
	CRect rc;
	GetDlgItem(IDC_THREAD_LIST)->GetWindowRect(rc);
	ScreenToClient(rc);
	m_ThreadList.Create(dwStyle, rc, this, IDC_PROPERTYTREE);

	m_ThreadList.SetColumn( (long) ( rc.Width() * 0.5 ) );

	// Only show threads that have edit allowed.
	UpdateThreadInfo(SVAffinityUser);

	// Enable button to save info to a file for debugging.
	GetDlgItem(IDC_SAVE)->ShowWindow( SW_HIDE );

	m_bManagerEnable = SVThreadManager::Instance().GetThreadAffinityEnabled();

	// Enable thread list if manager is enabled.
	m_ThreadList.EnableWindow( m_bManagerEnable );

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SVThreadInfoDlg::UpdateThreadInfo( SVThreadAttribute eAttribute)
{
	SVRPropertyItem* pRoot = nullptr;
	pRoot = m_ThreadList.InsertItem( new SVRPropertyItem());
	pRoot->SetLabelText( _T("Thread Affinity Setup"));
	pRoot->SetInfoText( _T("Define Processor Affinity"));
	pRoot->SetCanShrink(false);

	SVThreadManager::ThreadList l_ThreadList ;
	SVThreadManager::Instance().GetThreadInfo(l_ThreadList, eAttribute );
	m_ThreadList.LockWindowUpdate();
	// Loop through the threads that are editable.
	for( SVThreadManager::ThreadList::const_iterator it = l_ThreadList.begin() ; it != l_ThreadList.end() ; ++it)
	{
		if( FindName(nullptr, it->m_strName.c_str() ))
			continue;	// If already in the list do not show duplicates. 
						// An example is the Display threads which have the same name.

		// Always show Affinity Edit Allowed threads...
		if( (it->m_eAttribute & SVAffinityBasic) == SVAffinityBasic )
		{
			// Name
			InsertComboThreadItem( pRoot, it->m_strName.c_str(), it->m_lAffinity, PROP_THREADS_BASE + m_IdIndex);
		}

		m_IdIndex++;
	}

	for( SVThreadManager::ThreadList::const_iterator it = l_ThreadList.begin() ; it != l_ThreadList.end() ; ++it)
	{
		if( FindName(nullptr, it->m_strName.c_str() ))
			continue;	// If already in the list do not show duplicates. 
						// An example is the Display threads which have the same name.

				 // Always show Affinity PPQ...
		if( (it->m_eAttribute & SVAffinityPPQ) == SVAffinityPPQ )
		{
			InsertThreadItem( pRoot, it->m_strName.c_str(), static_cast<int>(it->m_lAffinity), PROP_THREADS_BASE + m_IdIndex);
		}
		else	// Show Dummy thread
		if( (it->m_eAttribute & SVAffinityDummy) == SVAffinityDummy )
		{
			InsertThreadItem( pRoot, it->m_strName.c_str(), static_cast<int>(it->m_lAffinity), PROP_THREADS_BASE + m_IdIndex);
		} 
		else	// Show Acquisition Thread
		if( (it->m_eAttribute & SVAffinityAcq) == SVAffinityAcq )
		{
			InsertThreadItem( pRoot, it->m_strName.c_str(), static_cast<int>(it->m_lAffinity), PROP_THREADS_BASE + m_IdIndex);
		} 
		else	// Show Trigger Thread
		if( (it->m_eAttribute & SVAffinityTrigger) == SVAffinityTrigger )
		{
			InsertThreadItem( pRoot, it->m_strName.c_str(), static_cast<int>(it->m_lAffinity), PROP_THREADS_BASE + m_IdIndex);
		} 
		m_IdIndex++;
	}

	for( SVThreadManager::ThreadList::const_iterator it = l_ThreadList.begin() ; it != l_ThreadList.end() ; ++it)
	{
		if( FindName(nullptr, it->m_strName.c_str() ))
			continue;	// If already in the list do not show duplicates. 
						// An example is the Display threads which have the same name.

		if( (it->m_eAttribute == SVNone) && eAttribute == SVNone )
		{
			InsertThreadItem( pRoot, it->m_strName.c_str(), static_cast<int>(it->m_lAffinity), PROP_THREADS_BASE + m_IdIndex);
		}

		m_IdIndex++;
	}

	m_ThreadList.UnlockWindowUpdate();
	pRoot->Select(true);
	pRoot->Expand();

}
void SVThreadInfoDlg::InsertComboThreadItem( SVRPropertyItem* pRoot, LPCTSTR Name, unsigned int Affinity, unsigned int nID )
{
	SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)(m_ThreadList.InsertItem( new SVRPropertyItemCombo(), pRoot ));
	pCombo->SetCtrlID( nID );
	pCombo->SetLabelText( Name );
	pCombo->SetInfoText(_T("This Item sets which processor pipe to run in."));
	pCombo->CreateComboBox();

	std::list<int> AffList;
	AffList.push_back( 0 );
	//Reserve these pipes (1-4) as per IF00100.S960.003
	AffList.push_back( 5 );
	AffList.push_back( 6 );
	AffList.push_back( 7 );
	AffList.push_back( 8 );

	AddComboAffinitys( pCombo, AffList );

	pCombo->SetItemValue( Affinity );
}

void SVThreadInfoDlg::AddComboAffinitys( SVRPropertyItemCombo* pCombo, AffinityBitList& affinitys )
{
	for( AffinityBitList::const_iterator it = affinitys.begin() ; it != affinitys.end() ; ++it )
	{
		SVString Name;
		if( *it > 0 )
		{
			Name = SvUl_SF::Format( _T("%d"), *it);
		}
		else
		{
			Name = _T("Not Set");
		}
		int iInsIndex = pCombo->AddString( Name.c_str() );
		pCombo->SetItemData( iInsIndex, *it );
	}
}

// This function searches the property Tree for the label text Name.
bool SVThreadInfoDlg::FindName( SVRPropertyItem* pRoot, LPCTSTR Name )
{
	bool bRet = false;
	if( nullptr == pRoot )
	{
		pRoot = m_ThreadList.GetRootItem();
	}
	else
	{
		if( 0 == std::strcmp( pRoot->GetLabelText(), Name ) )
		{
			return true;
		}
	}

	SVRPropertyItem* pChild = pRoot->GetChild();
	if( nullptr != pChild )
	{
		bRet = FindName( pChild, Name );
	}
	else
	{
		SVRPropertyItem* pSibling = pRoot->GetSibling();
		if( nullptr != pSibling )
		{
			bRet = FindName( pSibling, Name);
		}
	}

	return bRet;
}

void SVThreadInfoDlg::InsertThreadItem( SVRPropertyItem* pRoot, LPCTSTR Name, int Affinity, unsigned int nID )
{
	SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)(m_ThreadList.InsertItem( new SVRPropertyItemEdit(), pRoot ));

	pEdit->SetCtrlID( nID );
	pEdit->SetLabelText( Name );
	pEdit->SetInfoText(_T("These Items are read only"));
	if( Affinity != 0 )
	{
		pEdit->SetItemValue( Affinity );
	}
	else
	{
		pEdit->SetItemValue("Not Set");
	}
	pEdit->ReadOnly();
	
	pEdit->SetBackColor( RGB(191, 196, 215) );
}

void SVThreadInfoDlg::OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	*plResult = TRUE;

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItemCombo* pItem = reinterpret_cast<SVRPropertyItemCombo*>(pNMPropTree->pItem);
		if ( pItem->GetCtrlID() >= PROP_THREADS_BASE )
		{
			UINT i = pItem->GetCtrlID() - PROP_THREADS_BASE;
			SVString Text = pItem->GetLabelText();
			long lValue;
			pItem->GetItemValue( lValue );
			if( SVThreadManager::Instance().IsAllowed( Text.c_str(), SVAffinityEditAllowed ) )
			{
				SVThreadManager::Instance().SetAffinity( Text.c_str(), lValue );
				SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
			}
		}
	}

}

void SVThreadInfoDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	*plResult = TRUE;

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = reinterpret_cast<SVRPropertyItem*>(pNMPropTree->pItem);
		long lValue;
		pItem->GetItemValue(lValue);
		SVThreadManager::Instance().SetAffinity( pItem->GetLabelText(), lValue );
		SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
	}
	int i = 0;
}

void SVThreadInfoDlg::OnBnClickedSave()
{
	std::list<SVThreadSetup> tList;
	SVThreadManager::Instance().GetThreadInfo( tList, SVNone );
	CStdioFile file;
	if ( file.Open( SvStl::GlobalPath::Inst().GetTempPath(_T("Threads.txt")).c_str(), CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate ) )
	{
		file.WriteString("Name,Affinity\n");
		for( std::list<SVThreadSetup>::const_iterator it = tList.begin() ; it != tList.end() ; ++it)
		{
			SVString Text = SvUl_SF::Format( _T("%s,%d\n"),it->m_strName.c_str(), it->m_lAffinity );
			file.WriteString( Text.c_str() );
		}
		file.Close();
	}
}

void SVThreadInfoDlg::OnEnableCheck()
{
	UpdateData();
	BOOL bEnabled =  SVThreadManager::Instance().GetThreadAffinityEnabled();
	if( m_bManagerEnable != bEnabled )
	{
		SVThreadManager::Instance().SetThreadAffinityEnabled(m_bManagerEnable);
	}
	m_ThreadList.EnableWindow( m_bManagerEnable );
	SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
}

