//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListValuesPage
//* .File Name       : $Workfile:   MonitorListValuesPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 Jul 2014 09:15:30  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "MonitorListValuesPage.h"
#include "MonitorListSheet.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(MonitorListValuesPage, MonitorListBasePage)

BEGIN_MESSAGE_MAP(MonitorListValuesPage, MonitorListBasePage)
	//{{AFX_MSG_MAP(MonitorListValuesPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

MonitorListValuesPage::MonitorListValuesPage(const SvUl::NameSelectionList& rList, bool bWholeArray, MonitorListSheet* pParent, LPCTSTR Caption, int id)
: MonitorListBasePage(rList, pParent, Caption, id)
, m_bAllowWholeArray(bWholeArray)
{
	//{{AFX_DATA_INIT(MonitorListValuesPage)
	//}}AFX_DATA_INIT
}

MonitorListValuesPage::~MonitorListValuesPage()
{
}

bool MonitorListValuesPage::QueryAllowExit()
{
	return true;
}

void MonitorListValuesPage::OnOK()
{
}

void MonitorListValuesPage::DoDataExchange(CDataExchange* pDX)
{
	MonitorListBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MonitorListValuesPage)
	//}}AFX_DATA_MAP
}

BOOL MonitorListValuesPage::OnInitDialog() 
{
	MonitorListBasePage::OnInitDialog();
	if (m_bAllowWholeArray)
	{
		m_Tree.SetAllowWholeArray();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MonitorListValuesPage::OnSelchangeListSelected() 
{
	// Show full text
	ShowSelectionText();
}

void MonitorListValuesPage::InitPage()
{
	m_Tree.InitTreeCtrl();
	m_Tree.Populate(m_nameList);
}

