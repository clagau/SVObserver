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
#include "SVRemoteOutputGroupAddRemoveDlg.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

// SVRemoteOutputGroupAddRemoveDlg dialog
#define ONE_OUTPUT_GROUP_PER_PPQ 

IMPLEMENT_DYNAMIC(SVRemoteOutputGroupAddRemoveDlg, CDialog)

SVRemoteOutputGroupAddRemoveDlg::SVRemoteOutputGroupAddRemoveDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SVRemoteOutputGroupAddRemoveDlg::IDD, pParent)
	, m_lOutputGroupCount(0)
	, m_GroupNameEdit(_T(""))
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
		SVGroupDef NewGroup;
		NewGroup.m_Name = NextAvailableGroupName(); //.Format(_T("Output Group %d"), ++m_lOutputGroupCount );
		NewGroup.m_PPQName = l_strPPQ;
		m_SetupGroup.push_back(NewGroup);
		std::string Text = NewGroup.m_Name + _T("(") + NewGroup.m_PPQName + _T(")");
		int l_iInsert = m_UsedList.AddString( Text.c_str() );
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

std::string SVRemoteOutputGroupAddRemoveDlg::NextAvailableGroupName()
{
	std::string Result;
	int num = 1;
	bool bFinished = false;
	while( !bFinished )
	{
		bFinished = true;
		for( SVGroupDefVect::iterator it = m_SetupGroup.begin() ; it != m_SetupGroup.end() ; ++ it )
		{
			size_t Pos = it->m_Name.find_first_of(_T("1234567890"));
			if( std::string::npos != Pos )
			{
				int iTmp = atoi( SvUl::Mid(it->m_Name, Pos).c_str() );
				if( iTmp == num)
				{	// if we change num then check the entire list for a match.
					num++;
					bFinished = false;
				}
			}
		}
	}
	Result = SvUl::Format( _T("Output Group %d"), num );
	return Result;
}



