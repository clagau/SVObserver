//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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

namespace SvMc
{
	SVOConfigNameEdit::SVOConfigNameEdit(LPCTSTR const sExcludedChars)
	{
		m_sExcludedChars = sExcludedChars;
	}

	SVOConfigNameEdit::~SVOConfigNameEdit()
	{
	}


	BEGIN_MESSAGE_MAP(SVOConfigNameEdit, CEdit)
		//{{AFX_MSG_MAP(SVOConfigNameEdit)
		ON_WM_CHAR()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// SVOConfigNameEdit message handlers

	void SVOConfigNameEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
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
} //namespace SvMc
