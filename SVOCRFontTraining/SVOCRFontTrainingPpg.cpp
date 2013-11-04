//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRFontTrainingPropPage
//* .File Name       : $Workfile:   SVOCRFontTrainingPpg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:37:56  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRFontTraining.h"
#include "SVOCRFontTrainingPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSVOCRFontTrainingPropPage, COlePropertyPage)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CSVOCRFontTrainingPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CSVOCRFontTrainingPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CSVOCRFontTrainingPropPage, "SVOCRFONTTRAINING.FontTrainPropPage.1",
	0x9aefbd6a, 0xf061, 0x11d2, 0xa7, 0xb2, 0, 0x10, 0x6f, 0, 0xc, 0x7d)

/////////////////////////////////////////////////////////////////////////////
// CSVOCRFontTrainingPropPage::CSVOCRFontTrainingPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CSVOCRFontTrainingPropPage

BOOL CSVOCRFontTrainingPropPage::CSVOCRFontTrainingPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_SVOCRFONTTRAINING_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRFontTrainingPropPage::CSVOCRFontTrainingPropPage - Constructor

CSVOCRFontTrainingPropPage::CSVOCRFontTrainingPropPage() :
	COlePropertyPage(IDD, IDS_SVOCRFONTTRAINING_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CSVOCRFontTrainingPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRFontTrainingPropPage::DoDataExchange - Moves data between page and properties

void CSVOCRFontTrainingPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CSVOCRFontTrainingPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOCRFontTraining\SVOCRFontTrainingPpg.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 16:37:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/