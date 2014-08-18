//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListAddRemoveDlg
//* .File Name       : $Workfile:   MonitorListAddRemoveDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 Aug 2014 10:59:52  $
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
	RemoteMonitorList:: iterator it = m_MonitorList.find(oldName);
	if (it != m_MonitorList.end())
	{
		RemoteMonitorNamedList namedList = it->second;
		namedList.SetName(newName);
		m_MonitorList.erase(it);
		m_MonitorList.insert(std::make_pair(newName, namedList));
	}
}

bool MonitorListAddRemoveDlg::IsValidListName(const CString& name)
{
	bool bRetVal = true;
	// don't allow parentheses
	int sPos = name.ReverseFind(_T('('));
	if (sPos != -1)
	{
		bRetVal = false;
	}
	if (bRetVal)
	{
		// check for uniqueness
		RemoteMonitorList:: iterator it = m_MonitorList.find(name); // check if it's unique
		if (it != m_MonitorList.end())
		{
			bRetVal = false;
		}
	}
	return bRetVal;
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
	ON_MESSAGE(WM_APP_LB_ITEM_EDITED, OnUsedListEditFinished)
	ON_BN_CLICKED(IDC_BTN_PROPERTIES, &MonitorListAddRemoveDlg::OnBnClickedBtnProperties)
END_MESSAGE_MAP()

// MonitorListAddRemoveDlg message handlers

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
			m_MonitorList.insert(std::make_pair(name, RemoteMonitorNamedList(PPQName, name)));

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
		const SVString& name = GetListNameFromDisplayName(strDelete);
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

LRESULT MonitorListAddRemoveDlg::OnUsedListEditFinished(WPARAM wPar, LPARAM lPar)
{
	bool bRevert = true;
	int index = static_cast<int>(wPar);
	CString oldName = reinterpret_cast<LPCTSTR>(lPar);
	int rejectDepth = static_cast<int>(m_UsedList.GetItemData(index));

	const CString& PPQName = GetPPQName(oldName);
	const SVString& listName = GetListNameFromDisplayName(oldName);
	if (!PPQName.IsEmpty() && !listName.empty())
	{
		CString newName;
		m_UsedList.GetText(index, newName);
		newName.Trim();

		// if not the same name and is a Valid Name (does not contain parentheses and is unique)
		if (!newName.IsEmpty() && newName.Compare(listName.c_str()) != 0 && IsValidListName(newName))
		{
			// Replace List (remove old, add new)
			ReplaceList(listName.c_str(), newName);
			
			const CString& displayName = BuildListDisplayName(PPQName, newName);
			m_UsedList.DeleteString(index);
			int insertIndex = m_UsedList.InsertString(index, displayName);
			m_UsedList.SetItemData(insertIndex, rejectDepth);
			m_UsedList.SetCurSel(insertIndex);
			bRevert = false;
		}
	}
	if (bRevert)
	{
		// revert back
		m_UsedList.DeleteString(index);
		int insertIndex = m_UsedList.InsertString(index, oldName);
		m_UsedList.SetItemData(insertIndex, rejectDepth);
		m_UsedList.SetCurSel(insertIndex);
	}
	return 0;
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
	
		MonitorListPropertyDlg propDlg(m_MonitorList,sName);
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\MonitorListAddRemoveDlg.cpp_v  $
 * 
 *    Rev 1.3   15 Aug 2014 10:59:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised GetNextAvailableListName method to correct an issue with duplicate names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
 *    Rev 1.1   01 Jul 2014 15:21:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   commented out the #define ALLOW_ONLY_ONE_MONITOR_LIST so that multiple list can be created
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Apr 2014 16:22:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
