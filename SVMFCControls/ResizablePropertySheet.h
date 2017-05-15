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
#include "DlgItemResizer.h"
#pragma endregion Includes

namespace SvMc
{
	class ResizablePropertySheet : public CPropertySheet
	{
	#pragma region Declarations
		DECLARE_DYNAMIC(ResizablePropertySheet)
	#pragma endregion Declarations

	public:
	#pragma region Constructor
		ResizablePropertySheet();
		ResizablePropertySheet(UINT nIDCaption, CWnd *pParentWnd = nullptr, UINT iSelectPage = 0);
		ResizablePropertySheet(LPCTSTR pszCaption, CWnd *pParentWnd = nullptr, UINT iSelectPage = 0);
		virtual ~ResizablePropertySheet();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		virtual BOOL OnInitDialog() override;
		INT_PTR DoModal();
		// Returns the size/position of the page
		void GetPageRect(RECT *pRect);
	#pragma endregion Public Methods

	protected:
	#pragma region Protected Methods

		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
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
		DlgItemResizer m_Resize;
		CRect m_rcPage;
		CRect m_rcClient;
		CRect m_rcGripper;
	#pragma endregion Member Variables
	};
} //namespace SvMc
