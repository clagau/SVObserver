// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDialog
// * .File Name       : $Workfile:   SVDialog.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:31:18  $
// ******************************************************************************

#include "stdafx.h"
#include "SVDialog.h"

BEGIN_MESSAGE_MAP(SVDialog, CDialog)
	//{{AFX_MSG_MAP(SVDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//##ModelId=38D12F6D00F1
SVDialog::SVDialog(UINT DlgId, CWnd *pParent)
	:	CDialog (DlgId, pParent)
{
	mhAccel = LoadAccelerators (AfxGetResourceHandle(), MAKEINTRESOURCE (DlgId));
}

//##ModelId=38D12F6D00F0
SVDialog::~SVDialog()
{
}

//##ModelId=38D12FE1032C
BOOL SVDialog::PreTranslateMessage(MSG* pMsg)
{
	if (mhAccel)
	{
		if (TranslateAccelerator (this->m_hWnd, mhAccel, pMsg))
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVLibrary\SVDialog.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:31:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Dec 2000 11:30:30   mike
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