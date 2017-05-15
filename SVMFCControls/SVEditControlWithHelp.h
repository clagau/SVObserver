//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved 
//******************************************************************************
//* .Module Name     : SVEditControlWithHelp
//* .File Name       : $Workfile:   SVEditControlWithHelp.h  $
//* .Description     : This class is a CEdit-control with the add-on of a gray help text 
//*                    if the text of the control is empty and the control does not have focus.
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   16 Dec 2014 18:08:28  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <afxstr.h>
#pragma endregion Includes

namespace SvMc
{
	class SVEditControlWithHelp : public CEdit
	{
#pragma region Constructor
	public:
		SVEditControlWithHelp();
		virtual ~SVEditControlWithHelp();
#pragma endregion Constructor

#pragma region MFC Methods
		//{{AFX_MSG(SVEditControlWithHelp)
		afx_msg void OnSetFocus(CWnd* pNewWnd);
		afx_msg void OnKillFocus(CWnd* pNewWnd);
		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
#pragma endregion MFC Methods

#pragma region Public Methods
	public:
		//************************************
		// Description:  Return the text in the control (not the help text)
		//				 It is similar to GetWindowText, if the text is not empty. 
		//				 But do NOT use GetWindowText, because it can return the help text if the edit text is empty.
		// Returns:  CString:  the text entered into the control
		//************************************
		CString getEditText();

		//************************************
		// Description:  Return the help text. 
		//				 Is only to see if edit-control-text is empty and control has not the focus.
		// Returns:  CString:  the text displayed as a help
		//************************************
		CString getHelpText();

		//************************************
		// Description:  Set the edit text
		// Parameter:   text <in>:  the text to display as the contents of the edit control
		// Returns:  void
		//************************************
		void setEditText(CString text);

		//************************************
		// Description:  Set the help text
		// Parameter:  text <in>:  the text to display as a help
		// Returns:  void
		//************************************
		void setHelpText(CString text);
#pragma endregion Public Methods

#pragma region Member variables
	private:
		CString m_currentString;
		CString m_helpString;
		bool m_isCurrentStringActive;
		CBrush m_brush;
#pragma endregion Member variables
	};
} //namespace SvMc
