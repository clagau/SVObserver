// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVConnectionControlDlg
// * .File Name       : $Workfile:   SVConnectionControlDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:09:42  $
// ******************************************************************************

#if !defined(AFX_SVSELECTPROTOCOLDIALOG_H__08B9DFBA_BD38_11D3_A850_00106F000C7C__INCLUDED_)
#define AFX_SVSELECTPROTOCOLDIALOG_H__08B9DFBA_BD38_11D3_A850_00106F000C7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVSelectProtocolDialog.h : header file
//

#include "resource.h"

class SVRegistryClass;

/////////////////////////////////////////////////////////////////////////////
// SVConnectionControlDlg dialog

//##ModelId=3867C4CD0128
class SVConnectionControlDlg : public CDialog
{
// Construction
public:
	//##ModelId=386A2A3E03A9
	DWORD mSelection;
	//##ModelId=3867C4CD01B5
	SVConnectionControlDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVConnectionControlDlg)
	enum { IDD = IDD_SELECT_PROTOCOL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVConnectionControlDlg)
	protected:
	//##ModelId=3867C4CD01A6
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVConnectionControlDlg)
	//##ModelId=386A2A3E03AC
	virtual BOOL OnInitDialog();
	//##ModelId=386A2A3E03AB
	afx_msg void OnProtocolNamedPipes();
	//##ModelId=386A2A3E03AA
	afx_msg void OnProtocolTCPIP();
	//##ModelId=38B66FF7001F
	virtual void OnCancel();
	//##ModelId=38B66FF60399
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVSELECTPROTOCOLDIALOG_H__08B9DFBA_BD38_11D3_A850_00106F000C7C__INCLUDED_)

