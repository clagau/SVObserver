//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerDlg
//* .File Name       : $Workfile:   SoftwareTriggerDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:54:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SoftwareTriggerDlg.h"

#define COLOR_WARN RGB(255,240,64)

// SoftwareTriggerDlg dialog
namespace
{
	void dummy(SVTriggerObject*) {}
}

IMPLEMENT_DYNAMIC(SVSoftwareTriggerDlg, CDialog)

SVSoftwareTriggerDlg::SVSoftwareTriggerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVSoftwareTriggerDlg::IDD, pParent), set_dirty(dummy)
{
	m_spins = 0;
}

SVSoftwareTriggerDlg::~SVSoftwareTriggerDlg()
{
	delete m_spins;
	delete m_brush;
}

void SVSoftwareTriggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRIGGER_TABS, m_triggerTabs);
	DDX_Control(pDX, IDC_USEC_EDIT, m_intervalEdit);
	DDX_Control(pDX, IDC_INTERVALWHEEL, m_knobCtrl);
	DDX_Control(pDX, IDC_SPINMSEC, m_msecSpin);
	DDX_Control(pDX, IDC_EDITMSEC, m_msecEdit);
	DDX_Control(pDX, IDC_STATICMSEC, m_msecLabel);
	DDX_Control(pDX, IDC_SPINSEC, m_secSpin);
	DDX_Control(pDX, IDC_EDITSEC, m_secEdit);
	DDX_Control(pDX, IDC_STATICSEC, m_secLabel);
	DDX_Control(pDX, IDC_SPINMIN, m_minSpin);
	DDX_Control(pDX, IDC_EDITMIN, m_minEdit);
	DDX_Control(pDX, IDC_STATICMIN, m_minLabel);
	DDX_Control(pDX, IDC_SPINHOUR, m_hourSpin);
	DDX_Control(pDX, IDC_EDITHOUR, m_hourEdit);
	DDX_Control(pDX, IDC_STATICHOUR, m_hourLabel);
	DDX_Control(pDX, IDC_SPINDAY, m_daySpin);
	DDX_Control(pDX, IDC_EDITDAY, m_dayEdit);
	DDX_Control(pDX, IDC_STATICDAY, m_dayLabel);
	DDX_Control(pDX, IDC_FREQUENCY, m_frequency);
	DDX_Control(pDX, IDC_PARTSPERMINUTE, m_ppmLabel);
	DDX_Control(pDX, IDC_PAUSEBUTTON, m_pauseBtn);
}


BEGIN_MESSAGE_MAP(SVSoftwareTriggerDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TRIGGER_TABS, &SVSoftwareTriggerDlg::OnTcnSelchangeTriggerTabs)
	ON_EN_CHANGE(IDC_USEC_EDIT, &SVSoftwareTriggerDlg::OnEnChangeUsecEdit)
	ON_BN_CLICKED(IDOK, &SVSoftwareTriggerDlg::OnBnClickedOk)
	ON_WM_CREATE()
	ON_MESSAGE(WM_TRIGGER_CHANGE, &SVSoftwareTriggerDlg::OnTriggerChange)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINMSEC, &SVSoftwareTriggerDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINSEC, &SVSoftwareTriggerDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINMIN, &SVSoftwareTriggerDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINHOUR, &SVSoftwareTriggerDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINDAY, &SVSoftwareTriggerDlg::OnDeltaposSpin)
	ON_EN_KILLFOCUS(IDC_USEC_EDIT, &SVSoftwareTriggerDlg::OnEnKillfocusUsecEdit)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_PAUSEBUTTON, &SVSoftwareTriggerDlg::OnBnClickedPausebutton)
END_MESSAGE_MAP()


// SoftwareTriggerDlg message handlers

