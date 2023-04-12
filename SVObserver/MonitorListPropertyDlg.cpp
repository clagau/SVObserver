//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListPropertyDlg
//* .File Name       : $Workfile:   MonitorListPropertyDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   26 Aug 2014 17:19:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
#include "MonitorListPropertyDlg.h"
#include "Definitions/GlobalConst.h"
#include "RemoteMonitorListController.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVRPropertyTree/SVRPropTreeItemEdit.h"
#include "SVStatusLibrary/MessageManagerHelper.h"
#pragma endregion Includes

enum {IDC_MONITOR_PROPERTY_TREE = 100};

MonitorListPropertyDlg::MonitorListPropertyDlg(RemoteMonitorListMap& MonitorList, LPCTSTR Name, CWnd* pParent)
: CDialog(MonitorListPropertyDlg::IDD, pParent)
, m_MonitorList(MonitorList)
, m_MonitorListName(Name)
,m_IsMonitorListActive(false)
{
}

MonitorListPropertyDlg::~MonitorListPropertyDlg()
{
}

void MonitorListPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(MonitorListPropertyDlg, CDialog)
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_MONITOR_PROPERTY_TREE, OnItemChanged)
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()

// MonitorListPropertyDlg message handlers
BOOL MonitorListPropertyDlg::OnInitDialog()
{
//Create the Property Tree from the IDC_STATIC_MONITOR_PROP
	CRect rc;

	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | PTS_NOTIFY;

	GetDlgItem(IDC_STATIC_MONITOR_PROP)->GetWindowRect(rc);
	ScreenToClient(rc);
	
	// Create SVRPropTree control IDC_MONITOR_PROPERTY_TRUE
	m_Tree.Create(dwStyle, rc, this, IDC_MONITOR_PROPERTY_TREE);
	m_Tree.SetColumn(200);

	RemoteMonitorListMap::iterator it = m_MonitorList.find(std::string(m_MonitorListName));
	if ( it != m_MonitorList.end() )
	{
		m_DisplayName = it->second.GetName().c_str();
		m_MonitorListRejectQueueDepth = it->second.GetRejectDepthQueue();
		m_IsMonitorListActive = it->second.IsActive();
	}

	SetupMonitorListProperties();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Validate label text and remove unwanted characters.
std::string MonitorListPropertyDlg::makeValidLabeltext(const std::string& rNewText) const
{
	std::string Result = SvUl::RemoveCharactersByRegexAndTrim(rNewText, SvDef::cPatternAllExceptAlnumUnderscoreAndBlank);

	if( Result.empty() )
	{
		Log_Assert(m_DisplayName.size() > 0);
		Result = m_DisplayName;
	}
	return Result;
}

bool MonitorListPropertyDlg::IsValidListName(const std::string& rName, const std::string& rOriginalName) const
{
	bool bRetVal = true;
	// check for uniqueness (case insensitive)
	RemoteMonitorListMap::const_iterator it = std::find_if(m_MonitorList.begin(), m_MonitorList.end(), [&](const RemoteMonitorListMap::value_type& entry)->bool 
	{ 
		return (0 == SvUl::CompareNoCase(rName, entry.first) && 0 != SvUl::CompareNoCase(rOriginalName, entry.first)); 
	} 
	);
	if (it != m_MonitorList.end())
	{
		bRetVal = false;
	}
	return bRetVal;
}

void MonitorListPropertyDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT*)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;

		if ( PROP_MONITOR_LIST_NAME == pItem->GetCtrlID() )
		{
			std::string Name;
			m_Tree.FindItem(PROP_MONITOR_LIST_NAME)->GetItemValue(Name);
			Name = makeValidLabeltext(Name);
			if (IsValidListName(Name, m_DisplayName))
			{ 
				m_DisplayName = Name;
			}
			m_Tree.FindItem(PROP_MONITOR_LIST_NAME)->SetItemValue( m_DisplayName.c_str() );
		}
		else if ( PROP_MONITOR_LIST_DEPTH == pItem->GetCtrlID() )
		{
			int iDepth;
			m_Tree.FindItem(PROP_MONITOR_LIST_DEPTH)->GetItemValue(iDepth);
			if (iDepth >= RemoteMonitorNamedList::MinRejectQueueDepth && iDepth <= RemoteMonitorNamedList::GetMaxRejectQueueDepth())
			{
				m_MonitorListRejectQueueDepth = iDepth;
			}
			else
			{
				m_Tree.FindItem(PROP_MONITOR_LIST_DEPTH)->SetItemValue(m_MonitorListRejectQueueDepth);
			}
		}
		else if ( PROP_MONITOR_LIST_ISACTIVE == pItem->GetCtrlID() )
		{
			bool isActive;
			m_Tree.FindItem(PROP_MONITOR_LIST_ISACTIVE)->GetItemValue(isActive);		
				m_IsMonitorListActive = isActive;			
		}
	}
}

void MonitorListPropertyDlg::OnOK() 
{
	RemoteMonitorListMap::iterator it = m_MonitorList.find(std::string(m_MonitorListName));
	if ( it != m_MonitorList.end() )
	{
		RemoteMonitorNamedList namedList = it->second;

		namedList.SetName(SvUl::RemoveCharactersByRegexAndTrim(m_DisplayName, SvDef::cPatternAllExceptAlnumUnderscoreAndBlank));
		namedList.SetRejectDepthQueue(m_MonitorListRejectQueueDepth);
		m_MonitorList.erase(it);
		m_MonitorList.insert(std::make_pair(m_DisplayName, namedList));
	}

	//use ActivateRemoteMonitorList to avoid more then one active monitorlist per PPQ 
	RemoteMonitorListController::ActivateRemoteMonitorList(m_MonitorList, std::string(m_MonitorListName),m_IsMonitorListActive );
	CDialog::OnOK();
}

void MonitorListPropertyDlg::SetupMonitorListProperties()
{
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	SVRPropertyItemEdit* pPropItem;

	if (pRoot)
	{
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("Monitor List"));
		pRoot->SetInfoText(_T("The properties for the monitor list"));
		
		pPropItem = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(),pRoot);
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Monitor List Name"));
			pPropItem->SetCtrlID(PROP_MONITOR_LIST_NAME);
			pPropItem->SetInfoText(_T("The name of the monitor list"));
			pPropItem->SetItemValue( m_DisplayName.c_str() );
		}
		
		pPropItem = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(),pRoot);
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Reject Queue Depth"));
			pPropItem->SetCtrlID(PROP_MONITOR_LIST_DEPTH);
			pPropItem->SetInfoText(_T("The maximum number of rejected products whose data will be stored in the Run Reject Server. Default value: 10, Range: 0-50"));
			pPropItem->SetItemValue(m_MonitorListRejectQueueDepth);
		}

		pPropItem = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(),pRoot);

		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Is Activ"));
			pPropItem->SetCtrlID(PROP_MONITOR_LIST_ISACTIVE);
			pPropItem->SetInfoText(_T("Only one Monitorlist per PPQ can be active"));
			pPropItem->SetItemValue(m_IsMonitorListActive);
		}

		pRoot->Select(true);
		pRoot->Expand();
	}
}

const std::string& MonitorListPropertyDlg::GetMonitorListName() const
{
	return m_DisplayName;
}

int MonitorListPropertyDlg::GetMonitorListRejectQueueDepth() const
{
	return m_MonitorListRejectQueueDepth;
}

bool MonitorListPropertyDlg::GetIsMonitorListActive() const
{
	return  m_IsMonitorListActive;
}

