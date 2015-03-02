// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThickeningFilterDlg
// * .File Name       : $Workfile:   SVThickeningFilterDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   24 Apr 2013 11:37:22  $
// ******************************************************************************

#if !defined(AFX_SVTHICKENINGFILTERDLG_H__INCLUDED_)
#define AFX_SVTHICKENINGFILTERDLG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectInterfaces\IThickeningFilter.h"

namespace Seidenader
{
	namespace SVOGui
	{
		/////////////////////////////////////////////////////////////////////////////
		// SVThickeningFilterDlg dialog
		class SVThickeningFilterDlg : public CDialog
		{
			// Construction
		public:
			SVThickeningFilterDlg(SvOi::IThickeningFilter& rFilterClass, CWnd* pParent = nullptr);   // standard constructor

			virtual HRESULT SetInspectionData();

			// Dialog Data
			//{{AFX_DATA(SVThickeningFilterDlg)
			enum { IDD = IDD_THICKENING_FILTER };
			BOOL	m_bGrayScale;
			long	m_lIterations;
			//}}AFX_DATA


			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVThickeningFilterDlg)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		protected:

			// Generated message map functions
			//{{AFX_MSG(SVThickeningFilterDlg)
			virtual BOOL OnInitDialog();
			virtual void OnOK();
			virtual void OnCancel();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

		private:
			SvOi::IThickeningFilter& m_pFilterClass;
		};
	}  //end namespace SVOGUI
}  //end namespace Seidenader

namespace SvOg = Seidenader::SVOGui;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVTHICKENINGFILTERDLG_H__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVThickeningFilterDlg.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:37:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Aug 2005 08:32:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Sep 2004 10:02:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  446
 * SCR Title:  Add new filters to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in of Thickening Filter files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
