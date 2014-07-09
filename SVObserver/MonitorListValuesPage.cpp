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

MonitorListValuesPage::MonitorListValuesPage(const NameSelectionList& rList, bool bWholeArray, MonitorListSheet* pParent, const CString& szCaption, int id)
: MonitorListBasePage(rList, pParent, szCaption, id)
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\MonitorListValuesPage.cpp_v  $
 * 
 *    Rev 1.1   08 Jul 2014 09:15:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revsied MonitorListValuesPage constructor.
 * Revised OnInitDialog to support wholeArray selection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Apr 2014 16:24:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
