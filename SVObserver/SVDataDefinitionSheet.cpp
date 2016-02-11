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
#include "svobserver.h"
#include "SVDataDefinitionSheet.h"

#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVIPDoc.h"
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
SVDataDefinitionSheet::SVDataDefinitionSheet(LPCTSTR pszCaption, SVInspectionProcess& rInspection, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), ISVCancel()
,	m_rInspection( rInspection )
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
	SelectedObjectsPage* pValuesDlg = new SelectedObjectsPage( m_rInspection, _T("Value Names"), m_ValueList, SV_DD_VALUE );
	AddPage(pValuesDlg);

	SelectedObjectsPage* pImagesDlg = new SelectedObjectsPage( m_rInspection, _T("Image Names"), m_ImageList, SV_DD_IMAGE );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDataDefinitionSheet.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:07:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jul 2012 16:45:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Mar 2008 09:22:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New Property Sheet for Data and image Definition lists
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
