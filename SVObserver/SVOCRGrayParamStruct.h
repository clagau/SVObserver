//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayParamStruct
//* .File Name       : $Workfile:   SVOCRGrayParamStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:59:58  $
//******************************************************************************

#ifndef SVOCRGRAYPARAMSTRUCT_H
#define SVOCRGRAYPARAMSTRUCT_H

class SVOCRGrayAnalyzeResultClass;

struct SVOCRGrayParamStruct 
{
	SVOCRGrayParamStruct ();
  virtual ~SVOCRGrayParamStruct();

	SVOCRGrayParamStruct& operator=(SVOCRGrayParamStruct &rhs);

	CString ConvertParamsToString();
	void ConvertStringToParams(LPCTSTR pszStrValue);

	CString 				STRnoMatchLabel;
	CString					STRmatchString;
	
	int							useMatchFile;
	int							maxMatches;
	int							xVicinity;
	int							yVicinity;
	float						rejectThresh;
	float						acceptThresh;
	float						relThresh;
	float						minContrast;
	int							scale;
	int							maxCandidates;
	int							candsInVicinity;
	float						candidateThresh;
	float						candidateRelThresh;
	int							output;

	SVOCRGrayAnalyzeResultClass *psvOCRResult;
};

#endif   // SVOCRGRAYPARAMSTRUCT_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRGrayParamStruct.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:59:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Jun 2010 10:59:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 12:53:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Jun 2001 09:02:12   ed
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
 *    Rev 1.2   12 Apr 2001 11:28:42   cSchmittinger
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
 *    Rev 1.1   06 Feb 2001 13:31:52   cSchmittinger
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
