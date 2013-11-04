//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecuritySetupPage.cpp
//* .File Name       : $Workfile:   SVSecuritySetupPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:03:40  $
//******************************************************************************

#include "stdafx.h"
#include <winuser.h>
#include "resource.h"
#include "SVSecuritySetupPage.h"
#include "SVAccessClass.h"
#include "SVSecuritySetupSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	m_strGroup = _T("");
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
	DDX_Text(pDX, IDC_GROUP_EDIT, m_strGroup);
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

	SVSecurityStorage& svStore( m_pAccess->m_svStorage );
	HTREEITEM hItem( TVI_ROOT );
	long l_Index( 0 );

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

	HRESULT hr = S_OK;

	SVSecurityStorage& svStore = m_pAccess->m_svStorage;

	while( p_NodeIndex < static_cast< long >( svStore.m_aNodes.size() ) )
	{
		TV_INSERTSTRUCT     curTreeItem;

		// Get the name from tree
		CString strName( svStore.m_aNodes[ p_NodeIndex ].m_strName );
		bool l_HasData( svStore.m_aNodes[ p_NodeIndex ].m_bHasData );

		if( ! p_Root && ! l_HasData )
		{
			break;
		}

		curTreeItem.hParent = hParent;
		curTreeItem.hInsertAfter = TVI_LAST;
		curTreeItem.item.iImage = 0;
		curTreeItem.item.iSelectedImage = 0;
		curTreeItem.item.pszText =  ( LPTSTR ) strName.operator LPCTSTR();  // Node Name
		curTreeItem.item.mask = TVIF_TEXT;
		hItem = m_AccessPointTree.InsertItem( &curTreeItem );
		m_AccessPointTree.SetItemData( hItem, p_NodeIndex );

		++p_NodeIndex;

		if( p_Root && ! l_HasData )
		{
			BuildTree( p_NodeIndex, hItem, false );
		}
	}
}

void SVSecuritySetupPage::OnSelchangeNetGroups() 
{
	int	count = 0;
	CString	group;
	UpdateData();

	SVSecurityStorage& svStore = m_pAccess->m_svStorage;

	int iNodeIndex = GetNodeIndexFromTree();

	if( static_cast<unsigned int>(iNodeIndex) < svStore.m_aNodes.size() && iNodeIndex > 0 )
	{
		SVAccessPointNode& Node = svStore.m_aNodes[ iNodeIndex ];
		if( Node.m_bHasData && !Node.m_bDataCannotChange)
		{
			m_lbNetGroups.GetText(m_lbNetGroups.GetCurSel(), group );
			bool bSelectEverybody = (group.Compare(_T("Everybody")) == 0) ;
			if( m_strGroup.IsEmpty() || bSelectEverybody || m_strGroup.Compare(_T("Everybody"))==0 )
			{
				m_strGroup = group;
				if( bSelectEverybody )
				{
					Node.m_bForcePrompt = false;
					m_bForcePrompt = FALSE;
				}
			}
			else
			{
				if( m_strGroup.Find( group ) == -1)
				{
					m_strGroup += _T(",") + group;
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

CString SVSecuritySetupPage::GetGroups(void)
{
	CString	groups;
	int count = m_lbNetGroups.GetCount();

	if (count > 0)
	{
		for (int i=0; i<count; i++)
		{
			CString	group;
			if (m_lbNetGroups.GetSel(i))
			{
				m_lbNetGroups.GetText(i, group);
				if (!groups.IsEmpty())
					groups += _T(",");
				groups += group;
			}
		}
	}

	if (groups.Find(_T("Everybody")) != -1)
	{
		groups = _T("Everybody");
		SetGroups(groups);
	}

	return groups;
}

BOOL SVSecuritySetupPage::OnApply() 
{
	return CPropertyPage::OnApply();
}

void SVSecuritySetupPage::OnSelchangedAccessTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	SVSecurityStorage& svStore = m_pAccess->m_svStorage;

	int iNodeIndex = GetNodeIndexFromTree();

	if( static_cast<unsigned int>(iNodeIndex) < svStore.m_aNodes.size() && iNodeIndex > 0 )
	{
		SVAccessPointNode& Node = svStore.m_aNodes[ iNodeIndex ];
		if( !Node.m_bHasData )
		{
			GetDlgItem( IDC_FORCE_PROMPT )->EnableWindow( FALSE );
			GetDlgItem( IDC_GROUP_EDIT )->EnableWindow( FALSE );
			m_bForcePrompt = FALSE;
			m_strGroup.Empty();
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
			m_strGroup = Node.m_strNTGroup;
		}

		UpdateData(FALSE);
	}

	*pResult = 0;
	SetGroups( m_strGroup );
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
			bool bEverybody = m_strGroup.Compare(_T("Everybody")) == 0;
			m_bForcePrompt &= !bEverybody;
			Node.m_bForcePrompt = m_bForcePrompt ? true : false ;
			Node.m_strNTGroup = m_strGroup ;
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
			m_strGroup.Empty();
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
		iRet = m_AccessPointTree.GetItemData(hItem);
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
	::WinHelp( m_hWnd, _T("c:\\SVObserver\\bin\\SVObserver.hlp"),HELP_CONTEXT, pHelpInfo->iCtrlId );
	return TRUE;

}

void SVSecuritySetupPage::OnHelp() 
{
	::WinHelp( m_hWnd, _T("c:\\SVObserver\\bin\\SVObserver.hlp"),HELP_CONTEXT, IDD_SECURITY_PAGE + 0x60000 );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSecurity\SVSecuritySetupPage.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:03:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Sep 2012 18:43:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:35:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   20 Jun 2007 09:25:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   23 Mar 2006 08:59:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  558
 * SCR Title:  Expand Code in SVSecurity DLL
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added static_cast to replace implicit conversions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 Oct 2005 16:05:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified logic that checks for invalid ID in OnHelpInfo.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 Oct 2005 10:29:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnHelpInfo to call the appropriate help Id for basic types such as IDOK and IDCancel and Static_text with no ID.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Oct 2005 08:40:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnHelp and OnHelpInfo message handlers that take the help message Id, add an offset and then call into the SVObserver.hlp file.  The default operation would have tried to call into SVSecurity.hlp with normal IDs.  The offset was put in so there would be no conflict with SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Oct 2005 08:31:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnHelp and OnHelpInfo Message handlers for adding offsets to the resource IDs and calls the SVObserver Help file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Jul 2005 14:55:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   GUI Changes - Removed Auto Offline to Edit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   06 Jul 2005 13:49:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Status to password dialog, Cleaned up HRESULT return codes.  Added checkbox to SetupDlg for Auto Redirect o offline to edit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jun 2005 14:13:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   General Cleanup commented code ect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jun 2005 09:21:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSecurityStorageClass to use stl instead of the Microsoft CArray.  Fixed Load problem with SecurityStorage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 15:08:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
