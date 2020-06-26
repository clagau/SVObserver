//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecuritySetupPage.cpp
//* .File Name       : $Workfile:   SVSecuritySetupPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   30 May 2014 10:47:10  $
//******************************************************************************

#pragma region Includes 
#include "stdafx.h"
//Moved to precompiled header: #include <winuser.h>
#include "resource.h"
#include "SVSecuritySetupPage.h"
#include "SVAccessClass.h"
#include "SVSecuritySetupSheet.h"
#include "SVSecurity.h"

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSVSecurityApp theApp;

IMPLEMENT_DYNCREATE(SVSecuritySetupPage, CPropertyPage)

SVSecuritySetupPage::SVSecuritySetupPage() : CPropertyPage(SVSecuritySetupPage::IDD)
{
	SVSecuritySetupPage(SVSecuritySetupPage::IDD, 0 );
}

SVSecuritySetupPage::SVSecuritySetupPage(UINT nIDTemplate, UINT nIDCaption)
	: CPropertyPage(nIDTemplate, nIDCaption)
{
	//{{AFX_DATA_INIT(SVSecuritySetupPage)
	m_bForcePrompt = FALSE;
	m_bLogOnMode = FALSE;
	m_lTimeout = 0;
	m_Group = _T("");
	//}}AFX_DATA_INIT

}

SVSecuritySetupPage::~SVSecuritySetupPage()
{
}

void SVSecuritySetupPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVSecuritySetupPage)
	DDX_Control(pDX, IDC_ACCESS_TREE, m_AccessPointTree);
	DDX_Control(pDX, IDC_NET_GROUPS, m_lbNetGroups);
	DDX_Check(pDX, IDC_FORCE_PROMPT, m_bForcePrompt);
	DDX_Check(pDX, IDC_LOG_MODE, m_bLogOnMode);
	DDX_Text(pDX, IDC_TIMEOUT_MINUTES, m_lTimeout);
	DDX_Text(pDX, IDC_GROUP_EDIT, m_Group);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVSecuritySetupPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVSecuritySetupPage)
	ON_LBN_SELCHANGE(IDC_NET_GROUPS, OnSelchangeNetGroups)
	ON_NOTIFY(TVN_SELCHANGED, IDC_ACCESS_TREE, OnSelchangedAccessTree)
	ON_EN_KILLFOCUS(IDC_GROUP_EDIT, OnKillfocusGroupEdit)
	ON_BN_CLICKED(IDC_FORCE_PROMPT, OnForcePrompt)
	ON_BN_CLICKED(IDC_CLEAR_GRP, OnClearGrp)
	ON_WM_HELPINFO()
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp)

END_MESSAGE_MAP()

BOOL SVSecuritySetupPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	pPropSheet = (SVSecuritySetupSheet*)GetParent();

	pPropSheet->InitializeGroupList(m_lbNetGroups);

	m_pAccess = pPropSheet->m_pAccess;
	// Tree Control Init.

	HTREEITEM hItem( TVI_ROOT );

	//-RPY 270215
	// set l_Index to 2 - which gets rid of the extra Root & File Menu which does not contain
	// any real items that security can be set.
	long l_Index( 2 );

	BuildTree( l_Index, hItem, true );

	m_lTimeout = m_pAccess->GetUserTimeout();
	m_bLogOnMode = m_pAccess->GetUseLogon();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVSecuritySetupPage::BuildTree( long& p_NodeIndex, HTREEITEM hParent, bool p_Root ) 
{
	HTREEITEM hItem;

	SVSecurityStorage& svStore = m_pAccess->m_svStorage;

	while( p_NodeIndex < static_cast< long >( svStore.m_aNodes.size() ) )
	{
		// Get the name from tree
		std::string Name( svStore.m_aNodes[ p_NodeIndex ].m_Name );
		bool HasData( svStore.m_aNodes[ p_NodeIndex ].m_bHasData );

		if( ! p_Root && ! HasData )
		{
			break;
		}

		hItem = m_AccessPointTree.InsertItem( TVIF_TEXT, Name.c_str(), 0, 0, 0, 0, 0, hParent, TVI_LAST );
		m_AccessPointTree.SetItemData( hItem, p_NodeIndex );

		++p_NodeIndex;

		if( p_Root && ! HasData )
		{
			BuildTree( p_NodeIndex, hItem, false );
		}
	}
}

void SVSecuritySetupPage::OnSelchangeNetGroups() 
{
	CString	Group;
	UpdateData();

	SVSecurityStorage& svStore = m_pAccess->m_svStorage;

	int iNodeIndex = GetNodeIndexFromTree();

	if( static_cast<unsigned int>(iNodeIndex) < svStore.m_aNodes.size() && iNodeIndex > 0 )
	{
		SVAccessPointNode& Node = svStore.m_aNodes[ iNodeIndex ];
		if( Node.m_bHasData && !Node.m_bDataCannotChange)
		{
			m_lbNetGroups.GetText(m_lbNetGroups.GetCurSel(), Group );
			bool bSelectEverybody = (Group.Compare(_T("Everybody")) == 0) ;
			if( m_Group.IsEmpty() || bSelectEverybody || m_Group.Compare(_T("Everybody"))==0 )
			{
				m_Group = Group;
				if( bSelectEverybody )
				{
					Node.m_bForcePrompt = false;
					m_bForcePrompt = FALSE;
				}
			}
			else
			{
				if( m_Group.Find( Group ) == -1)
				{
					m_Group += _T(",") + Group;
				}
			}
			UpdateData(FALSE);
			SetData();
		}
	}
}

