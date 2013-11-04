//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolDetailsSheet
//* .File Name       : $Workfile:   SVExternalToolDetailsSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:30:38  $
//******************************************************************************
// SVExternalToolDetailsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "SVExternalToolDetailsSheet.h"

#include "svobserver.h"
#include "SVExternalToolImageSelectPage.h"
#include "SVExternalToolInputSelectPage.h"
#include "SVExternalToolResultPage.h"
#include "SVExternalToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVExternalToolDetailsSheet

IMPLEMENT_DYNAMIC(SVExternalToolDetailsSheet, CPropertySheet)


BEGIN_MESSAGE_MAP(SVExternalToolDetailsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(SVExternalToolDetailsSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()


SVExternalToolDetailsSheet::SVExternalToolDetailsSheet( LPCTSTR pszCaption, SVExternalToolDlg* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), ISVCancel()
{
	m_pSVExternalToolDlgParent = pParentWnd;
	m_pTask = NULL;
	m_pTool = NULL;
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

SVExternalToolDetailsSheet::~SVExternalToolDetailsSheet()
{
	DestroyPages();
	m_pTask = NULL;
	m_pTool = NULL;
}

HRESULT SVExternalToolDetailsSheet::CreatePages()
{
	ASSERT( m_pTask );
	ASSERT( m_pTool );


	SVExternalToolImageSelectPage* pImageDlg = new SVExternalToolImageSelectPage(this);
	AddPage(pImageDlg);

	const bool bTabbed = false;
	if ( !bTabbed )
	{
		SVExternalToolInputSelectPage* pInputDlg = new SVExternalToolInputSelectPage( CString(_T("External Tool Inputs")), this );
		AddPage(pInputDlg);
	}
	else
	{// If Tabbed then find all unique groups and create a page for each group.

		std::map<CString, SVRPropertyItem*> mapGroupItems;
		std::map<CString, SVRPropertyItem*>::iterator iterGroup;

		for( int i = 0 ; i < m_pTask->m_Data.m_lNumInputValues ; i++ )
		{
			SVStringValueObjectClass& rName = m_pTask->m_Data.m_aInputObjectNames[i];
			SVVariantValueObjectClass& rValue = m_pTask->m_Data.m_aInputObjects[i];
			InputValueDefinitionStruct& rDefinition = m_pTask->m_Data.m_aInputValueDefinitions[i];

			CString sGroup(rDefinition.bstrGroup);
			SVRPropertyItem* pGroupItem = NULL;
			if ( (iterGroup = mapGroupItems.find(sGroup)) == mapGroupItems.end() )
			{	// if new group, add tab

				SVExternalToolInputSelectPage* pInputDlg = new SVExternalToolInputSelectPage(sGroup, this);
				AddPage(pInputDlg);
				pInputDlg->m_sGroupName = sGroup;
				pInputDlg->m_bTabbed = bTabbed;
				
				mapGroupItems[sGroup] = pGroupItem;
			}
		}// end for( int i = 0 ; i < m_pTask->m_Data.m_iNumInputValues ; i++ )
	}// end else

	SVExternalToolResultPage* pResultsDlg = new SVExternalToolResultPage(CString(_T("External Tool Results")), this);
	AddPage(pResultsDlg);

	return S_OK;
}

SVIPDoc* SVExternalToolDetailsSheet::GetIPDoc() const
{
	SVIPDoc* l_pIPDoc = NULL;

	if( m_pSVExternalToolDlgParent != NULL )
	{
		l_pIPDoc = m_pSVExternalToolDlgParent->GetIPDoc();
	}
	
	return l_pIPDoc;
}

void SVExternalToolDetailsSheet::DestroyPages()
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
// SVExternalToolDetailsSheet message handlers

BOOL SVExternalToolDetailsSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	

	bool bCancelable = CanCancel();	// from ISVCancel base //SVPropertySheetCancelImpl base
	CWnd* pWnd = GetDlgItem(IDCANCEL);
	if ( pWnd )
		pWnd->ShowWindow( bCancelable ? SW_SHOW : SW_HIDE );


	return bResult;
}

void SVExternalToolDetailsSheet::OnOK() 
{

	int cnt = GetPageCount();

	for( int i = 0;i < cnt; i++ )
	{
		CPropertyPage* pPage = GetPage(i);
		if( pPage && pPage->GetSafeHwnd() ) 
		{
			pPage->OnOK();
		}
	}
	EndDialog(IDOK);
}

bool SVExternalToolDetailsSheet::CanCancel()
{
	ASSERT(m_pTask);
	return m_pTask->CanCancel();
}

HRESULT SVExternalToolDetailsSheet::GetCancelData(SVCancelData*& rpData)
{
	ASSERT(m_pTask);
	return m_pTask->GetCancelData(rpData);
}

HRESULT SVExternalToolDetailsSheet::SetCancelData(SVCancelData* pData)
{
	ASSERT(m_pTask);
	return m_pTask->SetCancelData(pData);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVExternalToolDetailsSheet.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:30:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jul 2012 16:52:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Apr 2009 14:58:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  657
 * SCR Title:  Remove Apply button from the Tool Adjust Dialogs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the apply button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Aug 2005 15:58:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Jul 2004 12:32:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

