// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOFileConfigDlg
// * .File Name       : $Workfile:   SVOFileConfigDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   03 Feb 2014 17:07:36  $
// ******************************************************************************

#include "stdafx.h"
#include <stdio.h>
#include <comdef.h>
#include <algorithm>
#include <set>

#include "SVOFileConfig.h"
#include "SVOFileConfigDlg.h"
#include "SVLibrary/SVBStr.h"

#include "SVConfigurationLibrary/SVOCMGlobals.h"

#include "SVOLibrary/SVOLibrary.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "AutoBuild/AutoBuild.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVOFileConfigDlg::SVOFileConfigDlg(CWnd* pParent /*=NULL*/)
: CDialog(SVOFileConfigDlg::IDD, pParent), m_XMLCTree( mTree )
{
	//{{AFX_DATA_INIT(SVOFileConfigDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

SVOFileConfigDlg::~SVOFileConfigDlg()
{
}

void SVOFileConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOFileConfigDlg)
	DDX_Control(pDX, IDC_LIST1, mList);
	DDX_Control(pDX, IDC_TREE1, mTree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVOFileConfigDlg, CDialog)
	//{{AFX_MSG_MAP(SVOFileConfigDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_BN_CLICKED(IDC_BUTTONLOAD_SVX, OnButtonloadSvx)
	ON_BN_CLICKED(IDC_BUTTONSAVE_SVX, OnButtonsaveSvx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOFileConfigDlg message handlers

BOOL SVOFileConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_ulCurrentVersion = 0x00050100; // initialize version with 501.
	CString l_Text;
	GetWindowText( l_Text );
	l_Text += " - With Detect and Repair IOEntries - version";
	l_Text +=  STRPRODUCTVER;
	SetWindowText( l_Text );
	m_strLastDirectory = "c:\\Run";
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void SVOFileConfigDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR SVOFileConfigDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void SVOFileConfigDlg::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	mList.ResetContent();

	if ( pNMTreeView )
	{
		if ( pNMTreeView->itemNew.lParam )
		{
			_variant_t svvData( *( ( VARIANT* )pNMTreeView->itemNew.lParam ) );

			int iIndex = -1;

			if ( svvData.vt & SVArrayType )
			{
				SVSAFEARRAY svArray( svvData );
				long lSize( static_cast<long>(svArray.size()) );

				for ( long l = 0; l < lSize; l++)
				{
					_variant_t l_Variant;

					if ( svArray.GetElement( l, l_Variant ) == S_OK )
					{
						_bstr_t l_String( l_Variant );

						iIndex = mList.InsertString( l, static_cast< LPCTSTR >( l_String ) );
					}
				}
			}
			else
			{
				_bstr_t l_String;

				if( svvData.vt != VT_EMPTY && svvData.vt != VT_NULL )
				{
					l_String = svvData;
				}
				else
				{
					l_String = _T( "<Empty>" );
				}

				iIndex = mList.InsertString( 0, static_cast< LPCTSTR >( l_String ) );
			}
		}
	}
	
	*pResult = 0;
}
bool SVOFileConfigDlg::GetDirectoryRemoveLevel( CString& p_strFile, int p_level )
{
	bool l_bChanged = false;
	if( p_strFile.ReverseFind( '.' ) > 0 )
	{
		int pos = p_strFile.ReverseFind( _T('\\') );
		int count = 0;
		while( pos > 0 && count < p_level )
		{
			int pos = p_strFile.ReverseFind( _T('\\') );
			if( pos > 0 )
			{
				p_strFile = p_strFile.Mid( 0, pos );
				count++;
				l_bChanged = true;
			}
		}
	}
	return l_bChanged;
}

void SVOFileConfigDlg::OnButtonloadSvx() 
{
	// TODO: Add your control notification handler code here

	SVBStr	bstrFileName;


	while (1)
	{
		CFileDialog dlg( TRUE, ".svx", NULL, 
										 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
										 OFN_ENABLESIZING | OFN_EXPLORER,
										 "SVObserver Environment Configuration Files (*.svx)|*.svx||", this );

		dlg.m_ofn.lpstrTitle = "Select Configuration File";
		dlg.m_ofn.lpstrInitialDir = m_strLastDirectory;

		if (dlg.DoModal () != IDOK)
		{
			break;
		}

		m_strLastDirectory = dlg.m_ofn.lpstrFile;

		GetDirectoryRemoveLevel( m_strLastDirectory, 1);


		mTree.SetRedraw( false );

		bstrFileName = dlg.GetPathName();

		m_XMLCTree.Clear();
		SVOCMLoadConfiguration(	m_ulCurrentVersion, m_ulCurrentVersion, bstrFileName, m_XMLCTree);
		if( m_ulCurrentVersion >= 0x50100)
		{
			CheckConfiguration();
		}
		else
		{
			MessageBox("This program will only detect IOEntry errors in Configurations saved with SVObserver 5.00 or newer");
		}
		mTree.SetRedraw( true );
			
	break;
	}
	
}

