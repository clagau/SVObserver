//******************************************************************************
//* COPYRIGHT (c) 2008, 2009 by Magnus Egelberg
//* All Rights Reserved 
//* Author           : Magnus Egelberg (magnus.egelberg@lundalogik.se)
//* Licenses         : No known license
//* From             : http://www.codeguru.com/cpp/controls/propertysheet/sizing/article.php/c599/Resizable-CPropertySheet.htm
//******************************************************************************
//* .Module Name     : CDlgItemResizer
//* .File Name       : $Workfile:   DlgItemResizer.h  $
//* .Description	 : The class resizes the registered controls in a dialog 
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 11:12:18  $
//******************************************************************************

#pragma once

namespace Seidenader { namespace ObjectSelectorLibrary
{
	#pragma region Declarations
		const UINT RESIZE_LOCKLEFT        = 0x0001; // Distance to left is fixed
		const UINT RESIZE_LOCKRIGHT       = 0x0002; // Distance to right is fixed
		const UINT RESIZE_LOCKTOP         = 0x0004; // Distance to top is fixed
		const UINT RESIZE_LOCKBOTTOM      = 0x0008; // Distance to bottom is fixed
		const UINT RESIZE_SHOWHIDE        = 0x0010; // Show/hide if not fully visible

		const UINT RESIZE_LOCKALL         = RESIZE_LOCKLEFT|RESIZE_LOCKRIGHT|RESIZE_LOCKTOP|RESIZE_LOCKBOTTOM;
		const UINT RESIZE_LOCKTOPLEFT     = RESIZE_LOCKLEFT|RESIZE_LOCKTOP;
		const UINT RESIZE_LOCKBOTTOMRIGHT = RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT;
	#pragma endregion Declarations

	class CDlgItemResizer
	{

	public:
	#pragma region Constructor
		CDlgItemResizer();
		virtual ~CDlgItemResizer();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		// Adds a control to the resize list
		void Add(CWnd *pControl, UINT uFlags);

		void Add(CWnd *pWnd, INT nCtrlID, UINT uFlags)
		{ Add(pWnd->GetDlgItem(nCtrlID), uFlags); }

		// Resizes the controls in the form
		void Resize(CFormView *pWnd);

		// Resizes the controls in the window
		void Resize(CWnd *pWnd);

		// Sets the original size of the parent
		void SetInitialSize(INT cx, INT cy)
		{ m_szInitial = CSize(cx, cy); }

		// Sets the original size of the parent
		void SetInitialSize(const CSize &size)
		{ m_szInitial = size; }

		// Returns the initial size
		CSize GetInitialSize() const
		{ return m_szInitial; }
	#pragma endregion Public Methods

	private:
	#pragma region Member Variables
		CPtrArray m_Controls;
		CSize m_szInitial;
	#pragma endregion Member Variables
	};
} /*namespace ObjectSelectorLibrary*/ } /*namespace Seidenader*/

namespace SvOsl = Seidenader::ObjectSelectorLibrary;
