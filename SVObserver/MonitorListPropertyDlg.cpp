//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListPropertyDlg
//* .File Name       : $Workfile:   MonitorListPropertyDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Apr 2014 16:23:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "resource.h"
#include "MonitorListPropertyDlg.h"
#include "RemoteMonitorNamedList.h"
#pragma endregion Includes

enum {IDC_MONITOR_PROPERTY_TRUE = 100};

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
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_MONITOR_PROPERTY_TRUE, OnItemChanged)
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()

// MonitorListPropertyDlg message handlers
BOOL MonitorListPropertyDlg::OnInitDialog()
{
//Create the Property Tree from the IDC_STATIC_MONITOR_PROP
	CRect rc;

	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;

	GetDlgItem(IDC_STATIC_MONITOR_PROP)->GetWindowRect(rc);
	ScreenToClient(rc);
	
	// Create SVRPropTree control IDC_MONITOR_PROPERTY_TRUE
	m_Tree.Create(dwStyle, rc, this, IDC_MONITOR_PROPERTY_TRUE);
	long lVal = m_Tree.GetColumn();
	m_Tree.SetColumn(200);

	RemoteMonitorList::iterator it = m_MonitorList.find(m_MonitorListName);
	if ( it != m_MonitorList.end() )
	{
		m_DisplayName = it->second.GetName().c_str();
		m_MonitorListRejectQueueDepth = it->second.GetRejectDepthQueue();
	}

	SetupMonitorListProperties();

	return TRUE;
}

//Add 
void MonitorListPropertyDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;

	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;

		if ( pItem->GetCtrlID() == PROP_MONITOR_LIST_NAME )
		{
			CString sName;
			m_Tree.FindItem(PROP_MONITOR_LIST_NAME)->GetItemValue(sName);
			m_DisplayName = sName;
		}
		
		if ( pItem->GetCtrlID() == PROP_MONITOR_LIST_DEPTH )
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
	RemoteMonitorList::iterator it = m_MonitorList.find(m_MonitorListName);
	if ( it != m_MonitorList.end() )
	{
		RemoteMonitorNamedList namedList = it->second;

		namedList.SetName(m_DisplayName);
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
			pPropItem->SetInfoText(_T("The maximum number of rejected products whose data will be stored in the Run Reject Server. Defaul value = 10, range 0-50"));
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\MonitorListPropertyDlg.cpp_v  $
 * 
 *    Rev 1.0   17 Apr 2014 16:23:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/


