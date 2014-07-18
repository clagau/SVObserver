//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVChildrenSetupDialog
//* .File Name       : $Workfile:   SVChildrenSetupDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Jul 2014 18:19:34  $
//******************************************************************************

#pragma once

#include "SVTaskObjectListCtrl.h"

class SVIPDoc;
class SVTaskObjectClass;
class SVClassInfoStructListClass;
class SVTaskObjectListClass;
class SVObjectClass;

class SVChildrenSetupDialogClass : public CDialog
{
public:
	SVChildrenSetupDialogClass( CWnd* pParent = NULL );

//******************************************************************************
// Message Handler(s):
//******************************************************************************
protected:
	//{{AFX_MSG(SVChildrenSetupDialogClass)
	afx_msg void OnAddButton();
	afx_msg void OnRemoveButton();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSetupButton();
	afx_msg void OnPublishButton();
	afx_msg void OnItemChangedChildrenList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedAvailableChildrenList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


//******************************************************************************
// Virtual(s):
//******************************************************************************

	//{{AFX_VIRTUAL(SVChildrenSetupDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************
protected:
	void redrawLists();
	BOOL checkOkToDelete( SVTaskObjectClass* pTaskObject );

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	SVClassInfoStructListClass*	m_pAvailableChildrenList;
	SVTaskObjectListClass*		m_pParentObject;
	CString StrTitle;
	BOOL						m_AllowMultipleChildrenInstances;
	SVIPDoc*					m_pDocument;

protected:
	SVObjectClass* m_pParentOwner;

	//{{AFX_DATA(SVChildrenSetupDialogClass)
	enum { IDD = IDD_CHILDREN_DIALOG };
	SVTaskObjectListCtrlClass m_ChildrenListCtrl;
	CListCtrl m_AvailableChildrenListCtrl;
	//}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVChildrenSetupDialog.h_v  $
 * 
 *    Rev 1.1   17 Jul 2014 18:19:34   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:53:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   04 Sep 2012 14:49:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove dead functionality and attributes from dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   17 Apr 2003 17:08:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   19 Nov 2002 10:59:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   30 Jan 2002 16:06:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  278
 * SCR Title:  Error message is displayed when removing result objects used by other tools
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function ::checkOkToDelete which calls SVShowDependentsDialog to show all objects connected to  the specified object.
 * Modified ::OnRemoveButton to call ::checkOkToDelete passing the result object before removing it.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Apr 17 2001 15:19:18   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file chages due to restructuring of include files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:11:04   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Oct 29 1999 13:13:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Repair some selection problems with the Gage Tool child analyzers selection buttons.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Sep 1999 13:17:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added member variable pointer to SVIPDoc for refreshing
 * of published results.
 * Revised OnPublishButtom method for new publish result
 * picker.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Aug 1999 12:38:00   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Initial Check In.
 * Replaces old Result Adjustment Dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/