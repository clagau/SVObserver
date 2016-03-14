//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOIPViewClass
//* .File Name       : $Workfile:   SVOIPViewClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:08:16  $
//******************************************************************************

#include "stdafx.h"
#include "SVOIPViewClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOIPViewClass

IMPLEMENT_DYNCREATE(SVOIPViewClass, CView)

SVOIPViewClass::SVOIPViewClass()
: CView()
{
}

SVOIPViewClass::~SVOIPViewClass()
{
}


BEGIN_MESSAGE_MAP(SVOIPViewClass, CView)
	//{{AFX_MSG_MAP(SVOIPViewClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOIPViewClass drawing

void SVOIPViewClass::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// SVOIPViewClass diagnostics

#ifdef _DEBUG
void SVOIPViewClass::AssertValid() const
{
	CView::AssertValid();
}

void SVOIPViewClass::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

