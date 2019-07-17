//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SoftwareTriggerDlg
//* .File Name       : $Workfile:   SoftwareTriggerDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   09 Dec 2013 12:16:04  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <limits>
//Moved to precompiled header: #include <boost/function.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVMFCControls/SVKnobControl.h"
#include "TriggerInformation/SVTriggerObject.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

// SoftwareTriggerDlg dialog
namespace sv
{
	struct Def
	{
		const bool visible;
		const int limit;
		Def(bool v, int l): visible(v), limit(l) {}
	};
}

class SVTriggerProxy
{
public:
	SVTriggerProxy(SvTi::SVTriggerObject* t): m_pTrigger(t), m_paused(false), m_period(200)
	{
	}
	int GetSoftwareTriggerPeriod() { if( !m_paused){m_period = m_pTrigger->GetSoftwareTriggerPeriod();} return m_period; }
	std::string GetName() const { return std::string( m_pTrigger->GetName() ); }
	void SetSoftwareTriggerPeriod(long period, bool setTimer = false) { m_period = period; if (!m_paused) m_pTrigger->SetSoftwareTriggerPeriod(period, setTimer); }
	SvTi::SVTriggerObject* GetTrigger() { return m_pTrigger; }
	void Pause() { m_paused = true; m_period = GetSoftwareTriggerPeriod(); m_pTrigger->SetSoftwareTriggerPeriod(INT_MAX, true); }
	void Continue() { if (m_paused) { m_paused = false; m_pTrigger->SetSoftwareTriggerPeriod(m_period, true); } }
	std::string ButtonText() const { return m_paused ? std::string(_T("Continue")) : std::string(_T("Pause")); }
	bool Toggle() { if (m_paused) Continue(); else Pause(); return m_paused; }
	bool Paused() const { return m_paused;  }

private:
	SvTi::SVTriggerObject* m_pTrigger;
	long m_period;
	bool m_paused;
};

class SVSpinGroup
{
public:
	SVSpinGroup(CSpinButtonCtrl & spin, CEdit & edit, CStatic & label, sv::Def & def, SVSpinGroup * next = 0);
	~SVSpinGroup() { delete m_next; }
	void Show();
	bool Hide();
	bool SetValue(int val);
	bool Increment(int val); //, boost::function<int ()> total);
	SVSpinGroup * find(int spinId);
	int GetValue() const;
private:
	CSpinButtonCtrl & m_spin;
	CEdit & m_edit;
	CStatic & m_label;
	int m_limit;
	int m_value;
	bool m_visible;
	SVSpinGroup * m_next;
	static int GetLimit() { return (std::numeric_limits<int>::max)(); }
};

class SVSoftwareTriggerDlg : public CDialog
{
	DECLARE_DYNAMIC(SVSoftwareTriggerDlg)

	SVSoftwareTriggerDlg(CWnd* pParent = nullptr);   // standard constructor
public:
	virtual ~SVSoftwareTriggerDlg();

// Dialog Data
	enum { IDD = IDD_TRIGGER_PERIOD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	void SetTriggerPeriod(int val);
	bool EditOK();
	void SetFrequency( int Value );

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_triggerTabs;
	CEdit m_intervalEdit;

	SvMc::SVKnobControl m_knobCtrl;
	afx_msg void OnTcnSelchangeTriggerTabs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeUsecEdit();
	afx_msg void OnBnClickedOk();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnTriggerChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusUsecEdit();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedPausebutton();
	
	void ClearTriggers();
	bool AddTrigger(SvTi::SVTriggerObject* pTrigger);
	bool HasTriggers() const { return m_triggerTabs.GetItemCount() > 0; }
	int SelectTrigger();

	void OnStop();

	static SVSoftwareTriggerDlg & Instance();

private:
	CSpinButtonCtrl		m_msecSpin;
	CEdit				m_msecEdit;
	CStatic				m_msecLabel;
	CSpinButtonCtrl		m_secSpin;
	CEdit				m_secEdit;
	CStatic				m_secLabel;
	CSpinButtonCtrl		m_minSpin;
	CEdit				m_minEdit;
	CStatic				m_minLabel;
	CSpinButtonCtrl		m_hourSpin;
	CEdit				m_hourEdit;
	CStatic				m_hourLabel;
	CSpinButtonCtrl		m_daySpin;
	CEdit				m_dayEdit;
	CStatic				m_dayLabel;
	CStatic				m_frequency;
	CStatic				m_ppmLabel;
	CButton				m_pauseBtn;

	SVSpinGroup*		m_pSpins = nullptr;
	CBrush*				m_pBrush;

public:
	virtual BOOL OnInitDialog() override;
};

// SVSpinGroup inline implementation

inline	SVSpinGroup::SVSpinGroup(CSpinButtonCtrl & spin, CEdit & edit, CStatic & label, sv::Def & def, SVSpinGroup * next) :
		m_spin(spin),
		m_edit(edit),
		m_label(label),
		m_limit(def.limit),
		m_visible(def.visible),
		m_value(0),
		m_next(next)
{ 
	if (m_visible) 
		Show(); 
	m_spin.SetRange(0, static_cast<short>(m_limit -1));
	m_spin.SetPos(0);
	m_edit.SetWindowText(_T("00"));
}

inline void SVSpinGroup::Show()
{
	m_spin.ShowWindow(SW_SHOW);
	m_edit.ShowWindow(SW_SHOW);
	m_label.ShowWindow(SW_SHOW);
}

inline bool SVSpinGroup::Hide()
{
	if (!m_value &&  (!m_next || m_next->Hide()))
	{
		m_spin.ShowWindow(SW_HIDE);
		m_edit.ShowWindow(SW_HIDE);
		m_label.ShowWindow(SW_HIDE);
		return true;
	}
	return false;
}

inline bool SVSpinGroup::SetValue(int val)
{
	int carry = val/m_limit;
	m_value = val%m_limit;
	
	std::string Text = SvUl::Format(_T("%d"), m_value);
	m_spin.SetPos(m_value);
	m_edit.SetWindowText( Text.c_str() );
	if (m_value || carry)
	{
		Show();
	}
	if (m_next)
	{
		if (m_value || carry)
		{
			m_next->SetValue(carry);
			m_next->Show();
		}
		else
		{
			m_next->SetValue(0);
			m_next->Hide();
		}
	}
	return true;
}

inline SVSpinGroup * SVSpinGroup::find(int spinId)
{
	if (m_spin.GetDlgCtrlID() == spinId)
	{
		return this;
	}
	if (m_next)
	{
		return m_next->find(spinId);
	}
	return 0;
}

inline int SVSpinGroup::GetValue() const
{
	return m_limit * (m_next?m_next->GetValue():0) + m_value;
}

inline bool SVSpinGroup::Increment(int val)
{
	m_value += val;

	while (m_value < 0 && m_next != 0)
	{
		m_value += m_limit;
		m_next->Increment(-1);
	}
	if (m_value < 0)
	{
		m_value = 0;
	}
	while (m_value >= m_limit && m_next != 0)
	{
		m_value -= m_limit;
		m_next->Increment(1);
	}

	std::string Text = SvUl::Format(_T("%d"), m_value);
	m_edit.SetWindowText( Text.c_str() );
	return true;
}

