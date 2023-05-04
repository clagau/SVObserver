//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgCommentPage
//* .File Name       : $Workfile:   TADlgCommentPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Sep 2014 09:36:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgCommentPage.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVOGuiUtility/BoundValue.h"
#pragma endregion Includes

namespace SvOg
{
	BEGIN_MESSAGE_MAP(TADlgCommentPage, CPropertyPage)
		ON_EN_KILLFOCUS(IDC_EDIT_COMMENT, &TADlgCommentPage::OnEnKillfocusEditComment)
		ON_EN_SETFOCUS(IDC_EDIT_COMMENT, &TADlgCommentPage::OnSetFocusToEditComment)
	END_MESSAGE_MAP()

	TADlgCommentPage::TADlgCommentPage(uint32_t inspectionId, uint32_t taskObjectId, bool isModuleComment) 
	: CPropertyPage(TADlgCommentPage::IDD, isModuleComment ? IDS_OBJECTNAME_MODULE_COMMENT : IDS_OBJECTNAME_COMMENT)
	, m_InspectionID{ inspectionId }
	, m_TaskObjectID{ taskObjectId }
	, m_isModuleComment{isModuleComment}
	, m_values(SvOgu::BoundValues(inspectionId, taskObjectId))
	{
	}

	TADlgCommentPage::~TADlgCommentPage()
	{
	}

	void TADlgCommentPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_EDIT_COMMENT, m_strComment);
	}

	BOOL TADlgCommentPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		if (m_isModuleComment)
		{
			SetHelpID(IDD_MODULE_COMMENT);
			static_cast<CEdit*>(GetDlgItem(IDC_EDIT_COMMENT))->SetReadOnly(true);
			m_values.Init();
			m_strComment = m_values.Get<CString>(SvPb::ModuleCommentEId);
		}
		else
		{
			m_strComment = SvCmd::getComment(m_InspectionID, m_TaskObjectID).c_str();
		}

		UpdateData(false);

		return TRUE;
	}


	HRESULT TADlgCommentPage::SetInspectionData()
	{
		HRESULT Result{ E_FAIL };

		UpdateData(true); // get data from dialog

		if (false == m_isModuleComment)
		{
			SvCmd::setComment(m_InspectionID, m_TaskObjectID, std::string {m_strComment});
		}

		return Result;
	}

	// TADlgCommentPage message handlers

	void TADlgCommentPage::OnEnKillfocusEditComment()
	{
		SetInspectionData();
	}

	void TADlgCommentPage::OnSetFocusToEditComment()
	{
		CEdit *editControl = static_cast<CEdit*>(GetDlgItem(IDC_EDIT_COMMENT));
		editControl->SetSel(0,0);
		UpdateData(FALSE);
	}
} //namespace SvOg
