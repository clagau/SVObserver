// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionExitDlg
// * .File Name       : $Workfile:   SVRegressionExitDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 14:41:08  $
// ******************************************************************************

#if !defined(AFX_SVREGRESSIONEXITDLG_H__C1DC1B01_722F_4792_9506_7321D69DE4ED__INCLUDED_)
#define AFX_SVREGRESSIONEXITDLG_H__C1DC1B01_722F_4792_9506_7321D69DE4ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVRegressionExitDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SVRegressionExitDlg dialog

class SVRegressionExitDlg : public CDialog
{
// Construction
public:
	SVRegressionExitDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVRegressionExitDlg)
	enum { IDD = IDD_DLG_EXIT_REGRESSION };
	CComboBox	m_ctlExitModes;
	CString	m_csExitModes;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVRegressionExitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVRegressionExitDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int m_iExitMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVREGRESSIONEXITDLG_H__C1DC1B01_722F_4792_9506_7321D69DE4ED__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRegressionExitDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:41:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Aug 2005 11:14:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/