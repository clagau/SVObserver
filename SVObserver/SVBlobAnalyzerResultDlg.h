//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzerResultDlg
//* .File Name       : $Workfile:   SVBlobAnalyzerResultDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:45:00  $
//******************************************************************************

#ifndef SVBLOBANALYZERRESULTDLG_H
#define SVBLOBANALYZERRESULTDLG_H

class SVBlobAnalyzerClass;

class SVBlobAnalyzerResultDlg : public CDialog
{
// Construction
public:
	SVBlobAnalyzerResultDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVBlobAnalyzerResultDlg)
	enum { IDD = IDD_BA_RESULT_DIALOG };
	CListBox	m_ResultList;
	//}}AFX_DATA

	SVBlobAnalyzerClass*      m_pBlobAnalyzer;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVBlobAnalyzerResultDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVBlobAnalyzerResultDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVBlobAnalyzerResultDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:45:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 Oct 2008 10:57:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  635
 * SCR Title:  Blob Analyzer Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved Fom SVBlobAnalyzer modules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
