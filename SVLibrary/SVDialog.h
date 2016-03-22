// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDialog
// * .File Name       : $Workfile:   SVDialog.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:31:22  $
// ******************************************************************************

#pragma once

//SVDialog is the base class for dialogs using accelerators. 
//The constructor loads the accelerator table and a function 
//is provided to handle the WM_KEYDOWN messages. Note that 
//WM_SYSKEYDOWN is not handled.
class SVDialog 
: public CDialog
{
public:
	SVDialog(UINT DlgId, CWnd *pParent);

	virtual ~SVDialog();

	//{{AFX_VIRTUAL(SVDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(SVDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HACCEL mhAccel;
};
