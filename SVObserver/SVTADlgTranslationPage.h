//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgTranslationPage
//* .File Name       : $Workfile:   SVTADlgTranslationPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:24:26  $
//******************************************************************************

#ifndef SVTADLGTRANSLATIONPAGE_H
#define SVTADLGTRANSLATIONPAGE_H

#include "SVTaskObjectInterfaceClass.h"

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVEvaluateClass;
class SVDoubleValueObjectClass;
class SVBoolValueObjectClass;

class SVToolAdjustmentDialogTranslationPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogTranslationPageClass)
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogTranslationPageClass)
protected:
	afx_msg void OnXFormulaButton();
	afx_msg void OnYFormulaButton();
	afx_msg void OnPerformTranslation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_DATA(SVToolAdjustmentDialogTranslationPageClass)
public:
	enum { IDD = IDD_TA_TRANSLATION_DIALOG };
	CString	StrTranslationXValue;
	CString	StrTranslationYValue;
	BOOL	m_performTranslation;
	//}}AFX_DATA

public:
	SVToolAdjustmentDialogTranslationPageClass( SVToolAdjustmentDialogSheetClass* Parent );

	virtual HRESULT SetInspectionData();

protected:
	void refresh();

	SVToolAdjustmentDialogSheetClass* pParentDialog;
	SVToolClass* pTool;

	SVEvaluateClass* pEvaluateTranslationX;
	SVEvaluateClass* pEvaluateTranslationY;

	//SVDoubleValueObjectClass*	pTranslationXResult;
	//SVDoubleValueObjectClass*	pTranslationYResult;
	//SVBoolValueObjectClass* pPerformTranslation;
	//SVLongValueObjectClass* pTranslationMode;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVTADLGTRANSLATIONPAGE_H

//** EOF **

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgTranslationPage.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:24:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Dec 2012 12:10:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changes made for adding the new Shift Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Nov 2012 13:39:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  798
 * SCR Title:  Change Transformation Tool to have an option for translation only
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added Tanslation Mode combo and static text to dialog - new method OnCbnSelCHangeComoboTransMode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Aug 2005 08:25:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 16:20:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 19:28:50   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 15:49:12   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:31:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Dialog Class to edit the translation parameters for the
 * Transformation Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
