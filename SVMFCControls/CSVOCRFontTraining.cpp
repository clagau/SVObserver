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

