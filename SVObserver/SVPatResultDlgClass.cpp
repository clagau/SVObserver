//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatResultDlgClass
//* .File Name       : $Workfile:   SVPatResultDlgClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:19:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVPatResultDlgClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVPatResultDlgClass dialog

SVPatResultDlgClass::SVPatResultDlgClass(CWnd* pParent /*=NULL*/)
	: CDialog(SVPatResultDlgClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVPatResultDlgClass)
	m_strAngle = _T("0.0");
	m_strScore = _T("0.0");
	m_strXPos = _T("0.0");
	m_strYPos = _T("0.0");
	//}}AFX_DATA_INIT
}

void SVPatResultDlgClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPatResultDlgClass)
	DDX_Text(pDX, IDC_PAT_M_ANGLE, m_strAngle);
	DDX_Text(pDX, IDC_PAT_M_SCORE, m_strScore);
	DDX_Text(pDX, IDC_PAT_X_POSITION, m_strXPos);
	DDX_Text(pDX, IDC_PAT_Y_POSITION, m_strYPos);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVPatResultDlgClass, CDialog)
	//{{AFX_MSG_MAP(SVPatResultDlgClass)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

