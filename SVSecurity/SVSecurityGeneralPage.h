//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecurityGeneralPage.cpp
//* .File Name       : $Workfile:   SVSecurityGeneralPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:02:58  $
//******************************************************************************
#ifndef SVSECURITYGENERAL_H
#define SVSECURITYGENERAL_H

#define INFO_BUFFER_SIZE 32767

class SVAccessClass;

class SVSecurityGeneralPage : public CPropertyPage
{
	DECLARE_DYNCREATE(SVSecurityGeneralPage)

public:
	SVSecurityGeneralPage();
	SVSecurityGeneralPage(UINT nIDTemplate, UINT nIDCaption=0);
	virtual ~SVSecurityGeneralPage();

	//{{AFX_DATA(SVSecurityGeneralPage)
	enum { IDD = IDD_GENERAL_SECURITY_PAGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVSecurityGeneralPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	SVAccessClass* m_pAccess;

	// Generated message map functions
	//{{AFX_MSG(SVSecurityGeneralPage)
	afx_msg void OnUserMgrBtn();
	afx_msg void OnEventViewerBtn();
	afx_msg void OnLoginBtn();
	virtual BOOL OnInitDialog();
	afx_msg void OnNoSecurityBtn();
	afx_msg void OnTraditionalSecurityBtn();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSecurity\SVSecurityGeneralPage.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:02:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Sep 2012 18:43:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:35:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   27 Oct 2010 08:58:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include issues and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   26 Mar 2008 09:33:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  603
 * SCR Title:  Update SVObserver to run under Windows XP
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed the code to use the API to get the System32 path, instead of using the hard coded path.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Oct 2005 08:40:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnHelp and OnHelpInfo message handlers that take the help message Id, add an offset and then call into the SVObserver.hlp file.  The default operation would have tried to call into SVSecurity.hlp with normal IDs.  The offset was put in so there would be no conflict with SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Oct 2005 08:31:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnHelp and OnHelpInfo Message handlers for adding offsets to the resource IDs and calls the SVObserver Help file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Aug 2005 14:54:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed High Security Button from General Page
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Aug 2005 09:52:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Default Security Buttons in the General Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Aug 2005 09:52:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Default Security Buttons in the General Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 15:08:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
