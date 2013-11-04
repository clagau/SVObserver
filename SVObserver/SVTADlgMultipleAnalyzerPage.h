//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogMultipleAnalyzerPage
//* .File Name       : $Workfile:   SVTADlgMultipleAnalyzerPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:16:02  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVTADLGMULTIPLEANALYZERPAGE_H
#define SVTADLGMULTIPLEANALYZERPAGE_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVClassInfoStruct.h"

#include "SVTaskObjectInterfaceClass.h"

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVAnalyzerClass;

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

class SVToolAdjustmentDialogMultipleAnalyzerPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVToolAdjustmentDialogMultipleAnalyzerPageClass( SVToolAdjustmentDialogSheetClass* PParent );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVToolAdjustmentDialogMultipleAnalyzerPageClass();

	virtual HRESULT SetInspectionData();

//******************************************************************************
// Message Handler(s):
//******************************************************************************
protected:
	//{{AFX_MSG(SVToolAdjustmentDialogMultipleAnalyzerPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDetails();
	afx_msg void OnInsertAnalyzerButton();
	afx_msg void OnResultButton();
	afx_msg void OnKeyDownMultipleAnalyzerList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedMultipleAnalyzerList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


//******************************************************************************
// Virtual(s):
//******************************************************************************

	//{{AFX_VIRTUAL(SVToolAdjustmentDialogMultipleAnalyzerPageClass)
	public:
	virtual void OnCancel();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL


//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************
protected:
	void redrawList();

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
protected:
	SVToolAdjustmentDialogSheetClass*	pSheet;
	SVToolClass*						pTool;
	SVAnalyzerClass*					pCurrentAnalyzer;
	
	SVClassInfoStructListClass			availableAnalyzers;


	//{{AFX_DATA(SVToolAdjustmentDialogMultipleAnalyzerPageClass)
	enum { IDD = IDD_TA_MULTIPLE_ANALYZER_DIALOG };
	CListCtrl	multipleAnalyzerListCtrl;
	//}}AFX_DATA

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVTADLGMULTIPLEANALYZERPAGE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgMultipleAnalyzerPage.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:16:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   09 Aug 2005 08:18:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 16:12:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:43:06   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Aug 1999 14:26:08   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Integrated Result Children Setup Dialog Button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Aug 1999 21:03:00   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Initial Check In.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
