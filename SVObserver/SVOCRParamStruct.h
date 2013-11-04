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

#ifndef SVOCRPARAMSTRUCT_H
#define SVOCRPARAMSTRUCT_H

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

#endif   /* SVOCRPARAMSTRUCT_H */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRParamStruct.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:00:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Jun 2010 10:59:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 12:52:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Jun 2001 09:17:06   ed
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
 *    Rev 1.3   12 Apr 2001 11:41:34   cSchmittinger
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
 *    Rev 1.2   06 Feb 2001 13:31:36   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Aug 2000 16:05:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  177
 * SCR Title:  Fix Automatic Load Last SEC Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update FastOCR to prevent memory access violations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 May 2000 10:14:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  149
 * SCR Title:  Fix OCR Geometry Maximum Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed the selection parameter from 2 to 3 in all instances.  Added two new files SVOCRParamStruct.h and SVOCRParamStruct.cpp to move the SVOCRParamStruct to its own file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