int SVSoftwareTriggerDlg::SelectTrigger()
{
	int result = 0;
	int idx = m_triggerTabs.GetCurSel();
	TCITEM item;
	memset(&item, 0, sizeof(TCITEM));
	item.mask = TCIF_PARAM;
	if (m_triggerTabs.GetItem(idx, &item))
	{
		SVTriggerProxy * l_trigger = reinterpret_cast<SVTriggerProxy *>(item.lParam);
		if (l_trigger)
		{
			int val = l_trigger->GetSoftwareTriggerPeriod();
			CString str;
			str.Format("%d", val);
			m_intervalEdit.SetWindowText(str);
			m_spins->SetValue(val);
			m_knobCtrl.SetValue(val);
			double l_freq = 1000.0 / val;
			str.Format("%.4fHz", l_freq);
			m_frequency.SetWindowText(str);
			l_freq *= 60;
			str.Format("%.2f/min", l_freq);
			m_ppmLabel.SetWindowText(str);
			m_pauseBtn.SetWindowText(l_trigger->ButtonText());
		}
		else
		{
			result = 1;
		}
	}
	else
	{
		result = 1;
	}
	return result;
}

void SVSoftwareTriggerDlg::OnTcnSelchangeTriggerTabs(NMHDR *, LRESULT *pResult)
{
	*pResult = SelectTrigger();
}

void SVSoftwareTriggerDlg::OnStop()
{
	for (int i = 0; i < m_triggerTabs.GetItemCount(); ++i)
	{
		TCITEM item;
		memset(&item, 0, sizeof(TCITEM));
		item.mask = TCIF_PARAM;
		if (m_triggerTabs.GetItem(i, &item))
		{
			SVTriggerProxy * l_trigger = reinterpret_cast<SVTriggerProxy *>(item.lParam);
			l_trigger->Continue();
		}
	}
}

bool SVSoftwareTriggerDlg::EditOK()
{
	CString str;
	m_intervalEdit.GetWindowText(str);
	int l_val = _ttoi(str);
	return (20 <= l_val && l_val <= KNOB_MAX);
}

void SVSoftwareTriggerDlg::OnEnChangeUsecEdit()
{
	CString str;
	m_intervalEdit.GetWindowText(str);
	int l_val = _ttoi(str);
	if (EditOK())
	{
		double l_freq = 1000.0 / l_val;
		str.Format("%.4fHz", l_freq);
		m_frequency.SetWindowText(str);
		l_freq *= 60;
		str.Format("%.2f/min", l_freq);
		m_ppmLabel.SetWindowText(str);
		m_knobCtrl.SetValue(l_val);
		m_spins->SetValue(l_val);
		SetTriggerPeriod(l_val);
	}
}

void SVSoftwareTriggerDlg::OnBnClickedOk()
{
	ShowWindow(SW_HIDE);
}

int SVSoftwareTriggerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_knobCtrl.CreateBitmaps();
	m_brush = new CBrush(COLOR_WARN);

	return 0;
}

LRESULT SVSoftwareTriggerDlg::OnTriggerChange(WPARAM wParam, LPARAM lParam)
{
	CString str;
	str.Format("%d", wParam);
	m_intervalEdit.SetWindowText(str);
	m_spins->SetValue( static_cast< int >( wParam ) );
	double l_freq = 1000.0 / wParam;
	str.Format("%.4fHz", l_freq);
	m_frequency.SetWindowText(str);
	l_freq *= 60;
	str.Format("%.2f/min", l_freq);
	m_ppmLabel.SetWindowText(str);
	SetTriggerPeriod( static_cast< int >( wParam ) );
	return TRUE;
}

void SVSoftwareTriggerDlg::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SVSpinGroup * l_spin = m_spins->find( static_cast< int >( pNMUpDown->hdr.idFrom ) );
	*pResult = 0;
	if (l_spin)
	{
		int val = pNMUpDown->iDelta;
		if (!l_spin->Increment(val)) //, boost::bind(&SVSpinGroup::GetValue, m_spins)))
		{
			*pResult = 1;
		}
		else
		{
			val = m_spins->GetValue();
			int tmp = m_knobCtrl.SetValue(val);		
			if (tmp != val)
			{
				m_spins->SetValue(tmp);
				val = tmp;
			}
			CString str;
			str.Format("%d", val);
			m_intervalEdit.SetWindowText(str);
			double l_freq = 1000.0 / val;
			str.Format("%.4fHz", l_freq);
			m_frequency.SetWindowText(str);
			l_freq *= 60;
			str.Format("%.2f/min", l_freq);
			m_ppmLabel.SetWindowText(str);
			SetTriggerPeriod(val);
		}
	}
}


