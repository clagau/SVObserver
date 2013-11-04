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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVLibrary\SVDialog.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:31:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Dec 2000 11:30:26   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  55
 * SCR Title:  SVFocus Remote control from PLC
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   No changes were made.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:28:04   mike
 * Initial revision.
*/