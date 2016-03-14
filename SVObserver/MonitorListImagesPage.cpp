//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListImagesPage
//* .File Name       : $Workfile:   MonitorListImagesPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Apr 2014 16:23:34  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "MonitorListImagesPage.h"
#include "MonitorListSheet.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(MonitorListImagesPage, MonitorListBasePage)

BEGIN_MESSAGE_MAP(MonitorListImagesPage, MonitorListBasePage)
	//{{AFX_MSG_MAP(MonitorListImagesPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

MonitorListImagesPage::MonitorListImagesPage(const SvUl::NameSelectionList& rList, MonitorListSheet* pParent, const CString& szCaption, int id)
: MonitorListBasePage(rList, pParent, szCaption, id)
{
	//{{AFX_DATA_INIT(MonitorListImagesPage)
	//}}AFX_DATA_INIT
}

MonitorListImagesPage::~MonitorListImagesPage()
{
}

void MonitorListImagesPage::DoDataExchange(CDataExchange* pDX)
{
	MonitorListBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MonitorListImagesPage)
	//}}AFX_DATA_MAP
}

bool MonitorListImagesPage::QueryAllowExit()
{
	return true;
}

void MonitorListImagesPage::OnSelchangeListSelected() 
{
	ShowSelectionText();
}

BOOL MonitorListImagesPage::OnInitDialog() 
{
	MonitorListBasePage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MonitorListImagesPage::InitPage()
{
	m_Tree.InitTreeCtrl();
	m_Tree.Populate(m_nameList);
}

void MonitorListImagesPage::OnOK()
{
}

