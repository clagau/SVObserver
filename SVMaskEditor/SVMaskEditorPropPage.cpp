//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskEditorPropPage
//* .File Name       : $Workfile:   SVMaskEditorPropPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Apr 2013 14:08:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "resource.h"
#include "SVMaskEditorPropPage.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVMaskEditorPropPage, COlePropertyPage)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(SVMaskEditorPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(SVMaskEditorPropPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(SVMaskEditorPropPage, "SVMASKEDITOR.SVMaskEditorPropPage.1",
	0xc87c9b75, 0xe6dc, 0x11d2, 0xa7, 0x7b, 0, 0x10, 0x6f, 0, 0xc, 0x7d)


/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorPropPage::SVMaskEditorPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for SVMaskEditorPropPage

BOOL SVMaskEditorPropPage::SVMaskEditorPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
	{
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(), m_clsid, IDS_SVMASKEDITOR_PPG);
	}
	return AfxOleUnregisterClass(m_clsid, nullptr);
}

/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorPropPage::SVMaskEditorPropPage - Constructor

SVMaskEditorPropPage::SVMaskEditorPropPage() :
	COlePropertyPage(IDD, IDS_SVMASKEDITOR_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(SVMaskEditorPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT

	SetHelpInfo(_T("Names to appear in the control"), _T("SVMASKEDITOR.HLP"), 0);
}

SVMaskEditorPropPage::~SVMaskEditorPropPage()
{
}

/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorPropPage::DoDataExchange - Moves data between page and properties

void SVMaskEditorPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(SVMaskEditorPropPage)
	DDX_Control(pDX, IDC_IMAGE_STATIC, image);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// SVMaskEditorPropPage message handlers



BOOL SVMaskEditorPropPage::OnInitDialog() 
{
	COlePropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here





	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

