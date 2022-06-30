// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOTESTDlg.h
// * .File Name       : $Workfile:   SVIOTESTDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   01 Oct 2013 09:21:30  $
// ******************************************************************************
#pragma once

#pragma region Includes
#include "resource.h"
#include "Triggering/SVTriggerInfoStruct.h"
#pragma endregion Includes

class SVIOConfigurationInterfaceClass;
namespace SvTrig
{
	class SVIOTriggerLoadLibraryClass;
}

enum IOBoardType
{
	SVRABBIT_X1 = 10,
	SVRABBIT_X2 = 20,
	SVRABBIT_X3 = 12,
};

constexpr int cFanIdNr = 4;
constexpr int cInputChannelNr = 8;

struct SVIOTriggerDataStruct
{
	unsigned long ulLastIndex {0UL};
	unsigned long ulIndex {0UL};
	unsigned long ulNextIndex {1UL};

	long lTriggerCount {0L};

	std::array<double, 100> m_TimeStamp {0.0};

	double m_LastTime {0.0};
	double m_TotalTime {0.0};
	double m_MinTime {0.0};
	double m_MaxTime {0.0};

	void OnTriggerStart();
};

class SVIOTestDlg : public CDialog
{
private:
	HANDLE m_hWorkerThread {nullptr};
public:
	bool m_bFast {false};
	bool m_bTestRand {false};
	bool m_bThreadRunning {false};

	SVIOTestDlg(CWnd* pParent = nullptr);	// standard constructor
	virtual ~SVIOTestDlg();

	void __stdcall triggerCallback(SvTrig::TriggerData&& triggerData);

	SVIOTriggerDataStruct* getTriggerData(unsigned long triggerChannel);

	// Dialog Data
		//{{AFX_DATA(SVIOTestDlg)
	enum { IDD = IDD_SVIOTEST_DIALOG };
	CComboBox	m_BoardModelCombo;
	CStatic	m_BoardTxt;

	CStatic	m_FanTxt[cFanIdNr];
	CStatic	m_Fan[cFanIdNr];
	CStatic	m_TriggerMinimum[cTriggerChannelNr];
	CStatic	m_TriggerMaximum[cTriggerChannelNr];
	CStatic	m_TriggerDistance[cTriggerChannelNr];
	CStatic	m_TriggerCountWnd[cTriggerChannelNr];
	CStatic	m_TriggerAverage[cTriggerChannelNr];

	CButton	m_cbtSlow;
	CStatic	m_input[cInputChannelNr];
	long	m_lStaticChannel {0L};
	int		m_BoardModel {-1};
	//}}AFX_DATA

	long m_lTrigInverts {0L};
	long m_lStrobeInverts {0L};
	CString m_csDigital;
	CString m_csTrigger;

	bool m_bInterruptEnabled {false};
	int nSpeed {0};
	int nSeq {0};

	SvTrig::SVIOTriggerLoadLibraryClass *m_psvTriggers {nullptr};

	long m_lSystemType {0L};

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVIOTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	HICON m_hIcon{nullptr};

	// Generated message map functions
	//{{AFX_MSG(SVIOTestDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg void OnClose();
	afx_msg void OnSequence();
	afx_msg void OnSlow();
	afx_msg void OnFast();
	afx_msg void OnDestroy();
	afx_msg void OnTestOutputs();
	afx_msg void OnStartTriggers();
	afx_msg void OnStopTriggers();
	afx_msg void OnBnClickedAcceptTriggers();
	afx_msg void OnStartTest();
	afx_msg void OnRandBtn();
	afx_msg void OnFastSlow();
	afx_msg void OnChangeStaticChannel();
	afx_msg void OnTriggerParam();
	afx_msg void OnSelchangeBoardModelCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void ShowFans( bool p_bShow );
	bool IsSoftwareTrigger() const;
	bool AllowTriggerParamEditing() const;

	void updateValues(int triggerchannel);
	void StartTrigger(int triggerchannel);


private:
	long m_TriggerCount[cTriggerChannelNr] {0L, 0L, 0L, 0L};

	SVIOTriggerDataStruct m_TriggerData[cTriggerChannelNr];

	long m_FanFreq[cFanIdNr] {0L, 0L, 0L, 0L};

	bool m_bCurrentlyAcceptingTriggers = false;
};


