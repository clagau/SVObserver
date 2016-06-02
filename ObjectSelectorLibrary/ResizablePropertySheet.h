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

namespace Seidenader { namespace ObjectSelectorLibrary
{
	class CResizablePropertySheet : public CPropertySheet
	{
	#pragma region Declarations
		DECLARE_DYNAMIC(CResizablePropertySheet)
	#pragma endregion Declarations

	public:
	#pragma region Constructor
		CResizablePropertySheet();
		CResizablePropertySheet(UINT nIDCaption, CWnd *pParentWnd = nullptr, UINT iSelectPage = 0);
		CResizablePropertySheet(LPCTSTR pszCaption, CWnd *pParentWnd = nullptr, UINT iSelectPage = 0);
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
} /*namespace ObjectSelectorLibrary*/ } /*namespace Seidenader*/

namespace SvOsl = Seidenader::ObjectSelectorLibrary;
