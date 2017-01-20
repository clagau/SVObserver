//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerSelectionDlg
//* .File Name       : $Workfile:   SVTriggerSelectionDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:52:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVTriggerSelectionDlg.h"

IMPLEMENT_DYNAMIC(SVTriggerSelectionDlg, CDialog)

SVTriggerSelectionDlg::SVTriggerSelectionDlg(const SVTriggerNameIdList& triggerList, CWnd* pParent /*=nullptr*/)
: CDialog(SVTriggerSelectionDlg::IDD, pParent)
, m_triggerCallbackMap(triggerList)
, m_selectedTriggerID(-1) 
{
}

SVTriggerSelectionDlg::~SVTriggerSelectionDlg()
{
}

void SVTriggerSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRIGGER_LIST, m_selectionListBox);
}

void SVTriggerSelectionDlg::PopulateSelectionList()
{
	for (SVTriggerNameIdList ::const_iterator it = m_triggerCallbackMap.begin();it != m_triggerCallbackMap.end();++it)
	{
		int index = m_selectionListBox.AddString(it->first.c_str());
		m_selectionListBox.SetItemData(index, it->second);
	}
}

void SVTriggerSelectionDlg::GetSelectedTrigger(SVString& rName, int& rID) const
{
	rName = m_selectedTriggerName;
	rID = m_selectedTriggerID;
}

BEGIN_MESSAGE_MAP(SVTriggerSelectionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &SVTriggerSelectionDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_TRIGGER_LIST, &SVTriggerSelectionDlg::OnLbnDblclkTriggerList)
END_MESSAGE_MAP()

BOOL SVTriggerSelectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	PopulateSelectionList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void SVTriggerSelectionDlg::OnBnClickedOk()
{
	// get selected trigger
	int index = m_selectionListBox.GetCurSel();
	if (index != LB_ERR)
	{
		m_selectionListBox.GetText(index, m_selectedTriggerName);
		m_selectedTriggerID = static_cast<int>(m_selectionListBox.GetItemData(index));
		OnOK();
	}
}

void SVTriggerSelectionDlg::OnLbnDblclkTriggerList()
{
	OnBnClickedOk();
}

