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

#pragma region Includes
#include "stdafx.h"
#include "SVExternalToolDetailsSheet.h"

#include "svobserver.h"
#include "SVExternalToolTask.h"
#include "SVOGui\SVExternalToolImageSelectPage.h"
#include "SVExternalToolInputSelectPage.h"
#include "SVExternalToolResultPage.h"
#include "SVExternalToolDlg.h"
#include "CameraLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

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


SVExternalToolDetailsSheet::SVExternalToolDetailsSheet( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID,long numImages, LPCTSTR pszCaption, SVExternalToolDlg* pParentWnd, UINT iSelectPage)
: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
, ISVCancel()
, m_InspectionID(rInspectionID)
, m_TaskObjectID(rTaskObjectID)
, m_numImages(numImages)
{
	m_pSVExternalToolDlgParent = pParentWnd;
	m_pTask = nullptr;
	m_pTool = nullptr;
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

SVExternalToolDetailsSheet::~SVExternalToolDetailsSheet()
{
	DestroyPages();
	m_pTask = nullptr;
	m_pTool = nullptr;
}

HRESULT SVExternalToolDetailsSheet::CreatePages()
{
	ASSERT( m_pTask );
	ASSERT( m_pTool );

	SvOg::SVExternalToolImageSelectPage* pImageDlg = new SvOg::SVExternalToolImageSelectPage(m_InspectionID, m_TaskObjectID, m_numImages);
	AddPage(pImageDlg);

	const bool bTabbed = false;
	if ( !bTabbed )
	{
		SVExternalToolInputSelectPage* pInputDlg = new SVExternalToolInputSelectPage( _T("External Tool Inputs"), this );
		AddPage(pInputDlg);
	}
	else
	{// If Tabbed then find all unique groups and create a page for each group.

		std::map<SVString, SVRPropertyItem*> mapGroupItems;
		std::map<SVString, SVRPropertyItem*>::iterator iterGroup;

		for( int i = 0 ; i < m_pTask->m_Data.m_lNumInputValues ; i++ )
		{
			SVStringValueObjectClass& rName = m_pTask->m_Data.m_aInputObjectNames[i];
			SVVariantValueObjectClass& rValue = m_pTask->m_Data.m_aInputObjects[i];
			InputValueDefinitionStruct& rDefinition = m_pTask->m_Data.m_aInputValueDefinitions[i];

			SVString GroupName = SvUl_SF::createSVString( rDefinition.m_bGroup );
			SVRPropertyItem* pGroupItem = nullptr;
			if ( (iterGroup = mapGroupItems.find(GroupName)) == mapGroupItems.end() )
			{	// if new group, add tab

				SVExternalToolInputSelectPage* pInputDlg = new SVExternalToolInputSelectPage(GroupName.c_str(), this);
				AddPage(pInputDlg);
				pInputDlg->m_sGroupName = GroupName;
				pInputDlg->m_bTabbed = bTabbed;
				
				mapGroupItems[GroupName] = pGroupItem;
			}
		}// end for( int i = 0 ; i < m_pTask->m_Data.m_iNumInputValues ; i++ )
	}// end else

	SVExternalToolResultPage* pResultsDlg = new SVExternalToolResultPage( _T("External Tool Results"), this);
	AddPage(pResultsDlg);

	return S_OK;
}

SVIPDoc* SVExternalToolDetailsSheet::GetIPDoc() const
{
	SVIPDoc* l_pIPDoc = nullptr;

	if( nullptr != m_pSVExternalToolDlgParent )
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

