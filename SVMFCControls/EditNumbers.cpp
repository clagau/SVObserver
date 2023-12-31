//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEditNumbers
//* .File Name       : $Workfile:   SVEditNumbers.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:26:08  $
//******************************************************************************

#include "stdafx.h"
#include "EditNumbers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvMc
{
	EditNumbers::EditNumbers()
	{
	}

	EditNumbers::~EditNumbers()
	{
	}


	BEGIN_MESSAGE_MAP(EditNumbers, CEdit)
		//{{AFX_MSG_MAP(EditNumbers)
		ON_WM_CHAR()
		ON_WM_SETFOCUS()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// 
	//
	//
	void EditNumbers::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
		//
		// Limit valid characters to ASCII '0' to '9'
		if((nChar < (int)(TCHAR)'0') ||
			(nChar > (int)(TCHAR)'9'))
		{
			switch(nChar)
			{
			case VK_BACK:                     // Big Back space key
				goto okChar;
				break;
			case (TCHAR)'-':
				{
					CString textStr;
					GetWindowText( textStr );
					int nStartChar,nEndChar;
					GetSel( nStartChar, nEndChar );

					if( textStr.IsEmpty() ||
						(textStr.Find( _T('-') )  == -1 && !nStartChar))
					{
						goto okChar;
					}
				}
				break;

			default:;
			}
			//
			// Char not allowed .. so beep
			//
			MessageBeep(0xFFFFFFFF);
			return;
		}

okChar:;

		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	void EditNumbers::OnSetFocus(CWnd* pOldWnd) 
	{
		CEdit::OnSetFocus(pOldWnd);

		//
		// Select all the characters in the edit
		//
		PostMessage(EM_SETSEL,(WPARAM)0,(LPARAM)-1);   // 32 Bit version
	}

	/////////////////////////////////////////////////////////////////////////////
	// CEditNumbersFloat

	CEditNumbersFloat::CEditNumbersFloat()
	{
	}

	CEditNumbersFloat::~CEditNumbersFloat()
	{
	}


	BEGIN_MESSAGE_MAP(CEditNumbersFloat, CEdit)
		//{{AFX_MSG_MAP(CEditNumbersFloat)
		ON_WM_CHAR()
		ON_WM_SETFOCUS()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// 
	//
	//
	void CEditNumbersFloat::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
		//
		// Limit valid characters to ASCII '0' to '9'
		if((nChar < (int)(TCHAR)'0') ||
			(nChar > (int)(TCHAR)'9'))
		{
			switch(nChar)
			{
			case (TCHAR)'.':                   // Decimals allowed
				{
					CString textStr;
					GetWindowText( textStr );
					if( textStr.Find( _T('.') )  == -1 )
						goto okChar;
				}
				break;

			case VK_BACK:                     // Big Back space key
				goto okChar;
				break;

			case (TCHAR)'-':
				{
					CString textStr;
					GetWindowText( textStr );
					int nStartChar,nEndChar;
					GetSel( nStartChar, nEndChar );

					if( textStr.IsEmpty() ||
						(textStr.Find( _T('-') )  == -1 && !nStartChar))
					{
						goto okChar;
					}
				}
				break;

			default:;
			}
			//
			// Char not allowed .. so beep
			//
			MessageBeep(0xFFFFFFFF);
			return;
		}

okChar:;

		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}

	void CEditNumbersFloat::OnSetFocus(CWnd* pOldWnd) 
	{
		CEdit::OnSetFocus(pOldWnd);

		//
		// Select all the characters in the edit
		//
		PostMessage(EM_SETSEL,(WPARAM)0,(LPARAM)-1);   // 32 Bit version
	}
} //namespace SvMc
