// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOFileConfigDlg
// * .File Name       : $Workfile:   SVOFileConfigDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   03 Feb 2014 17:07:36  $
// ******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <stdio.h>
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <set>

#include "SVOFileConfigDlg.h"
#include "AutoBuild/AutoBuild.h"
#include "Definitions/GlobalConst.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVXMLLibrary/LoadConfiguration.h"
#include "SVXMLLibrary/SVObjectXMLWriter.h"
#include "SVXMLLibrary/TreeToXml.h"


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
	CString Label;
	GetWindowText( Label );
	Label += _T(" - With Detect and Repair IOEntries - version");
	Label +=  STRPRODUCTVER;
	SetWindowText( Label );
	m_strLastDirectory = SvStl::GlobalPath::Inst().GetRunPath().c_str();
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

			if ( svvData.vt & VT_ARRAY )
			{
				SvUl::SVSAFEARRAY svArray(svvData);
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
bool SVOFileConfigDlg::GetDirectoryRemoveLevel( std::string& rFileName, int p_level )
{
	bool l_bChanged = false;
	if( rFileName.rfind( '.' ) > 0 )
	{
		size_t Pos = rFileName.rfind( _T('\\') );
		int count = 0;
		//Make sure that it is not a UNC path
		while( std::string::npos != Pos && 1 < Pos != Pos && count < p_level )
		{
			Pos = rFileName.rfind( _T('\\') );
			if( std::string::npos != Pos )
			{
				rFileName = rFileName.substr(0 , Pos );
				count++;
				l_bChanged = true;
			}
		}
	}
	return l_bChanged;
}

void SVOFileConfigDlg::OnButtonloadSvx() 
{
	//The dot is removed from cConfigExtension, because that can lead too two dots after the filename in specific use cases
	CFileDialog dlg( TRUE, &SvDef::cConfigExtension[1], NULL,
										OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
										OFN_ENABLESIZING | OFN_EXPLORER,
										_T("SVObserver Environment Configuration Files (*.svx)|*.svx||"), this );

	dlg.m_ofn.lpstrTitle = _T("Select Configuration File");
	dlg.m_ofn.lpstrInitialDir = m_strLastDirectory.c_str();

	if (dlg.DoModal () != IDOK)
	{
		return;
	}

	m_strLastDirectory = dlg.m_ofn.lpstrFile;

	GetDirectoryRemoveLevel( m_strLastDirectory, 1);

	mTree.SetRedraw( false );

	m_XMLCTree.Clear();

	//@TODO[gra][10.30][29.04.2022]: Conversion function from SVXMLMaterialTree to XMLCtree
	//SvXml::SVOCMLoadConfiguration(m_ulCurrentVersion, dlg.GetPathName(), m_XMLCTree);
	if( m_ulCurrentVersion >= 0x50100)
	{
		CheckConfiguration();
	}
	else
	{
		MessageBox(_T("This program will only detect IOEntry errors in Configurations saved with SVObserver 5.00 or newer"));
	}
	mTree.SetRedraw( true );
}

void SVOFileConfigDlg::OnButtonsaveSvx() 
{
	//The dot is removed from cConfigExtension, because that can lead too two dots after the filename in specific use cases
	CFileDialog dlg( TRUE, &SvDef::cConfigExtension[1], NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
		OFN_ENABLESIZING | OFN_EXPLORER,
		_T("SVObserver Environment Configuration Files (*.svx)|*.svx||"), this );

	dlg.m_ofn.lpstrTitle = _T("Select Configuration File");
	dlg.m_ofn.lpstrInitialDir = m_strLastDirectory.c_str();

	if (dlg.DoModal () == IDOK)
	{
		m_strLastDirectory = dlg.m_ofn.lpstrFile;
		GetDirectoryRemoveLevel( m_strLastDirectory, 1);
		
		FixConfiguration();
		std::ofstream XMLOutFile;
		XMLOutFile.open( dlg.GetPathName());
		if (XMLOutFile.is_open())
		{
			SvXml::SVObjectXMLWriter writer(XMLOutFile);
			writer.setNewLine( true );
			std::string RootName( SvDef::FqnRoot );
			writer.WriteRootElement( RootName.c_str() );
			writer.WriteSchema();
			SvXml::TreeToXMl::CopyTreeNodeToWriter<SVXMLCTreeCtrl>(m_XMLCTree,m_XMLCTree.getRoot(),writer );
			writer.EndAllElements();
			XMLOutFile.close();
		}
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
		std::string Inspection;
		if( ParseInspection( (*it).second, Inspection ) )
		{
			if( std::find( m_Inspections.begin(), m_Inspections.end(), Inspection) == m_Inspections.end() )
			{
				if( mTree.DeleteItem((*it).first) )
				{
					OutputDebugString( std::string(_T("Deleting ")+(*it).second).c_str() );
				}
			}
		}
		std::string l_strPPQ;
		if( ParsePPQ( (*it).second, l_strPPQ ) )
		{
			if( std::find( m_PPQs.begin(), m_PPQs.end(), l_strPPQ) == m_PPQs.end() )
			{
				if( mTree.DeleteItem((*it).first) )
				{
					OutputDebugString( std::string(_T("Deleting ")+(*it).second).c_str() );
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
							OutputDebugString( std::string(_T("Deleting ") + it->second + "\n").c_str() );
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
		std::string l_strName = mTree.GetItemText( l_Current ).GetString();

		if( l_strName == _T("IO"))
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
		std::string Name = mTree.GetItemText( l_Current).GetString();

		if( std::string::npos != Name.find( _T("IOEntry") ) )
		{
			std::string NewName = SvUl::Format( _T("IOEntry%d"), count);
			count++;
			mTree.SetItemText(l_Current, NewName.c_str() );
		}
		l_Current = mTree.GetNextSiblingItem(l_Current);
	}

	// Update NumberOfIOEntries Last.
	l_Current = mTree.GetChildItem( p_Item);
	while( l_Current != NULL )
	{
		std::string Name = mTree.GetItemText( l_Current).GetString();

		if( Name == _T("NumberOfIOEntries") )
		{
			VARIANT* l_pVt = reinterpret_cast<VARIANT*> (mTree.GetItemData( l_Current ));
			l_pVt->lVal = count;
			break;
		}
		l_Current = mTree.GetNextSiblingItem(l_Current);
	}
}


void SVOFileConfigDlg::CheckConfiguration()
{
	HTREEITEM Root = mTree.GetRootItem();
	HTREEITEM Child = mTree.GetNextSiblingItem( Root );
	HTREEITEM Current = Child;
	m_IOEntryPairs.clear();
	m_Inspections.clear();

	while( Current != NULL )
	{
		std::string Name = mTree.GetItemText( Current ).GetString();

		if( Name == _T("IO"))
		{
			GetIOChildren(Current);
		}

		if( Name == _T("Inspection"))
		{
			GetInspections(Current);
		}

		if( Name == _T("PPQ"))
		{
			GetPPQs(Current);
		}

		Current = mTree.GetNextSiblingItem(Current);
		
	}

	// search IOEntries for invalid inspections
	std::string strMainMessage;
	
	for( HTIStringPairs::iterator it = m_IOEntryPairs.begin() ; it != m_IOEntryPairs.end() ; ++it)
	{
		std::string Inspection;
		if( ParseInspection( (*it).second, Inspection ) )
		{
			if( std::find( m_Inspections.begin(), m_Inspections.end(), Inspection) == m_Inspections.end() )
			{
				std::string strMessage = SvUl::Format( _T("%s\n"), it->second.c_str());
				strMainMessage+= strMessage;

				if( mTree.DeleteItem((*it).first) )
				{
					OutputDebugString( std::string(_T("Deleting ") + it->second).c_str() );
				}
			}
		}
		std::string PPQName;
		if( ParsePPQ( (*it).second, PPQName ) )
		{
			if( std::find( m_PPQs.begin(), m_PPQs.end(), PPQName) == m_PPQs.end() )
			{
				std::string strMessage = SvUl::Format( _T("%s\n"), it->second.c_str());
				strMainMessage+= strMessage;

				if( mTree.DeleteItem((*it).first) )
				{
					OutputDebugString( std::string(_T("Deleting ") + it->second).c_str() );
				}
			}
		}

	}

	if( !strMainMessage.empty() )
	{
		MessageBox(strMainMessage.c_str(),_T("The following IOEntries are Invalid!"), MB_OK);
	}

}
bool SVOFileConfigDlg::ParseInspection( const std::string& rName, std::string& rOut)
{
	bool Result( false );
	size_t Pos = rName.find('.');
	if( std::string::npos != Pos )
	{
		std::string Temp;
		Result = true;
		Temp = SvUl::Left( rName, Pos );
		if( Temp == _T("DIO") || (Temp.find( _T("PPQ")) == 0))
		{
			Result = false;
		}
		else
		{
			rOut = Temp;
		}
	}
	return Result;
}

bool SVOFileConfigDlg::ParsePPQ( const std::string& rName, std::string& rOut)
{
	bool Result( false );
	size_t Pos = rName.find('.');
	if( std::string::npos != Pos )
	{
		std::string Temp;

		Temp = SvUl::Left( rName,  Pos );
		if( Temp.find( _T("PPQ") ) == 0)
		{
			Result = true;
			rOut = Temp;
		}
	}
	return Result;
}


void SVOFileConfigDlg::GetIOChildren(HTREEITEM p_Item)
{
	HTREEITEM l_Current = mTree.GetChildItem( p_Item);
	while( l_Current != NULL )
	{
		std::string Name = mTree.GetItemText( l_Current).GetString();

		if( std::string::npos != Name.find( _T("IOEntry") ) )
		{
			HTREEITEM l_InspItem = mTree.GetChildItem( l_Current );
			while( l_InspItem != NULL )
			{
				Name = mTree.GetItemText( l_InspItem );
				if( Name == _T("IOEntryName") )
				{
					VARIANT* l_vt = reinterpret_cast<VARIANT*> (mTree.GetItemData( l_InspItem ));
					if( l_vt->vt == VT_BSTR )
					{
						std::string IOEntryName = SvUl::createStdString( _bstr_t(l_vt->bstrVal) );
						HTIStringPair l_Pair(l_Current, IOEntryName);
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
		std::string PPQName = mTree.GetItemText( l_Current).GetString();

		m_PPQs.push_back( PPQName );

		l_Current = mTree.GetNextSiblingItem(l_Current);
	}
}


void SVOFileConfigDlg::GetInspections(HTREEITEM p_Item)
{
	HTREEITEM l_Current = mTree.GetChildItem( p_Item);
	while( l_Current != NULL )
	{
		std::string Name = mTree.GetItemText( l_Current).GetString();
		m_Inspections.push_back( Name );

		l_Current = mTree.GetNextSiblingItem(l_Current);
	}
}

