//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRFontTrainingCtrl
//* .File Name       : $Workfile:   SVOCRFontTrainingCtl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:37:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVLVFastOCR.h"
#include "SVOCRFontTraining.h"
#include "SVOCRFontTrainingCtl.h"
#include "SVPropertySheetFontTraining.h"
#include "SVOCRFontTrainingPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////
//abb
//
// Object definitions for FastOCR package. This is a temporary patch
// that can be eliminated with next update to wOCR.dll.
// This string definition, and the CorObjParseStr() function call that
// uses it should both be deleted.  All that is required is the
// currently commented out CorOcrInit(...) function call.//
//
static char ocrObjects[] =
"object SubPixCoeffs\n"
"{\n"
"  uint sums[];\n"
"  uint sqSums[];\n"
"  uint prods[];\n"
"}\n"
"\n"
"object ModelGroup\n"
"{\n"
"  image    mImage[];\n"
"  float    mSums[];\n"
"  float    mVars[];\n"
"  int      mW;\n"
"  int      mH;\n"
"  int      scale;\n"
"}\n"
"\n"
"object PatModel\n"
"{\n"
"  ModelGroup models[];\n"
"  SubPixCoeffs subpix[];\n"
"}\n"
"\n"
"object CharModel \n"
"{\n"
"  String label;\n"
"  float  sigVec[];\n"
"  float  paramVec[];\n"
"}\n"
"\n"
"object FontModel \n"
"{\n"
"  CharModel characters[];\n"
"  float globalParams[];\n"
"}\n"
"\n"
"object FontModel2\n"
"{\n"
"  PatModel patModel;\n"
"  String labels[];\n"
"}\n"
"\n"
"object OcrFont\n"
"{\n"
"  image    images[];\n"
"  String   labels[];\n"
"}\n";

IMPLEMENT_DYNCREATE(CSVOCRFontTrainingCtrl, COleControl)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CSVOCRFontTrainingCtrl, COleControl)
	//{{AFX_MSG_MAP(CSVOCRFontTrainingCtrl)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CSVOCRFontTrainingCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CSVOCRFontTrainingCtrl)
	DISP_FUNCTION(CSVOCRFontTrainingCtrl, "DoSVOCRFontTraining", DoSVOCRFontTraining, VT_I4, VTS_BSTR VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CSVOCRFontTrainingCtrl, COleControl)
	//{{AFX_EVENT_MAP(CSVOCRFontTrainingCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CSVOCRFontTrainingCtrl, 1)
	PROPPAGEID(CSVOCRFontTrainingPropPage::guid)
END_PROPPAGEIDS(CSVOCRFontTrainingCtrl)

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CSVOCRFontTrainingCtrl, "SVOCRFONTTRAINING.FontTrainingCtrl.1",
	0x9aefbd69, 0xf061, 0x11d2, 0xa7, 0xb2, 0, 0x10, 0x6f, 0, 0xc, 0x7d)

/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CSVOCRFontTrainingCtrl, _tlid, _wVerMajor, _wVerMinor)

/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DSVOCRFontTraining =
		{ 0x9aefbd67, 0xf061, 0x11d2, { 0xa7, 0xb2, 0, 0x10, 0x6f, 0, 0xc, 0x7d } };
const IID BASED_CODE IID_DSVOCRFontTrainingEvents =
		{ 0x9aefbd68, 0xf061, 0x11d2, { 0xa7, 0xb2, 0, 0x10, 0x6f, 0, 0xc, 0x7d } };

/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwSVOCRFontTrainingOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CSVOCRFontTrainingCtrl, IDS_SVOCRFONTTRAINING, _dwSVOCRFontTrainingOleMisc)

/////////////////////////////////////////////////////////////////////////////
// CSVOCRFontTrainingCtrl::CSVOCRFontTrainingCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CSVOCRFontTrainingCtrl

BOOL CSVOCRFontTrainingCtrl::CSVOCRFontTrainingCtrlFactory::UpdateRegistry(BOOL bRegister)
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
		                                  IDS_SVOCRFONTTRAINING,
		                                  IDB_SVOCRFONTTRAINING,
		                                  afxRegApartmentThreading,
		                                  _dwSVOCRFontTrainingOleMisc,
		                                  _tlid,
		                                  _wVerMajor,
		                                  _wVerMinor);
	}

	return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRFontTrainingCtrl::CSVOCRFontTrainingCtrl - Constructor

CSVOCRFontTrainingCtrl::CSVOCRFontTrainingCtrl()
{
	InitializeIIDs(&IID_DSVOCRFontTraining, &IID_DSVOCRFontTrainingEvents);
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRFontTrainingCtrl::~CSVOCRFontTrainingCtrl - Destructor

CSVOCRFontTrainingCtrl::~CSVOCRFontTrainingCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRFontTrainingCtrl::OnDraw - Drawing function

void CSVOCRFontTrainingCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRFontTrainingCtrl::DoPropExchange - Persistence support

void CSVOCRFontTrainingCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRFontTrainingCtrl::OnResetState - Reset control to default state

void CSVOCRFontTrainingCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
}

/////////////////////////////////////////////////////////////////////////////
// CSVOCRFontTrainingCtrl message handlers

long CSVOCRFontTrainingCtrl::DoSVOCRFontTraining(LPCTSTR PathFileSampleCharImage, 
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
		AfxMessageBox("ERROR: SVOCRFontTrainingOCX - CorGuiInit Failed");
		PostMessage(WM_COMMAND,IDCANCEL);
	}

	return lResult;
}

