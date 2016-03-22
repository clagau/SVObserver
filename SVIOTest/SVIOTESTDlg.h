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

#include "SVTimerLibrary/SVClock.h"

class SVIOConfigurationInterfaceClass;
class SVIOTriggerLoadLibraryClass;

enum IOBoardType
{
	SVRABBIT_X1 = 10,
	SVRABBIT_X2 = 20,
	SVRABBIT_X3 = 12,
	SVRABBIT_SVIM_ANALOG = 13,
};


/////////////////////////////////////////////////////////////////////////////
// CSVIOTESTDlg dialog

struct SVIOTriggerDataStruct
{
	unsigned long ulLastIndex;
	unsigned long ulIndex;
	unsigned long ulNextIndex;

	long lTriggerCount;

	SVClock::SVTimeStamp m_TimeStamp[100];

	SVClock::SVTimeStamp m_LastTime;
	SVClock::SVTimeStamp m_TotalTime;
	SVClock::SVTimeStamp m_MinTime;
	SVClock::SVTimeStamp m_MaxTime;
};

class CSVIOTESTDlg : public CDialog
{
// Construction
public:
	bool m_bFast;
	bool m_bTestRand;
	HANDLE m_hWorkerThread;
	bool m_bThreadRunning;
	CSVIOTESTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSVIOTESTDlg)
	enum { IDD = IDD_SVIOTEST_DIALOG };
	CComboBox	m_BoardModelCombo;
	CStatic	m_BoardTxt;
	CStatic	m_Fan4Txt;
	CStatic	m_Fan3Txt;
	CStatic	m_Fan2Txt;
	CStatic	m_Fan1Txt;
	CStatic	m_Fan4;
	CStatic	m_Fan3;
	CStatic	m_Fan2;
	CStatic	m_Fan1;
	CStatic	m_Trigger4Minimum;
	CStatic	m_Trigger3Minimum;
	CStatic	m_Trigger2Minimum;
	CStatic	m_Trigger1Minimum;
	CStatic	m_Trigger4Maximum;
	CStatic	m_Trigger3Maximum;
	CStatic	m_Trigger2Maximum;
	CStatic	m_Trigger1Maximum;
	CStatic	m_Trigger3Distance;
	CStatic	m_Trigger4Distance;
	CStatic	m_Trigger2Distance;
	CStatic	m_Trigger4Count;
	CStatic	m_Trigger3Count;
	CStatic	m_Trigger2Count;
	CStatic	m_Trigger4Average;
	CStatic	m_Trigger3Average;
	CStatic	m_Trigger2Average;
	CStatic	m_Trigger1Distance;
	CStatic	m_Trigger1Count;
	CStatic	m_Trigger1Average;
	CButton	m_cbtSlow;
	CStatic	m_input8;
	CStatic	m_input7;
	CStatic	m_input6;
	CStatic	m_input5;
	CStatic	m_input4;
	CStatic	m_input3;
	CStatic	m_input2;
	CStatic	m_input1;
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
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSVIOTESTDlg)
	virtual BOOL OnInitDialog();
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

private:

	long m_lTrigger1Count;
	long m_lTrigger2Count;
	long m_lTrigger3Count;
	long m_lTrigger4Count;

	SVIOTriggerDataStruct m_svTrigger1Data;
	SVIOTriggerDataStruct m_svTrigger2Data;
	SVIOTriggerDataStruct m_svTrigger3Data;
	SVIOTriggerDataStruct m_svTrigger4Data;
public:
	long m_lFanFreq1;
	long m_lFanFreq2;
	long m_lFanFreq3;
	long m_lFanFreq4;
};


