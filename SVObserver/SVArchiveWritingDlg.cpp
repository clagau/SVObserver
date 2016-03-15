// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVArchiveWritingDlg
// * .File Name       : $Workfile:   SVArchiveWritingDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 09:33:18  $
// ******************************************************************************

#include "stdafx.h"
#include "svobserver.h"
#include "SVArchiveWritingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVArchiveWritingDlg dialog


SVArchiveWritingDlg::SVArchiveWritingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVArchiveWritingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVArchiveWritingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void SVArchiveWritingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVArchiveWritingDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVArchiveWritingDlg, CDialog)
	//{{AFX_MSG_MAP(SVArchiveWritingDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

