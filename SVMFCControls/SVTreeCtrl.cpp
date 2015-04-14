//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTreeCtrl
//* .File Name       : $Workfile:   SVTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 08:24:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVMFCControls
	{
		IMPLEMENT_DYNCREATE(SVTreeCtrlClass, CTreeCtrl)

			SVTreeCtrlClass::SVTreeCtrlClass()
			: CTreeCtrl()
		{
		}

		SVTreeCtrlClass::~SVTreeCtrlClass()
		{
		}

		/////////////////////////////////////////////////////////////////////////////
		//
		BOOL SVTreeCtrlClass::InitTreeCtrl()
		{
			StateImageList.Create( 16, 16, TRUE, 5, 5 );

			// State image index 0 is not used
			StateImageList.Add( AfxGetApp()->LoadIcon( IDI_ICON_NOTEXIST ) );

			// State image index 1 is 'Not Checked'
			StateImageList.Add( AfxGetApp()->LoadIcon( IDI_ICON_NOTEXIST ) );  

			// State image index 2 is 'Checked'
			StateImageList.Add( AfxGetApp()->LoadIcon( IDI_ICON_EXIST ) );

			SetImageList( &StateImageList, TVSIL_STATE );

			return TRUE;
		}

		/////////////////////////////////////////////////////////////////////////////
		//
		BOOL SVTreeCtrlClass::SetCheckState(HTREEITEM hItem, BOOL fCheck)
		{
			TVITEM tvItem;

			tvItem.mask = TVIF_HANDLE | TVIF_STATE;
			tvItem.hItem = hItem;
			tvItem.stateMask = TVIS_STATEIMAGEMASK;

			// Since state images are one-based, 
			// 1 in this macro turns the check off, and 2 turns it on.
			tvItem.state = INDEXTOSTATEIMAGEMASK((fCheck ? 2 : 1));

			return SetItem( &tvItem );
		}

		/////////////////////////////////////////////////////////////////////////////
		//
		BOOL SVTreeCtrlClass::GetCheckState(HTREEITEM hItem)
		{
			TVITEM tvItem;

			// Prepare to receive the desired information.
			tvItem.mask = TVIF_HANDLE | TVIF_STATE;
			tvItem.hItem = hItem;
			tvItem.stateMask = TVIS_STATEIMAGEMASK;

			// Request the information.
			GetItem( &tvItem );

			// Return zero if it's not checked, or nonzero otherwise.
			return ((BOOL)(tvItem.state >> 12) -1);
		}

		BEGIN_MESSAGE_MAP(SVTreeCtrlClass, CTreeCtrl)
			//{{AFX_MSG_MAP(SVTreeCtrlClass)
			ON_WM_LBUTTONDOWN()
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		/////////////////////////////////////////////////////////////////////////////
		// SVTreeCtrlClass message handlers

		/////////////////////////////////////////////////////////////////////////////
		//
		void SVTreeCtrlClass::OnLButtonDown(UINT nFlags, CPoint point) 
		{
			UINT uFlags;

			HTREEITEM hItem = HitTest(point,&uFlags);

			if (hItem)
			{
				DWORD dwItemData = static_cast<DWORD>(GetItemData(hItem));
				if( dwItemData )
				{
					BOOL bCheckState = GetCheckState(hItem);

					BOOL bNewCheckState = bCheckState ? FALSE : TRUE;

					SetCheckState(hItem,bNewCheckState);
				}
			}

			CTreeCtrl::OnLButtonDown(nFlags, point);
		}
	} //SVMFCControls
} //Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTreeCtrl.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 08:24:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 12:27:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Jun 2010 11:10:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 17:07:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Nov 2002 14:47:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed OLE Automation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 May 2000 09:32:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Implements a generic tree control for SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
