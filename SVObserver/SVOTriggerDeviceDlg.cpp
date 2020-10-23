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
#include "SVOResource/ConstGlobalSvOr.h"
#include "TriggerInformation/SVOTriggerObj.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVOTriggerDeviceDlg::SVOTriggerDeviceDlg(CWnd* /*=nullptr*/)
: CPropertyPage(SVOTriggerDeviceDlg::IDD), m_pParent(nullptr)
{
	//{{AFX_DATA_INIT(SVOTriggerDeviceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVOTriggerDeviceDlg::~SVOTriggerDeviceDlg()
{
}

void SVOTriggerDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOTriggerDeviceDlg)
	DDX_Control(pDX, IDC_BTN_PROP_TRIG, m_btnPropTrigger);
	DDX_Control(pDX, IDC_BTN_NEW_TRIG, m_btnNewTrigger);
	DDX_Control(pDX, IDC_BTN_DELETE_TRIG, m_btnDeleteTrigger);
	DDX_Control(pDX, IDC_BTN_ADVANCED, m_btnAdvanced);
	DDX_Control(pDX, IDC_LST_TRIGGERS, m_ctlTriggerList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVOTriggerDeviceDlg, CDialog)
	//{{AFX_MSG_MAP(SVOTriggerDeviceDlg)
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
// SVOTriggerDeviceDlg message handlers

