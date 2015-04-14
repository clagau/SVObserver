//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAdjustToolSizePositionDlg
//* .File Name       : $Workfile:   SVUpDownButton.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:56:00  $
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVMFCControls
	{
		enum
		{
			WM_SV_NOTIFY_LBUTTONDOWN = WM_APP + 200,
			WM_SV_NOTIFY_LBUTTONUP   = WM_APP + 201,
		};

		class SVUpDownButton : public CButton
		{
		public:
			SVUpDownButton();
			afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
			afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
			afx_msg void OnKillFocus( CWnd* pNewWnd );

		protected:
			bool m_bLButtonDown;

			DECLARE_MESSAGE_MAP()
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVUpDownButton.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:56:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Feb 2005 12:12:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding new files to implement new extent functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/