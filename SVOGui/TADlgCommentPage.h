//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgCommentPage
//* .File Name       : $Workfile:   TADlgCommentPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   16 Sep 2014 09:36:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVOGuiUtility/DataController.h"
#pragma endregion Includes

namespace SvOg
{
	class TADlgCommentPage : public CPropertyPage
	{
	public:
		TADlgCommentPage(uint32_t inspectionId, uint32_t taskObjectId, bool isModuleComment = false);   // standard constructor

	public:
		virtual ~TADlgCommentPage();

		HRESULT SetInspectionData();


	// Dialog Data
		enum { IDD = IDD_TA_COMMENT_DIALOG };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		virtual BOOL OnInitDialog() override;
		afx_msg void OnEnKillfocusEditComment();
		afx_msg void OnSetFocusToEditComment();

		DECLARE_MESSAGE_MAP()

	private:
		CString m_strComment;

		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		bool m_isModuleComment;
		SvOgu::ValueController m_values;
	};
} //namespace SvOg

