//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayParamStruct
//* .File Name       : $Workfile:   SVOCRGrayParamStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:59:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRGrayParamStruct.h"
#include "SVOCRGrayAnalyzerResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define __FORMAT_OCRGRAY_PARAMS1 _T("%s\n%s\n%s\n%s\n%d %d %d %d %f %f %f %f %d %d %d %f %f %d")
#define __FORMAT_OCRGRAY_PARAMS2 _T("%d %d %d %d %f %f %f %f %d %d %d %f %f %d")

CString SVOCRGrayParamStruct::ConvertParamsToString()
{
	CString csResult;
	
	csResult.Format(__FORMAT_OCRGRAY_PARAMS1,
	                _T( "" ),
	                _T( "" ),
									STRnoMatchLabel,
									STRmatchString,
									useMatchFile,				// int
									maxMatches,				// int
									xVicinity,				// int
									yVicinity,				// int
									rejectThresh,			// float
									acceptThresh,			// float
									relThresh,				// float
									minContrast,			// float
									scale,					// int
									maxCandidates,			// int
									candsInVicinity,		// int
									candidateThresh,		// float
									candidateRelThresh,		// float
									output);
	
	return csResult;
}


/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCRGrayParamStruct::ConvertStringToParams(LPCTSTR pszStrValue)
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
			psvOCRResult->msvfnvocGrayFontFileName.SetValue( 1, csTemp.Left( nIndex ) );
		}
	}
	csTemp = csTemp.Right( csTemp.GetLength() - nIndex - 1 );
	
	nIndex = csTemp.Find((TCHAR)'\n',0);
	if ( nIndex > 0 )
	{
		if ( psvOCRResult != NULL )
		{
			psvOCRResult->msvfnvocGrayMatchStringFileName.SetValue( 1, csTemp.Left( nIndex ) );
		}
	}
	csTemp = csTemp.Right( csTemp.GetLength() - nIndex - 1 );
	
	nIndex = csTemp.Find((TCHAR)'\n',0);
	ASSERT(nIndex > -1);
	STRnoMatchLabel = csTemp.Left(nIndex);
	csTemp = csTemp.Right(csTemp.GetLength() - nIndex -1);
	
	nIndex = csTemp.Find((TCHAR)'\n',0);
	ASSERT(nIndex > -1);
	STRmatchString = csTemp.Left(nIndex);
	csTemp = csTemp.Right(csTemp.GetLength() - nIndex -1);
	
	int nCount = _stscanf(csTemp,
												__FORMAT_OCRGRAY_PARAMS2,
												&useMatchFile,
												&maxMatches,
												&xVicinity,
												&yVicinity,
												&rejectThresh,
												&acceptThresh,
												&relThresh,
												&minContrast,
												&scale,
												&maxCandidates,
												&candsInVicinity,
												&candidateThresh,
												&candidateRelThresh,
												&output);
	
	ASSERT(nCount == 14);
}


/////////////////////////////////////////////////////////////////////////////
//
//
//
SVOCRGrayParamStruct::~SVOCRGrayParamStruct()
{
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
SVOCRGrayParamStruct::SVOCRGrayParamStruct ()
{
	psvOCRResult = NULL;

	STRnoMatchLabel = _T("?");
	STRmatchString = _T("");
	
	useMatchFile				= 0;
	maxMatches				= 15;
	xVicinity				= 24;
	yVicinity				= 36;
	rejectThresh			= 0.5F;
	acceptThresh			= 1.0F;
	relThresh				= 0.0F;
	minContrast				= 0.2F;
	scale					= 1;
	maxCandidates			= 400;
	candsInVicinity			= 4;
	candidateThresh			= 0.3F;
	candidateRelThresh		= 0.5F;
	output					= 0;
}


SVOCRGrayParamStruct& SVOCRGrayParamStruct::operator=(SVOCRGrayParamStruct &rhs)
{
	psvOCRResult              = rhs.psvOCRResult;

	STRnoMatchLabel						= rhs.STRnoMatchLabel;
	STRmatchString						= rhs.STRmatchString;
	
	useMatchFile							= rhs.useMatchFile;
	maxMatches								= rhs.maxMatches;
	xVicinity									= rhs.xVicinity;
	yVicinity									= rhs.yVicinity;
	rejectThresh							= rhs.rejectThresh;
	acceptThresh							= rhs.acceptThresh;
	relThresh									= rhs.relThresh;
	minContrast								= rhs.minContrast;
	scale											= rhs.scale;
	maxCandidates							= rhs.maxCandidates;
	candsInVicinity						= rhs.candsInVicinity;
	candidateThresh						= rhs.candidateThresh;
	candidateRelThresh				= rhs.candidateRelThresh;
	output										= rhs.output;

	return *this;
}

