//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProgressDialog
//* .File Name       : $Workfile:   SVProgressDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:37:48  $
//******************************************************************************

#if !defined(AFX_SVPROGRESSDIALOG_H__99F49D44_91F0_11D3_A83A_00106F000C7D__INCLUDED_)
#define AFX_SVPROGRESSDIALOG_H__99F49D44_91F0_11D3_A83A_00106F000C7D__INCLUDED_

#include "SVIProgress.h"

class SVProgressDialog : public CDialog
{
public:
	SVIProgress* m_pTask;

protected:
	int m_TotalSize;
	int m_AmountProcessed;
	CString m_Text;
	CString m_Title;

public:
	SVProgressDialog(LPCTSTR rTitle, CWnd* pParent = NULL); // standard constructor

// Dialog Data
	//{{AFX_DATA(SVProgressDialog)
	enum { IDD = IDD_PROGRESS_DIALOG };
	CStatic	m_staticTextCtrl;
	CProgressCtrl	m_progressCtrl;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVProgressDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVProgressDialog)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateProgress( WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndProgressDialog( WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVProgressDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:37:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   04 Sep 2012 15:50:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new progress dialog functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 14:19:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   02 Dec 1999 11:03:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  64
 * SCR Title:  Progress Dialog
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added handlers for OnOK and OnCancel.
 * Removed dead code.
 * Revised to eliminate warnings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:29:04   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Nov 1999 11:33:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support to set Caption.
 * Revised to handle recursive progress.
 * Added Static Text Control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Nov 1999 18:13:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Check in, generic Progress Bar Dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
