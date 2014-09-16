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

#include "stdafx.h"
#include "SVToolAdjustmentDialogCommentPageClass.h"

#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"


// SVToolAdjustmentDialogCommentPageClass dialog

//IMPLEMENT_DYNAMIC(SVToolAdjustmentDialogCommentPageClass, CDialog)

SVToolAdjustmentDialogCommentPageClass::SVToolAdjustmentDialogCommentPageClass( SVToolAdjustmentDialogSheetClass* PSheet ) : CPropertyPage(SVToolAdjustmentDialogCommentPageClass::IDD)
, m_strComment(_T(""))
, pTool(NULL)
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

	//SVToolClass* pTool = NULL;
	if( pSheet && ( pTool = pSheet->GetTool() ) )
	{
		SetTaskObject( pTool );
		SVValueObjectClass* l_psvComment = pTool->GetToolComment();
		if( l_psvComment )
		{
			CString l_strValue;
			l_psvComment->GetValue( l_strValue );
			m_strComment = l_strValue;
		}
		UpdateData(FALSE);
	}
	return TRUE;
}


HRESULT SVToolAdjustmentDialogCommentPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	UpdateData( TRUE ); // get data from dialog


	l_hrOk = AddInputRequest( pTool->GetToolComment(), m_strComment );
	
	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequestMarker();
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = RunOnce( pTool );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolAdjustmentDialogCommentPageClass.cpp_v  $
 * 
 *    Rev 1.2   16 Sep 2014 09:36:06   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  927
 * SCR Title:  Improve comment function
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   update the comment value in rc-file to have the same size and font.
 * Add method OnSetFocusToEditComment to remove the highlight of the text if the comment will open.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Nov 2013 07:29:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed tool comment from SVStringValueObjectClass to SVStaticStringValueObjectClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:33:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Jul 2012 14:59:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
