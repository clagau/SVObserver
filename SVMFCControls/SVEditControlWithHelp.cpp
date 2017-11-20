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
#include  "Definitions/Color.h"
#pragma endregion Includes

namespace SvMc
{
#pragma region Declarations

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
		m_brush.CreateSolidBrush( SvDef::White );
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
			pDC->SetTextColor(SvDef::Black );
		}
		else
		{
			pDC->SetTextColor(SvDef::MiddleGray );
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
#pragma endregion Public Methods
} //namespace SvMc
