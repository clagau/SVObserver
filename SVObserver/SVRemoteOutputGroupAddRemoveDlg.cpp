//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputGroupAddRemoveDlg
//* .File Name       : $Workfile:   SVRemoteOutputGroupAddRemoveDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   14 Jul 2014 15:38:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVRemoteOutputGroupAddRemoveDlg.h"


// SVRemoteOutputGroupAddRemoveDlg dialog
#define ONE_OUTPUT_GROUP_PER_PPQ 

IMPLEMENT_DYNAMIC(SVRemoteOutputGroupAddRemoveDlg, CDialog)

SVRemoteOutputGroupAddRemoveDlg::SVRemoteOutputGroupAddRemoveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVRemoteOutputGroupAddRemoveDlg::IDD, pParent)
	, m_lOutputGroupCount(0)
	, m_strGroupNameEdit(_T(""))
{

}

SVRemoteOutputGroupAddRemoveDlg::~SVRemoteOutputGroupAddRemoveDlg()
{
}

void SVRemoteOutputGroupAddRemoveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AVAILABLE_LIST, m_AvailableList);
	DDX_Control(pDX, IDC_ADD_BTN, m_AddButton);
	DDX_Control(pDX, IDC_USED_LIST, m_UsedList);
	DDX_Control(pDX, IDC_REMOVE_BTN, m_RemoveBtn);
}


BEGIN_MESSAGE_MAP(SVRemoteOutputGroupAddRemoveDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_BTN, &SVRemoteOutputGroupAddRemoveDlg::OnBnClickedAddBtn)
	ON_BN_CLICKED(IDC_REMOVE_BTN, &SVRemoteOutputGroupAddRemoveDlg::OnBnClickedRemoveBtn)
	ON_BN_CLICKED(IDOK, &SVRemoteOutputGroupAddRemoveDlg::OnBnClickedOk)
	ON_MESSAGE(WM_APP_LB_ITEM_EDITED, OnUsedListEditFinished )
END_MESSAGE_MAP()

// SVRemoteOutputGroupAddRemoveDlg message handlers

void SVRemoteOutputGroupAddRemoveDlg::OnBnClickedAddBtn()
{
	// TODO: Add your control notification handler code here
	int l_lAddSel = m_AvailableList.GetCurSel();
	if( l_lAddSel > -1 )
	{
		CString l_strPPQ;
		m_AvailableList.GetText( l_lAddSel, l_strPPQ );
		SVGroupDef l_NewGroup;
		l_NewGroup.m_strName = NextAvailableGroupName(); //.Format(_T("Output Group %d"), ++m_lOutputGroupCount );
		l_NewGroup.m_strPPQ = l_strPPQ;
		m_SetupGroup.push_back(l_NewGroup);
		int l_iInsert = m_UsedList.AddString( l_NewGroup.m_strName + _T("(") + l_NewGroup.m_strPPQ + _T(")") );
		m_UsedList.SetItemData( l_iInsert, m_AvailableList.GetItemData( l_lAddSel ) );

		if( m_UsedList.GetCount() >= 0 )
		{
			m_RemoveBtn.EnableWindow( true );
			#ifdef ONE_OUTPUT_GROUP_PER_PPQ
				// If only one list
				m_AvailableList.DeleteString( l_lAddSel );
				m_AddButton.EnableWindow( false );
			#endif
		}

	}
}

CString SVRemoteOutputGroupAddRemoveDlg::NextAvailableGroupName()
{
	CString newName;
	int num = 1;
	bool bFinished = false;
	while( !bFinished )
	{
		bFinished = true;
		for( SVGroupDefVect::iterator it = m_SetupGroup.begin() ; it != m_SetupGroup.end() ; ++ it )
		{
			CString strSet = _T("1234567890");
			int iFirst = it->m_strName.FindOneOf(strSet);
			CString strTmp = it->m_strName.Mid(iFirst).SpanIncluding(strSet);
			int iTmp = atol(strTmp);
			if( iTmp == num)
			{	// if we change num then check the entire list for a match.
				num++;
				bFinished = false;
			}
		}
	}
	newName.Format( _T("Output Group %d"), num );
	return newName;
}



