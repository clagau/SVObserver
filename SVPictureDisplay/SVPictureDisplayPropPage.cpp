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
		return AfxOleUnregisterClass(m_clsid, NULL);
}

// SVPictureDisplayPropPage::SVPictureDisplayPropPage - Constructor

SVPictureDisplayPropPage::SVPictureDisplayPropPage() :
	COlePropertyPage(IDD, IDS_SVPICTUREDISPLAY_PPG_CAPTION)
{
}

// SVPictureDisplayPropPage::DoDataExchange - Moves data between page and properties

void SVPictureDisplayPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}

// SVPictureDisplayPropPage message handlers

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\SVPictureDisplayPropPage.cpp_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:27:56   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Mar 2010 13:25:28   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
