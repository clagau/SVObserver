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
#pragma region Includes
#include "stdafx.h"
#include "SVOCVDialog.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVOCore/SVUnaryImageOperatorList.h"
#include "SVOCVAnalyzerResult.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVOCVDialogClass::SVOCVDialogClass(SVIPDoc* pDoc, SVOCVAnalyzeResultClass* pResult, CWnd* pParent /*=nullptr*/)
: CPropertySheet(IDS_OCV_PARAMETERS, pParent)
, m_pDocument(pDoc)
, m_pOCVAnalyzerResult(pResult)
{
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

SVOCVAnalyzeResultClass* SVOCVDialogClass::GetAnalyzerResult() const
{
	return m_pOCVAnalyzerResult;
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

	ASSERT(m_pOCVAnalyzerResult);

	return bResult;
}

void SVOCVDialogClass::AddPageDialogs ()
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	AddPage (&m_GeneralParamsDlg);
	AddPage (&m_MatchStringParamsDlg);
}

void SVOCVDialogClass::OnOK()
{
	m_GeneralParamsDlg.UpdateOCVParameters ();
	m_MatchStringParamsDlg.UpdateOCVParameters ();
	if (m_pDocument)
	{
		m_pDocument->SetModifiedFlag(TRUE);       // Set document 'changed'.
	}

	CPropertySheet::EndDialog(IDOK);
}

void SVOCVDialogClass::OnCancel()
{
	CPropertySheet::EndDialog(IDCANCEL);
}

