// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIPViewClass
// * .File Name       : $Workfile:   SVOIPViewClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:35:44  $
// * ----------------------------------------------------------------------------
// * .Description     :
// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOFileConfig\SVOIPViewClass.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 11:35:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:00:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/// SVOIPViewClass.cpp : implementation file
//

#include "stdafx.h"
#include "SVOFileConfig.h"
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

/////////////////////////////////////////////////////////////////////////////
// SVOIPViewClass message handlers
