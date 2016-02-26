//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFormulaEditorPageClass
//* .File Name       : $Workfile:   SVFormulaEditorPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   07 Oct 2014 03:14:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls/SVToolBar.h"
#include "ObjectInterfaces/IFormulaController.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class SVFormulaEditorPageClass : public CPropertyPage
		{
		public:
			SVFormulaEditorPageClass( const GUID& rInspectionID, const GUID& rTaskObjectID, SvOi::IFormulaControllerPtr controller, bool isDisableCheckboxesVisible = false, UINT captionID = IDS_FORMULA_STRING, UINT disableExtentionID = IDS_TOOL_STRING );   // standard constructor
			virtual ~SVFormulaEditorPageClass();

			CString GetOwnerName() const;
			void SetDefaultInputs();
			bool validateAndSetEquation();

			// Dialog Data
			//{{AFX_DATA(SVFormulaEditorPageClass)
			enum { IDD = IDD_FORMULA_DIALOG };
			CEdit	m_ConstantEditCtrl;
			CButton	m_AddLocalVariableCtrl;
			CButton	m_AddConstantCtrl;
			CWnd m_EditWnd;
			CString	m_StrConstantValue;
			int		m_constantType;
			CButton m_decimalRadioButton;
			CButton m_hexadecimalRadioButton;
			CButton m_binaryRadioButton;
			CString	m_strToolsetOutputVariable;
			CButton	m_ToolsetOutputSelectButton;
			CButton m_DisableEquationCtrl;
			CButton m_DisableToolCtrl;
			BOOL	m_equationDisabled;
			BOOL	m_ownerDisabled;
			//}}AFX_DATA

		protected:
			void backspaceInEditor( long Pos );
			void insertIntoEditor( LPCTSTR tszValue );
			void advanceInEditor( long Pos );
			void deleteInEditor( long Pos );
			CString getEquationText() const;
			void setEquationText();
			BOOL createToolbars();
			int GetComboBoxStringExtent( CComboBox& rComboBox, LPCTSTR szStr );

			void HandleValidateError( int posFailed );

			void onValidate();
			void onUndo();

			void enableControls();
			void enableUndoButton();

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVFormulaEditorPageClass)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
			//}}AFX_VIRTUAL

			// Generated message map functions
			//{{AFX_MSG(SVFormulaEditorPageClass)
			afx_msg void OnHelp();
			afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
			afx_msg void OnAddLocalVariableButton();
			afx_msg void OnAddConstantButton();
			virtual BOOL OnInitDialog();
			afx_msg void OnLocalVariableSelect();
			afx_msg void OnDisable();
			afx_msg void OnEquationFieldChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
			virtual BOOL OnKillActive( );
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

			void createScintillaControl();
			bool IsBrace( int c );
			void CheckBrace();

		protected:
			SvMc::SVToolBarClass m_logicalOperatorBar;
			SvMc::SVToolBarClass m_conditionalOperatorBar;
			SvMc::SVToolBarClass m_trigOperatorBar;
			SvMc::SVToolBarClass m_miscOperatorBar;
			SvMc::SVToolBarClass m_basicOperatorBar;
			SvMc::SVToolBarClass m_cursorBar;
			SvMc::SVToolBarClass m_validateBar;
			SvMc::SVToolBarClass m_punctuationBar;
			SvMc::SVToolBarClass m_statisticsOperatorBar;

		private:
			SvOi::IFormulaControllerPtr m_FormulaController;
			bool m_isConditionalPage;
			UINT m_disableExtentionID;
			CBitmap m_downArrowBitmap;
		};
	}
}

namespace SvOg = Seidenader::SVOGui;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFormulaEditorPage.h_v  $
 * 
 *    Rev 1.4   07 Oct 2014 03:14:42   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  936
 * SCR Title:  Show Different Help for Formula and Conditional Pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add help methods OnHelp and OnHelpInfo for context help
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Jul 2014 19:02:38   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed m_AddPPQVariableCtrl, m_PPQVariableComboCtrl, m_ppqComboExtent, OnAddPPQVariableButton, OnDropdownPPQVariable
 * Code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jan 2014 12:19:12   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed and added the same methods listed in the .cpp.
 * Moved all object with not need for GUI to formulaController (m_pInspection, m_pToolSet etc.).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 May 2013 13:26:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 013.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   30 Apr 2013 11:27:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  829
 * SCR Title:  Fix tool adjust dialogs to validate all math equations before exiting.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified GetEquationText to return TRUE if the equation validates correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:39:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   08 Dec 2010 08:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   16 Dec 2009 10:24:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   10 Aug 2005 13:26:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed GUI nits
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   22 Jun 2005 08:55:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented SUM, STDDEV, array value object
 * cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   16 Feb 2005 14:24:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   22 Apr 2003 09:45:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   19 Nov 2002 13:55:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   relates to how the equation classes connect and display information for Input/Output from the PPQ
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   31 Oct 2001 16:02:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  252
 * SCR Title:  System hang on .5 dual when accessing Math Tool after going online
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed member from CRichEditCtrl to CEdit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   01 Mar 2000 17:17:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  106
 * SCR Title:  Math Tool - List processing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added new toolbar for punctuaution.
 * Revised createCursorToolbar for revised cursor toolbar and
 * new punctuation toolbar.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   29 Feb 2000 14:32:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  106
 * SCR Title:  Math Tool - List processing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised size of math toolbars.
 * Revised selection of toolset output from a combobox to
 * a tree control selection.
 * Added statistics toolbar and handling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:39:18   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Oct 1999 17:39:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  48
 * SCR Title:  Threshold Page Formula
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use formulas for setting thresholds
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Sep 1999 17:04:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added validation check for equations in OnOk method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Sep 14 1999 15:27:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced Math Container.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Sep 1999 20:04:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin. new class for editing math equations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/