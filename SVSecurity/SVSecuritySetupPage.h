//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecuritySetupPage
//* .File Name       : $Workfile:   SVSecuritySetupPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:03:54  $
//******************************************************************************

#ifndef SECURITYSETUPPAGE_H
#define SECURITYSETUPPAGE_H

class SVAccessClass;
class SVSecuritySetupSheet;

class SVSecuritySetupPage : public CPropertyPage
{
	DECLARE_DYNCREATE(SVSecuritySetupPage)

// Construction
public:
	
	SVSecuritySetupPage();
	
	SVSecuritySetupPage(UINT nIDTemplate, UINT nIDCaption=0);
	
	~SVSecuritySetupPage();
	void BuildTree( long& p_NodeIndex, HTREEITEM hParent, bool p_Root );

// Dialog Data
	//{{AFX_DATA(SVSecuritySetupPage)
	enum { IDD = IDD_SECURITY_PAGE };
	CTreeCtrl	m_AccessPointTree;
	CListBox	m_lbNetGroups;
	BOOL	m_bForcePrompt;
	BOOL	m_bLogOnMode;
	long	m_lTimeout;
	CString	m_strGroup;
	//}}AFX_DATA
	
	CString	m_strApp;
	
	int		m_nParent;
	
	int		m_nChild;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVSecuritySetupPage)
	public:
	virtual BOOL OnApply();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVSecuritySetupPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeNetGroups();
	afx_msg void OnSelchangedAccessTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusGroupEdit();
	afx_msg void OnForcePrompt();
	afx_msg void OnClearGrp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SetData();

	int GetNodeIndexFromTree();

	CString GetGroups(void);
	
	void  SetGroups(CString groups);
	
	void  ResetGroups(void);


protected:	
	
	SVSecuritySetupSheet*   pPropSheet;
	SVAccessClass*          m_pAccess;
	
//	SVDlgTemplate		DlgTemplate;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECURITYSETUPPAGE_H__FB876D1A_4765_11D4_A8FD_00106F0309AB__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSecurity\SVSecuritySetupPage.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:03:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Sep 2012 18:43:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:35:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Oct 2005 08:40:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnHelp and OnHelpInfo message handlers that take the help message Id, add an offset and then call into the SVObserver.hlp file.  The default operation would have tried to call into SVSecurity.hlp with normal IDs.  The offset was put in so there would be no conflict with SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Oct 2005 08:31:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnHelp and OnHelpInfo Message handlers for adding offsets to the resource IDs and calls the SVObserver Help file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Jul 2005 15:11:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   GUI - Removed Auto Offline to Edit 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 Jul 2005 13:49:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Status to password dialog, Cleaned up HRESULT return codes.  Added checkbox to SetupDlg for Auto Redirect o offline to edit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 15:10:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Jun 2005 09:28:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
