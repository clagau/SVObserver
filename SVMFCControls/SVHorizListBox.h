//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : HLB
//* .File Name       : $Workfile:   Hlb.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 Aug 2014 15:21:14  $
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVMFCControls
	{
		class SVHorizListBox : public CListBox
		{
		public:
			SVHorizListBox();
			virtual ~SVHorizListBox();

		protected:
			int CalcHorizExtent(CDC* pDC, LPCTSTR text) const;
			void ResetHorizExtent();
			void CalcNewHorizExtent(LPCTSTR text);

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVHorizListBox)
			//}}AFX_VIRTUAL

			// Generated message map functions
		protected:
			//{{AFX_MSG(SVHorizListBox)
			//}}AFX_MSG
			afx_msg LRESULT OnAddString(WPARAM wParam, LPARAM lParam);
			afx_msg LRESULT OnInsertString(WPARAM wParam, LPARAM lParam);
			afx_msg LRESULT OnDeleteString(WPARAM wParam, LPARAM lParam);
			afx_msg LRESULT OnSetTabStops(WPARAM wParam, LPARAM lParam);
			afx_msg LRESULT OnResetContent(WPARAM wParam, LPARAM lParam);
			DECLARE_MESSAGE_MAP()
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\Hlb.h_v  $
 * 
 *    Rev 1.1   15 Aug 2014 15:21:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised to calculate the exent for the list box horizontal scroll bar.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:13:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Nov 2010 16:09:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Apr 2003 15:51:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2001 22:07:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  213
 * SCR Title:  Upgrade Sapera Libraries from 3.00 to 3.11
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Used to diagnose issues with Coreco Acquisition process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