void SVOFileConfigDlg::OnButtonsaveSvx() 
{
	// TODO: Add your control notification handler code here

	SVBStr	bstrFileName;

	while (1)
	{
		CFileDialog dlg( TRUE, ".svx", NULL, 
										 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
										 OFN_ENABLESIZING | OFN_EXPLORER,
										 "SVObserver Environment Configuration Files (*.svx)|*.svx||", this );

		dlg.m_ofn.lpstrTitle = "Select Configuration File";
		dlg.m_ofn.lpstrInitialDir = m_strLastDirectory;

		if (dlg.DoModal () != IDOK)
		{
			break;
		}
		

		m_strLastDirectory = dlg.m_ofn.lpstrFile;
		GetDirectoryRemoveLevel( m_strLastDirectory, 1);

		bstrFileName = dlg.GetPathName();

		FixConfiguration();

		SVOCMSaveConfiguration(	m_ulCurrentVersion, 
								m_ulCurrentVersion, 
								bstrFileName,
								m_XMLCTree);


	break;
	}
	
}

BOOL SVOFileConfigDlg::DestroyWindow()
{
	if ( mTree.GetSafeHwnd() )
	{
		m_XMLCTree.Clear();
	}

	return CDialog::DestroyWindow();
}

void SVOFileConfigDlg::FixConfiguration()
{
	for( HTIStringPairs::iterator it = m_IOEntryPairs.begin() ; it != m_IOEntryPairs.end() ; ++it)
	{
		CString Inspection;
		if( ParseInspection( (*it).second, Inspection ) )
		{
			if( std::find( m_Inspections.begin(), m_Inspections.end(), Inspection) == m_Inspections.end() )
			{
				if( mTree.DeleteItem((*it).first) )
				{
					OutputDebugString( "Deleting "+(*it).second );
				}
			}
		}
		CString l_strPPQ;
		if( ParsePPQ( (*it).second, l_strPPQ ) )
		{
			if( std::find( m_PPQs.begin(), m_PPQs.end(), l_strPPQ) == m_PPQs.end() )
			{
				if( mTree.DeleteItem((*it).first) )
				{
					OutputDebugString( "Deleting "+(*it).second );
				}
			}
		}

	}

	// Find any IOEntries that are duplicates and remove them.
	std::set<HTREEITEM> l_DeletedHTreeItems;
	for( HTIStringPairs::iterator it = m_IOEntryPairs.begin() ; it != m_IOEntryPairs.end() ; ++it)
	{
		for( HTIStringPairs::iterator it1 = it ; it1 != m_IOEntryPairs.end() ; ++it1)
		{
			if( it1 != it )
			{
				if( it1->second == it->second )
				{
					if( l_DeletedHTreeItems.find( it->first ) == l_DeletedHTreeItems.end() )
					{
						l_DeletedHTreeItems.insert( it1->first );
						if( mTree.DeleteItem( it1->first ))
						{
							OutputDebugString( "Deleting " + it->second + "\n" );
						}
					}
				}
			}
		}
	}


	// Loop thru IO Entries and rename their indexes...
	HTREEITEM l_Root = mTree.GetRootItem();
	HTREEITEM l_Child = mTree.GetNextSiblingItem( l_Root );
	HTREEITEM l_Current = l_Child;
	m_IOEntryPairs.clear();
	m_Inspections.clear();

	while( l_Current != NULL )
	{
		CString l_strName = mTree.GetItemText( l_Current );
		CString l_strTmp;

		if( l_strName == "IO")
		{
			ReIndexIOEntries(l_Current);
			break;
		}

		l_Current = mTree.GetNextSiblingItem( l_Current );
	}
}

void SVOFileConfigDlg::ReIndexIOEntries(HTREEITEM p_Item)
{
	int count = 0;
	HTREEITEM l_Current = mTree.GetChildItem( p_Item);
	// Find all IOEntries.
	while( l_Current != NULL )
	{
		CString l_strName = mTree.GetItemText( l_Current);

		if( l_strName.Find( "IOEntry" ) >= 0 )
		{
			CString l_strNewName;
			l_strNewName.Format( "IOEntry%d", count);
			count++;
			mTree.SetItemText(l_Current, l_strNewName );
		}
		l_Current = mTree.GetNextSiblingItem(l_Current);
	}

	// Update NumberOfIOEntries Last.
	l_Current = mTree.GetChildItem( p_Item);
	while( l_Current != NULL )
	{
		CString l_strName = mTree.GetItemText( l_Current);

		if( l_strName == "NumberOfIOEntries")
		{
			DWORD l_dwCount = static_cast<DWORD>(mTree.GetItemData( l_Current ));
			VARIANT* l_pVt = (VARIANT*)l_dwCount;
			l_pVt->lVal = count;
			break;
		}
		l_Current = mTree.GetNextSiblingItem(l_Current);
	}
}


