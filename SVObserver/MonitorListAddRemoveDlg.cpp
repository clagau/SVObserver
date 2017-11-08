//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListAddRemoveDlg
//* .File Name       : $Workfile:   MonitorListAddRemoveDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   18 Aug 2014 16:09:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <set>
#include "MonitorListAddRemoveDlg.h"
#include "MonitorListPropertyDlg.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

BEGIN_MESSAGE_MAP(MonitorListAddRemoveDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_BTN, &MonitorListAddRemoveDlg::OnBnClickedAddBtn)
	ON_BN_CLICKED(IDC_REMOVE_BTN, &MonitorListAddRemoveDlg::OnBnClickedRemoveBtn)
	ON_BN_CLICKED(IDOK, &MonitorListAddRemoveDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_PROPERTIES, &MonitorListAddRemoveDlg::OnBnClickedBtnProperties)
	ON_LBN_DBLCLK(IDC_USED_LIST, OnDblClickUsedList)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(MonitorListAddRemoveDlg, CDialog)

MonitorListAddRemoveDlg::MonitorListAddRemoveDlg(const RemoteMonitorListMap& rMonitorList, const PPQNameListNames& rList, CWnd* pParent /*=nullptr*/)
: CDialog(MonitorListAddRemoveDlg::IDD, pParent)
, m_MonitorList(rMonitorList)
, m_SetupList(rList)
{
}

MonitorListAddRemoveDlg::~MonitorListAddRemoveDlg()
{
}

void MonitorListAddRemoveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AVAILABLE_PPQ_LIST, m_AvailableList);
	DDX_Control(pDX, IDC_USED_LIST, m_UsedList);
	DDX_Control(pDX, IDC_ADD_BTN, m_AddButton);
	DDX_Control(pDX, IDC_REMOVE_BTN, m_RemoveButton);
	DDX_Control(pDX, IDC_BTN_PROPERTIES, m_PropertiesButton);
}

const RemoteMonitorListMap& MonitorListAddRemoveDlg::GetRemoteMonitorList() const
{
	return m_MonitorList;
}

void MonitorListAddRemoveDlg::ReplaceList( const std::string& rOldName, const std::string& rNewName )
{
	RemoteMonitorListMap:: iterator it = m_MonitorList.find(rOldName);
	if (it != m_MonitorList.end())
	{
		RemoteMonitorNamedList namedList = it->second;
		namedList.SetName(rNewName);
		m_MonitorList.erase(it);
		m_MonitorList.insert(std::make_pair(rNewName, namedList));
	}
}

std::string MonitorListAddRemoveDlg::BuildListDisplayName(LPCTSTR PPQName, LPCTSTR Name) const
{
	return SvUl::Format(_T("%s(%s)"), Name, PPQName); // Append PPQ name
}

std::string MonitorListAddRemoveDlg::GetListNameFromDisplayName(LPCTSTR Name) const
{
	// Get the the PPQname from the end of the Name
	std::string DisplayName(Name);
	size_t Pos = DisplayName.rfind(_T('('));
	if( std::string::npos != Pos )
	{
		return SvUl::Left( DisplayName, Pos);
	}
	return std::string();
}

std::string MonitorListAddRemoveDlg::GetPPQName( LPCTSTR Name ) const
{
	// Get the the PPQname from the end of the Name
	std::string DisplayName(Name);
	size_t startPos = DisplayName.rfind(_T('('));
	size_t endPos = DisplayName.rfind(_T(')'));
	if( std::string::npos != startPos && std::string::npos != endPos )
	{
		return DisplayName.substr( startPos+1, (endPos - startPos) - 1);
	}
	return std::string();
}

void MonitorListAddRemoveDlg::OnDblClickUsedList()
{
	int lAddSel = m_AvailableList.GetCurSel();
	if (lAddSel > -1)
	{
		OnBnClickedBtnProperties(); //Show the properties dialog
	}
}

void MonitorListAddRemoveDlg::OnBnClickedAddBtn()
{
	int lAddSel = m_AvailableList.GetCurSel();
	if (lAddSel > -1)
	{
		CString PPQName;
		m_AvailableList.GetText(lAddSel, PPQName);
		std::string Name = NextAvailableListName();
		if (!Name.empty())
		{
			// Add to GUI
			std::string DisplayName = BuildListDisplayName(PPQName, Name.c_str());
			int iInsert = m_UsedList.AddString( DisplayName.c_str() );
			m_UsedList.SetItemData(iInsert,  RemoteMonitorNamedList::GetDefaultRejectQueueDepth()); 
			m_UsedList.SetCurSel(iInsert);
		
			// Add it to the master list
			m_MonitorList.insert(std::make_pair(Name, RemoteMonitorNamedList(std::string(PPQName), Name)));

			// Update Buttons
			if (m_UsedList.GetCount() > 0)
			{
				m_PropertiesButton.EnableWindow(true);
				m_RemoveButton.EnableWindow(true);
			}
		}
	}
}

