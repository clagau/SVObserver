//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCameraDlg
//* .File Name       : $Workfile:   SVOCameraDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Dec 2013 11:03:16  $
//******************************************************************************

#if !defined(AFX_SVOCAMERADLG_H__1704235F_5930_4698_BEA2_662312DF9EA0__INCLUDED_)
#define AFX_SVOCAMERADLG_H__1704235F_5930_4698_BEA2_662312DF9EA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSVOCameraDlg dialog

class CSVOConfigAssistantDlg;

class CSVOCameraDlg : public CPropertyPage
{
// Construction
public:
	CSVOCameraDlg(CWnd* pParent = NULL);   // standard constructor

    void SetupList();
	void DisableAdvanceBtn();
	void ShowCameraDlgBtn( int iShow );
// Dialog Data
	//{{AFX_DATA(CSVOCameraDlg)
	enum { IDD = IDD_DLG_PROPPAGE_AA_CAMERA };
	CButton	m_btnCameraManager;
	CListBox	m_ctlCameraList;
	CButton	m_btnVCProp;
	CButton	m_btnNew;
	CButton	m_btnDelete;
	CButton	m_btnAdvancedProp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVOCameraDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSVOCameraDlg)
	afx_msg void OnSelchangeLstCamera();
	afx_msg void OnBtnAdvanced();
	afx_msg void OnBtnDeleteVc();
	afx_msg void OnBtnNewVc();
	afx_msg void OnBtnPropVc();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkLstCamera();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnCameraManager();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    CSVOConfigAssistantDlg *m_pParent;
    BOOL m_bModified;
    BOOL m_bNewConfig;
    
    void SetCameraPropForAll(CString sCurrentCamera);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOCAMERADLG_H__1704235F_5930_4698_BEA2_662312DF9EA0__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOCameraDlg.h_v  $
 * 
 *    Rev 1.1   02 Dec 2013 11:03:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  870
 * SCR Title:  Fix Various Issues in the Edit Configuration Dialog
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic to show or hide the camera manager button based on system type and configuration type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:42:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Dec 2003 09:37:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  396
 * SCR Title:  Add New Product Types for 1394 SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added method for disabling the advanced button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   31 Oct 2003 08:29:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for SV1394CameraManagerDlg
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 12:29:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:35:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/