void SVRemoteOutputGroupAddRemoveDlg::OnBnClickedRemoveBtn()
{
	int l_lRemoveSel = m_UsedList.GetCurSel();
	if( l_lRemoveSel > -1 )
	{
		int l_lAvailablePPQsIndex = (int)m_UsedList.GetItemData( l_lRemoveSel );
		CString l_strDelete;
		m_UsedList.GetText(l_lRemoveSel, l_strDelete );
		m_UsedList.DeleteString( l_lRemoveSel );
		for( SVGroupDefVect::iterator it = m_SetupGroup.begin() ; it != m_SetupGroup.end() ; ++it )
		{
			if( l_strDelete.Find( (*it).m_strName ) > -1)
			{
				m_SetupGroup.erase( it );
				break;
			}
		}
		for( CStringPairVect::iterator l_it = m_aRenamedGroups.begin() ; l_it != m_aRenamedGroups.end() ; ++l_it )
		{
			if( l_strDelete == l_it->first )
			{
				m_aRenamedGroups.erase( l_it );
				break;
			}
		}

		CString l_strTmp;
		long l_iInsert = m_AvailableList.AddString( m_astrAvailablePPQs[l_lAvailablePPQsIndex] );
		m_AvailableList.SetItemData( l_iInsert, l_lAvailablePPQsIndex );
		if( m_UsedList.GetCount() == 0 )
		{
			m_RemoveBtn.EnableWindow( false );
			m_AddButton.EnableWindow( true );
		}

	}
}

void SVRemoteOutputGroupAddRemoveDlg::OnBnClickedOk()
{
	m_astrSetupGroups.clear();
	for( SVGroupDefVect::iterator it = m_SetupGroup.begin() ; it != m_SetupGroup.end() ; ++it )
	{
		m_astrSetupGroups.push_back( (*it).m_strName);
	}
	OnOK();
}



BOOL SVRemoteOutputGroupAddRemoveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Save the originals 
	m_astrOriginalGroups.clear();

	// Item Data is the available list index.
	for( CStringVec::iterator l_AvailPPQIt = m_astrAvailablePPQs.begin(); l_AvailPPQIt != m_astrAvailablePPQs.end() ; ++ l_AvailPPQIt )
	{
		bool l_bFound = false;
		for( SVGroupDefVect::iterator it = m_SetupGroup.begin(); it != m_SetupGroup.end() ; ++it )
		{
			if( *l_AvailPPQIt == (*it).m_strPPQ)
			{
				l_bFound = true;
				break;
			}
		}
		if( !l_bFound )
		{
			int l_iInsert = m_AvailableList.AddString( *l_AvailPPQIt );
			int lPos = static_cast<int>(l_AvailPPQIt - m_astrAvailablePPQs.begin());
			m_AvailableList.SetItemData( l_iInsert, lPos );
		}
	}


	// Add Setup Groups to the list.
	UpdateUsedList();

	if( m_UsedList.GetCount() == 0 )
	{
		m_RemoveBtn.EnableWindow( false );
		m_AddButton.EnableWindow( true );
	}

	// Setup the original list here.
	for( SVGroupDefVect::iterator it = m_SetupGroup.begin(); it != m_SetupGroup.end() ; ++it )
	{
		m_astrOriginalGroups.push_back( it->m_strName );
	}

	m_AddButton.EnableWindow( m_UsedList.GetCount() == 0 );

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SVRemoteOutputGroupAddRemoveDlg::UpdateUsedList()
{
	for( SVGroupDefVect::iterator it = m_SetupGroup.begin(); it != m_SetupGroup.end() ; ++it )
	{
		int iPos = m_UsedList.AddString( (*it).m_strName + _T("(") + (*it).m_strPPQ + _T(")") );
		m_UsedList.SetSel(iPos, true);
		++m_lOutputGroupCount;
		m_UsedList.SetItemData(iPos, StringPosition(m_astrAvailablePPQs, (*it).m_strPPQ ) );
	}
}


int SVRemoteOutputGroupAddRemoveDlg::StringPosition( CStringVec l_astrArray, CString l_strValue )
{
	int iRet = -1;
	for( CStringVec::iterator l_it = l_astrArray.begin() ; l_it != l_astrArray.end() ; ++l_it)
	{
		if( *l_it == l_strValue )
		{
			iRet = static_cast<int>(l_it - l_astrArray.begin());
		}
	}
	return iRet;
}

