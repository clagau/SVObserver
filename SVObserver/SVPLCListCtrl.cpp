//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCListCtrl
//* .File Name       : $Workfile:   SVPLCListCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:21:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVPLCListCtrl.h"
#include "SVMFCControls\SVInPlaceEdit.h"

#define IDC_EDITCTRL 0x1234

SVPLCListCtrl::SVPLCListCtrl(void)
{
}

SVPLCListCtrl::~SVPLCListCtrl(void)
{
}

void SVPLCListCtrl::OnEditItem(int iItem, int iSubItem, CPoint point, UINT nChar)
{
	CRect rect;
	int offset = 0;
	if (!EnsureVisible(iItem, TRUE)) 
		return;
	
#if _MSC_VER >= 1200
	GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, rect);
#else
	for (int i = 0; i < iSubItem; i++)
		offset += GetColumnWidth(i);
	GetItemRect(iItem, &rect, LVIR_BOUNDS);
#endif
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(rcClient);
	if (offset + rect.left < 0 || offset + rect.left > rcClient.right)
	{
		CSize size(offset + rect.left, 0);		
		Scroll(size);
		rect.left -= size.cx;
	}
	rect.left += offset;	
	rect.right = rect.left + GetColumnWidth(iSubItem);
	if (rect.right > rcClient.right) 
		rect.right = rcClient.right;
	
	// Get Column alignment	
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn(iSubItem, &lvcol);
		
	DWORD dwStyle;
	if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else
		dwStyle = ES_CENTER;	
		
	dwStyle |= ES_AUTOHSCROLL;
	rect.DeflateRect(1, 1, 1, 1);
	CEdit *pEdit = new SvMc::SVInPlaceEdit(this, rect, dwStyle, IDC_EDITCTRL, iItem, iSubItem, GetItemText(iItem, iSubItem), nChar, FALSE);
}	
	
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCListCtrl.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:21:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 14:06:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/