std::string MonitorListAddRemoveDlg::NextAvailableListName() const
{
	typedef std::set<int> IDSet;
	IDSet ids;
	// look for the next available number
	for (int i = 0;i < m_UsedList.GetCount();i++)
	{
		CString Temp;
		m_UsedList.GetText(i, Temp);
		// parse out PPQ ID (PPQ_N)
		std::string Name = GetListNameFromDisplayName(Temp);
		size_t Pos = Name.find_first_of(_T("1234567890"));
		if( std::string::npos != Pos )
		{
			int iTmp = atoi( SvUl::Mid(Name, Pos).c_str() );
			ids.insert(iTmp);
		}
	}

	int num( 1 );
	// Get next highest number
	IDSet::const_reverse_iterator it = ids.rbegin();
	if (it != ids.rend())
	{
        num = (*it) + 1;
    }
	    
    return SvUl::Format( _T("MonitorList%d"), num );
}

void MonitorListAddRemoveDlg::OnBnClickedRemoveBtn()
{
	int lRemoveSel = m_UsedList.GetCurSel();
	if (lRemoveSel > LB_ERR)
	{
		CString DeleteName;
		m_UsedList.GetText(lRemoveSel, DeleteName);
		m_UsedList.DeleteString(lRemoveSel);

		// remove from master list
		// parse out PPQ ID (PPQ_N)
		RemoteMonitorListMap::iterator it = m_MonitorList.find( GetListNameFromDisplayName( DeleteName ) );
		if (it != m_MonitorList.end())
		{
			m_MonitorList.erase(it);
		}
		
		if (0 == m_UsedList.GetCount())
		{
			m_RemoveButton.EnableWindow(false);
			m_AddButton.EnableWindow(true);
			m_PropertiesButton.EnableWindow(false);
		}
	}
}

void MonitorListAddRemoveDlg::OnBnClickedOk()
{
	CDialog::OnOK();
}

BOOL MonitorListAddRemoveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Item Data is the available list index.
	for (PPQNameListNames::const_iterator it = m_SetupList.begin();it != m_SetupList.end();++it)
	{
		m_AvailableList.AddString(it->first.c_str());
		// Add List names to the listbox.
		UpdateUsedList(it->first.c_str(), it->second);
	}
	m_AvailableList.SetCurSel(0); // select first item
	m_AddButton.EnableWindow(true);

	m_RemoveButton.EnableWindow(m_UsedList.GetCount() > 0);
	m_PropertiesButton.EnableWindow(m_UsedList.GetCount() > 0);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void MonitorListAddRemoveDlg::UpdateUsedList(LPCTSTR PPQName, const NameDepthPairList& rList)
{
	for (NameDepthPairList::const_iterator it = rList.begin();it != rList.end();++it)
	{
		std::string DisplayName = BuildListDisplayName(PPQName, it->first.c_str());
		int index = m_UsedList.AddString( DisplayName.c_str() );
		m_UsedList.SetItemData(index, it->second); // set the reject depth
	}
	//select first item in list
	if ( 0 < m_UsedList.GetCount() )
	{
		m_UsedList.SetCurSel(0);
	}
}

void MonitorListAddRemoveDlg::OnBnClickedBtnProperties()
{
	int iPos = m_UsedList.GetCurSel();
	if (iPos != LB_ERR)
	{
		CString Name;
		m_UsedList.GetText(iPos, Name);

		std::string DisplayName = GetListNameFromDisplayName( Name );
		std::string PPQName = GetPPQName( Name );
	
		MonitorListPropertyDlg propDlg(m_MonitorList, DisplayName.c_str());
		if (IDOK == propDlg.DoModal() )
		{
			std::string sNewName = propDlg.GetMonitorListName();
			int depth = propDlg.GetMonitorListRejectQueueDepth();
			if (sNewName != DisplayName)
			{
				m_UsedList.DeleteString(iPos);
				m_UsedList.InsertString(iPos, BuildListDisplayName(PPQName.c_str(), sNewName.c_str()).c_str() );
			}
			m_UsedList.SetItemData(iPos, depth);
		}
		m_UsedList.SetCurSel(iPos);
	}
}

