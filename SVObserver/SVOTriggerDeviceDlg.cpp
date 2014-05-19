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

#include "stdafx.h"
#include <map>
#include "SVOTriggerDeviceDlg.h"
#include "SVOConfigAssistantDlg.h"
#include "SVOPropertyPageDlg.h"
#include "SVOTriggerObj.h"
#include "SVTriggerSelectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const CString scCameraTriggerTag = _T("CameraTrigger_");

CSVOTriggerDeviceDlg::CSVOTriggerDeviceDlg(CWnd* pParent /*=NULL*/)
: CPropertyPage(CSVOTriggerDeviceDlg::IDD)
{
	//{{AFX_DATA_INIT(CSVOTriggerDeviceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
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
    
	SetWindowContextHelpId(IDD+0x20000);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSVOTriggerDeviceDlg::SetupList()
{
	typedef std::map<long, CString> SortedTriggerList;
	SortedTriggerList sortedList;

    m_ctlTriggerList.ResetContent();
    int iTrigCount = m_pParent->GetTriggerListCount();
    CSVOTriggerObj *pObj;

    for (int i = 0; i < iTrigCount; i++)
    {
        pObj = m_pParent->GetTriggerObject(i);
		sortedList.insert(std::make_pair(pObj->GetTriggerDigNumber(), pObj->GetTriggerDisplayName()));
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
		name = m_pParent->GetNextTriggerName();
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
		triggerName = it->first.ToString();
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
		CSVOTriggerObj *pObj = m_pParent->GetTriggerObjectByName(sTxt);
		CSVOTriggerObj TmpObj(*pObj);
		CSVOPropertyPageDlg oDlg;
		oDlg.SetTriggerObject(&TmpObj);
		oDlg.SetDlgPage(VIRTUAL_TRIGGER_DLG);
		oDlg.SetProductType(m_pParent->GetProductType());
		if (oDlg.DoModal() == IDOK)
		{
			*pObj = TmpObj;
			m_pParent->SetModified(TRUE);
			EnablePropertyEdit(iCurSel);
			m_pParent->ItemChanged(TRIGGER_DLG, sTxt, ITEM_ACTION_PROP);
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
        CSVOTriggerObj *pObj = m_pParent->GetTriggerObjectByName(sTxt);
        CSVOTriggerObj TmpObj(*pObj);
        CSVOPropertyPageDlg oDlg;
        oDlg.SetTriggerObject(&TmpObj);
        oDlg.SetDlgPage(VIRTUAL_TRIGGER_ADV);
        oDlg.SetProductType(m_pParent->GetProductType());
        if (oDlg.DoModal() == IDOK)
        {
            *pObj = TmpObj;
            m_pParent->SetModified(TRUE);
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
	::SendMessage( m_pParent->GetSafeHwnd(), WM_HELP, NULL, reinterpret_cast<DWORD_PTR>(pHelpInfo) );

	return TRUE;
}

void CSVOTriggerDeviceDlg::EnablePropertyEdit(int iSelection)
{
	EnablePropertyButton(true);

	// check for advanced properties
	CString sTxt;
	m_ctlTriggerList.GetText(iSelection, sTxt);
    CSVOTriggerObj* pObj = m_pParent->GetTriggerObjectByName(sTxt);
	if (pObj && pObj->IsSoftwareTrigger())
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOTriggerDeviceDlg.cpp_v  $
 * 
 *    Rev 1.5   15 May 2014 11:38:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed OnHelpInfo sendMessage to use DWORD_PTR cast.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Oct 2013 14:49:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  870
 * SCR Title:  Fix Various Issues in the Edit Configuration Dialog
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed the issue with deleting triggers that caused an error dialog to appear. Modified OnBtnDeleteTrig.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Oct 2013 07:01:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Aug 2013 09:43:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  851
 * SCR Title:  Fix bugs in the Configuration Assistant dialog
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added EnablePropertyEdit( to OnBtnDelete and OnBtnNew so the advanced button will be enabled or grayed accordingly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Jul 2013 13:28:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 023.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Jul 2013 12:38:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to sort by trigger handle not by Name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:12:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   21 Jan 2013 11:32:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVTriggerSelectionDlg
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Jan 2013 16:39:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated and added methods to handle the new look-up  and add  functionality.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   26 Jan 2011 10:29:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  713
 * SCR Title:  Fix Help button on Configuration Assistant Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the trigger dialog to be consistant with the other tabs.  when you double click on a trigger, it will bring up the properties.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   26 Jan 2011 09:07:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  713
 * SCR Title:  Fix Help button on Configuration Assistant Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to set the ContextHelp ID
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Sep 2008 17:07:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support settings for Software Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Nov 2003 13:46:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Board -> Product
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:27:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
