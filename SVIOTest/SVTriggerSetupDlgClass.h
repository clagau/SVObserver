// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTriggerSetupDlgClass.h
// * .File Name       : $Workfile:   SVTriggerSetupDlgClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:49:58  $
// ******************************************************************************

#ifndef SVTRIGGERSETUPDLGCLASS_H
#define SVTRIGGERSETUPDLGCLASS_H

// SVTriggerSetupDlgClass.h : header file
//
class SVIOConfigurationInterfaceClass;
class SVIOTriggerLoadLibraryClass;

/////////////////////////////////////////////////////////////////////////////
// SVTriggerSetupDlgClass dialog

class SVTriggerSetupDlgClass : public CDialog
{
// Construction
public:
	SVTriggerSetupDlgClass(CWnd* pParent = NULL);   // standard constructor
	SVIOTriggerLoadLibraryClass *m_psvTriggers;
	long m_lStrobeInverts;
	long m_lTrigInverts;
	long m_lSystemType;

// Dialog Data
	//{{AFX_DATA(SVTriggerSetupDlgClass)
	enum { IDD = IDD_TRIG_SETUP_DLG };
	BOOL	m_bTrig1Rising;
	BOOL	m_bTrig2Rising;
	BOOL	m_bTrig3Rising;
	BOOL	m_bTrig4Rising;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVTriggerSetupDlgClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVTriggerSetupDlgClass)
	afx_msg void OnStrobe1InvBtn();
	afx_msg void OnStrobe2InvBtn();
	afx_msg void OnStrobe3InvBtn();
	afx_msg void OnStrobe4InvBtn();
	afx_msg void OnTrig1InvBtn();
	afx_msg void OnTrig2InvBtn();
	afx_msg void OnTrig3InvBtn();
	afx_msg void OnTrig4InvBtn();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVTRIGGERSETUPDLGCLASS_H__6CBFA59C_1364_4CE9_9EF2_9540F5D32737__INCLUDED_)

