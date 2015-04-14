//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRFontTraining
//* .File Name       : $Workfile:   CSVOCRFontTraining.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:12:50  $
//******************************************************************************

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "CSVOCRFontTraining.h"

namespace Seidenader
{
	namespace SVMFCControls
	{
		IMPLEMENT_DYNCREATE(CSVOCRFontTraining, CWnd)

			/////////////////////////////////////////////////////////////////////////////
			// CSVOCRFontTraining properties

			/////////////////////////////////////////////////////////////////////////////
			// CSVOCRFontTraining operations

			long CSVOCRFontTraining::DoSVOCRFontTraining(LPCTSTR PathFileSampleCharImage, LPCTSTR PathFileFontImagesAndLabels)
		{
			long result;
			static BYTE parms[] =
				VTS_BSTR VTS_BSTR;
			InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
				PathFileSampleCharImage, PathFileFontImagesAndLabels);
			return result;
		}
	} //SVMFCControls
} //Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\CSVOCRFontTraining.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:12:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Apr 2003 15:51:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:38:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/