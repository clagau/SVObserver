//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#pragma endregion Includes

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogCommentPage, CPropertyPage)
		ON_EN_KILLFOCUS(IDC_EDIT_COMMENT, &SVToolAdjustmentDialogCommentPage::OnEnKillfocusEditComment)
		ON_EN_SETFOCUS(IDC_EDIT_COMMENT, &SVToolAdjustmentDialogCommentPage::OnSetFocusToEditComment)
	END_MESSAGE_MAP()

	SVToolAdjustmentDialogCommentPage::SVToolAdjustmentDialogCommentPage(uint32_t inspectionId, uint32_t taskObjectId) : CPropertyPage(SVToolAdjustmentDialogCommentPage::IDD)
	, m_values{ SvOg::BoundValues{ inspectionId, taskObjectId } }
	, m_InspectionID{ inspectionId }
	, m_TaskObjectID{ taskObjectId }
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

		m_values.Init();

		m_strComment = m_values.Get<CString>(SvPb::ToolCommentTypeEId);

		UpdateData(false);

		return TRUE;
	}


	HRESULT SVToolAdjustmentDialogCommentPage::SetInspectionData()
	{
		HRESULT Result{ E_FAIL };

		UpdateData(true); // get data from dialog

		m_values.Set<CString>(SvPb::ToolCommentTypeEId, m_strComment);
		m_values.Commit();

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
