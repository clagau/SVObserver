//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShowDependentsDialog
//* .File Name       : $Workfile:   SVShowDependentsDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:06:40  $
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVOResource/resource.h"
#include "SVObjectLibrary\SVOutObjectInfoStruct.h"
#include "ObjectInterfaces\DependencyList.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class SVShowDependentsDialog : public CDialog
		{
		#pragma region Declarations
		public:
			enum DialogType
			{
				DeleteConfirm,
				DeleteConfirmWithIP_Name,
				Show,
				ShowWithIP_Name,
			};
		#pragma endregion Declarations

		#pragma region Constructor
		public:
			SVShowDependentsDialog( const SVObjectPairVector& rList, LPCTSTR DisplayText, DialogType Type = DeleteConfirm, CWnd* pParent = nullptr ); // old
			SVShowDependentsDialog( const GUID& rInspectionId, const GUID& rTaskObjectID, bool bOnlyImages, SVObjectTypeEnum objectType = SVToolObjectType, LPCTSTR DisplayText = nullptr, DialogType Type = DeleteConfirm, CWnd* pParent = nullptr );

		#pragma endregion Constructor
			// Dialog Data
			//{{AFX_DATA(SVShowDependentsDialog)
			enum { IDD = IDD_SHOW_DEPENDENTS_DIALOG };
			CListCtrl	listCtrl;
			//}}AFX_DATA

		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVShowDependentsDialog)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
			//}}AFX_VIRTUAL

		public:
			static INT_PTR StandardDialog( const SVString& rName, const SVGUID rInspectionID, const SVGUID rTaskObjectID, bool OnlyImages=false );

		// Implementation
		protected:
			void RetreiveList();
			void ConvertList(const SVObjectPairVector& rList);
			void addColumnHeadings();
			void addItems();
			void setColumnWidths();

			// Generated message map functions
			//{{AFX_MSG(SVShowDependentsDialog)
			virtual BOOL OnInitDialog() override;
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

		private:
			GUID m_InspectionID;
			GUID m_TaskObjectID;
			bool m_bOnlyImages;
			SVObjectTypeEnum m_objectType;
			SvOi::DependencyList m_dependencyList;
			SVString m_DisplayText;
			DialogType m_DialogType;
		};
	}
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

