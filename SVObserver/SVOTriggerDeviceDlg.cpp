//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerDeviceDlg
//* .File Name       : $Workfile:   SVOTriggerDeviceDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   15 May 2014 11:38:40  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <memory>
#include "SVOTriggerDeviceDlg.h"
#include "SVOConfigAssistantDlg.h"
#include "SVOPropertyPageDlg.h"
#include "TriggerInformation/SVOTriggerObj.h"
#include "SVTriggerSelectionDlg.h"
#include "SVOResource\ConstGlobalSvOr.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const CString scCameraTriggerTag = _T("CameraTrigger_");

CSVOTriggerDeviceDlg::CSVOTriggerDeviceDlg(CWnd* pParent /*=nullptr*/)
: CPropertyPage(CSVOTriggerDeviceDlg::IDD)
{
	//{{AFX_DATA_INIT(CSVOTriggerDeviceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSVOTriggerDeviceDlg::~CSVOTriggerDeviceDlg()
{
}

void CSVOTriggerDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVOTriggerDeviceDlg)
	DDX_Control(pDX, IDC_BTN_PROP_TRIG, m_btnPropTrigger);
	DDX_Control(pDX, IDC_BTN_NEW_TRIG, m_btnNewTrigger);
	DDX_Control(pDX, IDC_BTN_DELETE_TRIG, m_btnDeleteTrigger);
	DDX_Control(pDX, IDC_BTN_ADVANCED, m_btnAdvanced);
	DDX_Control(pDX, IDC_LST_TRIGGERS, m_ctlTriggerList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSVOTriggerDeviceDlg, CDialog)
	//{{AFX_MSG_MAP(CSVOTriggerDeviceDlg)
	ON_BN_CLICKED(IDC_BTN_PROP_TRIG, OnBtnPropTrig)
	ON_BN_CLICKED(IDC_BTN_NEW_TRIG, OnBtnNewTrig)
	ON_BN_CLICKED(IDC_BTN_DELETE_TRIG, OnBtnDeleteTrig)
	ON_BN_CLICKED(IDC_BTN_ADVANCED, OnBtnAdvanced)
	ON_LBN_DBLCLK(IDC_LST_TRIGGERS, OnDblclkLstTriggers)
	ON_LBN_SELCHANGE(IDC_LST_TRIGGERS, OnSelchangeLstTriggers)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVOTriggerDeviceDlg message handlers

BOOL CSVOTriggerDeviceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    m_pParent = (CSVOConfigAssistantDlg*)GetParent()->GetParent();
    SetupList();
    
	SetWindowContextHelpId(IDD+SvOr::HELPFILE_SVORESOURCE_OFFSET);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSVOTriggerDeviceDlg::SetupList()
{
	typedef std::map<long, CString> SortedTriggerList;
	SortedTriggerList sortedList;

    m_ctlTriggerList.ResetContent();
    int iTrigCount = m_pParent->GetTriggerListCount();
    SvTi::SVOTriggerObjPtr pTriggerObj( nullptr );

    for (int i = 0; i < iTrigCount; i++)
    {
        pTriggerObj = m_pParent->GetTriggerObject(i);
		if( nullptr != pTriggerObj )
		{
			sortedList.insert(std::make_pair(pTriggerObj->GetTriggerDigNumber(), pTriggerObj->GetTriggerDisplayName()));
		}
    }
	for (SortedTriggerList::const_iterator it = sortedList.begin();it != sortedList.end();++it)
	{
		int iPos = m_ctlTriggerList.AddString(it->second);
        m_ctlTriggerList.SetItemData(iPos, it->first);
	}

    m_ctlTriggerList.SetCurSel(0); //set to top of list...

    if ( iTrigCount >= m_pParent->GetAllowedNumberOfDigs(TRUE) )
    {
        m_btnNewTrigger.EnableWindow(FALSE);
    }
    else
    {
        m_btnNewTrigger.EnableWindow(TRUE);
    }

	if (iTrigCount)
	{
		EnablePropertyEdit(0);
	}
}

