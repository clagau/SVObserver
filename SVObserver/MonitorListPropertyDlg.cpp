//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
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
#include <algorithm>
#include "MonitorListPropertyDlg.h"
#include "TextDefinesSvO.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

enum {IDC_MONITOR_PROPERTY_TREE = 100};

IMPLEMENT_DYNAMIC(MonitorListPropertyDlg, CDialog)

MonitorListPropertyDlg::MonitorListPropertyDlg(RemoteMonitorList& MonitorList, CString sName, CWnd* pParent)
: CDialog(MonitorListPropertyDlg::IDD, pParent)
, m_MonitorList(MonitorList)
, m_MonitorListName(sName)
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
	long lVal = m_Tree.GetColumn();
	m_Tree.SetColumn(200);

	RemoteMonitorList::iterator it = m_MonitorList.find(SVString(m_MonitorListName));
	if ( it != m_MonitorList.end() )
	{
		m_DisplayName = it->second.GetName().c_str();
		m_MonitorListRejectQueueDepth = it->second.GetRejectDepthQueue();
	}

	SetupMonitorListProperties();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Validate label text and remove unwanted characters.
void MonitorListPropertyDlg::ValidateLabelText(CString& newText) const
{
	SVString newName( newText );

	SvUl_SF::RemoveCharacters( newName, SvO::SVEXCLUDECHARS_TOOL_IP_NAME );

	if( 0 < newName.size() )
	{
		newText = newName.c_str();
	}
	else
	{
		ASSERT(m_DisplayName.GetLength() > 0);
		newText = m_DisplayName;
	}
}

bool MonitorListPropertyDlg::IsValidListName(const CString& name, const CString& originalName) const
{
	bool bRetVal = true;
	// check for uniqueness (case insensitive)
	RemoteMonitorList::const_iterator it = std::find_if(m_MonitorList.begin(), m_MonitorList.end(), [&](const RemoteMonitorList::value_type& entry)->bool 
	{ 
		return (0 == name.CompareNoCase(entry.first.c_str()) && 0 != originalName.CompareNoCase(entry.first.c_str())); 
	} 
	);
	if (it != m_MonitorList.end())
	{
		bRetVal = false;
	}
	return bRetVal;
}

void MonitorListPropertyDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;

		if ( PROP_MONITOR_LIST_NAME == pItem->GetCtrlID() )
		{
			CString sName;
			m_Tree.FindItem(PROP_MONITOR_LIST_NAME)->GetItemValue(sName);
			sName.Trim();
			
			ValidateLabelText(sName);
			if (IsValidListName(sName, m_DisplayName))
			{ 
				m_DisplayName = sName;
			}
			m_Tree.FindItem(PROP_MONITOR_LIST_NAME)->SetItemValue(m_DisplayName);
		}
		else if ( PROP_MONITOR_LIST_DEPTH == pItem->GetCtrlID() )
		{
			int iDepth;
			m_Tree.FindItem(PROP_MONITOR_LIST_DEPTH)->GetItemValue(iDepth);
			if (iDepth >= MinRejectQueueDepth && iDepth <= MaxRejectQueueDepth)
			{
				m_MonitorListRejectQueueDepth = iDepth;
			}
			else
			{
				m_Tree.FindItem(PROP_MONITOR_LIST_DEPTH)->SetItemValue(m_MonitorListRejectQueueDepth);
			}
		}
	}
}

void MonitorListPropertyDlg::OnOK() 
{
	RemoteMonitorList::iterator it = m_MonitorList.find(SVString(m_MonitorListName));
	if ( it != m_MonitorList.end() )
	{
		RemoteMonitorNamedList namedList = it->second;

		namedList.SetName(SVString(m_DisplayName));
		namedList.SetRejectDepthQueue(m_MonitorListRejectQueueDepth);
		m_MonitorList.erase(it);
		m_MonitorList.insert(std::make_pair(m_DisplayName, namedList));
	}

	CDialog::OnOK();
}

void MonitorListPropertyDlg::SetupMonitorListProperties()
{
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	SVRPropertyItemEdit* pPropItem;

	CString sValue;

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
			pPropItem->SetItemValue(m_DisplayName);
		}
		
		pPropItem = (SVRPropertyItemEdit*)m_Tree.InsertItem(new SVRPropertyItemEdit(),pRoot);
		if ( pPropItem )
		{
			pPropItem->SetLabelText(_T("Reject Queue Depth"));
			pPropItem->SetCtrlID(PROP_MONITOR_LIST_DEPTH);
			pPropItem->SetInfoText(_T("The maximum number of rejected products whose data will be stored in the Run Reject Server. Default value: 10, Range: 0-50"));
			pPropItem->SetItemValue(m_MonitorListRejectQueueDepth);
		}

		pRoot->Select(true);
		pRoot->Expand();
	}
}

CString MonitorListPropertyDlg::GetMonitorListName() const
{
	return m_DisplayName;
}

int MonitorListPropertyDlg::GetMonitorListRejectQueueDepth() const
{
	return m_MonitorListRejectQueueDepth;
}

