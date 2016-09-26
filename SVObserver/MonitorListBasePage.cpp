//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListBasePage
//* .File Name       : $Workfile:   MonitorListBasePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 Aug 2014 15:26:04  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "svobserver.h"
#include "MonitorListBasePage.h"
#include "MonitorListSheet.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOMFCLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(MonitorListBasePage, CPropertyPage)

BEGIN_MESSAGE_MAP(MonitorListBasePage, CPropertyPage)
	//{{AFX_MSG_MAP(MonitorListBasePage)
	ON_LBN_SELCHANGE(IDC_LIST_SELECTED, OnSelchangeListSelected_Base)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnBtnRemove)
	ON_BN_CLICKED(IDC_BTN_REMOVE_ALL, OnBtnRemoveAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

MonitorListBasePage::MonitorListBasePage(const SvUl::NameSelectionList& rList, MonitorListSheet* pParent, const CString& szCaption, int id)
: CPropertyPage(id)
, m_pSheet(pParent)
, m_nameList(rList)
, m_bObjectsChanged(false)
{
	//{{AFX_DATA_INIT(MonitorListBasePage)
	//}}AFX_DATA_INIT
	m_strCaption = szCaption;
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;
}

MonitorListBasePage::~MonitorListBasePage()
{
}

bool MonitorListBasePage::QueryAllowExit()
{
	return true;
}

const SvUl::NameSelectionList& MonitorListBasePage::GetNameSelectionList() const
{
	return m_nameList;
}

void MonitorListBasePage::ShowSelectionText()
{
	int index = m_lbSelected.GetCurSel();
	if (index != LB_ERR)
	{
		CString name;
		m_lbSelected.GetText(index, m_selectedItemText);
		UpdateData(false);
	}
}

BOOL MonitorListBasePage::OnSetActive() 
{
	UpdateData(false);
	return CPropertyPage::OnSetActive();
}

BOOL MonitorListBasePage::OnKillActive() 
{
	UpdateData(true);	
	return CPropertyPage::OnKillActive();
}

void MonitorListBasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MonitorListBasePage)
	DDX_Control(pDX, IDC_TREE, m_Tree);
	DDX_Control(pDX, IDC_BTN_REMOVE_ALL, m_btnRemoveAll);
	DDX_Control(pDX, IDC_BTN_REMOVE, m_btnRemove);
	DDX_Control(pDX, IDC_LIST_SELECTED, m_lbSelected);
	DDX_Text(pDX, IDC_SELECTED_ITEM_TEXT, m_selectedItemText);
	//}}AFX_DATA_MAP
}

BOOL MonitorListBasePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_Tree.AllowNodeItemChecks(true);
	m_Tree.SetCanSelectObjectCallback(boost::bind(&MonitorListBasePage::CanSelectObjectCallback, this, _1, _2));

	// initialize page
	InitPage();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MonitorListBasePage::OnSelchangeListSelected_Base() 
{
	OnSelchangeListSelected();
}

void MonitorListBasePage::OnBtnRemove() 
{
	std::vector<int> vecIndexes;
	int count = m_lbSelected.GetCount();
	if (count > 0)
	{
		vecIndexes.resize(count);
		int items = m_lbSelected.GetSelItems(count, &vecIndexes[0]);
		for (int i = items - 1;i >= 0;--i)
		{
			int index = static_cast<int>(m_lbSelected.GetItemData(vecIndexes.at(i)));
			m_Tree.SelectItemByIndex(index);
		}
	}
}

void MonitorListBasePage::OnBtnRemoveAll() 
{
	m_Tree.DeselectAll();
}

bool MonitorListBasePage::CanSelectObjectCallback(bool bCurrentState, int index)
{
	UpdateData();
	
	const SvUl::NameSelection& nameSelection	= m_nameList[index];
	const SVString& name = nameSelection.first;
	m_nameList[index].second = (bCurrentState) ? false : true;
	if (bCurrentState == false)// want to select
	{
		bool bInserted = false;
		for (int i = 0;i < m_lbSelected.GetCount() && !bInserted ;++i)
		{
			if (static_cast<int>(m_lbSelected.GetItemData(i)) > index)
			{
				m_lbSelected.InsertString(i, name.c_str());
				m_lbSelected.SetItemData(i, index);
				bInserted = true;
			}
		}
		if (!bInserted)
		{
			int lbIndex = m_lbSelected.AddString(name.c_str());
			m_lbSelected.SetItemData(lbIndex, index);
		}
	}
	else // want to deselect
	{
		int iIndex = m_lbSelected.FindStringExact(0, name.c_str());
		if (iIndex != LB_ERR)
		{
			m_lbSelected.DeleteString(iIndex);
		}
	}
	return true;
}

