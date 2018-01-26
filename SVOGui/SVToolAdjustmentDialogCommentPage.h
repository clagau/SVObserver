//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogCommentPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogCommentPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   16 Sep 2014 09:36:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOg
{
	class SVToolAdjustmentDialogCommentPage : public CPropertyPage
	{
	public:
		SVToolAdjustmentDialogCommentPage(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID);   // standard constructor

	public:
		virtual ~SVToolAdjustmentDialogCommentPage();

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

		const SVGUID& m_rInspectionID;
		const SVGUID& m_rTaskObjectID;
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;
		Controller m_Values;
	};
} //namespace SvOg

