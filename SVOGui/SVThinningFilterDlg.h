// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThinningFilterDlg
// * .File Name       : $Workfile:   SVThinningFilterDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   24 Apr 2013 12:25:20  $
// ******************************************************************************

#if !defined(AFX_SVTHINNINGFILTERDLG_H__INCLUDED_)
#define AFX_SVTHINNINGFILTERDLG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectInterfaces\IThinningFilter.h"

namespace Seidenader
{
	namespace SVOGui
	{
		/////////////////////////////////////////////////////////////////////////////
		// SVThinningFilterDlg dialog
		class SVThinningFilterDlg : public CDialog
		{
			// Construction
		public:
			SVThinningFilterDlg(SvOi::IThinningFilter& rFilterClass, CWnd* pParent = nullptr);   // standard constructor

			virtual HRESULT SetInspectionData();

			// Dialog Data
			//{{AFX_DATA(SVThinningFilterDlg)
			enum { IDD = IDD_THINNING_FILTER };
			BOOL	m_bGrayScale;
			long	m_lIterations;
			//}}AFX_DATA


			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVThinningFilterDlg)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		protected:

			// Generated message map functions
			//{{AFX_MSG(SVThinningFilterDlg)
			virtual BOOL OnInitDialog();
			virtual void OnOK();
			virtual void OnCancel();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

		private:
			SvOi::IThinningFilter& m_pFilterClass;
		};
	}  //end namespace SVOGUI
}  //end namespace Seidenader

namespace SvOg = Seidenader::SVOGui;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVTHINNINGFILTERDLG_H__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVThinningFilterDlg.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 12:25:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Aug 2005 08:32:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Sep 2004 10:03:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  446
 * SCR Title:  Add new filters to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in of Thinning Filter files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
