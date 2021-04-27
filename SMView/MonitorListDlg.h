//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MonitorLIstDlg.h 
/// All Rights Reserved 
//*****************************************************************************

/// MonitorLIstDlg dialog  to Display Entries of one Monitorlist 
//******************************************************************************
#pragma once
#include "resource.h"


//! MonitorLIstDlg dialog  to Display Entries of one Monitorlist 
class MonitorListDlg : public CDialog
{
public:
	enum ECOL {List =0, Name, Type, ECOL_Size };
	static LPCTSTR ColHeader[ECOL_Size+1];
	static DWORD ColHeaderWidth[ECOL_Size+1];

	explicit MonitorListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MonitorListDlg();

	afx_msg void OnBnClickedButtonProperties();

// Dialog Data
	enum { IDD = IDD_MONITORLIST };

		
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void FillControl();
	DECLARE_MESSAGE_MAP()

protected:
	CListCtrl m_ListCtrl;
	
};

