// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CModelessMsgBox
// * .File Name       : $Workfile:   ModelessMessageBox.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 09:12:36  $
// ******************************************************************************

#pragma once

//@TODO [Arvid][7.50][10.05.2017] At some point in time, CModelessMsgBox should be removed altogether and a different modeless message box be used instead

class CModelessMsgBox : public CDialog
{
	// Construction
public:
	CModelessMsgBox(const std::string& rMsg, HANDLE p_hEvent);   // standard constructor
	virtual ~CModelessMsgBox();


protected:
	std::string m_Msg;
	HANDLE m_hEvent;

public:
	// Dialog Data
	//{{AFX_DATA(CModelessMsgBox)
	enum { IDD = IDD_MODELESS_MESSAGE_BOX };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
protected:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelessMsgBox)
public:
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void PostNcDestroy() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelessMsgBox)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	virtual void OnCancel() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
