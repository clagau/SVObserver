// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
//******************************************************************************
//* .Module Name     : SecuritySetupSheet
//* .File Name       : $Workfile:   SVSecuritySetupSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:04:08  $
// ******************************************************************************

#include "stdafx.h"
#include "SVSecuritySetupSheet.h"
#include <lm.h>
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
	:CPropertySheet(_T("SECURITY SETUP"), NULL, 0)
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

int SVSecuritySetupSheet::DoModal() 
{
	SVSecuritySetupPage	page;
	AddPage(&page);

	SVSecurityGeneralPage GenPage;
	AddPage(&GenPage);
	
	int result = CPropertySheet::DoModal();
	if (result == IDOK)
	{
		// Save
	}

	return result;
}

void SVSecuritySetupSheet::InitializeGroupList(CListBox& list)
{
	NET_API_STATUS	nas;
	PGROUP_INFO_0	pBuf = NULL;
	PGROUP_INFO_0	pTmpBuf = NULL;
	DWORD			entriesread = 0;
	DWORD			totalentries = 0;
	DWORD			resume_handle = 0;

	list.AddString(_T("Everybody"));
	nas = NetLocalGroupEnum(NULL, 0, (LPBYTE*)&pBuf, -1, &entriesread, &totalentries, &resume_handle);
	if (nas == NERR_Success)
	{
		if ((pTmpBuf = pBuf) != NULL)
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSecurity\SVSecuritySetupSheet.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:04:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Sep 2012 18:43:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:35:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2009 10:55:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 15:10:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Jun 2005 09:28:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
