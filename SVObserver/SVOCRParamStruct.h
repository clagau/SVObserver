//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRParamStruct
//* .File Name       : $Workfile:   SVOCRParamStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:00:20  $
//******************************************************************************

#pragma once

class SVOCRAnalyzeResultClass;

struct SVOCRParamStruct 
{
	SVOCRParamStruct ();
  virtual ~SVOCRParamStruct();

	SVOCRParamStruct& operator=(SVOCRParamStruct &ROCRParameters);

	CString ConvertParamsToString();
	void ConvertStringToParams( LPCTSTR pszStrValue );

	//
	// Attributes
	//
	//  0 = In-variant; 1 = Variant, or 2 = Range of angles.
	int			rotation;

	//  0 to 1. Weighting of rotationally invariant features vs.
	//  rotationally non-invariant features.  0 = invariant 
	//  features only.
	float		forientationSensitivity;

	//  0 to 360 degrees.  Actually takes modulo 360.
	float		fminAngle;

	//  0 to 360 degrees.  Actually takes modulo 360.
	float		fmaxAngle;

	//  0 to 1.  Represents the fraction of total features which
	//  will be used for ocr analysis.
	float	    fFeaturesUsed;	

	//  Yes/No (1/0?).
	int			scaleInvariant;

	
	float		fminScale;

	float		fmaxScale;

	//  0 to 1.  0 means every blob will be assigned a match.
	float		fthreshold;

	//  Yes/No (1/0?). Yes = if a blob is not matched, no 
	//  corresponding character is referenced in the returned
	//  resultStringVector.
	int			ignoreNoMatch;

	//	If not matched blobs are not ignored, than this 
	//	represents the string which will be placed in the 
	//  returned resultStringVector to represent all unmatched
	//  blobs.
	CString 	STRnoMatchLabel;

	//  The selection parameter determines whether a minimum 
	//  and/or maximum width and height will be specified for 
	//  blobs extracted from the image ROIs. Blobs outside the 
	//	range are always ignored.
	//  0 = No selection
	//  1 = max size
	//  2 = min size
	//  3 = both min and max size
	int			selection;

	int			minWidth;
	int			maxWidth;
	int			minHeight;
	int			maxHeight;

	//  samples represents the number of points(pixels) from
	//	the font BMP images which will be used to build the 
	//  font model.  The higher the number, the more accurate the
	//  calulation, and the more time for processing.  This is an
	//  enumeration of the actual value.
	//	0 = 8
	//  1 = 16
	//  2 = 32
	//  3 = 64
	//  4 = 128
	//  5 = 256
	int			samples;

	//	0 = Do not use match file, which means use stored strings.
	//  1 = Use match string file.
	int			useMatchFile;
	
	CString     STRmatchString;

	//
	// The following added as hard code - 14 Apr 1999 - frb.
	// to accomodate ocrExecute() changes per Logical Vision
	//
	int nMinPixelsInBlob;
	int nMaxPixelsInBlob;

	SVOCRAnalyzeResultClass *psvOCRResult;
};

