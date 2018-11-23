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
#include "SVObjectLibrary/SVOutObjectInfoStruct.h"
#include "SVMFCControls/DlgItemResizer.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvOg
{
	class SVShowDependentsDialog : public CDialog
	{
	#pragma region Declarations
	public:
		enum { IDD = IDD_SHOW_DEPENDENTS_DIALOG };

		enum DialogType
		{
			DeleteConfirm,
			Show,
		};
	#pragma endregion Declarations

	#pragma region Constructor
	public:
		SVShowDependentsDialog( const SVGuidSet& rSourceSet, SvPb::SVObjectTypeEnum objectType = SvPb::SVToolObjectType, LPCTSTR DisplayText = nullptr, DialogType Type = DeleteConfirm, CWnd* pParent = nullptr );
	#pragma endregion Constructor

	public:
		static INT_PTR StandardDialog(const std::string& rName, const SVGUID rTaskObjectID);
	
	protected:
		virtual BOOL OnInitDialog() override;
		virtual void DoDataExchange(CDataExchange* pDX) override;
		afx_msg void OnPaint();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
		afx_msg void OnCustomdrawDependencyList(NMHDR* pNMHDR, LRESULT* pResult);

		//! Sets which controls are resized when the dialog is resized
		void setResizeControls();

		void RetreiveList();
		void addColumnHeadings();
		void addItems();
		void setColumnWidths();

		DECLARE_MESSAGE_MAP()

	private:
		SvMc::DlgItemResizer m_Resizer;
		CRect m_OrginalClient;
		CRect m_Gripper;
		CListCtrl m_ListCtrl;
		const SVGuidSet& m_rSourceSet;
		SvPb::SVObjectTypeEnum m_objectType;
		SvDef::StringPairVector m_dependencyList;
		std::string m_DisplayText;
		DialogType m_DialogType;
		SvDef::StringSet m_SourceNames;
	};
} //namespace SvOg
