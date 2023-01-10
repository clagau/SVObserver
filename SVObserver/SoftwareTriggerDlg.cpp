//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SoftwareTriggerDlg
//* .File Name       : $Workfile:   SoftwareTriggerDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:54:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SoftwareTriggerDlg.h"
#include "Triggering/SVTriggerClass.h"
#include "Triggering/SVTriggerConstants.h"
#include "Triggering/SVTriggerObject.h"
#pragma endregion Includes

constexpr COLORREF COLOR_WARN = RGB(255, 240, 64);

constexpr const char* cContinue = _T("Continue");
constexpr const char* cPause = _T("Pause");

bool SoftwareTriggerDlg::m_created {false};

BEGIN_MESSAGE_MAP(SoftwareTriggerDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TRIGGER_TABS, &SoftwareTriggerDlg::OnTcnSelchangeTriggerTabs)
	ON_EN_CHANGE(IDC_USEC_EDIT, &SoftwareTriggerDlg::OnEnChangeUsecEdit)
	ON_BN_CLICKED(IDOK, &SoftwareTriggerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SINGLETRIGGER, &SoftwareTriggerDlg::OnSingleTrigger)
	ON_WM_CREATE()
	ON_MESSAGE(WM_TRIGGER_CHANGE, &SoftwareTriggerDlg::OnTriggerChange)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINMSEC, &SoftwareTriggerDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINSEC, &SoftwareTriggerDlg::OnDeltaposSpin)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_PAUSEBUTTON, &SoftwareTriggerDlg::OnBnClickedPausebutton)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

SoftwareTriggerDlg::SoftwareTriggerDlg(CWnd* pParent /*=nullptr*/): 
	CDialog(SoftwareTriggerDlg::IDD, pParent),
	m_knobCtrl(SvTrig::MinTimerPeriod_ms, SvTrig::MaxTimerPeriod_ms),
	m_pBrush(nullptr)
{
}

void SoftwareTriggerDlg::DoDataExchange(CDataExchange* pDX)
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
	DDX_Control(pDX, IDC_FREQUENCY, m_frequency);
	DDX_Control(pDX, IDC_PARTSPERMINUTE, m_ppmLabel);
	DDX_Control(pDX, IDC_PAUSEBUTTON, m_pauseBtn);
	DDX_Control(pDX, IDC_SINGLETRIGGER, m_singleTriggerBtn);
}

