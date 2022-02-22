//*****************************************************************************
/// \copyright COPYRIGHT (c) 2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SelectToolForNewSourceImageDialog.h
/// All Rights Reserved
//*****************************************************************************
/// A dialog to select a tool for a new SourceImage.
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVUtilityLibrary/NameObjectIdList.h"
#pragma endregion Includes

namespace SvOg
{
	class SelectToolForNewSourceImageDialog : public CDialog
	{
		// Construction
	public:
		explicit SelectToolForNewSourceImageDialog(uint32_t inspectionId, SvPb::GetToolsWithReplaceableSourceImageResponse list, CWnd* pParent = nullptr);   // standard constructor
		virtual ~SelectToolForNewSourceImageDialog();

		SvUl::NameObjectIdPair getSelectedToolNameAndId() const { return m_selectedToolNameAndId;};

		// Dialog Data
		//{{AFX_DATA(SelectToolForNewSourceImageDialog)
	public:
		enum { IDD = IDD_DLG_SELECT_TOOL };
		CComboBox	m_toolList;
		//}}AFX_DATA

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SelectToolForNewSourceImageDialog)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

		// Generated message map functions
		//{{AFX_MSG(SelectToolForNewSourceImageDialog)
	protected:
		virtual BOOL OnInitDialog() override;
		virtual void OnOK() override;
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		const uint32_t m_InspectionID;
		SvPb::GetToolsWithReplaceableSourceImageResponse m_list;
		SvUl::NameObjectIdPair m_selectedToolNameAndId{};
	};

	SvPb::GetToolsWithReplaceableSourceImageResponse getToolsWithReplaceableSourceImage(uint32_t inspectionId);
} //namespace SvOg
