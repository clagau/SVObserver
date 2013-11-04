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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRGrayParamStruct.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:59:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Aug 2005 07:35:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 12:53:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Oct 2001 16:54:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Jun 2001 09:02:08   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: ConvertParamsToString, ConvertStringToParams,
 * ~SVOCRGrayParamStruct, SVOCRGrayParamStruct , operator=. 
 * 
 * Deleted the following functions: 	SelectFontFileName,  SelectMatchStringFileName,
 * GetFontFileName, GetMatchStringFileName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Apr 2001 11:28:44   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modifications to the handling of the variables
 * svfncFontFileName and svfncMatchStringFileName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Feb 2001 13:31:48   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Aug 2000 16:05:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  177
 * SCR Title:  Fix Automatic Load Last SEC Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update FastOCR to prevent memory access violations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/