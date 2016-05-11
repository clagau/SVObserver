//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVScrollWnd1Ppg
//* .File Name       : $Workfile:   SVScrollWnd1Ppg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Apr 2013 14:09:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMaskEditor.h"
#include "SVScrollWnd1Ppg.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVScrollWnd1PropPage, COlePropertyPage)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(SVScrollWnd1PropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(SVScrollWnd1PropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(SVScrollWnd1PropPage, "SVSCROLLWND.SVScrollWnd1PropPage.1",
	0x6df99a36, 0xebb8, 0x11d2, 0xa7, 0x59, 0, 0x10, 0x6f, 0, 0xc, 0x7a)

/////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1PropPage::SVScrollWnd1PropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for SVScrollWnd1PropPage

BOOL SVScrollWnd1PropPage::SVScrollWnd1PropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
	{
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(), m_clsid, IDS_SVSCROLLWND1_PPG);
	}
	return AfxOleUnregisterClass(m_clsid, nullptr);
}

SVScrollWnd1PropPage::~SVScrollWnd1PropPage()
{
}

/////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1PropPage::SVScrollWnd1PropPage - Constructor

SVScrollWnd1PropPage::SVScrollWnd1PropPage() :
	COlePropertyPage(IDD, IDS_SVSCROLLWND1_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(SVScrollWnd1PropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1PropPage::DoDataExchange - Moves data between page and properties

void SVScrollWnd1PropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(SVScrollWnd1PropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}

