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
#include <set>
#include "MonitorListAddRemoveDlg.h"
#include "MonitorListPropertyDlg.h"
#pragma endregion Includes

//#define ALLOW_ONLY_ONE_MONITOR_LIST

IMPLEMENT_DYNAMIC(MonitorListAddRemoveDlg, CDialog)

MonitorListAddRemoveDlg::MonitorListAddRemoveDlg(const RemoteMonitorList& rMonitorList, const PPQNameListNames& rList, CWnd* pParent /*=NULL*/)
: CDialog(MonitorListAddRemoveDlg::IDD, pParent)
, m_MonitorList(rMonitorList)
, m_SetupList(rList)
{
}

MonitorListAddRemoveDlg::~MonitorListAddRemoveDlg()
{
}

const RemoteMonitorList& MonitorListAddRemoveDlg::GetRemoteMonitorList() const
{
	return m_MonitorList;
}

void MonitorListAddRemoveDlg::ReplaceList(const CString& oldName, const CString& newName)
{
	RemoteMonitorList:: iterator it = m_MonitorList.find(SVString(oldName));
	if (it != m_MonitorList.end())
	{
		RemoteMonitorNamedList namedList = it->second;
		namedList.SetName(SVString(newName));
		m_MonitorList.erase(it);
		m_MonitorList.insert(std::make_pair(newName, namedList));
	}
}

CString MonitorListAddRemoveDlg::BuildListDisplayName(const CString& PPQName, const CString& name) const
{
	CString tmp;
	tmp.Format(_T("%s(%s)"), name, PPQName); // Append PPQ name
	return tmp;
}

CString MonitorListAddRemoveDlg::GetListNameFromDisplayName(const CString& name) const
{
	CString listName;
	// Get the the PPQname from the end of the Name
	int sPos = name.ReverseFind(_T('('));
	if (sPos != -1)
	{
		listName = name.Left(sPos);
	}
	return listName;
}

CString MonitorListAddRemoveDlg::GetPPQName(const CString& name) const
{
	CString ppqName;
	// Get the the PPQname from the end of the Name
	int sPos = name.ReverseFind(_T('('));
	if (sPos != -1)
	{
		int ePos = name.ReverseFind(_T(')'));
		if (ePos != -1)
		{
			ppqName = name.Mid(sPos+1, (ePos - sPos) - 1);
		}
	}
	return ppqName;
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

BEGIN_MESSAGE_MAP(MonitorListAddRemoveDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_BTN, &MonitorListAddRemoveDlg::OnBnClickedAddBtn)
	ON_BN_CLICKED(IDC_REMOVE_BTN, &MonitorListAddRemoveDlg::OnBnClickedRemoveBtn)
	ON_BN_CLICKED(IDOK, &MonitorListAddRemoveDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_PROPERTIES, &MonitorListAddRemoveDlg::OnBnClickedBtnProperties)
	ON_LBN_DBLCLK(IDC_USED_LIST, OnDblClickUsedList)
END_MESSAGE_MAP()

// MonitorListAddRemoveDlg message handlers

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
		const CString& name = NextAvailableListName();
		if (!name.IsEmpty())
		{
			// Add to GUI
			const CString& tmp = BuildListDisplayName(PPQName, name);
			int iInsert = m_UsedList.AddString(tmp);
			m_UsedList.SetItemData(iInsert, DefaultRejectQueueDepth); // default reject depth of 10
			m_UsedList.SetCurSel(iInsert);
		
			// Add it to the master list
			m_MonitorList.insert(std::make_pair(name, RemoteMonitorNamedList(SVString(PPQName), SVString(name))));

			// Update Buttons
			if (m_UsedList.GetCount() > 0)
			{
				m_PropertiesButton.EnableWindow(true);
				m_RemoveButton.EnableWindow(true);
				//only allowing 1 Monitor List for now
				#ifdef ALLOW_ONLY_ONE_MONITOR_LIST
				m_AddButton.EnableWindow(false);
				#endif
			}
		}
	}
}

CString MonitorListAddRemoveDlg::NextAvailableListName() const
{
	CString newName;
	int num = 1;
	CString strSet = _T("1234567890");
	
	typedef std::set<int> IDSet;
	IDSet ids;
	// look for the next available number
	for (int i = 0;i < m_UsedList.GetCount();i++)
	{
		CString name;
		m_UsedList.GetText(i, name);
		// parse out PPQ ID (PPQ_N)
		name = GetListNameFromDisplayName(name);
		int iFirst = name.FindOneOf(strSet);
		if (-1 != iFirst)
		{
			CString strTmp = name.Mid(iFirst).SpanIncluding(strSet);
			int iTmp = atol(strTmp);
			ids.insert(iTmp);
		}
	}

	// Get next highest number
	IDSet::const_reverse_iterator it = ids.rbegin();
	if (it != ids.rend())
	{
        num = (*it) + 1;
    }

	// find next non used number in the sequence
	/*IDSet::const_iterator it = ids.lower_bound(num);
    while (it != ids.end() && *it == num)
    {
        ++it;
        ++num;
    } */
    
    newName.Format( _T("MonitorList%d"), num );
	return newName;
}

void MonitorListAddRemoveDlg::OnBnClickedRemoveBtn()
{
	int lRemoveSel = m_UsedList.GetCurSel();
	if (lRemoveSel > LB_ERR)
	{
		CString strDelete;
		m_UsedList.GetText(lRemoveSel, strDelete);
		m_UsedList.DeleteString(lRemoveSel);

		// remove from master list
		// parse out PPQ ID (PPQ_N)
		const SVString& name = SVString(GetListNameFromDisplayName(strDelete));
		RemoteMonitorList::iterator it = m_MonitorList.find(name);
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

#ifdef ALLOW_ONLY_ONE_MONITOR_LIST
	m_AddButton.EnableWindow(m_UsedList.GetCount() < 1);
#else
	m_AddButton.EnableWindow(true);
#endif

	m_RemoveButton.EnableWindow(m_UsedList.GetCount() > 0);
	m_PropertiesButton.EnableWindow(m_UsedList.GetCount() > 0);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void MonitorListAddRemoveDlg::UpdateUsedList(const CString& PPQName, const NameDepthPairList& rList)
{
	for (NameDepthPairList::const_iterator it = rList.begin();it != rList.end();++it)
	{
		const CString& displayName = BuildListDisplayName(PPQName, it->first.c_str());
		int index = m_UsedList.AddString(displayName);
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
		CString sTmpName;
		m_UsedList.GetText(iPos,sTmpName);

		CString sName = GetListNameFromDisplayName(sTmpName);
		CString sPPQ = GetPPQName(sTmpName);
	
		MonitorListPropertyDlg propDlg(m_MonitorList, sName);
		if (IDOK == propDlg.DoModal() )
		{
			CString sNewName = propDlg.GetMonitorListName();
			int depth = propDlg.GetMonitorListRejectQueueDepth();
			if (sNewName != sName)
			{
				m_UsedList.DeleteString(iPos);
				m_UsedList.InsertString(iPos, BuildListDisplayName(sPPQ, sNewName));
			}
			m_UsedList.SetItemData(iPos, depth);
		}
		m_UsedList.SetCurSel(iPos);
	}
}

