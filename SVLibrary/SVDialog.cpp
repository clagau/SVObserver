// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDialog
// * .File Name       : $Workfile:   SVDialog.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:31:18  $
// ******************************************************************************

#include "stdafx.h"
#include "SVDialog.h"

BEGIN_MESSAGE_MAP(SVDialog, CDialog)
	//{{AFX_MSG_MAP(SVDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//##ModelId=38D12F6D00F1
SVDialog::SVDialog(UINT DlgId, CWnd *pParent)
	:	CDialog (DlgId, pParent)
{
	mhAccel = LoadAccelerators (AfxGetResourceHandle(), MAKEINTRESOURCE (DlgId));
}

//##ModelId=38D12F6D00F0
SVDialog::~SVDialog()
{
}

//##ModelId=38D12FE1032C
BOOL SVDialog::PreTranslateMessage(MSG* pMsg)
{
	if (mhAccel)
	{
		if (TranslateAccelerator (this->m_hWnd, mhAccel, pMsg))
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

