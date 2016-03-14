//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOIPFrameWndClass
//* .File Name       : $Workfile:   SVOIPFrameWndClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:07:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVOIPFrameWndClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOIPFrameWndClass

IMPLEMENT_DYNCREATE(SVOIPFrameWndClass, CFrameWnd)

SVOIPFrameWndClass::SVOIPFrameWndClass()
: CFrameWnd()
{
}

SVOIPFrameWndClass::~SVOIPFrameWndClass()
{
}


BEGIN_MESSAGE_MAP(SVOIPFrameWndClass, CFrameWnd)
	//{{AFX_MSG_MAP(SVOIPFrameWndClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

