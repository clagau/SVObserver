// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "TriggerInformation/SVTriggerInfoStruct.h"
#pragma endregion Includes

class SVIOConfigurationInterfaceClass;
class SVIOTriggerLoadLibraryClass;

enum IOBoardType
{
	SVRABBIT_X1 = 10,
	SVRABBIT_X2 = 20,
	SVRABBIT_X3 = 12,
};

/////////////////////////////////////////////////////////////////////////////
// CSVIOTESTDlg dialog

struct SVIOTriggerDataStruct
{
	unsigned long ulLastIndex;
	unsigned long ulIndex;
	unsigned long ulNextIndex;

	long lTriggerCount;

	double m_TimeStamp[100];

	double m_LastTime;
	double m_TotalTime;
	double m_MinTime;
	double m_MaxTime;

	void OnTriggerStart();

};

const int c_upperBoundForTriggerChannel = 4;
const int c_upperBoundForFanId = 4;
const int c_upperBoundForInputChannel = 9;

class CSVIOTESTDlg : public CDialog
{
// Construction
private:
	//@TODO[Arvid]: I have removed some code duplication in this project.
	//@TODO[Arvid]: currently the 0-th Array elements are not used and the array sizes are 5 (for just four trigger channels)
	//@TODO[Arvid]: I have left it like this so the numbers used remain the same.
	//@TODO[Arvid]: Whoever feels like correcting this is very welcome to tidy up the code further

	HANDLE m_hWorkerThread;
public:
	bool m_bFast;
	bool m_bTestRand;
	bool m_bThreadRunning;

	CSVIOTESTDlg(CWnd* pParent = nullptr);	// standard constructor
	virtual ~CSVIOTESTDlg();

	void __stdcall triggerCallback(const SvTi::IntVariantMap& rTriggerData);

	SVIOTriggerDataStruct* getTriggerData(unsigned long triggerChannel);

// Dialog Data
	//{{AFX_DATA(CSVIOTESTDlg)
	enum { IDD = IDD_SVIOTEST_DIALOG };
	CComboBox	m_BoardModelCombo;
	CStatic	m_BoardTxt;

	CStatic	m_FanTxt[c_upperBoundForFanId];
	CStatic	m_Fan[c_upperBoundForFanId];
	CStatic	m_TriggerMinimum[c_upperBoundForTriggerChannel];
	CStatic	m_TriggerMaximum[c_upperBoundForTriggerChannel];
	CStatic	m_TriggerDistance[c_upperBoundForTriggerChannel];
	CStatic	m_TriggerCountWnd[c_upperBoundForTriggerChannel];
	CStatic	m_TriggerAverage[c_upperBoundForTriggerChannel];

	CButton	m_cbtSlow;
	CStatic	m_input[c_upperBoundForInputChannel];
	long	m_lStaticChannel;
	int		m_BoardModel;
	//}}AFX_DATA

	long m_lTrigInverts;
	long m_lStrobeInverts;
	CString m_csDigital;
	CString m_csTrigger;

	bool m_bInterruptEnabled;
	int nSpeed;
	int nSeq;

	SVIOTriggerLoadLibraryClass *m_psvTriggers;

	long m_lSystemType;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVIOTESTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSVIOTESTDlg)
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
	long m_TriggerCount[c_upperBoundForTriggerChannel];

	SVIOTriggerDataStruct m_TriggerData[c_upperBoundForTriggerChannel];

	long m_FanFreq[c_upperBoundForFanId];

	bool m_bCurrentlyAcceptingTriggers = false;
};


