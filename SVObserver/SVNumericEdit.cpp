// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVNumericEdit
// * .File Name       : $Workfile:   SVNumericEdit.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:37:30  $
// ******************************************************************************

#include "stdafx.h"
#include "SVNumericEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSVOConfigNameEdit

CSVNumericEdit::CSVNumericEdit()
{
    m_sExcludedChars = _T("-\\/:*\"<>|AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz~`!@#$%^&()+=");
	m_sOrgExcludedChars = _T("-\\/:*\"<>|AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz~`!@#$%^&()+=");
}

CSVNumericEdit::~CSVNumericEdit()
{
}


BEGIN_MESSAGE_MAP(CSVNumericEdit, CEdit)
	//{{AFX_MSG_MAP(CSVOConfigNameEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVNumericEdit message handlers

void CSVNumericEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CString sWindowTxt;
	GetWindowText(sWindowTxt);
	char cTmp = (char)nChar;

	switch (nChar)
    {
        case VK_BACK:
        {
			//check to see if excludedChars contains the decimal point,
			//if it does and the string in the text does not remove it from 
			//excluded chars.
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			GetWindowText(sWindowTxt);

			if ( sWindowTxt.Find('.') == -1 )
			{
				m_sExcludedChars = m_sOrgExcludedChars;
			}
			return;
			//check to
            break;
        }
        default:
        {
			BOOL bAddDecimalPt = FALSE;
			if ( cTmp == '.' )
			{
				if ( sWindowTxt.Find('.') >= 0 )
				{
					return;
				}
				if ( m_sExcludedChars.Find('.',0) == - 1 )
				{
					bAddDecimalPt = TRUE;
				}
			}

            if (::_tcschr(m_sExcludedChars,nChar) )
            {
                return;
            }
			if ( bAddDecimalPt )
			{
				m_sExcludedChars += '.';
			}
        }
    }
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);

	GetWindowText(sWindowTxt);
	if ( sWindowTxt.Find('.') == -1 )
	{
		m_sExcludedChars = m_sOrgExcludedChars;
	}
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVNumericEdit.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:37:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Jun 2003 14:58:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  349
 * SCR Title:  Fix Bug I0001101 Enter text in minimum scale field
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/