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

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "CSVOCRGrayFontTrain.h"

namespace Seidenader
{
	namespace SVMFCControls
	{
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
	} //SVMFCControls
} //Seidenader

