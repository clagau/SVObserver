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
#include "InspectionEngine/SVTaskObjectValueInterface.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;



// SVToolAdjustmentDialogCommentPageClass dialog

class SVToolAdjustmentDialogCommentPageClass : public CPropertyPage, public SVTaskObjectValueInterface
{
//	DECLARE_DYNAMIC(SVToolAdjustmentDialogCommentPageClass)
//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVToolAdjustmentDialogCommentPageClass(SVToolAdjustmentDialogSheetClass* PSheet);   // standard constructor

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVToolAdjustmentDialogCommentPageClass();

	HRESULT SetInspectionData();


// Dialog Data
	enum { IDD = IDD_TA_COMMENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	virtual BOOL OnInitDialog() override;

	DECLARE_MESSAGE_MAP()

	SVToolAdjustmentDialogSheetClass*	pSheet;
	SVToolClass*						pTool;

public:
	CString m_strComment;
	afx_msg void OnEnKillfocusEditComment();
	afx_msg void OnSetFocusToEditComment();
};

