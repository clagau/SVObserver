//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgPolarTransform
//* .File Name       : $Workfile:   SVTADlgPolarTransformPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:16:58  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEnumerateCombo.h"
#include "SVTaskObjectInterfaceClass.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVEvaluateClass;

class SVToolAdjustmentDialogPolarTransformPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
public:
	SVToolAdjustmentDialogPolarTransformPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVToolAdjustmentDialogPolarTransformPageClass();
	virtual HRESULT SetInspectionData() override;

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogPolarTransformPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
protected:
	void refresh();

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogPolarTransformPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnCenterXFormulaButton();
	afx_msg void OnCenterYFormulaButton();
	afx_msg void OnStartRadiusFormulaButton();
	afx_msg void OnEndRadiusFormulaButton();
	afx_msg void OnStartAngleFormulaButton();
	afx_msg void OnEndAngleFormulaButton();
	afx_msg void OnUseFormulaCheck();
	afx_msg void OnSelchangeInterpolationModeCombo();
	afx_msg void OnSelchangeAngularMethod();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	//{{AFX_DATA(SVToolAdjustmentDialogPolarTransformPageClass)
	enum { IDD = IDD_TA_POLARTRANSFORM_DIALOG };
	SVEnumerateComboClass	m_AngularMethodCombo;
	SVEnumerateComboClass	interpolationComboCtrl;
	CString	StrCenterXValue;
	CString	StrCenterYValue;
	CString	StrStartRadiusValue;
	CString	StrEndRadiusValue;
	CString	StrStartAngleValue;
	CString	StrEndAngleValue;
	BOOL	bUseFormula;
	//}}AFX_DATA

protected:
	SVToolAdjustmentDialogSheetClass* pParentDialog;
	SVToolClass* pTool;

	SVEvaluateClass*			pEvaluateCenterX;
	SVDoubleValueObjectClass*	pCenterXResult;

	SVEvaluateClass*			pEvaluateCenterY;
	SVDoubleValueObjectClass*	pCenterYResult;

	SVEvaluateClass*			pEvaluateStartRadius;
	SVDoubleValueObjectClass*	pStartRadiusResult;

	SVEvaluateClass*			pEvaluateEndRadius;
	SVDoubleValueObjectClass*	pEndRadiusResult;

	SVEvaluateClass*			pEvaluateStartAngle;
	SVDoubleValueObjectClass*	pStartAngleResult;

	SVEvaluateClass*			pEvaluateEndAngle;
	SVDoubleValueObjectClass*	pEndAngleResult;

	SVBoolValueObjectClass*		 pUseFormula;
	SVEnumerateValueObjectClass* m_pAngleMethod;
	SVEnumerateValueObjectClass* pInterpolationMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgPolarTransformPage.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:16:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Aug 2005 08:18:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Feb 2005 10:23:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  449
 * SCR Title:  Modify Polar Un-Wrap Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Made changes to use a Angular Method combo instead of a use new method check box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 16:04:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new reset / create methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 16:21:00   rschock
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
 *    Rev 1.1   19 Apr 2001 15:44:34   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 01 2000 12:15:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   NEW Module: Provides access to attributes of polar transform tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
