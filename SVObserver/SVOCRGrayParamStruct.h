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

#pragma once

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

