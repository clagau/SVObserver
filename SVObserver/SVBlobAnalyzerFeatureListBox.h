//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzerFeatureListBox
//* .File Name       : $Workfile:   SVBlobAnalyzerFeatureListBox.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:44:46  $
//******************************************************************************

#ifndef __SVBLOBANALYZERFEATURELISTBOX__INCLUDED
#define __SVBLOBANALYZERFEATURELISTBOX__INCLUDED

class SVBlobAnalyzerClass;

class SVBlobAnalyzeFeatureListBoxClass : public CListBox
{
	// Generated message map functions
protected:
	//{{AFX_MSG(SVBlobAnalyzeFeatureListBoxClass)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
// Standard constructor
	SVBlobAnalyzeFeatureListBoxClass();

public:
// Standard destructor
	virtual ~SVBlobAnalyzeFeatureListBoxClass();

public:
/*- SVBlobAnalyzeFeatureListBoxClass::init() -----------------------------------*/
/*- The parameter, atcTestChar, is the actual character to be searched in the   */
/*- enabled feature array to determine whether or not that feature should be    */
/*- added to the list. For example, pass '0' to generate "available" list, and  */
/*- pass '1' to generate "selected" list. --------------------------------------*/
	void init(SVBlobAnalyzerClass* apCurrentAnalyzer, TCHAR atcTestChar);

	//{{AFX_VIRTUAL(SVBlobAnalyzeFeatureListBoxClass)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
};

#endif //__SVBLOBANALYZERFEATURELISTBOX__INCLUDED

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVBlobAnalyzerFeatureListBox.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:44:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   17 Apr 2003 17:07:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Apr 2001 20:30:50   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:11:00   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Sep 1999 17:25:36   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   The Blob analyzer is integrated and semi functional.  Still needs debugging.  
 * 
 * Sorting is included.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 10 1999 13:57:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Files extracted from v2.42 code and changed to v3.00 architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/