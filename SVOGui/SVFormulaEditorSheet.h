//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFormulaEditorSheet
//* .File Name       : $Workfile:   SVFormulaEditorSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Jul 2014 19:11:04  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#include "SVFormulaEditorPage.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#pragma endregion Includes

namespace SvDef
{
struct SVObjectTypeInfoStruct;
}

class SVGUID;

namespace SvOg
{
	class SVFormulaEditorSheetClass : public CPropertySheet
	{
	public:
		SVFormulaEditorSheetClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SvDef::SVObjectTypeInfoStruct& rInfo, LPCTSTR pCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
		SVFormulaEditorSheetClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SVGUID& rEquationID, LPCTSTR pCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
		SVFormulaEditorSheetClass(SvOi::IFormulaControllerPtr formulaController, LPCTSTR pCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

		virtual ~SVFormulaEditorSheetClass();

		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVFormulaEditorSheetClass)
		public:
		virtual BOOL OnInitDialog() override;
		//}}AFX_VIRTUAL

	protected:
		/// Initialized the formulaEditorSheetClass 
		/// \param rInfo [in] info struct of the equation
		void init(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SvDef::SVObjectTypeInfoStruct& rInfo);

		void init(SvOi::IFormulaControllerPtr formulaController, bool isConditional=false);

		//{{AFX_MSG(SVFormulaEditorSheetClass)
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		afx_msg void OnOK();

		typedef std::shared_ptr<SVFormulaEditorPageClass> FormulaEditorPagePtr;
		FormulaEditorPagePtr m_formulaPage;
	};
} //namespace SvOg