void SVRemoteOutputGroupAddRemoveDlg::OnBnClickedRemoveBtn()
{
	int l_lRemoveSel = m_UsedList.GetCurSel();
	if( l_lRemoveSel > -1 )
	{
		int l_lAvailablePPQsIndex = (int)m_UsedList.GetItemData( l_lRemoveSel );
		CString Delete;
		m_UsedList.GetText(l_lRemoveSel, Delete );
		m_UsedList.DeleteString( l_lRemoveSel );
		for( SVGroupDefVect::iterator it = m_SetupGroup.begin() ; it != m_SetupGroup.end() ; ++it )
		{
			if( 0 == Delete.Find(it->m_Name.c_str()) )
			{
				m_SetupGroup.erase( it );
				break;
			}
		}
		for(SvDef::StringPairVector::iterator l_it = m_RenamedGroups.begin() ; l_it != m_RenamedGroups.end() ; ++l_it )
		{
			if( Delete.GetString() == l_it->first )
			{
				m_RenamedGroups.erase( l_it );
				break;
			}
		}

		CString l_strTmp;
		long l_iInsert = m_AvailableList.AddString( m_AvailablePPQs[l_lAvailablePPQsIndex] .c_str());
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
	m_SetupGroups.clear();
	for( SVGroupDefVect::iterator it = m_SetupGroup.begin() ; it != m_SetupGroup.end() ; ++it )
	{
		m_SetupGroups.push_back( (*it).m_Name);
	}
	OnOK();
}



BOOL SVRemoteOutputGroupAddRemoveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Save the originals 
	m_OriginalGroups.clear();

	// Item Data is the available list index.
	for( SvDef::StringVector::iterator Iter = m_AvailablePPQs.begin(); Iter != m_AvailablePPQs.end() ; ++ Iter )
	{
		bool l_bFound = false;
		for( SVGroupDefVect::iterator it = m_SetupGroup.begin(); it != m_SetupGroup.end() ; ++it )
		{
			if( *Iter == (*it).m_PPQName)
			{
				l_bFound = true;
				break;
			}
		}
		if( !l_bFound )
		{
			int l_iInsert = m_AvailableList.AddString( Iter->c_str() );
			int lPos = static_cast<int>(Iter - m_AvailablePPQs.begin());
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
		m_OriginalGroups.push_back( it->m_Name );
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
		std::string Text = it->m_Name + _T("(") + it->m_PPQName + _T(")");
		int iPos = m_UsedList.AddString( Text.c_str() );
		m_UsedList.SetSel(iPos, true);
		++m_lOutputGroupCount;
		m_UsedList.SetItemData(iPos, StringPosition(m_AvailablePPQs, (*it).m_PPQName ) );
	}
}


int SVRemoteOutputGroupAddRemoveDlg::StringPosition( const SvDef::StringVector& rList, const std::string& rValue )
{
	int iRet = -1;
	SvDef::StringVector::const_iterator Iter = std::find( rList.begin(), rList.end(), rValue);
	if( rList.end() != Iter )
	{
		iRet = static_cast<int> (std::distance( rList.begin(), Iter));
	}
	return iRet;
}

// This function finds all items that are in the original list but not in the newlist and puts
// them in the outputlist.
HRESULT SVRemoteOutputGroupAddRemoveDlg::GetRemoved( SvDef::StringVector& rOutputList )
{
	rOutputList.clear();
	for( size_t i = 0 ; i < m_OriginalGroups.size() ; i++)
	{
		bool l_bFound = false;
		for( size_t j = 0 ; j< m_SetupGroups.size() ; j++ )
		{
			if( m_OriginalGroups[i] == m_SetupGroups[j] )
			{
				l_bFound = true;
				break;
			}
		}
		if( !l_bFound )
		{
			for(SvDef::StringPairVector::iterator l_it = this->m_RenamedGroups.begin(); l_it != m_RenamedGroups.end(); ++l_it )
			{
				if( l_it->first == m_OriginalGroups[i])
				{
					l_bFound = true;
					break;
				}
			}
		}
		if( !l_bFound )
		{
			rOutputList.push_back( m_OriginalGroups[i] );
		}
	}
	return S_OK;
}

// This function finds all items that are in the New list but not in the Original and puts
// them in the outputlist.
HRESULT SVRemoteOutputGroupAddRemoveDlg::GetNewItems( SvDef::StringVector& rOutputList )
{
	rOutputList.clear();
	for( size_t i = 0 ; i < m_SetupGroups.size() ; i++)
	{
		bool l_bFound = false;
		for( size_t j = 0 ; j< m_OriginalGroups.size() ; j++ )
		{
			if( m_SetupGroups[i] == m_OriginalGroups[j] )
			{
				l_bFound = true;
				break;
			}
		}
		if( !l_bFound )
		{
			if (i < m_OriginalGroups.size())
			{
				for(SvDef::StringPairVector::iterator l_it = this->m_RenamedGroups.begin(); l_it != m_RenamedGroups.end(); ++l_it )
				{
					if (l_it->first == m_OriginalGroups[i])
					{
						l_bFound = true;
						break;
					}
				}
			}
		}
		if( !l_bFound)
		{
			rOutputList.push_back( m_SetupGroups[i] );
		}
	}
	return S_OK;
}

LRESULT  SVRemoteOutputGroupAddRemoveDlg::OnUsedListEditFinished(WPARAM, LPARAM )
{
	CRect rec ;
	CRect wRec;

	int sel = m_UsedList.GetCurSel();
	m_GroupNameEdit = m_SetupGroup[sel].m_Name;
	m_UsedList.GetItemRect(sel, &rec);
	GetClientRect( wRec );


	rec += wRec;
	ClientToScreen(&rec);

	int pos = 0;
	int index = static_cast<int>(m_UsedList.GetItemData( pos ));
	if( index >= 0 )
	{
		std::string oldName = m_SetupGroup[index].m_Name;
		CString Text;
		m_UsedList.GetText(sel, Text);
		//trim white space from left and right of the new name
		std::string newName = SvUl::Trim(Text);

		SvUl::RemoveCharacters( newName, SvDef::cGeneralExcludeChars );

		//don't all blank strings
		if ( !newName.empty() )
		{
			bool l_bFoundInRenameList=false;
			for(SvDef::StringPairVector::iterator it = m_RenamedGroups.begin(); it != m_RenamedGroups.end(); ++it)
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
					m_RenamedGroups.push_back(SvDef::StringPair(oldName, newName ));
				}
			}
			m_SetupGroup[sel].m_Name = newName;
		}
		m_UsedList.ResetContent();
		UpdateUsedList();
	}
	return 0;
}

HRESULT SVRemoteOutputGroupAddRemoveDlg::GetRenamed(SvDef::StringPairVector& rRenamedList)
{
	rRenamedList = m_RenamedGroups;
	return S_OK;
}

