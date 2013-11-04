// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOTESTDlg.h
// * .File Name       : $Workfile:   SVIOTESTDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:01:16  $
// ******************************************************************************

#ifndef SVIOTESTDLG_H
#define SVIOTESTDLG_H

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
	afx_msg void OnTimer(UINT nIDEvent);
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

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVIOTESTDLG_H__8F1A2A93_E92E_410A_BF9A_A6E98B13BFD7__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIOTest\SVIOTESTDlg.h_v  $
 * 
 *    Rev 1.1   08 May 2013 15:01:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   29 Apr 2013 12:58:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 12:50:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   15 Aug 2012 14:27:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singlton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   28 Apr 2011 10:04:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added board type 13 for Q57A Analog SVIM
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 May 2009 14:38:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  663
 * SCR Title:  Add support to SVTVicLpt dll to lock and unlock outputs.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added test code for the X2 IO Board.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   11 Sep 2008 08:25:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised for Software Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Jan 2007 16:13:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  590
 * SCR Title:  Modify SVObserver to send the IO Board type to the parallel board
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added code to fill board type combo from the hardware.ini.
 * Added logic to show inputs 1 & 2 disabled for X3 IO Board.
 * Added logic to hide fans 3 and 4 for X3 Board.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jun 2006 09:31:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  564
 * SCR Title:  Add configurability to SVTVicLpt DLL to set the system type
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New functionality in Test App to test new functionality in SVParallelIO board.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
