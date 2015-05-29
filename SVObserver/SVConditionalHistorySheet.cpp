//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistorySheet
//* .File Name       : $Workfile:   SVConditionalHistorySheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:02:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVConditionalHistorySheet.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVInspectionProcess.h"
#include "svobserver.h"
#include "SVPPQObject.h"
#include "SVOGui\ISVPropertyPageDialog.h"
#include "SVConditionalHistoryValuesPage.h"
#include "SVConditionalHistoryImagesPage.h"
#include "SVConditionalHistoryConditionalPage.h"
#include "SVIPDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(SVConditionalHistorySheet, CPropertySheet)

BEGIN_MESSAGE_MAP(SVConditionalHistorySheet, CPropertySheet)
	//{{AFX_MSG_MAP(SVConditionalHistorySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()


SVConditionalHistorySheet::SVConditionalHistorySheet( LPCTSTR pszCaption, SVInspectionProcess* p_pInspection, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), ISVCancel()
{
	m_pInspection = p_pInspection;

	m_strInspection = p_pInspection->GetName();

	SVScalarValueMapType mapProperties;
	p_pInspection->GetConditionalHistoryProperties( mapProperties );
	m_strMaxHistory = mapProperties[PPQCH_Size].strValue;
	CString strOverwrite = mapProperties[PPQCH_Overwrite].strValue;
	SVBoolValueObjectClass bvo;
	bvo.SetValue( 1, strOverwrite );
	bvo.GetValue( m_bOverwrite );

	CreatePages();
}

SVConditionalHistorySheet::~SVConditionalHistorySheet()
{
	DestroyPages();
	m_pInspection = NULL;
}

HRESULT SVConditionalHistorySheet::CreatePages()
{
	ASSERT( m_pInspection );

	SVConditionalHistoryValuesPage* pValuesDlg = new SVConditionalHistoryValuesPage( this, CString(_T("Values")) );
	AddPage(pValuesDlg);

	SVConditionalHistoryImagesPage* pImagesDlg = new SVConditionalHistoryImagesPage( this, CString(_T("Images")) );
	AddPage(pImagesDlg);

	SVConditionalHistoryConditionalPage* pConditionalsDlg = new SVConditionalHistoryConditionalPage( this, CString(_T("Conditions")) );
	AddPage(pConditionalsDlg);

	return S_OK;
}

void SVConditionalHistorySheet::DestroyPages()
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
// SVConditionalHistorySheet message handlers

BOOL SVConditionalHistorySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	

	bool bCancelable = CanCancel();	// from ISVCancel base //SVPropertySheetCancelImpl base
	CWnd* pWnd = GetDlgItem(IDCANCEL);
	if ( pWnd )
		pWnd->ShowWindow( bCancelable ? SW_SHOW : SW_HIDE );

	return bResult;
}

void SVConditionalHistorySheet::OnOK() 
{
	CWnd* pActivePage = GetActivePage();
	int iNumPages = GetPageCount();
	int i=0;

	// can we exit?
	for( i = 0; i < iNumPages; i++ )
	{
		if ( SvOg::ISVPropertyPageDialog* pIDlg = dynamic_cast <SvOg::ISVPropertyPageDialog*> ( GetPage(i) ) )
		{
			if ( pIDlg->QueryAllowExit() == false )	// exit not allowed
			{
				return;
			}
		}
	}

	// OK to exit; call OnOK for all pages

	bool bChanges = false;

	for( i = 0; i < iNumPages; i++ )
	{
		CPropertyPage* pPage = GetPage(i);
		if( pPage && pPage->GetSafeHwnd() ) 
		{
			if ( pPage == pActivePage )
				pPage->OnKillActive();
			pPage->OnOK();
			if ( SVConditionalHistoryBasePage* pCHPage = dynamic_cast <SVConditionalHistoryBasePage*> (pPage) )
			{
				bChanges = bChanges || pCHPage->m_bObjectsChanged;
			}
		}
	}

	// Set Properties
	SVScalarValueVectorType vecProperties;
	vecProperties.push_back( SVScalarValue(PPQCH_Size, m_strMaxHistory) );
	vecProperties.push_back( SVScalarValue(PPQCH_Overwrite, AsString(m_bOverwrite) ) );

	m_pInspection->SetConditionalHistoryProperties(vecProperties, false);
	bChanges = true;	// how to detect if properties changed?

	//*
	if ( bChanges )
	{
		SVIPDoc* l_pIPDoc = NULL;

		if( m_pInspection != NULL )
		{
			l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( m_pInspection->GetUniqueObjectID() );

			if( l_pIPDoc != NULL )
			{
				l_pIPDoc->SetModifiedFlag();
			}
		}

		m_pInspection->ResetConditionalHistory();
	}
	//*/

	EndDialog(IDOK);
}

bool SVConditionalHistorySheet::CanCancel()
{
	return true;
	ASSERT(m_pInspection);
	//return m_pPPQ->CanCancel();
}

HRESULT SVConditionalHistorySheet::GetCancelData(SVCancelData*& rpData)
{
	return E_NOTIMPL;
	ASSERT(m_pInspection);
	//return m_pPPQ->GetCancelData(rpData);
}

HRESULT SVConditionalHistorySheet::SetCancelData(SVCancelData* pData)
{
	return E_NOTIMPL;
	ASSERT(m_pInspection);
	//return m_pPPQ->SetCancelData(pData);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditionalHistorySheet.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:02:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Jul 2012 16:39:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Dec 2010 07:51:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Jan 2006 12:36:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added object list functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Jan 2006 14:17:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed Conditional History bugs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jan 2006 14:55:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed property updating
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:19:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

