//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShowDependentsDialog
//* .File Name       : $Workfile:   SVShowDependentsDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:06:40  $
//******************************************************************************

#if !defined(AFX_SVSHOWDEPENDENTSDIALOG_H__0531515D_8233_11D3_A827_00106F000C7D__INCLUDED_)
#define AFX_SVSHOWDEPENDENTSDIALOG_H__0531515D_8233_11D3_A827_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVShowDependentsDialog.h : header file
//

#include "SVTaskObject.h"
class SVTaskObjectClass;

/////////////////////////////////////////////////////////////////////////////
// SVShowDependentsDialog dialog

class SVShowDependentsDialog : public CDialog
{
// Construction
public:
	SVShowDependentsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVShowDependentsDialog)
	enum { IDD = IDD_SHOW_DEPENDENTS_DIALOG };
	CListCtrl	listCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVShowDependentsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	SVTaskObjectClass* PTaskObject;		// which object
	UINT StrMessageID;					// id of message to show
	BOOL OnlyImageDependencies;			// only images
	int NumberOfDependents;
	
	void SetFilterList( const SVObjectVector& rList );

protected:
	SVObjectVector m_FilterList;	// used when providing a custom list of objects
	bool m_bUseFilter;

// Implementation
protected:

	void addColumnHeadings();
	bool addItems();
	void setColumnWidths();

	// Generated message map functions
	//{{AFX_MSG(SVShowDependentsDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVSHOWDEPENDENTSDIALOG_H__0531515D_8233_11D3_A827_00106F000C7D__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVShowDependentsDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:06:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   12 Oct 2004 12:59:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  448
 * SCR Title:  Fix External Tool Change DLL functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for filtering the list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 15:34:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/