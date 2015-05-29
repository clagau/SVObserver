//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolDlg
//* .File Name       : $Workfile:   SVExternalToolDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:04  $
//******************************************************************************

#ifndef SVEXTERNALTOOLDLG_H
#define SVEXTERNALTOOLDLG_H

#include "SVTaskObject.h"
#include "ISVCancel.h"
#include "SVOGui\ISVPropertyPageDialog.h"
#include "SVMFCControls/SVActiveDisableButton.h"

class SVToolAdjustmentDialogSheetClass;
class SVExternalTool;
class SVExternalToolTask;

class SVExternalToolDlg : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
public:
	SVExternalToolDlg(SVToolAdjustmentDialogSheetClass* pSheet);   // standard constructor
	~SVExternalToolDlg();

	// ISVPropertyPageDialog
	bool QueryAllowExit();

	SVIPDoc* GetIPDoc() const;

// Dialog Data
	//{{AFX_DATA(SVExternalToolDlg)
	enum { IDD = IDD_TA_EXTERNAL_TOOL_DLG };
	CEdit	m_StatusEdit;
	CButton	m_btnDeleteAll;
	CButton	m_btnDelete;
	CButton	m_btnBrowse;
	CButton	m_btnAdd;
	CButton	m_btnTest;
	SvMc::SVActiveDisableButton m_btnDetails;
	CListBox m_lbDependentList;
	CString	m_strDLLPath;
	CString	m_strStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVExternalToolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HRESULT CleanUpOldToolInfo();
	HRESULT RestoreOriginalData();
	bool ShowDependentsDlg();
	bool m_bToolTipEnabled;
	CString m_sDetailBtnToolTipText;
	bool EnableDetailTip(  bool bEnable, const CString& strTip = CString(_T("")));
	void InitializeDll();
	void SetDependencies();
	void NotifyProgress(const CString& rstrMessage);


	SVToolAdjustmentDialogSheetClass* m_pSheet;
	SVExternalTool* m_pTool;
	SVExternalToolTask* m_pTask;
	CString m_strLastDllPath;
	CToolTipCtrl m_ToolTip;
	SVCancelData* m_pCancelData;

	// Generated message map functions
	//{{AFX_MSG(SVExternalToolDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDetails();
	afx_msg void OnDeleteAll();
	afx_msg void OnDelete();
	afx_msg void OnAdd();
	afx_msg void OnBrowse();
	afx_msg void OnTest();
	//}}AFX_MSG
    afx_msg LRESULT OnUpdateStatus(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVExternalToolDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:32:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jul 2012 16:52:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 10:18:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Oct 2004 12:52:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  448
 * SCR Title:  Fix External Tool Change DLL functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified to allow correct functioning when choosing a different DLL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 12:22:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
