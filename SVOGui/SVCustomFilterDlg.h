//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomFilterDlg
//* .File Name       : $Workfile:   SVCustomFilterDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:04:38  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectInterfaces\ICustomFilter.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		/////////////////////////////////////////////////////////////////////////////
		// SVCustomFilterDlg dialog
		class SVCustomFilterDlg : public CDialog 
		{
			// Construction
		public:
			SVCustomFilterDlg(SvOi::ICustomFilter& rFilterClass, CWnd* pParent = nullptr);   // standard constructor
			~SVCustomFilterDlg();

		protected:

			virtual HRESULT SetInspectionData();
			void EnableCells();

			// Dialog Data
			//{{AFX_DATA(SVCustomFilterDlg)
		public:
			enum { IDD = IDD_CUSTOM_FILTER };
			CComboBox	m_ctlKernelWidth;
			CComboBox	m_ctlKernelHeight;
			BOOL	m_bAbsoluteValue;
			BOOL	m_bClippingOn;
			long	m_lTransformationFactor;
			long	m_lKernelWidth;
			long	m_lKernelHeight;
			//}}AFX_DATA

			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVCustomFilterDlg)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

			// Generated message map functions
			//{{AFX_MSG(SVCustomFilterDlg)
		protected:
			virtual BOOL OnInitDialog();
			virtual void OnOK();
			virtual void OnCancel();
			afx_msg void OnSelchangeKernelWidth();
			afx_msg void OnSelchangeKernelHeight();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

		private:	
			long	m_lKernelCells[49];
			SvOi::ICustomFilter& m_pCustomFilterClass;
		};
	}  //end namespace SVOGUI
}  //end namespace Seidenader

namespace SvOg = Seidenader::SVOGui;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCustomFilterDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:04:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Aug 2005 15:35:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Nov 2003 14:11:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Update the custom filter class to be 1-7x1-7 customizable kernel instead of a static 3x3 one.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2003 10:16:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code and classes for the new custom filters objects and dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 *
*/