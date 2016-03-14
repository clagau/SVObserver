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
#include "afxwin.h"
#include <map>
#include <vector>
#include "SVTestIOCallback.h"
#include "SVTestIOExports.h"

struct SVTestIOTriggerStruct
{
	SVTestIOCallbackPtr pCallback;
	void* pOwner;
	void* pData;
	bool bStarted;
};

typedef std::vector<SVTestIOTriggerStruct> TriggerCallbackList;
typedef std::map<unsigned long, TriggerCallbackList> TriggerList;

class SVTestIODlg : public CDialog
{
	DECLARE_DYNAMIC(SVTestIODlg)

public:
	SVTestIODlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVTestIODlg();

// Dialog Data
	enum { IDD = IDD_TEST_IO_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	TriggerList m_triggerList;

	DECLARE_MESSAGE_MAP()
public:

	HRESULT AddTriggerCallback(unsigned long handle, SVTestIOCallbackPtr pCallback, void* pOwner, void* pData);
	HRESULT RemoveTriggerCallback(unsigned long handle, SVTestIOCallbackPtr pCallback);
	HRESULT RemoveAllTriggerCallbacks(unsigned long handle);
	HRESULT StartTrigger(unsigned long handle);
	HRESULT StopTrigger(unsigned long handle);
	void SetOutput( unsigned long p_iChannel, bool p_bState);
	CRITICAL_SECTION m_CriticalSection;
	unsigned __int64 m_i64Frequency;
	unsigned __int64 m_i64Start;
	afx_msg void OnStnClickedInput1();
	afx_msg void OnStnClickedInput2();
	afx_msg void OnStnClickedInput3();
	afx_msg void OnStnClickedInput4();
	afx_msg void OnStnClickedInput5();
	afx_msg void OnStnClickedInput6();
	afx_msg void OnStnClickedInput7();
	afx_msg void OnStnClickedInput8();
	virtual BOOL OnInitDialog();
	CStatic m_Input1;
	CStatic m_Input2;
	CStatic m_Input3;
	CStatic m_Input4;
	CStatic m_Input5;
	CStatic m_Input6;
	CStatic m_Input7;
	CStatic m_Input8;
	long m_lInputs;
	CStatic m_Output1;
	CStatic m_Output2;
	CStatic m_Output3;
	CStatic m_Output4;
	CStatic m_Output5;
	CStatic m_Output6;
	CStatic m_Output7;
	CStatic m_Output8;
	CStatic m_Output9;
	CStatic m_Output10;
	CStatic m_Output11;
	CStatic m_Output12;
	CStatic m_Output13;
	CStatic m_Output14;
	CStatic m_Output15;
	CStatic m_Output16;
	CListBox m_LogList;

	afx_msg void OnBnClickedLogButton();
	afx_msg void OnBnClickedClearButton();
	CButton m_LogBtn;
	bool m_bResetStart;
	afx_msg void OnBnClickedTriggerButton();
	std::vector<SVTestIOCallbackPtr> m_pCallBacks;
	afx_msg void OnBnClickedTrigger2();
	afx_msg void OnBnClickedTrigger3();
	afx_msg void OnBnClickedTrigger4();
protected:
	virtual void OnCancel();
};

