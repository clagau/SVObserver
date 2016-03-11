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

SVTriggerSelectionDlg::SVTriggerSelectionDlg(const SVTriggerNameIdList& triggerList, CWnd* pParent /*=NULL*/)
: CDialog(SVTriggerSelectionDlg::IDD, pParent)
, m_triggerList(triggerList)
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
	for (SVTriggerNameIdList ::const_iterator it = m_triggerList.begin();it != m_triggerList.end();++it)
	{
		int index = m_selectionListBox.AddString(it->first.c_str());
		m_selectionListBox.SetItemData(index, it->second);
	}
}

void SVTriggerSelectionDlg::GetSelectedTrigger(CString& name, int& id) const
{
	name = m_selectedTriggerName;
	id = m_selectedTriggerID;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTriggerSelectionDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:52:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Jan 2013 11:03:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

