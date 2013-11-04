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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOTest\SVTriggerSetupDlgClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:49:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Aug 2012 14:27:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singlton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Jan 2007 16:16:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  590
 * SCR Title:  Modify SVObserver to send the IO Board type to the parallel board
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic to disable trigger and strobe control setups for the X3 IO Board triggers 2, 3, and 4.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jun 2006 09:35:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  564
 * SCR Title:  Add configurability to SVTVicLpt DLL to set the system type
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Dialog to support Trigger Parameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
