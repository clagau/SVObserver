//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEditNumbers
//* .File Name       : $Workfile:   SVEditNumbers.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:26:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVEditNumbers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVMFCControls
	{
		CEditNumbers::CEditNumbers()
		{
		}

		CEditNumbers::~CEditNumbers()
		{
		}


		BEGIN_MESSAGE_MAP(CEditNumbers, CEdit)
			//{{AFX_MSG_MAP(CEditNumbers)
			ON_WM_CHAR()
			ON_WM_SETFOCUS()
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		/////////////////////////////////////////////////////////////////////////////
		// 
		//
		//
		void CEditNumbers::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
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
		void CEditNumbers::OnSetFocus(CWnd* pOldWnd) 
		{
			///SetSel(0,-1);

			CEdit::OnSetFocus(pOldWnd);

			//
			// Select all the characters in the edit
			//
			///SetSel(0,-1);             // This one has never worked
			///PostMessage(EM_SETSEL,0,MAKELPARAM(0,-1));     // 16 bit version
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
			///SetSel(0,-1);

			CEdit::OnSetFocus(pOldWnd);

			//
			// Select all the characters in the edit
			//
			///SetSel(0,-1);             // This one has never worked
			///PostMessage(EM_SETSEL,0,MAKELPARAM(0,-1));     // 16 bit version
			PostMessage(EM_SETSEL,(WPARAM)0,(LPARAM)-1);   // 32 Bit version
		}
	} //SVMFCControls
} //Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVEditNumbers.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:26:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 09:16:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:29:46   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Oct 14 1999 11:30:38   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Handle minus numbers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Oct 1999 16:27:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CEditNumbersFloat to only allow a single decimal point
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 23 1999 13:54:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  37
 * SCR Title:  Version 3.00 Beta 3 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   New classes specific to this version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/