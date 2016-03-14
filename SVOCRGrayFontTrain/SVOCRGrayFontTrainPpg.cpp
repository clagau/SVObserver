//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRGrayFontTrainPropPage
//* .File Name       : $Workfile:   SVOCRGrayFontTrainPpg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 10:31:16  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRGrayFontTrain.h"
#include "SVOCRGrayFontTrainPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSVOCRGrayFontTrainPropPage, COlePropertyPage)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CSVOCRGrayFontTrainPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CSVOCRGrayFontTrainPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CSVOCRGrayFontTrainPropPage, "SVOCRGRAYFONTTRAIN.FontTrainPropPage.1",
	0x9aefbd6a, 0xf061, 0x11d2, 0xa7, 0xb2, 0, 0x10, 0x6f, 0, 0xc, 0x7d)

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrainPropPage::CSVOCRGrayFontTrainPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CSVOCRGrayFontTrainPropPage

BOOL CSVOCRGrayFontTrainPropPage::CSVOCRGrayFontTrainPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_SVOCRGRAYFONTTRAIN_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrainPropPage::CSVOCRGrayFontTrainPropPage - Constructor

CSVOCRGrayFontTrainPropPage::CSVOCRGrayFontTrainPropPage() :
	COlePropertyPage(IDD, IDS_SVOCRGRAYFONTTRAIN_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CSVOCRGrayFontTrainPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrainPropPage::DoDataExchange - Moves data between page and properties

void CSVOCRGrayFontTrainPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CSVOCRGrayFontTrainPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}

