//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraListBox
//* .File Name       : $Workfile:   SVCameraListBox.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:49:42  $
//******************************************************************************

#include "stdafx.h"

#include "SVCameraListBox.h"
#include "SVCameraStructInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVCameraListBox::SVCameraListBox()
{
}

SVCameraListBox::~SVCameraListBox()
{
}

BEGIN_MESSAGE_MAP(SVCameraListBox, CListBox)
	//{{AFX_MSG_MAP(SVCameraListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVCameraListBox message handlers

void SVCameraListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	int iIndex = lpDrawItemStruct->itemID;
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

		if (lpDrawItemStruct->itemData)
		{
			SVCameraStructInterface* pCamStruct = (SVCameraStructInterface *)lpDrawItemStruct->itemData;

			// Save these value to restore them when done drawing.
			switch (pCamStruct->eChangeType)
			{
			case SVCameraStructInterface::SVNoChange:
				dc.SetTextColor(RGB(0,0,0));
				break;

			case SVCameraStructInterface::SVNewCamera:
				dc.SetTextColor(RGB(0,165,0));
				break;

			case SVCameraStructInterface::SVReplacedCamera:
				dc.SetTextColor(RGB(0,0,200));
				break;

			case SVCameraStructInterface::SVCameraRemoved:
				dc.SetTextColor(RGB(240,0,0));
				break;
			}

			// Draw the text.
			if( pCamStruct->HasSerialNumber() )
			{
				if( pCamStruct->HasModelName() )
				{
					sTmp = pCamStruct->GetSerialNumber();
					sTmp += (_T(" (") + pCamStruct->GetModelName() + _T(")"));
				}
				else
				{
					sTmp = _T( "<no device> " );
					sTmp += pCamStruct->GetSerialNumber();
				}
			}
			else
			{
				sTmp = _T( "<no device>" );
			}
		}
		else
		{
			dc.SetTextColor(RGB(0,0,0));
		}

		CRect rect(lpDrawItemStruct->rcItem);
		rect.left += 4;
		dc.DrawText(
			sTmp,
			sTmp.GetLength(),
			&rect,
			DT_LEFT|DT_SINGLELINE|DT_VCENTER);

		// Reset the background color and the text color back to their
		// original values.
		dc.SetTextColor(crOldTextColor);
		dc.SetBkColor(crOldBkColor);

		dc.Detach();
	}
}

