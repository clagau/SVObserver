//******************************************************************************
// * COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestIODlg
//* .File Name       : $Workfile:   SVTestIODlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Jul 2013 13:22:16  $
//******************************************************************************

#pragma once

// SVTestIODlg dialog
#include "resource.h"
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "TriggerInformation/IODeviceBase.h"
#include "SVTestIOExports.h"


const int c_upperBoundForInputChannel = 9;
const int c_upperBoundForOutputChannel = 17;


class SVTestIODlg : public CDialog, public SvTi::IODeviceBase
{
	DECLARE_DYNAMIC(SVTestIODlg)

public:
	SVTestIODlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVTestIODlg();

// Dialog Data
	enum { IDD = IDD_TEST_IO_DLG };

private:
	FILE *m_fpLog;
	afx_msg void OnInputButtonClicked( UINT nID );


protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	void SetOutput( unsigned long p_iChannel, bool p_bState);
	CRITICAL_SECTION m_CriticalSection;
	unsigned __int64 m_i64Frequency;
	unsigned __int64 m_i64Start;

	void ToggleInput(unsigned int inputchannel);

	virtual BOOL OnInitDialog() override;

	CStatic m_input[c_upperBoundForInputChannel];
	long m_lInputs;
	CStatic m_Output[c_upperBoundForOutputChannel];
	CListBox m_LogList;

	afx_msg void OnBnClickedLogButton();
	afx_msg void OnBnClickedClearButton();
	CButton m_LogBtn;
	bool m_bResetStart;
	std::vector<SvTh::SVTriggerCallbackPtr> m_pCallBacks;

	void OnTriggerButtonClicked( UINT nID );

protected:
	virtual void OnCancel() override;
};

