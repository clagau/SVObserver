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
#include "SVTaskObjectInterfaceClass.h"

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;



// SVToolAdjustmentDialogCommentPageClass dialog

class SVToolAdjustmentDialogCommentPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
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

	virtual HRESULT SetInspectionData();


// Dialog Data
	enum { IDD = IDD_TA_COMMENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	BOOL OnInitDialog() ;

	DECLARE_MESSAGE_MAP()

	SVToolAdjustmentDialogSheetClass*	pSheet;
	SVToolClass*						pTool;

public:
	CString m_strComment;
	afx_msg void OnEnKillfocusEditComment();
	afx_msg void OnSetFocusToEditComment();
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolAdjustmentDialogCommentPageClass.h_v  $
 * 
 *    Rev 1.1   16 Sep 2014 09:36:04   mziegler
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
 *    Rev 1.0   23 Apr 2013 15:33:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Jul 2012 14:59:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