// This function finds all items that are in the original list but not in the newlist and puts
// them in the outputlist.
HRESULT SVRemoteOutputGroupAddRemoveDlg::GetRemoved( CStringVec& OutputList )
{
	OutputList.clear();
	for( size_t i = 0 ; i < m_astrOriginalGroups.size() ; i++)
	{
		bool l_bFound = false;
		for( size_t j = 0 ; j< m_astrSetupGroups.size() ; j++ )
		{
			if( m_astrOriginalGroups[i] == m_astrSetupGroups[j] )
			{
				l_bFound = true;
				break;
			}
		}
		if( !l_bFound )
		{
			for( CStringPairVect::iterator l_it = this->m_aRenamedGroups.begin(); l_it != m_aRenamedGroups.end(); ++l_it )
			{
				if( l_it->first == m_astrOriginalGroups[i])
				{
					l_bFound = true;
					break;
				}
			}
		}
		if( !l_bFound )
		{
			OutputList.push_back( m_astrOriginalGroups[i] );
		}
	}
	return S_OK;
}

// This function finds all items that are in the New list but not in the Original and puts
// them in the outputlist.
HRESULT SVRemoteOutputGroupAddRemoveDlg::GetNewItems( CStringVec& OutputList )
{
	OutputList.clear();
	for( size_t i = 0 ; i < m_astrSetupGroups.size() ; i++)
	{
		bool l_bFound = false;
		for( size_t j = 0 ; j< m_astrOriginalGroups.size() ; j++ )
		{
			if( m_astrSetupGroups[i] == m_astrOriginalGroups[j] )
			{
				l_bFound = true;
				break;
			}
		}
		if( !l_bFound )
		{
			if (i < m_astrOriginalGroups.size())
			{
				for( CStringPairVect::iterator l_it = this->m_aRenamedGroups.begin(); l_it != m_aRenamedGroups.end(); ++l_it )
				{
					if (l_it->first == m_astrOriginalGroups[i])
					{
						l_bFound = true;
						break;
					}
				}
			}
		}
		if( !l_bFound)
		{
			OutputList.push_back( m_astrSetupGroups[i] );
		}
	}
	return S_OK;
}

HRESULT SVRemoteOutputGroupAddRemoveDlg::AddValue( CString GroupName, CString PPQName )
{
	SVGroupDef l_grp(GroupName, PPQName);
	m_SetupGroup.push_back( l_grp);
	return S_OK;
}

LRESULT  SVRemoteOutputGroupAddRemoveDlg::OnUsedListEditFinished(WPARAM wPar, LPARAM lPar)
{
	CRect rec ;
	CRect wRec;

	int sel = m_UsedList.GetCurSel();
	m_strGroupNameEdit = m_SetupGroup[sel].m_strName;
	m_UsedList.GetItemRect(sel, &rec);
	GetClientRect( wRec );


	rec += wRec;
	ClientToScreen(&rec);

	int pos = 0;
	int index = static_cast<int>(m_UsedList.GetItemData( pos ));
	if( index >= 0 )
	{
		CString oldName = m_SetupGroup[index].m_strName;
		CString newName;
		m_UsedList.GetText(sel, newName);

		//trim white space from left and right of the new name
		newName.Trim();

		//don't all blank strings
		if ( !newName.IsEmpty() )
		{
			bool l_bFoundInRenameList=false;
			for( CStringPairVect::iterator it = m_aRenamedGroups.begin(); it != m_aRenamedGroups.end(); ++it)
			{
				if( it->second == oldName )
				{
					l_bFoundInRenameList = true;
					it->second = newName;
					break;
				}
			}
			if( !l_bFoundInRenameList )
			{
				if( oldName != newName )
				{
					m_aRenamedGroups.push_back( std::pair<CString, CString>(oldName, newName ));
				}
			}
			m_SetupGroup[sel].m_strName = newName;
		}
		m_UsedList.ResetContent();
		UpdateUsedList();
	}
	return 0;
}

HRESULT SVRemoteOutputGroupAddRemoveDlg::GetRenamed( CStringPairVect& OutputList)
{
	OutputList = m_aRenamedGroups;
	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRemoteOutputGroupAddRemoveDlg.cpp_v  $
 * 
 *    Rev 1.2   14 Jul 2014 15:38:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the editing of the name to Trim whitespace and not allow blank names
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 07:58:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:52:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Jan 2013 16:43:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated Ouput fucntionality assoicated with the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Oct 2012 10:55:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   14 Aug 2012 15:25:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added code to disable add and remove buttons when they do not make sense.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Aug 2012 12:52:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified used list to use editable listbox.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   06 Mar 2012 11:44:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues associated with PVCS headers and footers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