// need to get a list of next trigger types based on:
// 1. must be a digital system
// 2. must have a corresponding camera
void CSVOTriggerDeviceDlg::GetNextAvailableTriggerList(SVTriggerNameIdList& rList) const
{
	SVString name;
	int id = m_pParent->GetNextTriggerID();
	bool bNonIOSVIM = m_pParent->IsNonIOSVIM(m_pParent->GetProductType());
	
	if (!bNonIOSVIM)
	{
		name = m_pParent->GetNextTriggerName(SvO::cTriggerFixedName);
		rList.insert(std::make_pair(name, id));
	}
	if (m_pParent->IsValidCamera(id) && m_pParent->IsDigitalSystem())
	{
		name = m_pParent->GetNextTriggerName(scCameraTriggerTag);
		rList.insert(std::make_pair(name, id));
	}
}

void CSVOTriggerDeviceDlg::OnBtnNewTrig() 
{
	SVTriggerNameIdList list;
	GetNextAvailableTriggerList(list);
	CString triggerName;
	int iDig = -1;
	if (list.size() > 1)
	{
		SVTriggerSelectionDlg dlg(list, this);

		if ( dlg.DoModal() == IDOK )
		{
			dlg.GetSelectedTrigger(triggerName, iDig);
		}
	}
	else if (list.size())
	{
		SVTriggerNameIdList::const_iterator it = list.begin();
		triggerName = it->first.c_str();
		iDig = it->second;
	}
	if (!triggerName.IsEmpty())
	{
		// find higher iDig
		int pos = -1;
		for (int i = 0;i < m_ctlTriggerList.GetCount() && pos < 0;i++)
		{
			int digitizerNum = static_cast<int>(m_ctlTriggerList.GetItemData(i));
			if (digitizerNum > iDig)
			{
				pos = i;
			}
		}
		
		// Insert @
		int iPos = m_ctlTriggerList.InsertString(pos, triggerName);

		m_pParent->AddToTriggerList(triggerName, iDig);
		m_ctlTriggerList.SetCurSel(iPos); //select new Trigger
		m_ctlTriggerList.SetItemData(iPos, iDig);

		m_pParent->SetModified(true);
		m_pParent->ItemChanged(TRIGGER_DLG, triggerName, ITEM_ACTION_NEW);
	}
	if ( m_ctlTriggerList.GetCount() >= m_pParent->GetAllowedNumberOfDigs(true) )
	{
		m_btnNewTrigger.EnableWindow(false);
	}
	EnablePropertyEdit(m_ctlTriggerList.GetCurSel());
}

void CSVOTriggerDeviceDlg::OnBtnDeleteTrig() 
{
	CString sTrigger;
    int iPos = m_ctlTriggerList.GetCurSel();

    if (iPos != LB_ERR)
    {
        m_ctlTriggerList.GetText(iPos,sTrigger);
        m_ctlTriggerList.DeleteString(iPos);
        m_pParent->RemoveTriggerFromList(sTrigger);
        m_pParent->SetModified(TRUE);
        m_pParent->ItemChanged(TRIGGER_DLG,sTrigger,ITEM_ACTION_DELETE);
    }

    if (iPos > 0)
    {
        m_ctlTriggerList.SetCurSel(iPos-1);
		EnablePropertyEdit(m_ctlTriggerList.GetCurSel());
    }
    else
    {
        m_ctlTriggerList.SetCurSel(0);
		EnableAdvancedPropertyButton(false);
    }

    if ( m_ctlTriggerList.GetCount() < m_pParent->GetAllowedNumberOfDigs(TRUE) )
    {
        m_btnNewTrigger.EnableWindow(TRUE);
    }	
}

