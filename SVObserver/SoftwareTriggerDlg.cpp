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

#pragma region Includes
#include "stdafx.h"
#include "SoftwareTriggerDlg.h"
#include "TriggerInformation\SVTriggerConstants.h"
#pragma endregion Includes

#define COLOR_WARN RGB(255,240,64)

// SoftwareTriggerDlg dialog

IMPLEMENT_DYNAMIC(SVSoftwareTriggerDlg, CDialog)

SVSoftwareTriggerDlg::SVSoftwareTriggerDlg(CWnd* pParent /*=nullptr*/): 
	CDialog(SVSoftwareTriggerDlg::IDD, pParent),
	m_knobCtrl(SvTi::MinTimerPeriod_ms, SvTi::MaxTimerPeriod_ms)
{
}

SVSoftwareTriggerDlg::~SVSoftwareTriggerDlg()
{
	delete m_pSpins;
	delete m_pBrush;
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
		SVTriggerProxy* pTrigger = reinterpret_cast<SVTriggerProxy*> (item.lParam);
		if( nullptr != pTrigger )
		{
			int Value = pTrigger->GetSoftwareTriggerPeriod();

			std::string Text = SvUl::Format( _T("%d"), Value );
			m_intervalEdit.SetWindowText( Text.c_str() );
			m_pSpins->SetValue( Value );
			m_knobCtrl.SetValue( Value );
			SetFrequency( Value );
			m_pauseBtn.SetWindowText( pTrigger->ButtonText().c_str() );
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
			SVTriggerProxy* pTrigger = reinterpret_cast<SVTriggerProxy *>(item.lParam);
			pTrigger->Continue();
		}
	}
}

bool SVSoftwareTriggerDlg::EditOK()
{
	CString Text;
	m_intervalEdit.GetWindowText(Text);
	int Value = _ttoi(Text);
	return (SvTi::MinTimerPeriod_ms <= Value && Value <= SvTi::MaxTimerPeriod_ms);
}

void SVSoftwareTriggerDlg::OnEnChangeUsecEdit()
{
	CString Text;
	m_intervalEdit.GetWindowText(Text);
	int Value = _ttoi(Text);
	if( EditOK() )
	{
		m_pSpins->SetValue( Value );
		m_knobCtrl.SetValue( Value );
		SetFrequency( Value );
		SetTriggerPeriod( Value );
	}
}

void SVSoftwareTriggerDlg::OnBnClickedOk()
{
	ShowWindow(SW_HIDE);
}

int SVSoftwareTriggerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	m_knobCtrl.CreateBitmaps();
	m_pBrush = new CBrush(COLOR_WARN);

	return 0;
}

LRESULT SVSoftwareTriggerDlg::OnTriggerChange(WPARAM wParam, LPARAM lParam)
{
	int Value = static_cast< int >( wParam );
	std::string Text = SvUl::Format( _T("%d"), Value );
	m_intervalEdit.SetWindowText( Text.c_str() );
	m_pSpins->SetValue( Value );
	SetFrequency( Value );
	SetTriggerPeriod( Value );
	return TRUE;
}

void SVSoftwareTriggerDlg::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SVSpinGroup * l_spin = m_pSpins->find( static_cast< int >( pNMUpDown->hdr.idFrom ) );
	*pResult = 0;
	if (l_spin)
	{
		int Value = pNMUpDown->iDelta;
		if (!l_spin->Increment(Value)) //, boost::bind(&SVSpinGroup::GetValue, m_spins)))
		{
			*pResult = 1;
		}
		else
		{
			Value = m_pSpins->GetValue();
			int tmp = m_knobCtrl.SetValue(Value);		
			if (tmp != Value)
			{
				m_pSpins->SetValue(tmp);
				Value = tmp;
			}
			std::string Text = SvUl::Format( _T("%d"), Value );
			m_intervalEdit.SetWindowText( Text.c_str() );
			SetFrequency( Value );
			SetTriggerPeriod( Value );
		}
	}
}


BOOL SVSoftwareTriggerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pSpins = 
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
			SVTriggerProxy* pTrigger = reinterpret_cast<SVTriggerProxy *>(item.lParam);
			delete pTrigger;
		}
	}
	m_triggerTabs.DeleteAllItems();
}

bool SVSoftwareTriggerDlg::AddTrigger(SvTi::SVTriggerObject* pTrigger)
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
	static SVSoftwareTriggerDlg dlg(nullptr);
	static bool created = false;
	if (!created)
	{
		created = true;
		CWnd* pParent = ::AfxGetMainWnd();
		while (nullptr != pParent->GetParent())
		{
			pParent = pParent->GetParent();
		}
		dlg.Create(IDD_TRIGGER_PERIOD_DLG, pParent);
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
		SVTriggerProxy* pTrigger = reinterpret_cast<SVTriggerProxy*> (item.lParam);
		pTrigger->SetSoftwareTriggerPeriod(val, true);
	}
}

void SVSoftwareTriggerDlg::SetFrequency( int Value )
{
	double Frequency = 1000.0 / Value;
	std::string Text = SvUl::Format( _T("%.4fHz"), Frequency);
	m_frequency.SetWindowText( Text.c_str() );
	Frequency *= 60;
	Text = SvUl::Format( _T("%.2f/min"), Frequency);
	m_ppmLabel.SetWindowText( Text.c_str() );
}

void SVSoftwareTriggerDlg::OnEnKillfocusUsecEdit()
{
	int Value = m_knobCtrl.GetValue();
	std::string Text = SvUl::Format( _T("%d"), Value );
	m_intervalEdit.SetWindowText( Text.c_str() );
}

HBRUSH SVSoftwareTriggerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor == CTLCOLOR_EDIT && !EditOK())
	{
		pDC->SetBkColor(COLOR_WARN);
		return (HBRUSH)m_pBrush->GetSafeHandle();
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
		SVTriggerProxy* pTrigger = reinterpret_cast<SVTriggerProxy *>(item.lParam);
		pTrigger->Toggle();
		m_pauseBtn.SetWindowText( pTrigger->ButtonText().c_str() );
	}
}

