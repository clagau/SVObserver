//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMFCControls/DlgItemResizer.h"
#include "Definitions/StringTypeDef.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVProtoBuf/SVO-Enum.h"
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
		explicit SVShowDependentsDialog( const std::set<uint32_t>& rIdsOfObjectsDependedOn, SvPb::SVObjectTypeEnum objectType = SvPb::SVToolObjectType, LPCTSTR DisplayText = nullptr, DialogType Type = DeleteConfirm, CWnd* pParent = nullptr);
		explicit SVShowDependentsDialog(SvDef::StringPairVector dependencyList, LPCTSTR DisplayText = nullptr, DialogType Type = Show, CWnd* pParent = nullptr);
	#pragma endregion Constructor

	protected:
		virtual BOOL OnInitDialog() override;
		virtual void DoDataExchange(CDataExchange* pDX) override;
		afx_msg void OnPaint();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
		afx_msg void OnCustomdrawDependencyList(NMHDR* pNMHDR, LRESULT* pResult);

		//! Sets which controls are resized when the dialog is resized
		void setResizeControls();

		void FillDependencyList(const std::set<uint32_t>& m_rIdsOfObjectsDependedOn, SvPb::SVObjectTypeEnum objectType);
		void addColumnHeadings();
		void addItems();
		void setColumnWidths();

		DECLARE_MESSAGE_MAP()

	private:
		SvMc::DlgItemResizer m_Resizer;
		CRect m_OrginalClient;
		CRect m_Gripper;
		CListCtrl m_ListCtrl;
		SvDef::StringPairVector m_dependencyList;
		std::string m_DisplayText;
		DialogType m_DialogType;
		SvDef::StringSet m_SourceNames;
		SvPb::GetDependencyResponse m_dependencyResponse;
	};

	INT_PTR showDependentsDialogIfNecessary(const std::set<uint32_t>& rTaskIds, const std::string& rDisplayString);
} //namespace SvOg
