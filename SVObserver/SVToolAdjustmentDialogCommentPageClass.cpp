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
#include "SVToolAdjustmentDialogCommentPageClass.h"

#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

// SVToolAdjustmentDialogCommentPageClass dialog

//IMPLEMENT_DYNAMIC(SVToolAdjustmentDialogCommentPageClass, CDialog)

SVToolAdjustmentDialogCommentPageClass::SVToolAdjustmentDialogCommentPageClass( SVToolAdjustmentDialogSheetClass* PSheet ) : CPropertyPage(SVToolAdjustmentDialogCommentPageClass::IDD)
, m_strComment(_T(""))
, pTool(nullptr)
{
	pSheet = PSheet;

}

SVToolAdjustmentDialogCommentPageClass::~SVToolAdjustmentDialogCommentPageClass()
{
}

void SVToolAdjustmentDialogCommentPageClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_strComment);
}


BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogCommentPageClass, CDialog)
	ON_EN_KILLFOCUS(IDC_EDIT_COMMENT, &SVToolAdjustmentDialogCommentPageClass::OnEnKillfocusEditComment)
	ON_EN_SETFOCUS(IDC_EDIT_COMMENT, &SVToolAdjustmentDialogCommentPageClass::OnSetFocusToEditComment)
END_MESSAGE_MAP()


BOOL SVToolAdjustmentDialogCommentPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if( pSheet && ( pTool = pSheet->GetTool() ) )
	{
		SetTaskObject( pTool );
		SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pTool->GetToolComment());
		if( nullptr != pValueObject )
		{
			SVString Value;
			pValueObject->getValue( Value );
			m_strComment = Value.c_str();
		}
		UpdateData(FALSE);
	}
	return TRUE;
}


HRESULT SVToolAdjustmentDialogCommentPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	UpdateData( TRUE ); // get data from dialog


	l_hrOk = AddInputRequest( SVObjectReference( pTool->GetToolComment() ), _variant_t(m_strComment) );
	
	if( S_OK == l_hrOk )
	{
		l_hrOk = AddInputRequestMarker();
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = RunOnce( pTool->GetUniqueObjectID() );
	}

	UpdateData( FALSE );

	return l_hrOk;
}

// SVToolAdjustmentDialogCommentPageClass message handlers

void SVToolAdjustmentDialogCommentPageClass::OnEnKillfocusEditComment()
{
	SetInspectionData();
}

void SVToolAdjustmentDialogCommentPageClass::OnSetFocusToEditComment()
{
	CEdit *editControl = static_cast<CEdit*>(GetDlgItem(IDC_EDIT_COMMENT));
	editControl->SetSel(0,0);
	UpdateData(FALSE);
}

