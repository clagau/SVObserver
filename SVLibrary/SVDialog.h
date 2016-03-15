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

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_SVDIALOG_38D12F6D00BB_INCLUDED
#define _INC_SVDIALOG_38D12F6D00BB_INCLUDED

//SVDialog is the base class for dialogs using accelerators. 
//The constructor loads the accelerator table and a function 
//is provided to handle the WM_KEYDOWN messages. Note that 
//WM_SYSKEYDOWN is not handled.
//##ModelId=38D12F6D00BB
class SVDialog 
: public CDialog
{
public:
	//##ModelId=38D12F6D00F1
	SVDialog(UINT DlgId, CWnd *pParent);

	//##ModelId=38D12F6D00F0
	virtual ~SVDialog();

	//{{AFX_VIRTUAL(SVDialog)
	public:
	//##ModelId=38D12FE1032C
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(SVDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	//##ModelId=38D12F6D00EB
	HACCEL mhAccel;
};

#endif /* _INC_SVDIALOG_38D10E550177_INCLUDED */

