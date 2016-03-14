//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataDefinitionSheet
//* .File Name       : $Workfile:   SVDataDefinitionSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:07:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDataDefinitionSheet.h"
#include "SelectedObjectsPage.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(SVDataDefinitionSheet, CPropertySheet)

BEGIN_MESSAGE_MAP(SVDataDefinitionSheet, CPropertySheet)
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
SVDataDefinitionSheet::SVDataDefinitionSheet(LPCTSTR pszCaption, const SVString& rInspectionName, const SVGUID& rInspectionID, CWnd* pParentWnd, UINT iSelectPage)
: CPropertySheet(pszCaption, pParentWnd, iSelectPage), ISVCancel()
, m_InspectionName( rInspectionName )
, m_InspectionID( rInspectionID )
{
	CreatePages();
}

SVDataDefinitionSheet::~SVDataDefinitionSheet()
{
	DestroyPages();
}
#pragma endregion Constructor

#pragma region Public Methods
bool SVDataDefinitionSheet::CanCancel()
{
	return true;
}

HRESULT SVDataDefinitionSheet::GetCancelData(SVCancelData*& rpData)
{
	return E_NOTIMPL;
}

HRESULT SVDataDefinitionSheet::SetCancelData(SVCancelData* pData)
{
	return E_NOTIMPL;
}
#pragma endregion Public Methods

#pragma region Private Methods
HRESULT SVDataDefinitionSheet::CreatePages()
{
	SelectedObjectsPage* pValuesDlg = new SelectedObjectsPage( m_InspectionName, m_InspectionID, _T("Value Names"), m_ValueList, SV_DD_VALUE );
	AddPage(pValuesDlg);

	SelectedObjectsPage* pImagesDlg = new SelectedObjectsPage( m_InspectionName, m_InspectionID, _T("Image Names"), m_ImageList, SV_DD_IMAGE );
	AddPage(pImagesDlg);

	return S_OK;
}

void SVDataDefinitionSheet::DestroyPages()
{
	CPropertyPage* pPage;
	for( int i = GetPageCount() - 1; i >= 0; -- i )
	{
		pPage = GetPage( i );
		RemovePage( i );
		if( nullptr != pPage )
		{
			delete pPage;
		}
	}
}

BOOL SVDataDefinitionSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	

	bool bCancelable = CanCancel();	// from ISVCancel base //SVPropertySheetCancelImpl base
	CWnd* pWnd = GetDlgItem(IDCANCEL);
	if ( pWnd )
		pWnd->ShowWindow( bCancelable ? SW_SHOW : SW_HIDE );

	return bResult;
}

void SVDataDefinitionSheet::OnOK() 
{
	CWnd* pActivePage = GetActivePage();
	int iNumPages = GetPageCount();
	int i=0;

	// OK to exit; call OnOK for all pages


	for( i = 0; i < iNumPages; i++ )
	{
		CPropertyPage* pPage = GetPage(i);
		if( pPage && pPage->GetSafeHwnd() ) 
		{
			if ( pPage == pActivePage )
				pPage->OnKillActive();
			pPage->OnOK();
		}
	}

	EndDialog(IDOK);
}

