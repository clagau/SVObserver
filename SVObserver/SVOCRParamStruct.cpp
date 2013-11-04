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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRParamStruct.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:00:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Aug 2005 07:51:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 12:53:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Jun 2001 09:17:04   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: ConvertParamsToString, ConvertStringToParams,
 * SVOCRParamStruct, ~SVOCRParamStruct, operator=.
 * 
 * Deleted the following functions: 	SelectFontFileName,  SelectMatchStringFileName,
 * GetFontFileName, GetMatchStringFileName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Apr 2001 11:41:40   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modifications made to the way the following variables are handled
 * svfncFontFileName and svfncMatchStringFileName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Feb 2001 13:31:56   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 May 2000 10:15:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  149
 * SCR Title:  Fix OCR Geometry Maximum Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed the selection parameter from 2 to 3 in all instances.  Added two new files SVOCRParamStruct.h and SVOCRParamStruct.cpp to move the SVOCRParamStruct to its own file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/