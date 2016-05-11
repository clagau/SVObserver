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
#include "SVFormulaEditorPage.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"
#pragma endregion Includes

namespace Seidenader
{ 
	namespace SVOGui
	{
		class SVFormulaEditorSheetClass : public CPropertySheet
		{
		public:
			SVFormulaEditorSheetClass(const GUID& rInspectionID, const GUID& rTaskOjectID, const SVObjectTypeInfoStruct& rInfo, UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
			SVFormulaEditorSheetClass(const GUID& rInspectionID, const GUID& rTaskOjectID, const SVObjectTypeInfoStruct& rInfo, LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

			virtual ~SVFormulaEditorSheetClass();

			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVFormulaEditorSheetClass)
			public:
			virtual BOOL OnInitDialog();
			//}}AFX_VIRTUAL

		protected:
			void init();

			//{{AFX_MSG(SVFormulaEditorSheetClass)
			afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

		protected:
			afx_msg void OnOK();

			typedef SVSharedPtr<SVFormulaEditorPageClass> FormulaEditorPagePtr;
			FormulaEditorPagePtr m_formulaPage;
			GUID m_InspectionID;
			GUID m_TaskObjectID;
			SVObjectTypeInfoStruct m_info;
		};
	}
}

namespace SvOg = Seidenader::SVOGui;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

