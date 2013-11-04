//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogTransformationLearnPage
//* .File Name       : $Workfile:   SVTADlgTransformationLearnPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:23:36  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVTADLGTRANSFORMATIONLEARNPAGE_H
#define SVTADLGTRANSFORMATIONLEARNPAGE_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVTaskObjectInterfaceClass.h"

class SVToolAdjustmentDialogSheetClass;
class SVImageTransformClass;
class SVToolClass;
class SVBoolValueObjectClass;
class SVEvaluateClass;
class SVDoubleValueObjectClass;

class SVToolAdjustmentDialogTransformationLearnPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
// Construction
public:
	SVToolAdjustmentDialogTransformationLearnPageClass( SVToolAdjustmentDialogSheetClass* Parent );

	virtual HRESULT SetInspectionData();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogTransformationLearnPageClass)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
protected:
	
	void refresh();
	void refreshLearnedValues();

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogTransformationLearnPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnLearnButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	//{{AFX_DATA(SVToolAdjustmentDialogTransformationLearnPageClass)
	enum { IDD = IDD_TA_TRANSFORMATION_LEARN_DIALOG };
	CString	StrTranslationXValue;
	CString	StrTranslationYValue;
	CString	StrRotationXValue;
	CString	StrRotationYValue;
	CString	StrRotationAngleValue;
	CString	StrLearnedTranslationXValue;
	CString	StrLearnedTranslationYValue;
	CString	StrLearnedRotationXValue;
	CString	StrLearnedRotationYValue;
	CString	StrLearnedRotationAngleValue;
	//}}AFX_DATA

protected:
	SVToolAdjustmentDialogSheetClass* pParentDialog;
	SVToolClass* pTool;

	SVBoolValueObjectClass*		pPerformTranslation;

	SVEvaluateClass*			pEvaluateTranslationX;
	SVDoubleValueObjectClass*	pTranslationXResult;

	SVEvaluateClass*			pEvaluateTranslationY;
	SVDoubleValueObjectClass*	pTranslationYResult;

	SVBoolValueObjectClass*		pPerformRotation;

	SVEvaluateClass*			pEvaluateRotationX;
	SVDoubleValueObjectClass*	pRotationXResult;

	SVEvaluateClass*			pEvaluateRotationY;
	SVDoubleValueObjectClass*	pRotationYResult;

	SVEvaluateClass*			pEvaluateRotationAngle;
	SVDoubleValueObjectClass*	pRotationAngleResult;

	SVImageTransformClass*	    pImageTransform;

	SVDoubleValueObjectClass*   pLearnedTranslationX;
	SVDoubleValueObjectClass*   pLearnedTranslationY;
	SVDoubleValueObjectClass*   pLearnedRotationX;
	SVDoubleValueObjectClass*   pLearnedRotationY;
	SVDoubleValueObjectClass*   pLearnedRotationAngle;

	CFont angleFont;

private:
	double m_translationXValue;
	double m_translationYValue;
	double m_rotationXValue;
	double m_rotationYValue;
	double m_rotationAngleValue;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVTADLGTRANSFORMATIONLEARNPAGE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgTransformationLearnPage.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:23:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Aug 2005 08:25:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 16:21:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 19:28:54   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Apr 2001 15:47:14   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:33:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Dialog Class to support the Learn feature of the
 * Transformation Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
