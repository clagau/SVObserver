//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecuritySetupSheet
//* .File Name       : $Workfile:   SVSecuritySetupSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:04:24  $
//******************************************************************************

#ifndef SECURITYSETUPSHEET_H
#define SECURITYSETUPSHEET_H

//	item - A pointer to another branch off the tree (NULL if none).
//	name - The string the TreeCtrl displays for that node.
//	func - A function pointer to the function to execute upon double-clicking the node.
//	image - An integer specifying which image in the image list that the node uses.
//	simage - An integer specifying which image in the image list that the node uses when selected.

typedef struct tagItem
{
	HTREEITEM	parent;
	int			id;
	int			image;
	int			simage;
} Item;

/////////////////////////////////////////////////////////////////////////////
// SVSecuritySetupSheet

class SVAccessClass;

class SVSecuritySetupSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(SVSecuritySetupSheet)

// Construction
public:
	
	SVSecuritySetupSheet();
	
	SVSecuritySetupSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	
	SVSecuritySetupSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	SVAccessClass* m_pAccess;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVSecuritySetupSheet)
	public:
	
	virtual BOOL OnInitDialog();
	
	virtual int DoModal();
	protected:
	
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	
	virtual ~SVSecuritySetupSheet();
	
	void InitializeGroupList(CListBox& list);

	void Initialize(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(SVSecuritySetupSheet)
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	
	friend BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LPARAM ptr);
	
	friend BOOL CALLBACK EnumApplicationNames(CString &szKey, void* ptr);
	friend class SVSecuritySetupPage;

private:
	
	
	Item			item;
};

/////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EnumResNameProc(HANDLE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG lParam);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECURITYSETUPSHEET_H__FB876D1A_4765_11D4_A8FD_00106F0309AB__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSecurity\SVSecuritySetupSheet.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:04:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Oct 2012 08:43:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed class scope declaration on header methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Sep 2012 18:43:04   jspila
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
 *    Rev 1.1   21 Jun 2005 15:10:30   tbair
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
