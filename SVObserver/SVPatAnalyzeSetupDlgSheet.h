//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatAnalyzeSetupDlgSheet
//* .File Name       : $Workfile:   SVPatAnalyzeSetupDlgSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:19:02  $
//******************************************************************************

#if !defined(AFX_SVPATAANALYZESETUPDLGSHEETCLASS_H__38DF47B5_F364_11D3_A896_00106F021742__INCLUDED_)
#define AFX_SVPATAANALYZESETUPDLGSHEETCLASS_H__38DF47B5_F364_11D3_A896_00106F021742__INCLUDED_
class SVPatternAnalyzerClass;

class SVPatAnalyzeSetupDlgSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(SVPatAnalyzeSetupDlgSheet)

public:
	SVPatAnalyzeSetupDlgSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVPatAnalyzeSetupDlgSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~SVPatAnalyzeSetupDlgSheet();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPatAnalyzeSetupDlgSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(SVPatAnalyzeSetupDlgSheet)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVPATAANALYZESETUPDLGSHEETCLASS_H__38DF47B5_F364_11D3_A896_00106F021742__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPatAnalyzeSetupDlgSheet.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:19:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   31 Dec 2008 11:17:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised for Pattern Analyzer fixes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:37:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/