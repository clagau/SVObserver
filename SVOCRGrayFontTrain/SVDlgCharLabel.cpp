//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgCharLabel
//* .File Name       : $Workfile:   SVDlgCharLabel.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 09:25:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRGrayFontTrain.h"
#include "SVDlgCharLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVDlgCharLabel dialog

SVDlgCharLabel::SVDlgCharLabel(CWnd* pParent /*=NULL*/)
	: CDialog(SVDlgCharLabel::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVDlgCharLabel)
	m_csCharLabel = _T("");
	//}}AFX_DATA_INIT
}

void SVDlgCharLabel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVDlgCharLabel)
	DDX_Text(pDX, IDC_EDIT_CHAR_LABEL, m_csCharLabel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVDlgCharLabel, CDialog)
	//{{AFX_MSG_MAP(SVDlgCharLabel)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

