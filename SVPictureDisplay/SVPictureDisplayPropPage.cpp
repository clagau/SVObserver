//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureDisplayPropPage
//* .File Name       : $Workfile:   SVPictureDisplayPropPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:27:56  $
//******************************************************************************
#include "stdafx.h"
#include "SVPictureDisplay.h"
#include "SVPictureDisplayPropPage.h"

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

IMPLEMENT_DYNCREATE(SVPictureDisplayPropPage, COlePropertyPage)

// Message map

BEGIN_MESSAGE_MAP(SVPictureDisplayPropPage, COlePropertyPage)
END_MESSAGE_MAP()

// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(SVPictureDisplayPropPage, "SVPICTUREDISPL.SVPictureDisplPropPage.1",
	0xc3b07479, 0xe7fa, 0x4177, 0x81, 0xb, 0xc7, 0x4b, 0xb0, 0x89, 0x4f, 0xa2)

// SVPictureDisplayPropPage::SVPictureDisplayPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for SVPictureDisplayPropPage

BOOL SVPictureDisplayPropPage::SVPictureDisplayPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_SVPICTUREDISPLAY_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, nullptr);
}

// SVPictureDisplayPropPage::SVPictureDisplayPropPage - Constructor

SVPictureDisplayPropPage::SVPictureDisplayPropPage() :
	COlePropertyPage(IDD, IDS_SVPICTUREDISPLAY_PPG_CAPTION)
{
}

SVPictureDisplayPropPage::~SVPictureDisplayPropPage()
{
}

// SVPictureDisplayPropPage::DoDataExchange - Moves data between page and properties

void SVPictureDisplayPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}

// SVPictureDisplayPropPage message handlers