void SVOFileConfigDlg::CheckConfiguration()
{
	HTREEITEM l_Root = mTree.GetRootItem();
	HTREEITEM l_Child = mTree.GetNextSiblingItem( l_Root );
	HTREEITEM l_Current = l_Child;
	m_IOEntryPairs.clear();
	m_Inspections.clear();

	while( l_Current != NULL )
	{
		CString l_strName = mTree.GetItemText( l_Current );
		CString l_strTmp;

		if( l_strName == "IO")
		{
			GetIOChildren(l_Current);
		}

		if( l_strName == "Inspection")
		{
			GetInspections(l_Current);
		}

		if( l_strName == "PPQ")
		{
			GetPPQs(l_Current);
		}

		l_Current = mTree.GetNextSiblingItem(l_Current);
		
	}

	// search IOEntries for invalid inspections
	CString strMainMessage;
	
	for( HTIStringPairs::iterator it = m_IOEntryPairs.begin() ; it != m_IOEntryPairs.end() ; ++it)
	{
		CString Inspection;
		if( ParseInspection( (*it).second, Inspection ) )
		{
			if( std::find( m_Inspections.begin(), m_Inspections.end(), Inspection) == m_Inspections.end() )
			{
				CString strMessage;
				strMessage.Format( "%s\n", (*it).second);
				strMainMessage+= strMessage;

				if( mTree.DeleteItem((*it).first) )
					OutputDebugString( "Deleting "+(*it).second );
			}
		}
		CString l_strPPQ;
		if( ParsePPQ( (*it).second, l_strPPQ ) )
		{
			if( std::find( m_PPQs.begin(), m_PPQs.end(), l_strPPQ) == m_PPQs.end() )
			{
				CString strMessage;
				strMessage.Format( "%s\n", (*it).second);
				strMainMessage+= strMessage;

				if( mTree.DeleteItem((*it).first) )
					OutputDebugString( "Deleting "+(*it).second );
			}
		}

	}

	if( strMainMessage.GetLength() > 0 )
	{
		MessageBox(strMainMessage,"The following IOEntries are Invalid!", MB_OK);
	}

}
bool SVOFileConfigDlg::ParseInspection( CString p_strName, CString& p_rstrOut)
{
	bool l_bRet = false;
	CString strRet ="";
	int loc = p_strName.Find('.');
	if( loc >= 0 )
	{
		l_bRet = true;
		strRet = p_strName.Left( loc );
		if( strRet == "DIO" ||
			(strRet.Find("PPQ",0) == 0))
		{
			l_bRet = false;
		}
		else
		{
			p_rstrOut = strRet;
		}
	}
	return l_bRet;
}

bool SVOFileConfigDlg::ParsePPQ( CString p_strName, CString& p_rstrOut)
{
	bool l_bRet = false;
	CString strRet ="";
	int loc = p_strName.Find('.');
	if( loc >= 0 )
	{
		strRet = p_strName.Left( loc );
		if( strRet.Find("PPQ",0) == 0)
		{
			l_bRet = true;
			p_rstrOut = strRet;
		}
	}
	return l_bRet;
}


void SVOFileConfigDlg::GetIOChildren(HTREEITEM p_Item)
{
	HTREEITEM l_Current = mTree.GetChildItem( p_Item);
	while( l_Current != NULL )
	{
		CString l_strName = mTree.GetItemText( l_Current);

		if( l_strName.Find( "IOEntry" ) >= 0 )
		{
			HTREEITEM l_InspItem = mTree.GetChildItem( l_Current );
			while( l_InspItem != NULL )
			{
				CString Name = mTree.GetItemText( l_InspItem );
				if( Name == "IOEntryName" )
				{
					DWORD l_InspData = static_cast<DWORD>(mTree.GetItemData( l_InspItem ));
					VARIANT* l_vt = (VARIANT*)l_InspData;
					if( l_vt->vt == VT_BSTR )
					{
						CString l_IOEntryName = l_vt->bstrVal;
						HTIStringPair l_Pair(l_Current, l_IOEntryName);
						m_IOEntryPairs.push_back( l_Pair );
						break;
					}
				}
				l_InspItem = mTree.GetNextSiblingItem(l_InspItem);
			}
		}
		l_Current = mTree.GetNextSiblingItem(l_Current);
	}
}

void SVOFileConfigDlg::GetPPQs(HTREEITEM p_Item)
{
	m_PPQs.clear();
	HTREEITEM l_Current = mTree.GetChildItem( p_Item);
	while( l_Current != NULL )
	{
		CString l_PPQName = mTree.GetItemText( l_Current);

		m_PPQs.push_back( l_PPQName );

		l_Current = mTree.GetNextSiblingItem(l_Current);
	}
}


void SVOFileConfigDlg::GetInspections(HTREEITEM p_Item)
{
	HTREEITEM l_Current = mTree.GetChildItem( p_Item);
	while( l_Current != NULL )
	{
		CString l_strName = mTree.GetItemText( l_Current);
		m_Inspections.push_back( l_strName );

		l_Current = mTree.GetNextSiblingItem(l_Current);
	}
}

