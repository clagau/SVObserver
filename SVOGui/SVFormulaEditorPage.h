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

namespace SvOg
{
	class SVFormulaEditorPageClass : public CPropertyPage
	{
	public:
		explicit SVFormulaEditorPageClass(SvOi::IFormulaControllerPtr controller, bool isDisableCheckboxesVisible = false, UINT captionID = IDS_FORMULA_STRING, UINT disableExtentionID = IDS_TOOL_STRING);   // standard constructor
		virtual ~SVFormulaEditorPageClass();

		bool validateAndSetEquation();

		// Dialog Data
		//{{AFX_DATA(SVFormulaEditorPageClass)
		enum { IDD = IDD_FORMULA_DIALOG };
		CEdit	m_ConstantEditCtrl;
		CButton	m_AddLocalVariableCtrl;
		CButton	m_AddConstantCtrl;
		CWnd m_EditWnd;
		CString	m_ConstantValue;
		int		m_constantType;
		CButton m_decimalRadioButton;
		CButton m_hexadecimalRadioButton;
		CButton m_binaryRadioButton;
		CString	m_ToolsetOutputVariable;
		CButton	m_ToolsetOutputSelectButton;
		CButton m_DisableEquationCtrl;
		CButton m_DisableToolCtrl;
		CButton m_EditModeFreezeCtrl;
		BOOL	m_equationDisabled{ false };
		BOOL	m_ownerDisabled{ false };
		BOOL	m_editModeFreezeFlag{ false };
		//}}AFX_DATA

	protected:
		void backspaceInEditor( long Pos );
		void insertIntoEditor( LPCTSTR tszValue );
		void advanceInEditor( long Pos );
		void deleteInEditor( long Pos );
		std::string getEquationText() const;
		std::string getSelectedEquationText() const;
		void setEquationText();
		bool createToolbars();

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
		afx_msg void OnAddLocalVariableButton();
		afx_msg void OnAddConstantButton();
		virtual BOOL OnInitDialog() override;
		afx_msg void OnLocalVariableSelect();
		afx_msg void OnDisable();
		afx_msg void OnEditFreezeFlag();
		afx_msg void OnEquationFieldChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
		virtual BOOL OnKillActive( ) override;
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		void createScintillaControl();
		bool IsBrace( int c );
		void CheckBrace();

	protected:
		SvMc::SVToolBar m_logicalOperatorBar;
		SvMc::SVToolBar m_functionOperatorBar;
		SvMc::SVToolBar m_conditionalOperatorBar;
		SvMc::SVToolBar m_trigOperatorBar;
		SvMc::SVToolBar m_miscOperatorBar;
		SvMc::SVToolBar m_basicOperatorBar;
		SvMc::SVToolBar m_cursorBar;
		SvMc::SVToolBar m_validateBar;
		SvMc::SVToolBar m_punctuationBar;
		SvMc::SVToolBar m_statisticsOperatorBar;

	private:
		SvOi::IFormulaControllerPtr m_FormulaController;
		bool m_isConditionalPage;
		UINT m_disableExtentionID;
		CBitmap m_downArrowBitmap;
	};
} //namespace SvOg
