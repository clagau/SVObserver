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

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVRemoteOutputGroupAddRemoveDlg.h"
#include "TextDefinesSvO.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVOMFCLibrary/SVDeviceParams.h"

#pragma endregion Includes

// SVRemoteOutputGroupAddRemoveDlg dialog
#define ONE_OUTPUT_GROUP_PER_PPQ 

IMPLEMENT_DYNAMIC(SVRemoteOutputGroupAddRemoveDlg, CDialog)

SVRemoteOutputGroupAddRemoveDlg::SVRemoteOutputGroupAddRemoveDlg(CWnd* pParent /*=nullptr*/)
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

		//Remove any excluded characters from the name
		SVString checkName( newName );
		SvUl_SF::RemoveCharacters( checkName, SvO::SVEXCLUDECHARS_TOOL_IP_NAME );
		newName = checkName.c_str();

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

