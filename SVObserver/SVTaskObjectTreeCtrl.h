//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectTreeCtrlClass
//* .File Name       : $Workfile:   SVTaskObjectTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:35:30  $
//******************************************************************************

#ifndef SVTASKOBJECTTREECTRL_H
#define SVTASKOBJECTTREECTRL_H

// SVTaskObjectTreeCtrl.h : header file
//


class SVTaskObjectListClass;
class SVTaskObjectClass;

/////////////////////////////////////////////////////////////////////////////
// SVTaskObjectTreeCtrlClass window

class SVTaskObjectTreeCtrlClass : public CTreeCtrl
{
	DECLARE_DYNCREATE(SVTaskObjectTreeCtrlClass)

// Construction
public:
	SVTaskObjectTreeCtrlClass();

// Attributes
public:
protected:
	SVTaskObjectListClass* pOwnerTaskList;

// Operations
public:

	BOOL SetTaskObjectList( SVTaskObjectListClass* POwnerTaskList );
	void SetHasButtonsStyle(void);
	void SetNoEditLabelsStyle(void);

	HTREEITEM InsertTaskItem( HTREEITEM HParent, SVTaskObjectClass* PTask );
	void Rebuild();

	SVTaskObjectClass * GetSelectedTaskObject();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVTaskObjectTreeCtrlClass)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SVTaskObjectTreeCtrlClass();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVTaskObjectTreeCtrlClass)
//SEJ 101	afx_msg void OnEditCopy();
//SEJ 101	afx_msg void OnEditCut();
//SEJ 101	afx_msg void OnEditDelete();
//SEJ 101	afx_msg void OnEditPaste();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVTASKOBJECTTREECTRL_H__436D5A83_C8F6_11D2_ACC1_00C04FC38F76__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTaskObjectTreeCtrl.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:35:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   04 Sep 2012 15:56:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 16:36:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
