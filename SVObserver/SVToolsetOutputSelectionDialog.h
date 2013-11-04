//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : <SVToolsetOutputSelectionDialog>
//* .File Name       : $Workfile:   SVToolsetOutputSelectionDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:25:48  $
//******************************************************************************

#if !defined(SVTOOLSETOUTPUTSELECTIONDIALOG_H)
#define SVTOOLSETOUTPUTSELECTIONDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVDlgResultPicker.h"

/////////////////////////////////////////////////////////////////////////////
// SVToolsetOutputSelectionDialog dialog

class SVToolsetOutputSelectionDialog : public SVDlgResultPicker
{
public:
	// Input: set before DoModal
	bool m_bUpdateObjectAttributes;	// default false

	// Output: valid after DoModal
	CString m_sSelectedOutputName;
	SVObjectReference m_refSelectedObject;

// Construction
public:
	SVToolsetOutputSelectionDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVToolsetOutputSelectionDialog)
	enum { IDD = IDD_RESULT_PICKER };
	//}}AFX_DATA

// Methods

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVToolsetOutputSelectionDialog)
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVToolsetOutputSelectionDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(SVTOOLSETOUTPUTSELECTIONDIALOG_H)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolsetOutputSelectionDialog.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 12:25:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   28 Jul 2005 08:16:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added capability for SVToolsetOutputSelectionDialog to update object attributes when done selecting
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jul 2005 16:05:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to make available the SVObjectReference that was selected
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Jul 2004 12:32:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  429
 * SCR Title:  Fix Select Object tree control
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified to work with the new tree control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 16:55:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Feb 2000 14:17:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  106
 * SCR Title:  Math Tool - List processing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn. New Class for Selecting a single toolset
 * output via a tree control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
