//******************************************************************************
//* COPYRIGHT (c) 2008, 2009 by Magnus Egelberg
//* All Rights Reserved 
//* Author           : Magnus Egelberg (magnus.egelberg@lundalogik.se)
//* Licenses         : No known license
//* From             : http://www.codeguru.com/cpp/controls/propertysheet/sizing/article.php/c599/Resizable-CPropertySheet.htm
//******************************************************************************
//* .Module Name     : CResizablePropertySheet
//* .File Name       : $Workfile:   ResizablePropertySheet.h  $
//* .Description	 : The class resizes the registered controls in a dialog 
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Aug 2014 12:35:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "ResizablePropertySheet.h"
#include "DlgItemResizer.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectSelectorLibrary
	{
		class CResizablePropertySheet : public CPropertySheet
		{
		#pragma region Declarations
			DECLARE_DYNAMIC(CResizablePropertySheet)
		#pragma endregion Declarations

		public:
		#pragma region Constructor
			CResizablePropertySheet();
			CResizablePropertySheet(UINT nIDCaption, CWnd *pParentWnd = NULL, UINT iSelectPage = 0);
			CResizablePropertySheet(LPCTSTR pszCaption, CWnd *pParentWnd = NULL, UINT iSelectPage = 0);
			virtual ~CResizablePropertySheet();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			virtual BOOL OnInitDialog();
			INT_PTR DoModal();
			// Returns the size/position of the page
			void GetPageRect(RECT *pRect);
		#pragma endregion Public Methods

		protected:
		#pragma region Protected Methods

			virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
			static int CALLBACK ResizePropSheetCallback(HWND hWnd, UINT message, LPARAM lParam);
			//{{AFX_MSG(CResizablePropertySheet)
			afx_msg void OnSize(UINT nType, int cx, int cy);
			afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
			afx_msg void OnPaint();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
		#pragma endregion Protected Methods

		private:
		#pragma region Member Variables
			CDlgItemResizer m_Resize;
			CRect m_rcPage;
			CRect m_rcClient;
			CRect m_rcGripper;
		#pragma endregion Member Variables
		};
	} //namespace ObjectSelectorLibrary
} //namespace Seidenader

namespace SvOsl = Seidenader::ObjectSelectorLibrary;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ResizablePropertySheet.h_v  $
 * 
 *    Rev 1.1   12 Aug 2014 12:35:06   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed button display problem in the resize property sheet Win7 64bit
 * Added the methods DoModal and ResizePropSheetCallback
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 11:16:28   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
