//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogCommentPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogCommentPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Sep 2014 09:36:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAdjustmentDialogCommentPage.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVOGuiUtility/BoundValue.h"
#pragma endregion Includes

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogCommentPage, CPropertyPage)
		ON_EN_KILLFOCUS(IDC_EDIT_COMMENT, &SVToolAdjustmentDialogCommentPage::OnEnKillfocusEditComment)
		ON_EN_SETFOCUS(IDC_EDIT_COMMENT, &SVToolAdjustmentDialogCommentPage::OnSetFocusToEditComment)
	END_MESSAGE_MAP()

	SVToolAdjustmentDialogCommentPage::SVToolAdjustmentDialogCommentPage(uint32_t inspectionId, uint32_t taskObjectId, bool isModuleComment) 
	: CPropertyPage(SVToolAdjustmentDialogCommentPage::IDD, isModuleComment ? IDS_OBJECTNAME_MODULE_COMMENT : IDS_OBJECTNAME_COMMENT)
	, m_InspectionID{ inspectionId }
	, m_TaskObjectID{ taskObjectId }
	, m_isModuleComment{isModuleComment}
	, m_values(SvOgu::BoundValues(inspectionId, taskObjectId))
	{
	}

	SVToolAdjustmentDialogCommentPage::~SVToolAdjustmentDialogCommentPage()
	{
	}

	void SVToolAdjustmentDialogCommentPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_EDIT_COMMENT, m_strComment);
	}

	BOOL SVToolAdjustmentDialogCommentPage::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();

		if (m_isModuleComment)
		{
			SetHelpID(IDD_MODULE_COMMENT);
			GetDlgItem(IDC_EDIT_COMMENT)->EnableWindow(false);
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


	HRESULT SVToolAdjustmentDialogCommentPage::SetInspectionData()
	{
		HRESULT Result{ E_FAIL };

		UpdateData(true); // get data from dialog

		if (false == m_isModuleComment)
		{
			SvCmd::setComment(m_InspectionID, m_TaskObjectID, std::string {m_strComment});
		}

		return Result;
	}

	// SVToolAdjustmentDialogCommentPageClass message handlers

	void SVToolAdjustmentDialogCommentPage::OnEnKillfocusEditComment()
	{
		SetInspectionData();
	}

	void SVToolAdjustmentDialogCommentPage::OnSetFocusToEditComment()
	{
		CEdit *editControl = static_cast<CEdit*>(GetDlgItem(IDC_EDIT_COMMENT));
		editControl->SetSel(0,0);
		UpdateData(FALSE);
	}
} //namespace SvOg
