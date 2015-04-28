//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved 
//******************************************************************************
//* .Module Name     : SVEditControlWithHelp
//* .File Name       : $Workfile:   SVEditControlWithHelp.cpp  $
//* .Description     : This class is a CEdit-control with the add-on of a gray help text 
//*                    if the text of the control is empty and the control does not have focus.
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 01:54:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEditControlWithHelp.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOMFCLibrary
	{
#pragma region Declarations
		const COLORREF Black        = RGB(   0,   0,   0); //0x00000000;	//        0
		const COLORREF White        = RGB( 255, 255, 255); //0x00FFFFFF;	// 16777215
		const COLORREF MiddleGray   = RGB( 150, 150, 150); //0x00C00000;	// 12582912

		BEGIN_MESSAGE_MAP(SVEditControlWithHelp, CEdit)
			//{{AFX_MSG_MAP(SVEditControlWithHelp)
			ON_WM_SETFOCUS()
			ON_WM_KILLFOCUS()
			ON_WM_CTLCOLOR_REFLECT()
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
		SVEditControlWithHelp::SVEditControlWithHelp()
			: CEdit()
			, m_isCurrentStringActive(false)
			, m_helpString( _T("") )
			, m_currentString( _T("") )
		{
			m_brush.CreateSolidBrush( White );
		}

		SVEditControlWithHelp::~SVEditControlWithHelp()
		{
			m_brush.DeleteObject();
		}
#pragma endregion Constructor

#pragma region MFC Methods
		void SVEditControlWithHelp::OnSetFocus(CWnd* pNewWnd)
		{
			SetWindowText(m_currentString);
			m_isCurrentStringActive = true;

			CEdit::OnSetFocus(pNewWnd);
		}

		void SVEditControlWithHelp::OnKillFocus(CWnd* pNewWnd)
		{
			CString textStr;
			GetWindowText( textStr );
			m_currentString = textStr;
			m_isCurrentStringActive = false;
			//set help text if edit text empty
			if (_T("") == m_currentString)
			{
				SetWindowText(m_helpString);
			}

			CEdit::OnKillFocus(pNewWnd);
		}

		HBRUSH SVEditControlWithHelp::CtlColor( CDC* pDC, UINT nCtlColor )
		{
			if (m_isCurrentStringActive || !m_currentString.IsEmpty())
			{
				pDC->SetTextColor( Black );
			}
			else
			{
				pDC->SetTextColor( MiddleGray );
			}

			return m_brush;
		}
#pragma endregion MFC Methods

#pragma region Public Methods
		CString SVEditControlWithHelp::getEditText()
		{
			if (m_isCurrentStringActive)
			{
				GetWindowText( m_currentString );
			}

			return m_currentString;
		}

		CString SVEditControlWithHelp::getHelpText()
		{
			return m_helpString;
		}

		void SVEditControlWithHelp::setEditText(CString text)
		{
			m_currentString = text;

			if (GetFocus() == this)
			{
				SetWindowText(m_currentString);
			}
			else
			{
				//set help text if edit text empty
				if (_T("") == m_currentString)
				{
					SetWindowText(m_helpString);
				}
				else
				{
					SetWindowText(m_currentString);
				}
			}
		}

		void SVEditControlWithHelp::setHelpText(CString text)
		{
			m_helpString = text;

			if (GetFocus() != this && _T("") == m_currentString)
			{
				SetWindowText(m_helpString);
			}
		}
	}
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVOMFCLibrary\SVEditControlWithHelp.cpp_v  $
 * 
 *    Rev 1.1   19 Dec 2014 01:54:16   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   bugfix: setEditWindow have to set WindowText also if control is not in focus
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Dec 2014 18:08:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/