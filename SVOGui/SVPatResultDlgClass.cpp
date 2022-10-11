//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatResultDlgClass
//* .File Name       : $Workfile:   SVPatResultDlgClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:19:36  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVPatResultDlgClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
SVPatResultDlgClass::SVPatResultDlgClass(CWnd* pParent /*=nullptr*/)
	: CDialog(SVPatResultDlgClass::IDD, pParent)
{
}

SVPatResultDlgClass::~SVPatResultDlgClass()
{
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
} //namespace SvOg