int SoftwareTriggerDlg::SelectTrigger()
{
	int result = 0;
	int idx = m_triggerTabs.GetCurSel();
	TCITEM item;
	memset(&item, 0, sizeof(TCITEM));
	item.mask = TCIF_PARAM;
	if (m_triggerTabs.GetItem(idx, &item))
	{
		SvTrig::SVTriggerObject* pTrigger = reinterpret_cast<SvTrig::SVTriggerObject*> (item.lParam);
		if( nullptr != pTrigger )
		{
			int Value = pTrigger->GetSoftwareTriggerPeriod();

			std::string Text = SvUl::Format( _T("%d"), Value );
			m_intervalEdit.SetWindowText( Text.c_str() );
			m_pSpins->SetValue( Value );
			m_knobCtrl.SetValue( Value );
			SetFrequency( Value );

			if (nullptr != pTrigger->getDevice())
			{
				bool paused = pTrigger->getDevice()->getPause();
				m_pauseBtn.SetWindowText(paused ? cContinue : cPause);
				m_singleTriggerBtn.EnableWindow(paused);
			}
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

void SoftwareTriggerDlg::OnTcnSelchangeTriggerTabs(NMHDR *, LRESULT *pResult)
{
	*pResult = SelectTrigger();
}

bool SoftwareTriggerDlg::EditOK(int value)
{
	return (SvTrig::MinTimerPeriod_ms <= value && value <= SvTrig::MaxTimerPeriod_ms && false == m_editFlag);
}

void SoftwareTriggerDlg::OnEnChangeUsecEdit()
{
	m_warnColor = false;
	CString Text;
	m_intervalEdit.GetWindowText(Text);
	int value = _ttoi(Text);
	if( EditOK(value) )
	{
		EditFlag lockEdit {m_editFlag};
		m_pSpins->SetValue( value );
		m_knobCtrl.SetValue( value );
		SetFrequency( value );
		SetTriggerPeriod( value );
	}
	else if(value < SvTrig::MinTimerPeriod_ms || value > SvTrig::MaxTimerPeriod_ms)
	{
		m_warnColor = true;
		Invalidate(true);
	}
}

void SoftwareTriggerDlg::OnBnClickedOk()
{
	ShowWindow(SW_HIDE);
}

int SoftwareTriggerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	m_knobCtrl.CreateBitmaps();
	m_pBrush = new CBrush(COLOR_WARN);

	return 0;
}

LRESULT SoftwareTriggerDlg::OnTriggerChange(WPARAM wParam, LPARAM lParam)
{
	int value = static_cast< int >( wParam );
	if (EditOK(value))
	{
		EditFlag lockEdit {m_editFlag};
		std::string Text = SvUl::Format(_T("%d"), value);
		m_intervalEdit.SetWindowText(Text.c_str());
		m_pSpins->SetValue(value);
		SetFrequency(value);
		//Set the period only after the mouse button is released
		if (1 == lParam)
		{
			SetTriggerPeriod(value);
		}
	}
	return TRUE;
}

void SoftwareTriggerDlg::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SVSpinGroup * l_spin = m_pSpins->find( static_cast< int >( pNMUpDown->hdr.idFrom ) );
	*pResult = 0;
	if (l_spin)
	{
		int value = pNMUpDown->iDelta;
		if (!l_spin->Increment(value))
		{
			*pResult = 1;
		}
		else
		{
			value = m_pSpins->GetValue();
			if (EditOK(value))
			{
				EditFlag lockEdit {m_editFlag};
				m_knobCtrl.SetValue(value);
				std::string Text = SvUl::Format(_T("%d"), value);
				m_intervalEdit.SetWindowText(Text.c_str());
				SetFrequency(value);
				SetTriggerPeriod(value);
			}
		}
	}
}


BOOL SoftwareTriggerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	sv::Def limit1(true, 1000);
	sv::Def limit2(false, 121);
	SVSpinGroup* pNext = new SVSpinGroup(m_secSpin, m_secEdit, m_secLabel, limit2, nullptr);
	m_pSpins = new SVSpinGroup(m_msecSpin, m_msecEdit, m_msecLabel, limit1, pNext);
	m_singleTriggerBtn.EnableWindow(false);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void SoftwareTriggerDlg::ClearTriggers()
{
	m_triggerTabs.DeleteAllItems();
}

bool SoftwareTriggerDlg::AddTrigger(SvTrig::SVTriggerObject* pTrigger, bool paused)
{
	Log_Assert( nullptr != pTrigger );
	if( nullptr == pTrigger ){ return false; }

	if (paused && nullptr != pTrigger->getDevice())
	{
		pTrigger->getDevice()->setPause(paused);
	}
	int count = m_triggerTabs.GetItemCount();
	long tabIndex = m_triggerTabs.InsertItem(
		TCIF_TEXT|TCIF_PARAM, 
		count, 
		pTrigger->GetName(), 
		0, 
		reinterpret_cast<LPARAM>(pTrigger));
	if (tabIndex < cMaxTriggerCount)
	{
		m_triggerPauseState[tabIndex] = paused;
	}
	return tabIndex > -1;
}

void SoftwareTriggerDlg::Close()
{
	if (m_created)
	{
		m_created = false;
		DestroyWindow();
	}
}

SoftwareTriggerDlg& SoftwareTriggerDlg::Instance()
{
	static SoftwareTriggerDlg dlg(nullptr);
	if (false == m_created)
	{
		CWnd* pParent = ::AfxGetMainWnd();
		if (nullptr != pParent)
		{
			while (nullptr != pParent->GetParent())
			{
				pParent = pParent->GetParent();
			}
			dlg.Create(IDD_TRIGGER_PERIOD_DLG, pParent);
			m_created = true;
		}
	}
	return dlg;
}

void SoftwareTriggerDlg::SetTriggerPeriod(int val)
{
	int idx = m_triggerTabs.GetCurSel();
	TCITEM item;
	memset(&item, 0, sizeof(TCITEM));
	item.mask = TCIF_PARAM;
	if (m_triggerTabs.GetItem(idx, &item) && item.lParam)
	{
		SvTrig::SVTriggerObject* pTrigger = reinterpret_cast<SvTrig::SVTriggerObject*> (item.lParam);
		pTrigger->SetSoftwareTriggerPeriod(val, true);
	}
}

void SoftwareTriggerDlg::SetFrequency( int Value )
{
	double Frequency = 1000.0 / Value;
	std::string Text = SvUl::Format( _T("%.4fHz"), Frequency);
	m_frequency.SetWindowText( Text.c_str() );
	Frequency *= 60;
	Text = SvUl::Format( _T("%.2f/min"), Frequency);
	m_ppmLabel.SetWindowText( Text.c_str() );
}

HBRUSH SoftwareTriggerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor == CTLCOLOR_EDIT && m_warnColor)
	{
		pDC->SetBkColor(COLOR_WARN);
		return (HBRUSH)m_pBrush->GetSafeHandle();
	}

	return hbr;
}

void SoftwareTriggerDlg::OnBnClickedPausebutton()
{
	int idx = m_triggerTabs.GetCurSel();
	TCITEM item;
	memset(&item, 0, sizeof(TCITEM));
	item.mask = TCIF_PARAM;
	if (m_triggerTabs.GetItem(idx, &item) && item.lParam)
	{
		SvTrig::SVTriggerObject* pTrigger = reinterpret_cast<SvTrig::SVTriggerObject*> (item.lParam);
		if (nullptr != pTrigger->getDevice() && idx < cMaxTriggerCount)
		{
			m_triggerPauseState[idx] = !m_triggerPauseState[idx];
			pTrigger->getDevice()->setPause(m_triggerPauseState[idx]);
			m_pauseBtn.SetWindowText(m_triggerPauseState[idx] ? cContinue : cPause);
			m_singleTriggerBtn.EnableWindow(m_triggerPauseState[idx]);
		}
	}
}

void SoftwareTriggerDlg::OnSingleTrigger()
{
	int idx = m_triggerTabs.GetCurSel();
	TCITEM item;
	memset(&item, 0, sizeof(TCITEM));
	item.mask = TCIF_PARAM;
	if (m_triggerTabs.GetItem(idx, &item) && item.lParam)
	{
		SvTrig::SVTriggerObject* pTrigger = reinterpret_cast<SvTrig::SVTriggerObject*>(item.lParam);
		pTrigger->Fire();
	}
}

void SoftwareTriggerDlg::OnDestroy()
{
	delete m_pSpins;
	delete m_pBrush;
	m_pSpins = nullptr;
	m_pBrush = nullptr;
	CDialog::OnDestroy();
}
