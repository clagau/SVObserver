//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeProperties
//* .File Name       : $Workfile:   SVBarCodeProperties.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:42:32  $
//******************************************************************************

#include "stdafx.h"

#include "SVBarCodeProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVBarCodeProperties

IMPLEMENT_DYNAMIC(SVBarCodeProperties, CPropertySheet)

SVBarCodeProperties::SVBarCodeProperties(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
  AddPage (&m_dlgBarCodeGeneral);
  AddPage (&m_dlgBarCodeStringMatch);
  AddPage (&m_dlgBarCodeAttributes);
  AddPage (&m_dlgBarCodeDataMatrix);
}


SVBarCodeProperties::~SVBarCodeProperties()
{
}


BEGIN_MESSAGE_MAP(SVBarCodeProperties, CPropertySheet)
	//{{AFX_MSG_MAP(SVBarCodeProperties)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