void SVSecuritySetupPage::ResetGroups(void)
{
	int count = m_lbNetGroups.GetCount();

	if (count > 0)
		for (int i=0; i<count; i++)
			m_lbNetGroups.SetSel(i, FALSE);
}

void SVSecuritySetupPage::SetGroups(CString groups)
{
	ResetGroups();
	int count = m_lbNetGroups.GetCount();
	for (int i=0; i<count; i++)
	{
		CString	group;
		m_lbNetGroups.GetText(i, group);
		int nStart = -1, nFinish;
		do
		{
			if ((nFinish = groups.Find(_T(','), ++nStart)) == -1)
				nFinish = groups.GetLength();
			if (groups.Mid(nStart, (nFinish-nStart)) == group)
			{
				m_lbNetGroups.SetSel(i, TRUE);
				break;
			}
			nStart = nFinish;
		}
		while (nStart < groups.GetLength());
	}
}

BOOL SVSecuritySetupPage::OnApply() 
{
	return CPropertyPage::OnApply();
}

void SVSecuritySetupPage::OnSelchangedAccessTree(NMHDR*, LRESULT* pResult) 
{
	SVSecurityStorage& svStore = m_pAccess->m_svStorage;

	UpdateData();
	int iNodeIndex = GetNodeIndexFromTree();

	if( static_cast<unsigned int>(iNodeIndex) < svStore.m_aNodes.size() && iNodeIndex > 0 )
	{
		SVAccessPointNode& Node = svStore.m_aNodes[ iNodeIndex ];
		if( !Node.m_bHasData )
		{
			GetDlgItem( IDC_FORCE_PROMPT )->EnableWindow( FALSE );
			GetDlgItem( IDC_GROUP_EDIT )->EnableWindow( FALSE );
			m_bForcePrompt = FALSE;
			m_Group.Empty();
		}
		else
		{
			if( Node.m_bDataCannotChange )
			{
				GetDlgItem( IDC_FORCE_PROMPT )->EnableWindow( FALSE );
				GetDlgItem( IDC_GROUP_EDIT )->EnableWindow( FALSE );
			}
			else
			{
				GetDlgItem( IDC_FORCE_PROMPT )->EnableWindow( TRUE );
				GetDlgItem( IDC_GROUP_EDIT )->EnableWindow( TRUE );
			}
			m_bForcePrompt = Node.m_bForcePrompt ;
			m_Group = Node.m_NTGroup.c_str();
		}

		UpdateData(FALSE);
	}

	*pResult = 0;
	SetGroups( m_Group );
}

void SVSecuritySetupPage::OnOK() 
{

	UpdateData();
	// Set Logon mode & timeout
	m_pAccess->SetUseLogon( m_bLogOnMode ? true : false );
	m_pAccess->SetUserTimeout( m_lTimeout );

	CPropertyPage::OnOK();
}

void SVSecuritySetupPage::OnKillfocusGroupEdit() 
{
	SetData();
}

void SVSecuritySetupPage::OnForcePrompt() 
{
	SetData();
}

void SVSecuritySetupPage::SetData()
{
	int iNodeIndex = GetNodeIndexFromTree();

	SVSecurityStorage& svStore = m_pAccess->m_svStorage;

	if( static_cast<unsigned int>(iNodeIndex) < svStore.m_aNodes.size() && iNodeIndex > 0 )
	{
		SVAccessPointNode& Node = svStore.m_aNodes[ iNodeIndex ];
		UpdateData();
		if( Node.m_bHasData )
		{
			bool bEverybody = m_Group.Compare(_T("Everybody")) == 0;
			m_bForcePrompt &= !bEverybody;
			Node.m_bForcePrompt = m_bForcePrompt ? true : false ;
			Node.m_NTGroup = m_Group ;
			UpdateData(FALSE);
		}
	}

}

void SVSecuritySetupPage::OnClearGrp() 
{
	int iIndex = GetNodeIndexFromTree();
	if( iIndex > 0 && iIndex < static_cast<int>(m_pAccess->m_svStorage.m_aNodes.size()) )
	{
		SVAccessPointNode Node = m_pAccess->m_svStorage.m_aNodes[iIndex];
		if( Node.m_bHasData && !Node.m_bDataCannotChange )
		{
			m_Group.Empty();
			UpdateData(FALSE);
			SetData();
		}
	}
}

int SVSecuritySetupPage::GetNodeIndexFromTree()
{
	int iRet = -1;

	HTREEITEM hItem = m_AccessPointTree.GetSelectedItem();
	if( hItem )
	{
		iRet = static_cast<int>(m_AccessPointTree.GetItemData(hItem));
	}
	return iRet;
}


BOOL SVSecuritySetupPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// If the control ID comming thru is from static text or something invalid then
	// redirect it to the password dialog Id.
	if(( pHelpInfo->iCtrlId & 0xffff ) == 0xffff || 
		pHelpInfo->iCtrlId < 500)
	{
		pHelpInfo->iCtrlId = IDD_SECURITY_PAGE + 0x60000;
	}
	else
	{
		pHelpInfo->iCtrlId += 0x70000;
	}
	theApp.HtmlHelp(pHelpInfo->iCtrlId,HH_HELP_CONTEXT);
	return TRUE;
}

void SVSecuritySetupPage::OnHelp() 
{
	theApp.HtmlHelp(IDD_SECURITY_PAGE + 0x60000,HH_HELP_CONTEXT);
}

