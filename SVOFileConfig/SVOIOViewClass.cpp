// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIOViewClass
// * .File Name       : $Workfile:   SVOIOViewClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:34:12  $
// ******************************************************************************

#include "stdafx.h"
#include "SVOFileConfig.h"
#include "SVOIOViewClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOIOViewClass

IMPLEMENT_DYNCREATE(SVOIOViewClass, CView)

SVOIOViewClass::SVOIOViewClass()
{
}

SVOIOViewClass::~SVOIOViewClass()
{
}

BEGIN_MESSAGE_MAP(SVOIOViewClass, CView)
	//{{AFX_MSG_MAP(SVOIOViewClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOIOViewClass drawing

void SVOIOViewClass::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// SVOIOViewClass diagnostics

#ifdef _DEBUG
void SVOIOViewClass::AssertValid() const
{
	CView::AssertValid();
}

void SVOIOViewClass::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

