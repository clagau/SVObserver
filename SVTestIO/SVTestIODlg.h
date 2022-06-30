//******************************************************************************
// * COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestIODlg
//* .File Name       : $Workfile:   SVTestIODlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Jul 2013 13:22:16  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "resource.h"
#include "Triggering/IODeviceBase.h"
#pragma endregion Includes


const int cInputChannelNr = 8;
const int cOutputChannelNr = 16;


class SVTestIODlg : public CDialog, public SvTrig::IODeviceBase
{
public:
	SVTestIODlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVTestIODlg() = default;

// Dialog Data
	enum { IDD = IDD_TEST_IO_DLG };

private:
	FILE* m_fpLog {nullptr};
	afx_msg void OnInputButtonClicked( UINT nID );


protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnSetOutput(WPARAM wParam, LPARAM lParam);
	
	void SetOutput( unsigned long p_iChannel, bool p_bState);
	unsigned __int64 m_i64Frequency {0ULL};
	unsigned __int64 m_i64Start {0ULL};

	void ToggleInput(unsigned int inputchannel);

	virtual BOOL OnInitDialog() override;

	CStatic m_input[cInputChannelNr];
	unsigned long m_lInputs{ 0UL };
	CStatic m_Output[cOutputChannelNr];
	CListBox m_LogList;

	afx_msg void OnBnClickedResetTs();
	afx_msg void OnBnClickedClearButton();
	afx_msg void  OnBnClickedLogging();
	CButton m_LogBtn;
	bool m_isLogButtonChecked {false};
	bool m_bResetStart{ true };
	void OnTriggerButtonClicked( UINT nID );

protected:
	virtual void OnCancel() override;

};

