// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVUnloadDeviceDialog.h
// * .File Name       : $Workfile:   SVUnloadDeviceDialog.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:15:14  $
// ******************************************************************************

#pragma once

class SVUnloadDeviceDialog : public CDialog
{
// Construction
public:
	SVUnloadDeviceDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVUnloadDeviceDialog();

	void Show(CWnd* pParent);
	void Destroy();

// Dialog Data
	//{{AFX_DATA(SVUnloadDeviceDialog)
	enum { IDD = IDD_UNLOADDEVICE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVUnloadDeviceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVUnloadDeviceDialog)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
