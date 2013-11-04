//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogBuildReferencePage
//* .File Name       : $Workfile:   SVTADlgBuildReferencePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:19:04  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVTADLGBUILDREFERENCEPAGE_H
#define SVTADLGBUILDREFERENCEPAGE_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVTaskObjectInterfaceClass.h"

class SVToolClass;
class SVEvaluateXClass;
class SVEvaluateYClass;
class SVEvaluateBetaClass;
class SVRotateReferenceClass;
class SVBoolValueObjectClass;
class SVDoubleValueObjectClass;

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

class SVToolAdjustmentDialogSheetClass;


class SVToolAdjustmentDialogBuildReferencePageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
// Construction
public:
	SVToolAdjustmentDialogBuildReferencePageClass( SVToolAdjustmentDialogSheetClass* Parent );

	virtual HRESULT SetInspectionData();

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogBuildReferencePageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
protected:
	
	void refresh();

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogBuildReferencePageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnLearnButton();
	afx_msg void OnThetaFormulaButton();
	afx_msg void OnXFormulaButton();
	afx_msg void OnYFormulaButton();
	afx_msg void OnThetaEnabledCheck();
	afx_msg void OnXEnabledCheck();
	afx_msg void OnYEnabledCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	//{{AFX_DATA(SVToolAdjustmentDialogBuildReferencePageClass)
	enum { IDD = IDD_TA_BUILD_REFERENCE_DIALOG };
	CString	StrThetaValue;
	CString	StrXValue;
	CString	StrYValue;
	BOOL	bThetaEnabled;
	BOOL	bXEnabled;
	BOOL	bYEnabled;
	//}}AFX_DATA

protected:
	SVToolAdjustmentDialogSheetClass* pParentDialog;
	SVToolClass* pTool;

	SVEvaluateXClass*			pEvaluateX;
	SVBoolValueObjectClass*		pXEnabled;
	SVDoubleValueObjectClass*	pXResult;

	SVEvaluateYClass*			pEvaluateY;
	SVBoolValueObjectClass*		pYEnabled;
	SVDoubleValueObjectClass*	pYResult;

	SVEvaluateBetaClass*		pEvaluateBeta;
	SVBoolValueObjectClass*		pBetaEnabled;
	SVDoubleValueObjectClass*	pBetaResult;

	SVRotateReferenceClass*     pRotate;
	SVDoubleValueObjectClass*   pLearnedX;
	SVDoubleValueObjectClass*   pLearnedY;
	SVDoubleValueObjectClass*   pLearnedBeta;

	double m_xResult;
	double m_yResult;
	double m_betaResult;

	CFont thetaFont;
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVTADLGBUILDREFERENCEPAGE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgBuildReferencePage.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:19:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   09 Aug 2005 08:15:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 15:43:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Apr 2001 19:23:40   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Apr 2001 15:34:22   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:40:24   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Oct 1999 14:00:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added logic in InitDialog to set the symbol font and text for the theta static text control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 14 1999 15:13:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provide Build Reference Dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/