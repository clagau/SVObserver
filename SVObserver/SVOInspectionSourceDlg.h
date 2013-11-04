//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOInspectionSourceDlg
//* .File Name       : $Workfile:   SVOInspectionSourceDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   26 Jul 2013 10:16:06  $
//******************************************************************************
#if !defined(AFX_SVOINSPECTIONSOURCEDLG_H__5C162AEB_4F59_4485_BB0F_615148FF99B1__INCLUDED_)
#define AFX_SVOINSPECTIONSOURCEDLG_H__5C162AEB_4F59_4485_BB0F_615148FF99B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVOInspectionSourceDlg.h : header file
//
#include "SVOMFCLibrary/SVDataItemManagerTemplate.h"

#define INTERNAL_INSPECTION_NAME "Inspection_"

/////////////////////////////////////////////////////////////////////////////
// CSVOInspectionSourceDlg dialog

class CSVOConfigAssistantDlg;

class CSVOInspectionSourceDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSVOInspectionSourceDlg)

// Construction
public:
	CSVOInspectionSourceDlg();
	~CSVOInspectionSourceDlg();
    
    void SetupList();
// Dialog Data  
	//{{AFX_DATA(CSVOInspectionSourceDlg)
	enum { IDD = IDD_DLG_PROPPAGE_AA_INSPECTION };
	CListBox	m_ctlIPDlist;
	CEdit	m_ctlInspectionEdt;
	CButton	m_ctlPropVIbtn;
	CButton	m_ctlNewVIbtn;
	CButton	m_ctlDeleteVIbtn;
	CButton	m_ctlAttachVIbtn;
	CButton	m_ctlAddIPDbtn;
	CString	m_sDeviceName;
	CButton m_ctlImportIPDbtn;
	CButton m_ctlExportIPDbtn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSVOInspectionSourceDlg)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSVOInspectionSourceDlg)
	afx_msg void OnBtnAddIpd();
	afx_msg void OnBtnDeleteVi();
	afx_msg void OnBtnPropVi();
	afx_msg void OnBtnImportIpd();
	afx_msg void OnBtnExportIpd();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLstIpdList();
	afx_msg void OnDblclkLstIpdList();
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	void ClearList();
    CString GetNextInspectionDisplayName(CString sLabeName);

    void LoadInspectionFileNameList(); 
    void LoadVInspectionList();
	void EnableDisableExport();

    int m_iVIHorzScroll;
    int m_iIPDHorzScroll;
    CSVOConfigAssistantDlg *m_pParent;
    int m_iInspectionNextCount;
    int m_iCursel;
    int m_iEditingSel;

	typedef SVDataItemManagerTemplate< CString > SVDataItemManager;

	SVDataItemManager m_Items;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOINSPECTIONSOURCEDLG_H__5C162AEB_4F59_4485_BB0F_615148FF99B1__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOInspectionSourceDlg.h_v  $
 * 
 *    Rev 1.2   26 Jul 2013 10:16:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   made changes to enable/disable the export button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:30:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Apr 2013 14:19:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVDataItemManagerTemplate to manage Item Data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:05:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Mar 2013 16:42:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Inspection Import and Export buttons and associated logic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Jan 2013 16:21:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Added new clear list method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:06:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:38:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