BOOL SVOTriggerDeviceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    m_pParent = (SVOConfigAssistantDlg*)GetParent()->GetParent();
    SetupList();
    
	SetWindowContextHelpId(IDD+SvOr::HELPFILE_DLG_IDD_OFFSET);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVOTriggerDeviceDlg::SetupList()
{
	typedef std::map<long, std::string> SortedTriggerList;
	SortedTriggerList sortedList;

    m_ctlTriggerList.ResetContent();
    int iTrigCount = m_pParent->GetTriggerListCount();
    SvTi::SVOTriggerObjPtr pTriggerObj( nullptr );

    for (int i = 0; i < iTrigCount; i++)
    {
        pTriggerObj = m_pParent->GetTriggerObject(i);
		if( nullptr != pTriggerObj )
		{
			sortedList.insert(std::make_pair(pTriggerObj->GetTriggerDigNumber(), std::string( pTriggerObj->GetTriggerDisplayName() ) ) );
		}
    }
	for (SortedTriggerList::const_iterator it = sortedList.begin();it != sortedList.end();++it)
	{
		int iPos = m_ctlTriggerList.AddString(it->second.c_str());
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

void SVOTriggerDeviceDlg::OnBtnNewTrig() 
{
	std::string triggername = m_pParent->GetNextTriggerName(SvDef::cTriggerFixedName);
	if (!triggername.empty())
	{
		int id = m_pParent->GetNextTriggerID();
		// find higher iDig
		int pos = -1;
		for (int i = 0;i < m_ctlTriggerList.GetCount() && pos < 0;i++)
		{
			int digitizerNum = static_cast<int>(m_ctlTriggerList.GetItemData(i));
			if (digitizerNum > id)
			{
				pos = i;
			}
		}
		
		// Insert @
		int iPos = m_ctlTriggerList.InsertString(pos, triggername.c_str());

		m_pParent->AddToTriggerList(triggername.c_str(), id);
		m_ctlTriggerList.SetCurSel(iPos); //select new Trigger
		m_ctlTriggerList.SetItemData(iPos, id);

		m_pParent->SetModified(true);
		m_pParent->ItemChanged(TRIGGER_DLG, triggername.c_str(), ITEM_ACTION_NEW);
	}
	if ( m_ctlTriggerList.GetCount() >= m_pParent->GetAllowedNumberOfDigs(true) )
	{
		m_btnNewTrigger.EnableWindow(false);
	}
	EnablePropertyEdit(m_ctlTriggerList.GetCurSel());
}

void SVOTriggerDeviceDlg::OnBtnDeleteTrig() 
{
	CString TriggerName;
    int iPos = m_ctlTriggerList.GetCurSel();

    if (iPos != LB_ERR)
    {
        m_ctlTriggerList.GetText(iPos, TriggerName);
        m_ctlTriggerList.DeleteString(iPos);
        m_pParent->RemoveTriggerFromList( TriggerName );
        m_pParent->SetModified(TRUE);
        m_pParent->ItemChanged( TRIGGER_DLG, TriggerName, ITEM_ACTION_DELETE);
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

void SVOTriggerDeviceDlg::OnBtnPropTrig() 
{
    int iCurSel = m_ctlTriggerList.GetCurSel();
	if (iCurSel != LB_ERR)
	{
		CString TriggerName;
		m_ctlTriggerList.GetText(iCurSel,TriggerName);
		SvTi::SVOTriggerObjPtr pTriggerObj = m_pParent->GetTriggerObjectByName(TriggerName);
		if( nullptr != pTriggerObj )
		{
			SVOPropertyPageDlg oDlg;
			SvTi::SVOTriggerObj& rTmpObj( oDlg.getTriggerObject() );

			rTmpObj = *pTriggerObj;
			oDlg.SetDlgPage(VIRTUAL_TRIGGER_DLG);
			oDlg.SetProductType(m_pParent->GetProductType());
			if (oDlg.DoModal() == IDOK)
			{
				*pTriggerObj = rTmpObj;
				m_pParent->SetModified(TRUE);
				EnablePropertyEdit(iCurSel);
				m_pParent->ItemChanged(TRIGGER_DLG, TriggerName, ITEM_ACTION_PROP);
			}
		}
	}
}

void SVOTriggerDeviceDlg::OnBtnAdvanced() 
{
    int iCurSel = m_ctlTriggerList.GetCurSel();

	if ( iCurSel != LB_ERR )
    {
		CString TriggerName;
        m_ctlTriggerList.GetText(iCurSel,TriggerName);
		SvTi::SVOTriggerObjPtr pTriggerObj = m_pParent->GetTriggerObjectByName(TriggerName);
		if( nullptr != pTriggerObj )
		{
			SVOPropertyPageDlg oDlg;
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

void SVOTriggerDeviceDlg::OnDblclkLstTriggers() 
{
    OnBtnPropTrig();
}

void SVOTriggerDeviceDlg::OnSelchangeLstTriggers() 
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

BOOL SVOTriggerDeviceDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	::SendMessage( m_pParent->GetSafeHwnd(), WM_HELP, 0, reinterpret_cast<DWORD_PTR>(pHelpInfo) );

	return TRUE;
}

void SVOTriggerDeviceDlg::EnablePropertyEdit(int iSelection)
{
	EnablePropertyButton(true);

	// check for advanced properties
	CString TriggerName;
	m_ctlTriggerList.GetText(iSelection, TriggerName);
    SvTi::SVOTriggerObjPtr pTriggerObj = m_pParent->GetTriggerObjectByName(TriggerName);
	bool buttonEnabled{ false };
	if( nullptr != pTriggerObj)
	{
		buttonEnabled = SvDef::TriggerType::SoftwareTrigger == pTriggerObj->getTriggerType();
		buttonEnabled |= SvDef::TriggerType::CameraTrigger == pTriggerObj->getTriggerType();
	}
	EnableAdvancedPropertyButton(buttonEnabled);
}

void SVOTriggerDeviceDlg::DisablePropertyEdit()
{
	EnablePropertyButton(false);
	EnableAdvancedPropertyButton(false);
}

void SVOTriggerDeviceDlg::EnablePropertyButton(bool bEnable)
{
	m_btnPropTrigger.EnableWindow(bEnable);
}

void SVOTriggerDeviceDlg::EnableAdvancedPropertyButton(bool bEnable)
{
	m_btnAdvanced.EnableWindow(bEnable);
}

