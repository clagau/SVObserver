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
// SVDataDefinitionSheet.cpp : implementation file
//

#include "stdafx.h"
#include "svobserver.h"
#include "SVDataDefinitionSheet.h"

#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "ISVPropertyPageDialog.h"
#include "SVDataDefinitionPage.h"
#include "SVIPDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVDataDefinitionSheet

IMPLEMENT_DYNAMIC(SVDataDefinitionSheet, CPropertySheet)


SVDataDefinitionSheet::SVDataDefinitionSheet(LPCTSTR pszCaption, SVInspectionProcess* p_pInspection, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), ISVCancel()
{
	m_pInspection = p_pInspection;

	m_strInspection = p_pInspection->GetName();

	CreatePages();
}

SVDataDefinitionSheet::~SVDataDefinitionSheet()
{
	DestroyPages();
	m_pInspection = nullptr;
}


BEGIN_MESSAGE_MAP(SVDataDefinitionSheet, CPropertySheet)
	//{{AFX_MSG_MAP(SVDataDefinitionSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVDataDefinitionSheet message handlers


HRESULT SVDataDefinitionSheet::CreatePages()
{
	ASSERT( m_pInspection );

	SVDataDefinitionPage* pDataDlg = new SVDataDefinitionPage( this, CString(_T("Value Names")) );
	AddPage(pDataDlg);

	SVDataDefinitionPage* pImagesDlg = new SVDataDefinitionPage( this, CString(_T("Image Names")) );
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
		if( pPage )
		{
			delete pPage;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// SVDataDefinitionSheet message handlers

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

bool SVDataDefinitionSheet::CanCancel()
{
	return true;
	ASSERT(m_pInspection);
	//return m_pPPQ->CanCancel();
}

HRESULT SVDataDefinitionSheet::GetCancelData(SVCancelData*& rpData)
{
	return E_NOTIMPL;
	ASSERT(m_pInspection);
	//return m_pPPQ->GetCancelData(rpData);
}

HRESULT SVDataDefinitionSheet::SetCancelData(SVCancelData* pData)
{
	return E_NOTIMPL;
	ASSERT(m_pInspection);
	//return m_pPPQ->SetCancelData(pData);
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
