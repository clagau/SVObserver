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
#include "SVOResource/resource.h"
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

			void HandleValidateError( int posFailed );

			void onValidate();
			void onUndo();

			void enableControls();
			void enableUndoButton();

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVFormulaEditorPageClass)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
			virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
			//}}AFX_VIRTUAL

			// Generated message map functions
			//{{AFX_MSG(SVFormulaEditorPageClass)
			afx_msg void OnHelp();
			afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
			afx_msg void OnAddLocalVariableButton();
			afx_msg void OnAddConstantButton();
			virtual BOOL OnInitDialog() override;
			afx_msg void OnLocalVariableSelect();
			afx_msg void OnDisable();
			afx_msg void OnEquationFieldChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
			virtual BOOL OnKillActive( ) override;
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

