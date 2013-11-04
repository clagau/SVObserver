//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRGrayFontTrain
//* .File Name       : $Workfile:   CSVOCRGrayFontTrain.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:13:04  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////


//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

#include "CSVOCRGrayFontTrain.h"

/////////////////////////////////////////////////////////////////////////////
//
//
// Font Training OCX 
//
//
/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrain

IMPLEMENT_DYNCREATE(CSVOCRGrayFontTrain, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrain properties

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrain operations

long CSVOCRGrayFontTrain::DoSVOCRGrayFontTrain(LPCTSTR PathFileSampleCharImage, LPCTSTR PathFileFontImagesAndLabels)
{
	long result;
	static BYTE parms[] =	VTS_BSTR VTS_BSTR;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		PathFileSampleCharImage, PathFileFontImagesAndLabels);
	return result;
}

BOOL CSVOCRGrayFontTrain::SetLVLastCallBackErrorCode(long *Code)
{
	BOOL result;
	static BYTE parms[] =	VTS_PI4;

	InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Code);

	return result;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\CSVOCRGrayFontTrain.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:13:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Apr 2003 15:51:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Aug 2000 12:52:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  173
 * SCR Title:  Fix OCR Font Training - Pressing ROI Ok button when no ROI is selected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added code to pass the global variable containing the FastOCR error code into the Gray Font Training OCX.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:38:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/