//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MonEntryDlg.h
/// All Rights Reserved 
//*****************************************************************************
/// MonitorEntryDialog displays detail for one Monitorentry
//******************************************************************************

#pragma once
#include "resource.h"

/// MonEntryDlg dialog to display details for one monitorentry
class MonEntryDlg : public CDialog
{
	DECLARE_DYNAMIC(MonEntryDlg)

public:
	MonEntryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MonEntryDlg();

// Dialog Data
	enum { IDD = IDD_MONENTRY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedName();
	CString m_Name;
	DWORD m_InspectionStoreId;
	DWORD m_ItemId;
	DWORD m_Type;
	DWORD m_SToreOffset;
	DWORD m_Width;
	DWORD m_Height;
	DWORD m_Pitch;
	DWORD m_Size;
};
