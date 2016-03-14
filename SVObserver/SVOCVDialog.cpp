//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVDialog
//* .File Name       : $Workfile:   SVOCVDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:58:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCVDialog.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVUnaryImageOperatorList.h"
#include "SVOCVAnalyzerResult.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVOCVDialogClass::SVOCVDialogClass(CWnd* pParent /*=NULL*/)
	: CPropertySheet(IDS_OCV_PARAMETERS, pParent)
{

    pOCVAnalyzerResult  = NULL;
	pTool				= NULL;
	pDocument			= NULL;

	AddPageDialogs ();
	//{{AFX_DATA_INIT(SVOCVDialogClass)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVOCVDialogClass::~SVOCVDialogClass()
{
	RemovePage (&m_GeneralParamsDlg);
	RemovePage (&m_MatchStringParamsDlg);
}

void SVOCVDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertySheet::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCVDialogClass)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCVDialogClass, CPropertySheet)
	//{{AFX_MSG_MAP(SVOCVDialogClass)
	//}}AFX_MSG_MAP
	ON_COMMAND( IDOK, OnOK)
	ON_COMMAND( IDCANCEL, OnCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL SVOCVDialogClass::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	ASSERT(pOCVAnalyzerResult);

	if( pTool && pDocument && pOCVAnalyzerResult )
	{
	}
	
	return bResult;
}

void SVOCVDialogClass::AddPageDialogs ()
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	AddPage (&m_GeneralParamsDlg);
	AddPage (&m_MatchStringParamsDlg);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCVDialogClass::OnOK()
{
	m_GeneralParamsDlg.UpdateOCVParameters ();
	m_MatchStringParamsDlg.UpdateOCVParameters ();

    pDocument->SetModifiedFlag(TRUE);       // Set document 'changed'.

	CPropertySheet::EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCVDialogClass::OnCancel()
{
	CPropertySheet::EndDialog(IDCANCEL);
}

