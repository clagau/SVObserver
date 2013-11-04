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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCameraListBox.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:49:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Mar 2010 11:52:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove duplicate containers and update dialog information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Feb 2010 16:05:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed assert.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Jun 2009 11:01:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVCameraStructInterface
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 Jan 2004 09:57:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to make the model and vendor name available in the camera manager dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Nov 2003 09:36:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
