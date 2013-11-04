//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerDeviceDlg
//* .File Name       : $Workfile:   SVOTriggerDeviceDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:12:50  $
//******************************************************************************
#ifndef SVOTRIGGERDEVICEDLG_H
#define SVOTRIGGERDEVICEDLG_H


#include "SVTriggerSelectionDlg.h"

class CSVOConfigAssistantDlg;

class CSVOTriggerDeviceDlg : public CPropertyPage
{
// Construction
public:
	CSVOTriggerDeviceDlg(CWnd* pParent = NULL);   // standard constructor
    void SetupList();

// Dialog Data
	//{{AFX_DATA(CSVOTriggerDeviceDlg)
	enum { IDD = IDD_DLG_PROPPAGE_AA_TRIGGER };
	CButton	m_btnPropTrigger;
	CButton	m_btnNewTrigger;
	CButton	m_btnDeleteTrigger;
	CButton	m_btnAdvanced;
	CListBox	m_ctlTriggerList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVOTriggerDeviceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSVOTriggerDeviceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnPropTrig();
	afx_msg void OnBtnNewTrig();
	afx_msg void OnBtnDeleteTrig();
	afx_msg void OnBtnAdvanced();
	afx_msg void OnDblclkLstTriggers();
	afx_msg void OnSelchangeLstTriggers();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
    CSVOConfigAssistantDlg *m_pParent;
	void EnablePropertyEdit(int iSelection);
	void EnablePropertyButton(bool bEnable);
	void EnableAdvancedPropertyButton(bool bEnable);
	void DisablePropertyEdit();
	void GetNextAvailableTriggerList(SVTriggerNameIdList& rList) const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOTRIGGERDEVICEDLG_H__2EA120CE_BE74_45B5_9774_E88E9903DCA9__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOTriggerDeviceDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:12:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jan 2013 11:31:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVTriggerSelectionDlg
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Jan 2013 16:39:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated and added methods to handle the new look-up  and add  functionality.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Sep 2008 17:07:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support settings for Software Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:27:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
