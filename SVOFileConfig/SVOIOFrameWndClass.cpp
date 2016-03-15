// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIOFrameWndClass
// * .File Name       : $Workfile:   SVOIOFrameWndClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:33:40  $
// ******************************************************************************

#include "stdafx.h"
#include "SVOFileConfig.h"
#include "SVOIOFrameWndClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOIOFrameWndClass

IMPLEMENT_DYNCREATE(SVOIOFrameWndClass, CFrameWnd)

SVOIOFrameWndClass::SVOIOFrameWndClass()
{
}

SVOIOFrameWndClass::~SVOIOFrameWndClass()
{
}

BEGIN_MESSAGE_MAP(SVOIOFrameWndClass, CFrameWnd)
	//{{AFX_MSG_MAP(SVOIOFrameWndClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

