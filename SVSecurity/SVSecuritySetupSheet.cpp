// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
//******************************************************************************
//* .Module Name     : SecuritySetupSheet
//* .File Name       : $Workfile:   SVSecuritySetupSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 10:00:50  $
// ******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <lm.h>
#include "SVSecuritySetupSheet.h"
#include "SVStatusLibrary/SVRegistry.h"
#include "resource.h"
#include "SVAccessClass.h"
#include "SVSecuritySetupPage.h"
#include "SVSecurityGeneralPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(SVSecuritySetupSheet, CPropertySheet)

void SVSecuritySetupSheet::Initialize(void)
{
	m_psh.dwFlags &= ~PSH_HASHELP;
}

SVSecuritySetupSheet::SVSecuritySetupSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	Initialize();
}

SVSecuritySetupSheet::SVSecuritySetupSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	Initialize();
}

SVSecuritySetupSheet::SVSecuritySetupSheet()
	:CPropertySheet(_T("SECURITY SETUP"), nullptr, 0)
{
	Initialize();
}

SVSecuritySetupSheet::~SVSecuritySetupSheet()
{
}

BEGIN_MESSAGE_MAP(SVSecuritySetupSheet, CPropertySheet)
	//{{AFX_MSG_MAP(SVSecuritySetupSheet)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL SVSecuritySetupSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	CRect	rect;
	GetDlgItem(IDOK)->GetClientRect(&rect);
	GetDlgItem(IDCANCEL)->GetClientRect(&rect);
	GetDlgItem(ID_APPLY_NOW)->GetClientRect(&rect);
	
	return bResult;
}

INT_PTR SVSecuritySetupSheet::DoModal() 
{
	SVSecuritySetupPage	page;
	AddPage(&page);

	SVSecurityGeneralPage GenPage;
	AddPage(&GenPage);
	
	INT_PTR result = CPropertySheet::DoModal();

	return result;
}

void SVSecuritySetupSheet::InitializeGroupList(CListBox& list)
{
	DWORD			nas;
	PGROUP_INFO_0	pBuf = nullptr;
	PGROUP_INFO_0	pTmpBuf = nullptr;
	DWORD			entriesread = 0;
	DWORD			totalentries = 0;
	PDWORD_PTR		resume_handle = 0;

	list.AddString( _T( "Everybody" ) );
	nas = NetLocalGroupEnum( nullptr, 0, ( LPBYTE* )&pBuf, -1, &entriesread, &totalentries, resume_handle );
	if (nas == NERR_Success)
	{
		if ( nullptr !=  (pTmpBuf = pBuf) )
		{
			for (int i=0; i < (int)entriesread; i++)
			{
				CString group = pTmpBuf->grpi0_name;
				list.AddString(group);
				pTmpBuf++;
			}
		}
	}
}

BOOL SVSecuritySetupSheet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	return CPropertySheet::OnCommand(wParam, lParam);
}

void SVSecuritySetupSheet::OnSize(UINT nType, int cx, int cy) 
{
	CPropertySheet::OnSize(nType, cx, cy);
}

