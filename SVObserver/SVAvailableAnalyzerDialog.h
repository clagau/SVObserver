//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAvailableAnalyzerDialog
//* .File Name       : $Workfile:   SVAvailableAnalyzerDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:40:14  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVAVAILABLEANALYZERDIALOG_H
#define SVAVAILABLEANALYZERDIALOG_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#include "SVClassInfoStruct.h"

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************


class SVAvailableAnalyzerDialogClass : public CDialog
{
//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVAvailableAnalyzerDialogClass( CWnd* pParent = NULL );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:

//******************************************************************************
// Message Handler(s):
//******************************************************************************
protected:
	//{{AFX_MSG(SVAvailableAnalyzerDialogClass)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Virtual(s):
//******************************************************************************

	//{{AFX_VIRTUAL(SVAvailableAnalyzerDialogClass)
	protected:
	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL


//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************
protected:

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:

	SVClassInfoStructListClass*	PAvailableAnalyzerList;
	SVClassInfoStructListClass	SelectedAnalyzerList;

	//{{AFX_DATA(SVAvailableAnalyzerDialogClass)
	enum { IDD = IDD_AVAILABLE_ANALYZER_DIALOG };
	CListCtrl	AvailableAnalyzerListCtrl;
	//}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVAVAILABLEANALYZERDIALOG_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAvailableAnalyzerDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:40:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   17 Apr 2003 16:45:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Nov 2002 10:30:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:00:30   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Aug 1999 12:27:56   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Ported to 3.0. Used by TA Multiple Analyzer Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/