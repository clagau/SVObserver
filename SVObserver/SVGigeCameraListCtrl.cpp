//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraListCtrl
//* .File Name       : $Workfile:   SVGigeCameraListCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Jun 2013 15:47:44  $
//******************************************************************************

#include "stdafx.h"

#include "SVGigeCameraListCtrl.h"
#include "SVGigeCameraStructInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVGigeCameraListCtrl::SVGigeCameraListCtrl()
{
}

SVGigeCameraListCtrl::~SVGigeCameraListCtrl()
{
}

BEGIN_MESSAGE_MAP(SVGigeCameraListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(SVGigeCameraListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVGigeCameraListBox message handlers

void SVGigeCameraListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	int iIndex = lpDrawItemStruct->itemID;
	int i = 0;
	i++;
	if (iIndex > -1)
	{
		CDC dc;

		CString sTmp = _T( "<no device>" );

		dc.Attach(lpDrawItemStruct->hDC);

		COLORREF crOldTextColor = dc.GetTextColor();
		COLORREF crOldBkColor = dc.GetBkColor();
		CBrush tmpBrush;

		if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
			(lpDrawItemStruct->itemState & ODS_SELECTED))
		{
			dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHT));
			tmpBrush.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
		}
		else
		{
			tmpBrush.CreateSolidBrush(crOldBkColor);
		}

		dc.FrameRect(&lpDrawItemStruct->rcItem, &tmpBrush);
		CRect tmpRec = lpDrawItemStruct->rcItem;
		tmpRec.DeflateRect(1,1);
		dc.FrameRect (&tmpRec, &tmpBrush);

		COLORREF crText = RGB(0,0,0);

		if (lpDrawItemStruct->itemData >= 0)
		{
			SVGigeCameraStructInterface* pCamStruct = (SVGigeCameraStructInterface *)lpDrawItemStruct->itemData;

			if ( pCamStruct )
			{
				// Save these value to restore them when done drawing.
				switch (pCamStruct->eChangeType)
				{
					case SVGigeCameraStructInterface::SVNoChange:
					{
						dc.SetTextColor(RGB(0,0,0));
						crText = RGB(0,0,0);
						break;
					}

					case SVGigeCameraStructInterface::SVNewCamera:
					{
						dc.SetTextColor(RGB(0,165,0));
						crText = RGB(0,165,0);
						break;
					}


					case SVGigeCameraStructInterface::SVReplacedCamera:
					{
						dc.SetTextColor(RGB(0,0,200));
						crText = RGB(0,0,200);
						break;
					}

					case SVGigeCameraStructInterface::SVCameraRemoved:
					{
						dc.SetTextColor(RGB(240,0,0));
						crText = RGB(240,0,0);
						break;
					}
				}
			}
		} 
		else
		{
			dc.SetTextColor(RGB(0,0,0));
		}

		CRect rcLabel;
		CRect rcItem(lpDrawItemStruct->rcItem);
		GetItemRect(lpDrawItemStruct->itemID, rcItem, LVIR_LABEL);
		rcLabel = rcItem;
		rcLabel.DeflateRect(3, 0);
			
		CString strText = GetItemText(lpDrawItemStruct->itemID, 0);
		dc.SetTextColor(crText);
		dc.DrawText(strText, - 1, rcLabel, DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING | DT_END_ELLIPSIS);
		
		CRect rcClipBox;
		dc.GetClipBox(rcClipBox);

		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_WIDTH;
		for (int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++)
		{
			rcItem.left = rcItem.right;
			rcItem.right += lvc.cx;
			if (rcItem.left < rcClipBox.right && rcItem.right > rcClipBox.left && rcItem.right > rcItem.left)
			{
				CString strText= GetItemText(lpDrawItemStruct->itemID, nColumn);
				if (strText.IsEmpty())
					continue;

				UINT nJustify = DT_LEFT;
					
				rcLabel = rcItem;
				rcLabel.DeflateRect(6, 0);

				if (!strText.IsEmpty())
					dc.DrawText(strText, - 1, rcLabel, nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING | DT_END_ELLIPSIS);
			}
		}

		dc.SetTextColor(crOldTextColor);
		dc.SetBkColor(crOldBkColor);

		dc.Detach();
	}

}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVGigeCameraListCtrl.cpp_v  $
 * 
 *    Rev 1.0   12 Jun 2013 15:47:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 May 2013 10:19:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
