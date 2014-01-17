//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgRotationPage
//* .File Name       : $Workfile:   SVTADlgRotationPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 Jan 2014 16:47:56  $
//******************************************************************************

#ifndef SVTADLGROTATIONPAGE_H
#define SVTADLGROTATIONPAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma region Includes
#include "SVTaskObjectInterfaceClass.h"
#include "SVEnumerateCombo.h"
#pragma endregion

#pragma region Declarations
class SVEvaluateClass;
class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVDoubleValueObjectClass;
class SVBoolValueObjectClass;
#pragma endregion;

class SVToolAdjustmentDialogRotationPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
#pragma region Constructor
public:
	SVToolAdjustmentDialogRotationPageClass( SVToolAdjustmentDialogSheetClass* Parent );
#pragma endregion

#pragma region Protected Methods
protected:
#pragma region MFC Methods
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogRotationPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogRotationPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnAngleFormulaButton();
	afx_msg void OnXFormulaButton();
	afx_msg void OnYFormulaButton();
	afx_msg void OnPerformRotation();
	afx_msg void OnSelChangeInterpolationModeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#pragma endregion MFC Methods

	virtual HRESULT SetInspectionData();
	void refresh();
#pragma endregion

#pragma region Member variables
#pragma region Data Elements
public:
	//{{AFX_DATA(SVToolAdjustmentDialogRotationPageClass)
	enum { IDD = IDD_TA_ROTATION_DIALOG };
	SVEnumerateComboClass	m_cbInterpolation;
	CString	m_strRotationAngleValue;
	CString	m_strRotationXValue;
	CString	m_strRotationYValue;
	BOOL	m_performRotation;
	//}}AFX_DATA
#pragma endregion Data Elements

protected:
	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	SVToolClass* m_pTool;

	SVEvaluateClass*			m_pEvaluateRotationX;
	SVDoubleValueObjectClass*	m_pRotationXResult;

	SVEvaluateClass*			m_pEvaluateRotationY;
	SVDoubleValueObjectClass*	m_pRotationYResult;

	SVEvaluateClass*			m_pEvaluateRotationAngle;
	SVDoubleValueObjectClass*	m_pRotationAngleResult;

	SVBoolValueObjectClass*		m_pPerformRotation;
	SVEnumerateValueObjectClass* m_pInterpolationMode;

	CFont angleFont;
#pragma endregion
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif	// SVTADLGROTATIONPAGE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgRotationPage.h_v  $
 * 
 *    Rev 1.1   15 Jan 2014 16:47:56   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  879
 * SCR Title:  Add interpolation mode to transformation tool
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed code to conform with guidelines.
 * Added SVEnumerateCombo.h.
 * Changed SetInspectionData method from public to protected.
 * Added protected MFC methods OnSelChangeInterpolationModeCombo and OnSetActive.
 * Added members m_cbInterpolation (public) and m_pInterpolationMode (protected).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:17:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Dec 2012 11:59:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed items associated with SCR 198
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Nov 2012 13:39:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  798
 * SCR Title:  Change Transformation Tool to have an option for translation only
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new method OnSetActive, changed method OnInitDialog for the new tanslation mode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Aug 2005 08:25:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 16:21:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:34:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Dialog Class to edit the Rotation parameters for the
 * Transformation Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/