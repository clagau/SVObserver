//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilityArgumentDialog
//* .File Name       : $Workfile:   SVUtilityArgumentDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:57:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOResource\resource.h"
#include "SVUtilityArgumentDialog.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVUtilityArgumentDialog dialog

void SVUtilityArgumentDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVUtilityArgumentDialog)
	DDX_Text(pDX, IDC_UTILITY_ARGUMENTS, mszArguments);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVUtilityArgumentDialog, CDialog)
	//{{AFX_MSG_MAP(SVUtilityArgumentDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