BOOL SVSoftwareTriggerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_spins = 
		new SVSpinGroup(
			m_msecSpin, 
			m_msecEdit, 
			m_msecLabel, 
			sv::Def(true, 1000), 
			new SVSpinGroup(
				m_secSpin,
				m_secEdit,
				m_secLabel,
				sv::Def(false, 121), 0
				//new SVSpinGroup(
				//	m_minSpin,
				//	m_minEdit,
				//	m_minLabel,
				//	sv::Def(false, 60), 
				//	new SVSpinGroup(
				//		m_hourSpin,
				//		m_hourEdit,
				//		m_hourLabel,
				//		sv::Def(false, 24), 
				//		new SVSpinGroup(
				//			m_daySpin,
				//			m_dayEdit,
				//			m_dayLabel,
				//			sv::Def(false, 100), 
				//			0
				//		)
				//	)
				//)
			)				
		);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void SVSoftwareTriggerDlg::ClearTriggers()
{
	OnStop();
	for (int i = 0; i < m_triggerTabs.GetItemCount(); ++i)
	{
		TCITEM item;
		memset(&item, 0, sizeof(TCITEM));
		item.mask = TCIF_PARAM;
		if (m_triggerTabs.GetItem(i, &item))
		{
			SVTriggerProxy * l_trigger = reinterpret_cast<SVTriggerProxy *>(item.lParam);
			delete l_trigger;
		}
	}
	m_triggerTabs.DeleteAllItems();
}

bool SVSoftwareTriggerDlg::AddTrigger(SVTriggerObject* pTrigger)
{
	ASSERT( nullptr != pTrigger );
	if( nullptr == pTrigger ){ return false; }

	int l_count = m_triggerTabs.GetItemCount();
	return m_triggerTabs.InsertItem(
		TCIF_TEXT|TCIF_PARAM, 
		l_count, 
		pTrigger->GetName(), 
		0, 
		reinterpret_cast<LPARAM>(new SVTriggerProxy(pTrigger))) > -1;
}

SVSoftwareTriggerDlg & SVSoftwareTriggerDlg::Instance()
{
	static SVSoftwareTriggerDlg dlg(NULL);
	static bool created = false;
	if (!created)
	{
		created = true;
		CWnd * l_parent = ::AfxGetMainWnd();
		while (l_parent->GetParent() != NULL)
		{
			l_parent = l_parent->GetParent();
		}
		dlg.Create(IDD_TRIGGER_PERIOD_DLG, l_parent);
	}
	return dlg;
}

void SVSoftwareTriggerDlg::SetTriggerPeriod(int val)
{
	int idx = m_triggerTabs.GetCurSel();
	TCITEM item;
	memset(&item, 0, sizeof(TCITEM));
	item.mask = TCIF_PARAM;
	if (m_triggerTabs.GetItem(idx, &item) && item.lParam)
	{
		SVTriggerProxy * l_trigger = reinterpret_cast<SVTriggerProxy *>(item.lParam);
		l_trigger->SetSoftwareTriggerPeriod(val, true);
		set_dirty(l_trigger->GetTrigger());
	}
}

void SVSoftwareTriggerDlg::OnEnKillfocusUsecEdit()
{
	int l_val = m_knobCtrl.GetValue();
	CString str;
	str.Format("%d", l_val);
	m_intervalEdit.SetWindowText(str);
}

HBRUSH SVSoftwareTriggerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor == CTLCOLOR_EDIT && !EditOK())
	{
		pDC->SetBkColor(COLOR_WARN);
		return (HBRUSH)m_brush->GetSafeHandle();
	}

	return hbr;
}

void SVSoftwareTriggerDlg::OnBnClickedPausebutton()
{
	int idx = m_triggerTabs.GetCurSel();
	TCITEM item;
	memset(&item, 0, sizeof(TCITEM));
	item.mask = TCIF_PARAM;
	if (m_triggerTabs.GetItem(idx, &item) && item.lParam)
	{
		SVTriggerProxy * l_trigger = reinterpret_cast<SVTriggerProxy *>(item.lParam);
		l_trigger->Toggle();
		m_pauseBtn.SetWindowText(l_trigger->ButtonText());
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SoftwareTriggerDlg.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 11:54:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:17:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/