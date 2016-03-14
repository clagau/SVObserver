//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRGrayFontTrainCtrl
//* .File Name       : $Workfile:   SVOCRGrayFontTrainCtl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 10:30:46  $
//******************************************************************************

#include "stdafx.h"
#include "SVLVFastOCR.h"
#include "SVOCRGrayFontTrain.h"
#include "SVOCRGrayFontTrainCtl.h"
#include "SVPropertySheetFontTraining.h"
#include "SVOCRGrayFontTrainPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSVOCRGrayFontTrainCtrl, COleControl)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CSVOCRGrayFontTrainCtrl, COleControl)
	//{{AFX_MSG_MAP(CSVOCRGrayFontTrainCtrl)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CSVOCRGrayFontTrainCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CSVOCRGrayFontTrainCtrl)
	DISP_FUNCTION(CSVOCRGrayFontTrainCtrl, "DoSVOCRGrayFontTrain", DoSVOCRGrayFontTrain, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CSVOCRGrayFontTrainCtrl, "SetLVLastCallBackErrorCode", SetLVLastCallBackErrorCode, VT_BOOL, VTS_PI4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CSVOCRGrayFontTrainCtrl, COleControl)
	//{{AFX_EVENT_MAP(CSVOCRGrayFontTrainCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CSVOCRGrayFontTrainCtrl, 1)
	PROPPAGEID(CSVOCRGrayFontTrainPropPage::guid)
END_PROPPAGEIDS(CSVOCRGrayFontTrainCtrl)

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CSVOCRGrayFontTrainCtrl, "SVOCRGRAYFONTTRAIN.FontTrainingCtrl.1",
  0x29BF4CF3, 0x27AF, 0x11d4, 0xA8, 0xBA, 0x00, 0x10, 0x6F, 0x03, 0x09, 0xAB)

/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CSVOCRGrayFontTrainCtrl, _tlid, _wVerMajor, _wVerMinor)

/////////////////////////////////////////////////////////////////////////////
// Interface IDs

// {E3CAE512-22AC-11d4-A8B8-00106F0309AB}
const IID BASED_CODE IID_DSVOCRGrayFontTraining =
{ 0x29BF4CF1, 0x27AF, 0x11d4, { 0xA8, 0xBA, 0x00, 0x10, 0x6F, 0x03, 0x09, 0xAB} };
const IID BASED_CODE IID_DSVOCRGrayFontTrainingEvents =
{ 0x29BF4CF2, 0x27AF, 0x11d4, { 0xA8, 0xBA, 0x00, 0x10, 0x6F, 0x03, 0x09, 0xAB} };

/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwSVOCRGrayFontTrainOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CSVOCRGrayFontTrainCtrl, IDS_SVOCRGRAYFONTTRAIN, _dwSVOCRGrayFontTrainOleMisc)

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrainCtrl::CSVOCRGrayFontTrainCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CSVOCRGrayFontTrainCtrl

BOOL CSVOCRGrayFontTrainCtrl::CSVOCRGrayFontTrainCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
	{
		return AfxOleRegisterControlClass(AfxGetInstanceHandle(),
		                                  m_clsid,
		                                  m_lpszProgID,
		                                  IDS_SVOCRGRAYFONTTRAIN,
		                                  IDB_SVOCRGRAYFONTTRAIN,
		                                  afxRegApartmentThreading,
		                                  _dwSVOCRGrayFontTrainOleMisc,
		                                  _tlid,
		                                  _wVerMajor,
		                                  _wVerMinor);
	}

	return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

int glvTemp;
int *gplvLastOpStatus;

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrainCtrl::CSVOCRGrayFontTrainCtrl - Constructor

CSVOCRGrayFontTrainCtrl::CSVOCRGrayFontTrainCtrl()
{
	InitializeIIDs(&IID_DSVOCRGrayFontTraining, &IID_DSVOCRGrayFontTrainingEvents);

	glvTemp = 0;
	gplvLastOpStatus = &glvTemp;
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrainCtrl::~CSVOCRGrayFontTrainCtrl - Destructor

CSVOCRGrayFontTrainCtrl::~CSVOCRGrayFontTrainCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrainCtrl::OnDraw - Drawing function

void CSVOCRGrayFontTrainCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrainCtrl::DoPropExchange - Persistence support

void CSVOCRGrayFontTrainCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrainCtrl::OnResetState - Reset control to default state

void CSVOCRGrayFontTrainCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrainCtrl message handlers

long CSVOCRGrayFontTrainCtrl::DoSVOCRGrayFontTrain(LPCTSTR PathFileSampleCharImage, 
                                                 LPCTSTR PathFileFontImagesAndLabels) 
{
	long lResult = -1;

	SVLVFastOCR FastOCR;

	if (FastOCR.Init(GetSafeHwnd()))
	{
		//
		// Do a property sheet dialog or a standard dialog.
		//
		CSVPropertySheetFontTraining dlg("SVObserver Font Training");

		//
		// Set the parameters to use.
		//
		dlg.m_csPathFileFontParameters = PathFileFontImagesAndLabels;
		dlg.m_csPathFileInputImage = PathFileSampleCharImage;
		dlg.m_pFastOCR = &FastOCR;

		switch (dlg.DoModal())
		{
			case IDOK:
			{
				lResult = 0;
				break;
			}
			default:
			case IDCANCEL:
			{
				lResult = 0;
				break;
			}
		}
	}
	else
	{
		AfxMessageBox("ERROR: SVOCRGrayFontTrainOCX - CorGuiInit Failed");
		PostMessage(WM_COMMAND,IDCANCEL);
	}

	return lResult;
}

BOOL CSVOCRGrayFontTrainCtrl::SetLVLastCallBackErrorCode(long FAR* Code) 
{
	gplvLastOpStatus = (int *)Code;

	return TRUE;
}

