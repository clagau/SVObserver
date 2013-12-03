//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgLUTPage
//* .File Name       : $Workfile:   SVTADlgLUTPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 08:17:50  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVTADLGLUTPAGE_H
#define SVTADLGLUTPAGE_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVEnumerateCombo.h"
#include "SVDlgGraph.h"

#include "SVTaskObjectInterfaceClass.h"

class SVLUTOperatorClass;
class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVLUTEquationClass;

class SVToolAdjustmentDialogLUTPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
// Construction
public:
	SVToolAdjustmentDialogLUTPageClass( SVToolAdjustmentDialogSheetClass* Parent );

	virtual HRESULT SetInspectionData();

	SVByteValueObjectClass* m_pLUTVector;

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogLUTPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
protected:
	
	void refresh(bool p_bSave );

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogLUTPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnActivateCheck();
	afx_msg void OnSelchangeLutModeCombo();
	afx_msg void OnLUTFormulaButton();
	afx_msg void OnContinuousRecalcCheck();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	afx_msg LRESULT OnGraphRefresh( WPARAM mp1, LPARAM mp2 );
	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	//{{AFX_DATA(SVToolAdjustmentDialogLUTPageClass)
	enum { IDD = IDD_TA_LUT_DIALOG };
	CSliderCtrl	m_upperSlider;
	CSliderCtrl	m_lowerSlider;
	SVDlgGraphClass	m_LUTGraph;
	SVEnumerateComboClass	m_LUTModeCombo;
	CString	m_strUpperClipValue;
	CString	m_strLowerClipValue;
	BOOL	m_bUseLUT;
	BOOL	m_bContinuousRecalcLUT;
	//}}AFX_DATA

protected:
	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	SVToolClass*                    m_pTool;

	SVLUTOperatorClass*             m_pLUTOperator;
	SVLUTEquationClass*             m_pLUTEquation;
	SVBoolValueObjectClass*         m_pUseLUT;
	SVBoolValueObjectClass*         m_pContinuousRecalcLUT;
	SVEnumerateValueObjectClass*    m_pLUTMode;
	SVLongValueObjectClass*         m_pLUTUpperClip;
	SVLongValueObjectClass*         m_pLUTLowerClip;
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVTADLGLUTPAGE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgLUTPage.h_v  $
 * 
 *    Rev 1.1   02 Oct 2013 08:17:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:15:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Oct 2005 08:57:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix Bug OnInitDialog for Lut clip function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Aug 2005 08:18:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Aug 2005 12:01:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   replaced SVVectorObject with SVValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 16:12:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2001 19:28:50   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2001 15:41:14   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jun 2000 13:34:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  153
 * SCR Title:  LUT free form operation causes crash
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised declaration and implementation of OnGraphRefresh
 * to have two parameters as required.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Mar 2000 14:52:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Provides Tool Adjust Dialog Page for LUT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
