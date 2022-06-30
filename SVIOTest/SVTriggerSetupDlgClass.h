// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	SVTriggerSetupDlgClass(CWnd* pParent = nullptr) : CDialog(IDD, pParent) {};   // standard constructor
	virtual ~SVTriggerSetupDlgClass() = default;

	SvTrig::SVIOTriggerLoadLibraryClass *m_psvTriggers{nullptr};
	long m_lStrobeInverts {0L};
	long m_lTrigInverts {0L};
	long m_lSystemType {0L};

// Dialog Data
	//{{AFX_DATA(SVTriggerSetupDlgClass)
	enum { IDD = IDD_TRIG_SETUP_DLG };
	BOOL	m_bTrigRising[cTriggerChannelNr] {false, false, false, false};
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
	afx_msg void OnStrobeInvBtn(UINT nID);
	afx_msg void OnTrigInvBtn(UINT nID);
	virtual void OnOK() override;
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

