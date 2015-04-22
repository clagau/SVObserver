//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgLUTPage
//* .File Name       : $Workfile:   SVTADlgLUTPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   27 Jan 2014 15:50:42  $
//******************************************************************************

#ifndef SVTADLGLUTPAGE_H
#define SVTADLGLUTPAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma region Includes
#include "SVEnumerateCombo.h"
#include "SVOMFCLibrary/SVDlgGraph.h"
#include "SVTaskObjectInterfaceClass.h"
#include "SVMFCControls/CLabel.h"
#pragma endregion Includes

#pragma region Declarations
class SVLUTOperatorClass;
class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVLUTEquationClass;
#pragma endregion Declarations

class SVToolAdjustmentDialogLUTPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
#pragma region Constructor
public:
	SVToolAdjustmentDialogLUTPageClass( SVToolAdjustmentDialogSheetClass* Parent );
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual HRESULT SetInspectionData();
	SVByteValueObjectClass*	getLUTVector() { return m_pLUTVector; };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:

#pragma region virtual
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogLUTPageClass)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
#pragma endregion virtual
	
	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogLUTPageClass)
	afx_msg void OnActivateCheck();
	afx_msg void OnSelChangeLutModeCombo();
	afx_msg void OnLUTFormulaButton();
	afx_msg void OnContinuousRecalcCheck();
	afx_msg void OnClipModeCheck();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLowerEditLostFocus();
	afx_msg void OnUpperEditLostFocus();
	//}}AFX_MSG
	afx_msg LRESULT OnGraphRefresh( WPARAM mp1, LPARAM mp2 );
	DECLARE_MESSAGE_MAP()

	//************************************
	// Method:    refresh
	// Description: Refresh the GUI.
	// Access:    protected
	// Returns:   void
	// Parameter: bool p_bSave = true: If true set data from GUI to inspection.
	//************************************
	void refresh( bool p_bSave = true );

	//************************************
	// Method:    refreshLUTGraph
	// Description: Refresh the LUT graph.
	// Access:    protected
	// Returns:   void
	//************************************
	void refreshLUTGraph();

	//************************************
	// Method:    refreshClip
	// Description: Refresh the clip controls (Lower and Upper).
	// Access:    protected
	// Returns:   void
	//************************************
	void refreshClip();

	//************************************
	// Method:    showFormulaControls
	// Description: Show formula controls and hide the others (e.g. clip controls).
	// Access:    protected
	// Returns:   void
	//************************************
	void showFormulaControls();

	//************************************
	// Method:    showClipControls
	// Description: Show clip controls and hide the others (e.g. formula controls).
	// Access:    protected
	// Returns:   void
	//************************************
	void showClipControls();

	//************************************
	// Method:    hideAllUncommonControls
	// Description: Hide all controls (e.g. formula and clip controls), expect of common controls (useLTU, contRecalc, LTUMode).
	// Access:    protected
	// Returns:   void
	//************************************
	void hideAllUncommonControls();

	//************************************
	// Method:    enableAllControls
	// Description: Enable or disable all controls, expect of the useLTU checkbox.
	// Access:    protected
	// Returns:   void
	// Parameter: BOOL isEnable: Flag if controls enabled (TRUE) or disabled (FALSE).
	//************************************
	void enableAllControls(BOOL isEnable);
#pragma endregion Protected Methods

#pragma region Member variables
//******************************************************************************
// Data Element(s):
//******************************************************************************
private:
	//{{AFX_DATA(SVToolAdjustmentDialogLUTPageClass)
	enum { IDD = IDD_TA_LUT_DIALOG };
	CSliderCtrl m_upperSlider;
	CSliderCtrl m_lowerSlider;
	SvOml::SVDlgGraphClass m_LUTGraph;
	SVEnumerateComboClass m_LUTModeCombo;
	CString m_strUpperClipValue;
	CString m_strLowerClipValue;
	CLabel m_yAxisLabel;
	BOOL m_bUseLUT;
	BOOL m_bContinuousRecalcLUT;
	BOOL m_isFormulaClip;
	//}}AFX_DATA

	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	SVToolClass*                    m_pTool;
	SVByteValueObjectClass*			m_pLUTVector;
	SVLUTOperatorClass*             m_pLUTOperator;
	SVLUTEquationClass*             m_pLUTEquation;
	SVBoolValueObjectClass*         m_pUseLUT;
	SVBoolValueObjectClass*         m_pContinuousRecalcLUT;
	SVEnumerateValueObjectClass*    m_pLUTMode;
	SVLongValueObjectClass*         m_pLUTUpperClip;
	SVLongValueObjectClass*         m_pLUTLowerClip;
	SVBoolValueObjectClass*			m_pIsLUTFormulaClipped;
#pragma endregion Member variables
};

#endif	// SVTADLGLUTPAGE_H
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgLUTPage.h_v  $
 * 
 *    Rev 1.2   27 Jan 2014 15:50:42   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  883
 * SCR Title:  Clip-mode for LUT formula mode
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed code to conform with guidelines.
 * Changed member m_pLUTVector from public to private and added public method getLUTVector.
 * Changed OnSelchangeLutModeCombo to OnSelChangeLutModeCombo.
 * Added comments for refresh-related methods.
 * Added protected methods OnClipModeCheck, OnLowerEditLostFocus, OnUpperEditLostFocus, refreshLUTGraph, refreshClip, showFormulaControls, showClipControls, hideAllUncommonControls, and enableAllControls.
 * Added private member variables m_pIsLUTFormulaClipped and m_isFormulaClip.
 * Changed public member variables to private:  m_upperSlider, m_lowerSlider, m_LUTGraph, m_LUTModeCombo, m_strUpperClipValue, m_strLowerClipValue, m_bUseLUT, m_bContinuousRecalcLUT.
 * Changed protected member variables to private:  m_pParentDialog, m_pTool, m_pLUTOperator, m_pLUTEquation, m_pUseLUT, m_pContinuousRecalcLUT, m_pLUTMode, m_pLUTUpperClip, m_pLUTLowerClip.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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