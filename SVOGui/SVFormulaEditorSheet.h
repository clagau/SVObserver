//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVOGuiUtility/SVFormulaEditorPage.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#pragma endregion Includes

namespace SvDef
{
struct SVObjectTypeInfoStruct;
}


namespace SvOg
{
	class SVFormulaEditorSheetClass : public CPropertySheet
	{
	public:
		SVFormulaEditorSheetClass(uint32_t inspectionId, uint32_t taskObjectId, const SvDef::SVObjectTypeInfoStruct& rInfo, LPCTSTR pCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
		SVFormulaEditorSheetClass(uint32_t inspectionId, uint32_t taskObjectId, uint32_t equationId, LPCTSTR pCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
		SVFormulaEditorSheetClass(SvOi::IFormulaControllerPtr formulaController, LPCTSTR pCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

		virtual ~SVFormulaEditorSheetClass();

		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVFormulaEditorSheetClass)
		public:
		virtual BOOL OnInitDialog() override;
		//}}AFX_VIRTUAL

	protected:
		void init(SvOi::IFormulaControllerPtr formulaController);

		//{{AFX_MSG(SVFormulaEditorSheetClass)
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		afx_msg void OnOK();

		typedef std::shared_ptr<SvOgu::SVFormulaEditorPageClass> FormulaEditorPagePtr;
		FormulaEditorPagePtr m_formulaPage;
	};
} //namespace SvOg
