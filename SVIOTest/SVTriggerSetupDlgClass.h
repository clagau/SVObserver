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

#pragma once

#pragma region Includes
#include "resource.h"
#pragma endregion Includes


namespace SvTrig
{
	class SVIOTriggerLoadLibraryClass;
}


class SVTriggerSetupDlgClass : public CDialog
{
// Construction
public:
	SVTriggerSetupDlgClass(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVTriggerSetupDlgClass();

	SvTrig::SVIOTriggerLoadLibraryClass *m_psvTriggers;
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
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
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
	virtual void OnOK() override;
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

