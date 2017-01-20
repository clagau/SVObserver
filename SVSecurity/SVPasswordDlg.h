//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPasswordDlg.h
//* .File Name       : $Workfile:   SVPasswordDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:01:06  $
//******************************************************************************

#pragma once

class SVPasswordDlg : public CDialog
{
public:
	SVPasswordDlg(LPCTSTR Stsus, CWnd* pParent = nullptr);
	virtual ~SVPasswordDlg();
	enum { IDD = IDD_PASSWORD_DLG };

	LPCTSTR getPassword() { return m_Password; };
	LPCTSTR getUser() { return m_User; };
protected:

	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnInitDialog() override;
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	DECLARE_MESSAGE_MAP()

private:
	CString	m_Password;
	CString	m_User;
	CString	m_Status;

};

