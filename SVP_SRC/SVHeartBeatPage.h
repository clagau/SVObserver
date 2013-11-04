// ******************************************************************************
// * COPYRIGHT (c) 2008 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVHeartBeatPage
// * .File Name       : $Workfile:   SVHeartBeatPage.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   :     $Date:   25 Apr 2013 15:48:30  $
// ******************************************************************************

#ifndef SVHEARTBEATPAGE_H_INCLUDED
#define SVHEARTBEATPAGE_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif 

#include "SetupDialogSheet.h"

/////////////////////////////////////////////////////////////////////////////
// SVHeartBeatPage dialog

class SVHeartBeatPage : public CPropertyPage
{
	DECLARE_DYNCREATE(SVHeartBeatPage)

// Construction
public:
	SVHeartBeatPage();
	~SVHeartBeatPage();

// Dialog Data
	//{{AFX_DATA(SVHeartBeatPage)
	enum { IDD = IDD_HEART_BEAT };
	CEdit	m_HeartBeatAddress;
	CEdit	m_HeartBeatTimeOut;
//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVHeartBeatPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
   SetupDialogSheetClass*  svmpParentSheet;

protected:
	// Generated message map functions
	//{{AFX_MSG(SVHeartBeatPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // #ifndef SVHEARTBEATPAGE_H_INCLUDED

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVHeartBeatPage.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:48:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:57:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/