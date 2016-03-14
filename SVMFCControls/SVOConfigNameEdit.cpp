//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOConfigNameEdit
//* .File Name       : $Workfile:   SVOConfigNameEdit.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:49:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVOConfigNameEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVMFCControls
	{
		CSVOConfigNameEdit::CSVOConfigNameEdit(LPCTSTR const sExcludedChars)
		{
			m_sExcludedChars = sExcludedChars;
		}

		CSVOConfigNameEdit::~CSVOConfigNameEdit()
		{
		}


		BEGIN_MESSAGE_MAP(CSVOConfigNameEdit, CEdit)
			//{{AFX_MSG_MAP(CSVOConfigNameEdit)
			ON_WM_CHAR()
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		/////////////////////////////////////////////////////////////////////////////
		// CSVOConfigNameEdit message handlers

		void CSVOConfigNameEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
		{
			switch (nChar)
			{
			case VK_BACK:
				{
					break;
				}
			default:
				{
					if (::_tcschr(m_sExcludedChars,nChar) )
					{
						return;
					}
				}
			}

			CEdit::OnChar(nChar, nRepCnt, nFlags);
		}
	} //SVMFCControls
} //Seidenader

