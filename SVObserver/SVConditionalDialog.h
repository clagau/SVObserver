//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalDialogClass
//* .File Name       : $Workfile:   SVConditionalDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:59:48  $
//******************************************************************************

#ifndef SVCONDITIONALDIALOG_H
#define SVCONDITIONALDIALOG_H

#include "SVToolBar.h"
#include "SVTaskObjectInterfaceClass.h"

class SVIPDoc;
class SVToolClass;
class SVToolSetClass;
class SVInspectionProcess;
class SVConditionalClass;
struct SVEquationStruct;
class SVTaskObjectListClass;

/////////////////////////////////////////////////////////////////////////////
// SVConditionalDialogClass dialog

class SVConditionalDialogClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
// Construction
public:
	SVConditionalDialogClass();   // standard constructor

	virtual HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVConditionalDialogClass)
	enum { IDD = IDD_CONDITIONAL_DIALOG };
	CButton	ValidateCtrl;
	CEdit	ConstantEditCtrl;
	CButton	AddPPQVariableCtrl;
	CButton	AddLocalVariableCtrl;
	CButton	AddConstantCtrl;
	CButton	DisableConditionCtrl;
	CComboBox	PPQVariableComboCtrl;
	CEdit	MathRichEditCtrl;
	CString	StrConstantValue;
	BOOL	ConditionDisabled;
	BOOL	Disabled;
	CString	ToolsetOutputVariable;
	CButton ToolsetOutputSelectButton;
	//}}AFX_DATA

	// Data members
protected:
	SVToolBarClass	logicalOperatorBar;
	SVToolBarClass	conditionalOperatorBar;
	SVToolBarClass	constantOperatorBar;
	SVToolBarClass	cursorBar;

private:
	SVIPDoc* m_pIPDoc;
	SVInspectionProcess *pInspection;
	SVToolSetClass* pToolSet;
	SVToolClass* pTool;
	SVConditionalClass*	pEquation;
	SVEquationStruct*	pEquationStruct;
	int numChars;

	int ppqComboExtent;

	CBitmap downArrowBitmap;

	CString csToolTypeText;   // For disable button text

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVConditionalDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTaskObjectList( SVTaskObjectListClass* pObject, LPCTSTR pszToolText );
	void GetEquationText();
	BOOL ValidateEquation();

protected:
	void setEquation(SVConditionalClass* PEquation);

	void backspaceRichEdit( long Pos );
	void insertIntoRichEdit( LPCTSTR TStrValue );
	void advanceRichEdit( long Pos );
	void deleteRichEdit( long Pos );
	void setEquationText();
	BOOL createCursorToolbar();
	BOOL createOperatorToolbars();
	void EnableControls();
	int GetComboBoxStringExtent( CComboBox& RComboBox, LPCTSTR szStr );

protected:
	// Generated message map functions
	//{{AFX_MSG(SVConditionalDialogClass)
	virtual void OnOK();
	afx_msg void OnAddPPQVariableButton();
	afx_msg void OnAddLocalVariableButton();
	afx_msg void OnAddConstantButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnDisableCondition();
	afx_msg void OnDisable();
	afx_msg void OnValidate();
	afx_msg void OnDropdownPpqVariableCombo();
	afx_msg void OnLocalVariableSelect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditionalDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:59:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   02 Jul 2012 16:39:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   08 Dec 2010 07:45:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   08 Aug 2005 15:33:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   17 Apr 2003 17:16:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   19 Nov 2002 11:08:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   31 Oct 2001 16:02:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  252
 * SCR Title:  System hang on .5 dual when accessing Math Tool after going online
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed member from CRichEditCtrl to CEdit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Apr 17 2001 15:23:56   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file chages due to restructuring of include files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   29 Feb 2000 14:31:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  106
 * SCR Title:  Math Tool - List processing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised size of math toolbars.
 * Revised selection of toolset output from a combobox to
 * a tree control selection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:14:38   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Oct 28 1999 11:31:46   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  52
 * SCR Title:  Version 3.00 Beta 14 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Tool/Tool set conditional dialog adjustment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Sep 1999 17:05:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added validateEquation method to be called from the
 * parent PropertySheet's OnOk method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   11 Sep 1999 19:55:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised initialization for use with SVToolClass as well as
 * SVToolSetClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Sep 1999 17:52:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to resize drop down list in comboboxes.
 * Revised call to SVEquationClass::Test()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   31 Aug 1999 23:41:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to be derived from CPropertyPage.
 * Added ToolSet variable combobox handler.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
