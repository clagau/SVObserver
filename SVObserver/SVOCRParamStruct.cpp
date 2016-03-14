//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRParamStruct
//* .File Name       : $Workfile:   SVOCRParamStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:00:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRParamStruct.h"
#include "SVOCRAnalyzerResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define __FORMAT_OCR_PARAMS1 _T("%s\n%s\n%s\n%s\n%d %f %f %f %f %d %f %f %f %d %d %d %d %d %d %d %d %d %d")
#define __FORMAT_OCR_PARAMS2 _T("%d %f %f %f %f %d %f %f %f %d %d %d %d %d %d %d %d %d %d")

CString SVOCRParamStruct::ConvertParamsToString()
{
	CString csResult;
	
	csResult.Format( __FORMAT_OCR_PARAMS1,
                   _T( "" ),   // Font File Name converted to a File Name Value Object
                   _T( "" ),   // Match String File Name converted to a File Name Value Object
                   (LPCTSTR)STRnoMatchLabel,
                   (LPCTSTR)STRmatchString,
                   rotation,                  // int
                   forientationSensitivity,   // float
                   fminAngle,                 // float
                   fmaxAngle,                 // float
                   fFeaturesUsed,             // float
                   scaleInvariant,            // int
                   fminScale,                 // float
                   fmaxScale,                 // float
                   fthreshold,                // float
                   ignoreNoMatch,             // int
                   selection, 
                   minWidth,
                   maxWidth,
                   minHeight,
                   maxHeight,
                   samples,
                   useMatchFile,
                   nMinPixelsInBlob,
                   nMaxPixelsInBlob );
	
	return csResult;
}


/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCRParamStruct::ConvertStringToParams(	LPCTSTR pszStrValue )
{
	//
	// Extract the 4 string objects first.
	//
	CString csTemp = pszStrValue;
	
	int nIndex = csTemp.Find((TCHAR)'\n',0);
	if ( nIndex > 0 )
	{
		if ( psvOCRResult != NULL )
		{
			psvOCRResult->msvfnvocFontFileName.SetValue( 1, csTemp.Left( nIndex ) );
		}
	}
	csTemp = csTemp.Right( csTemp.GetLength() - nIndex - 1 );
	
	nIndex = csTemp.Find((TCHAR)'\n',0);
	if ( nIndex > 0 )
	{
		if ( psvOCRResult != NULL )
		{
			psvOCRResult->msvfnvocMatchStringFileName.SetValue( 1, csTemp.Left( nIndex ) );
		}
	}
	csTemp = csTemp.Right( csTemp.GetLength() - nIndex - 1 );
	
	nIndex = csTemp.Find((TCHAR)'\n',0);
	ASSERT(nIndex > -1);
	STRnoMatchLabel = csTemp.Left( nIndex );
	csTemp = csTemp.Right( csTemp.GetLength() - nIndex - 1 );
	
	nIndex = csTemp.Find((TCHAR)'\n',0);
	ASSERT(nIndex > -1);
	STRmatchString = csTemp.Left( nIndex );
	csTemp = csTemp.Right( csTemp.GetLength() - nIndex - 1 );
	
	int nCount;
	nCount = _stscanf( (LPCTSTR)csTemp,            // The string to convert back to values.
		                 __FORMAT_OCR_PARAMS2,
		                 &rotation,                  // int
		                 &forientationSensitivity,   // float
		                 &fminAngle,                 // float
		                 &fmaxAngle,                 // float
		                 &fFeaturesUsed,             // float
		                 &scaleInvariant,            // int
		                 &fminScale,                 // float
		                 &fmaxScale,                 // float
		                 &fthreshold,                // float
		                 &ignoreNoMatch,             // int
		                 &selection, 
		                 &minWidth,
		                 &maxWidth,
		                 &minHeight,
		                 &maxHeight,
		                 &samples,
		                 &useMatchFile,
		                 &nMinPixelsInBlob,
		                 &nMaxPixelsInBlob );
	
	/* JMS - Section start - Added to fix the OCR Geometry problem. */
	
	if (selection != 3)
	{
		selection = 3;
	}
	
	ASSERT(nCount == 19);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
SVOCRParamStruct::~SVOCRParamStruct()
{
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
SVOCRParamStruct::SVOCRParamStruct ()
{
	psvOCRResult = NULL;

	rotation = 1;
	forientationSensitivity = .5;
	fminAngle = -5;
	fmaxAngle = 5;
	fFeaturesUsed = 1;
	scaleInvariant = 1;			// YES
	fminScale = .5;
	fmaxScale = 5;
	fthreshold =(float)0.2;             // 25 Mar 1999 - frb.
	ignoreNoMatch = 0;			// NO
	selection = 3;								// JMS -- 1(Max) + 2(Min) = 3(Both)
	minWidth = 15;                // 8;   10 Apr 1999 - frb.
	maxWidth = 100;               // 0;   10 Apr 1999 - frb.
	minHeight = 20;               // 12;  10 Apr 1999 - frb.
	maxHeight = 100;              // 0;   10 Apr 1999 - frb.
	samples = 2;
	useMatchFile = 0;
	
	STRmatchString  = _T("");    // 01 Nov 1999 - frb.
	STRnoMatchLabel = _T("?");
	
	nMinPixelsInBlob = 20;
	nMaxPixelsInBlob = 1000;
}


SVOCRParamStruct& SVOCRParamStruct::operator=(SVOCRParamStruct &ROCRParameters)
{
	psvOCRResult = ROCRParameters.psvOCRResult;

	STRnoMatchLabel = ROCRParameters.STRnoMatchLabel;
	STRmatchString = ROCRParameters.STRmatchString;
	
	rotation = ROCRParameters.rotation;
	forientationSensitivity = ROCRParameters.forientationSensitivity;
	fminAngle = ROCRParameters.fminAngle;
	fmaxAngle = ROCRParameters.fmaxAngle;
	fFeaturesUsed = ROCRParameters.fFeaturesUsed;
	scaleInvariant = ROCRParameters.scaleInvariant;
	fminScale = ROCRParameters.fminScale;
	fmaxScale = ROCRParameters.fmaxScale;
	fthreshold = ROCRParameters.fthreshold;
	ignoreNoMatch = ROCRParameters.ignoreNoMatch;
	
	selection = ROCRParameters.selection;
	minWidth = ROCRParameters.minWidth;
	maxWidth = ROCRParameters.maxWidth;
	minHeight = ROCRParameters.minHeight;
	maxHeight = ROCRParameters.maxHeight;
	samples = ROCRParameters.samples;
	useMatchFile = ROCRParameters.useMatchFile;
	
	nMinPixelsInBlob = ROCRParameters.nMinPixelsInBlob;
	nMaxPixelsInBlob = ROCRParameters.nMaxPixelsInBlob;
	
	return (*this);
}