void CSVOTriggerDeviceDlg::OnBtnPropTrig() 
{
    int iCurSel = m_ctlTriggerList.GetCurSel();
	if (iCurSel != LB_ERR)
	{
		CString sTxt;
		CString sLabel;
		m_ctlTriggerList.GetText(iCurSel,sTxt);
		SvTi::SVOTriggerObjPtr pTriggerObj = m_pParent->GetTriggerObjectByName(sTxt);
		if( nullptr != pTriggerObj )
		{
			CSVOPropertyPageDlg oDlg;
			SvTi::SVOTriggerObj& rTmpObj( oDlg.getTriggerObject() );

			rTmpObj = *pTriggerObj;
			oDlg.SetDlgPage(VIRTUAL_TRIGGER_DLG);
			oDlg.SetProductType(m_pParent->GetProductType());
			if (oDlg.DoModal() == IDOK)
			{
				*pTriggerObj = rTmpObj;
				m_pParent->SetModified(TRUE);
				EnablePropertyEdit(iCurSel);
				m_pParent->ItemChanged(TRIGGER_DLG, sTxt, ITEM_ACTION_PROP);
			}
		}
	}
}

void CSVOTriggerDeviceDlg::OnBtnAdvanced() 
{
    int iCurSel = m_ctlTriggerList.GetCurSel();
    CString sTxt;
    CString sLabel;
    if ( iCurSel != LB_ERR )
    {
        m_ctlTriggerList.GetText(iCurSel,sTxt);
		SvTi::SVOTriggerObjPtr pTriggerObj = m_pParent->GetTriggerObjectByName(sTxt);
		if( nullptr != pTriggerObj )
		{
			CSVOPropertyPageDlg oDlg;
			SvTi::SVOTriggerObj& rTmpObj( oDlg.getTriggerObject() );

			rTmpObj = *pTriggerObj;
			oDlg.SetDlgPage(VIRTUAL_TRIGGER_ADV);
			oDlg.SetProductType(m_pParent->GetProductType());
			if (oDlg.DoModal() == IDOK)
			{
				*pTriggerObj = rTmpObj;
				m_pParent->SetModified(TRUE);
			}
		}
    }
}

void CSVOTriggerDeviceDlg::OnDblclkLstTriggers() 
{
    OnBtnPropTrig();
}

void CSVOTriggerDeviceDlg::OnSelchangeLstTriggers() 
{
	int iSelection = m_ctlTriggerList.GetCurSel();
	if (iSelection != LB_ERR)
	{
		EnablePropertyEdit(iSelection);
	}
	else
	{
		DisablePropertyEdit();
	}
}

BOOL CSVOTriggerDeviceDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	::SendMessage( m_pParent->GetSafeHwnd(), WM_HELP, 0, reinterpret_cast<DWORD_PTR>(pHelpInfo) );

	return TRUE;
}

void CSVOTriggerDeviceDlg::EnablePropertyEdit(int iSelection)
{
	EnablePropertyButton(true);

	// check for advanced properties
	CString sTxt;
	m_ctlTriggerList.GetText(iSelection, sTxt);
    SvTi::SVOTriggerObjPtr pTriggerObj = m_pParent->GetTriggerObjectByName(sTxt);
	if( nullptr != pTriggerObj && pTriggerObj->IsSoftwareTrigger())
	{
		EnableAdvancedPropertyButton(true);
	}
	 else
	{
		EnableAdvancedPropertyButton(false);
	}
}

void CSVOTriggerDeviceDlg::DisablePropertyEdit()
{
	EnablePropertyButton(false);
	EnableAdvancedPropertyButton(false);
}

void CSVOTriggerDeviceDlg::EnablePropertyButton(bool bEnable)
{
	m_btnPropTrigger.EnableWindow(bEnable);
}

void CSVOTriggerDeviceDlg::EnableAdvancedPropertyButton(bool bEnable)
{
	m_btnAdvanced.EnableWindow(bEnable);
}

