//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadInfoDlg
//* .File Name       : $Workfile:   SVThreadInfoDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   01 Dec 2014 13:11:10  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVThreadInfoDlg.h"
#include "afxdialogex.h"
#include "SVSystemLibrary/SVThreadManager.h"

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
{
	
}

SVThreadInfoDlg::~SVThreadInfoDlg()
{
}

void SVThreadInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SVThreadInfoDlg, CDialog)
    ON_NOTIFY(PTN_ITEMBUTTONCLICK, IDC_PROPERTYTREE, OnItemButtonClick)
    ON_NOTIFY(PTN_ITEMCHANGED, IDC_PROPERTYTREE, OnItemChanged)
	ON_BN_CLICKED(IDC_SAVE, &OnBnClickedSave)
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
	UpdateThreadInfo(SVAffinityEditAllowed);

	// For showing all threads update with SVNone.
	//UpdateThreadInfo(SVNone); 

	// Enable button to save info to a file.
	//GetDlgItem(IDC_SAVE)->ShowWindow( SW_SHOW ); 
	GetDlgItem(IDC_SAVE)->ShowWindow( SW_HIDE );

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
		if( (it->m_eAttribute & SVAffinityEditAllowed) == SVAffinityEditAllowed )
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
void SVThreadInfoDlg::InsertComboThreadItem( SVRPropertyItem* pRoot, CString name, unsigned int Affinity, unsigned int nID )
{
	SVRPropertyItemCombo* pCombo = (SVRPropertyItemCombo*)(m_ThreadList.InsertItem( new SVRPropertyItemCombo(), pRoot ));
	pCombo->SetCtrlID( nID );
	pCombo->SetLabelText( name );
	pCombo->SetInfoText(_T("This Item sets which processor pipe to run in."));
	pCombo->CreateComboBox();

	std::list<int> AffList;
	AffList.push_back( 0 );
	//AffList.push_back( 1 );
	//AffList.push_back( 2 );
	AffList.push_back( 3 );
	AffList.push_back( 4 );
	//AffList.push_back( 5 );
	//AffList.push_back( 6 );
	AffList.push_back( 7 );
	AffList.push_back( 8 );

	AddComboAffinitys( pCombo, AffList );

	pCombo->SetItemValue( Affinity );
}

void SVThreadInfoDlg::AddComboAffinitys( SVRPropertyItemCombo* pCombo, AffinityBitList& affinitys )
{
	for( AffinityBitList::const_iterator it = affinitys.begin() ; it != affinitys.end() ; ++it )
	{
		CString strName;
		if( *it > 0 )
		{
			strName.Format( _T("%d"), *it);
		}
		else
		{
			strName = _T("Not Set");
		}
		int iInsIndex = pCombo->AddString( strName );
		pCombo->SetItemData( iInsIndex, *it );
	}
}

// This function searches the property Tree for the label text Name.
bool SVThreadInfoDlg::FindName( SVRPropertyItem* pRoot, LPCTSTR Name )
{
	bool bRet = false;
	if( pRoot == nullptr )
	{
		pRoot = m_ThreadList.GetRootItem();
	}
	else
	{
		if( std::strcmp( pRoot->GetLabelText(), Name ) == 0)
		{
			return true;
		}
	}

	SVRPropertyItem* pChild = pRoot->GetChild();
	if( pChild != nullptr )
	{
		bRet = FindName( pChild, Name );
	}
	else
	{
		SVRPropertyItem* pSibling = pRoot->GetSibling();
		if( pSibling != nullptr )
		{
			bRet = FindName( pSibling, Name);
		}
	}

	return bRet;
}


void SVThreadInfoDlg::InsertThreadItem( SVRPropertyItem* pRoot, CString name, int Affinity, unsigned int nID )
{
	SVRPropertyItemEdit* pEdit = (SVRPropertyItemEdit*)(m_ThreadList.InsertItem( new SVRPropertyItemEdit(), pRoot ));

	pEdit->SetCtrlID( nID );
	pEdit->SetLabelText( name );
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
			CString strTmp = pItem->GetLabelText();
			long lValue;
			pItem->GetItemValue( lValue );
			if( SVThreadManager::Instance().IsAllowed( strTmp, SVAffinityEditAllowed ) )
			{
				SVThreadManager::Instance().SetAffinity( strTmp, lValue );
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
		CString strTmp = pItem->GetLabelText();
		SVThreadManager::Instance().SetAffinity( strTmp, lValue );
	}
	int i = 0;
}

void SVThreadInfoDlg::OnBnClickedSave()
{
	std::list<SVThreadSetup> tList;
	SVThreadManager::Instance().GetThreadInfo( tList, SVNone );
	CStdioFile file;
	if ( file.Open( "c:\\temp\\Threads.txt", CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate ) )
	{
		file.WriteString("Name,Affinity\n");
		for( std::list<SVThreadSetup>::const_iterator it = tList.begin() ; it != tList.end() ; ++it)
		{
			CString strTmp;
			strTmp.Format( "%s,%d\n",it->m_strName.c_str(), it->m_lAffinity );
			file.WriteString( strTmp );
		}
		file.Close();
	}
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVThreadInfoDlg.cpp_v  $
 * 
 *    Rev 1.0   01 Dec 2014 13:11:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
