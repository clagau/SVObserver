//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRAnalyzerResult
//* .File Name       : $Workfile:   SVOCRAnalyzerResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 May 2013 14:38:28  $
//******************************************************************************
#ifndef SV_OCR_ANALYZER_RESULT_H
#define SV_OCR_ANALYZER_RESULT_H

//******************************************************************************
// Include(s):
//******************************************************************************

#include "SVObjectLibrary/SVLockableClass.h"
#include "SVLVFastOCR.h"
#include "SVOCRBlobRecord.h"
#include "SVOCRParamStruct.h"
#include "SVResult.h"

//******************************************************************************
// Define(s):
//******************************************************************************

#define  MAX_ENTREES       10000  // Max number of lines allowed in the match file
#define  CAR_RETURN        0x0D

//******************************************************************************
// Class Definition:
//******************************************************************************
class SVOCRAnalyzeResultClass : public SVResultClass
{
//******************************************************************************
// Class Macros(s):
//******************************************************************************
protected:
	SV_DECLARE_CLASS( SVOCRAnalyzeResultClass )

//******************************************************************************
// Operation(s):
//******************************************************************************
public:
	SVOCRAnalyzeResultClass( SVObjectClass* POwner = NULL, 
		                       int StringResourceID = IDS_CLASSNAME_SVOCRANALYZERESULT );

	~SVOCRAnalyzeResultClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual CRect Draw( HDC DC, CRect R );
	virtual BOOL OnValidate();

	BOOL GetFontFileName( CString & csName );
	BOOL GetMatchStringFileName( CString & csName );

	SVOCRParamStruct& GetOCRParameters();
	void GetOCRResultString( CString & csResult );
	void UpdateOCRScriptString();
	void WriteToArchive(CFile & fileArchive);

	BOOL GenerateFontModel();
	HRESULT LoadMatchString();

	BOOL BuildHashTable(char *pBuffer);
	void InsertValueToTable(short nValue, int index);
	int CheckStringInTable(CString MatchString);

	SVImageClass* getInputImage();

	virtual HRESULT ResetObject();

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext );

private:
	void clearAll();
	BOOL isWitCompleteLoaded();

//******************************************************************************
// Attribute(s):
//******************************************************************************
public:
	SVLVFastOCR svlvFastOcr;

	SVOCRBlobRecordArray arrayOCRBlobRecords;

	//
	// Blob sort direction - right,left,up,down
	//
	SVLongValueObjectClass valueBlobSortDirection;

	//
	// Number of pixels between 'blobs' for which a larger space implies a
	// space in the char blobs being analyzed.
	SVLongValueObjectClass valuePixelsForASpace;

	// Exposing OCR Match Scores
	SVDoubleValueObjectClass msvdvocHighestMatchScore;
	SVDoubleValueObjectClass msvdvocLowestMatchScore;
	SVDoubleValueObjectClass msvdvocAverageMatchScore;

	SVFileNameValueObjectClass msvfnvocMatchStringFileName;
	SVFileNameValueObjectClass msvfnvocFontFileName;

protected:

	long     m_lTotalBytes;
	char     *m_pBuffer;
	long     m_lLowValue;
	long     m_lHighValue;
	int      m_nTotalCount;
	char     *m_pDataArr[MAX_ENTREES];
	short    *m_pIndexTable;
	double   m_dFactor;

	SVLockableClass lock;

	// Input: Image
	SVInObjectInfoStruct inputImageObjectInfo;

  //
	// The OCR parameters
	//
	SVOCRParamStruct ocrParameters;

  //
	// A string value to represent all the OCR parameters so they can
	// be stored and retrieved from storage.
	// StrOcrParameters is converted to ocrParameters and 
	// back again as required.
	//
	SVStringValueObjectClass   StrOcrParameters;
	
	// *** // ***
	// New value objects instead of parameterized string value object
	SVStringValueObjectClass	m_voStrNoMatchLabel;
	SVStringValueObjectClass	m_voStrMatchString;

	SVLongValueObjectClass		m_voRotation;
	SVLongValueObjectClass		m_voScaleInvariant;
	SVLongValueObjectClass		m_voIgnoreNoMatch;
	SVLongValueObjectClass		m_voSelection;
	SVLongValueObjectClass		m_voMinWidth;
	SVLongValueObjectClass		m_voMaxWidth;
	SVLongValueObjectClass		m_voMinHeight;
	SVLongValueObjectClass		m_voMaxHeight;
	SVLongValueObjectClass		m_voSamples;
	SVLongValueObjectClass		m_voUseMatchFile;
	SVLongValueObjectClass		m_voMinPixelsInBlob;
	SVLongValueObjectClass		m_voMaxPixelsInBlob;

	SVDoubleValueObjectClass	m_voOrientationSensitivity;
	SVDoubleValueObjectClass	m_voMinAngle;
	SVDoubleValueObjectClass	m_voMaxAngle;
	SVDoubleValueObjectClass	m_voFeaturesUsed;
	SVDoubleValueObjectClass	m_voMinScale;
	SVDoubleValueObjectClass	m_voMaxScale;
	SVDoubleValueObjectClass	m_voThreshold;
	// *** // ***

	// Output
	SVStringValueObjectClass matchString;
	SVStringValueObjectClass foundString;

	// line number of the Match string.
	SVLongValueObjectClass msvlMatchLineNumber;

	//
	//  Wit Objects
	//
	
	//	Generated by call to ocrInitialize.
	CorFontModel *pOCRFontModel;	

	//  Array of individual characters found by cor_ocrPerimRun.
	CorStringVector	*presultStringVector;
	CorFpointVector	*plocationVector;
	CorFloatVector	*plvcfvMatchScoreVector;
	CorObj			*pROIObj;

	//  Wit wrapper for noMatchLabel.
	char *witNoMatchLabel;

private:
	bool m_bHasLicenseError;

};

#endif   // SV_OCR_ANALYZER_RESULT_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRAnalyzerResult.h_v  $
 * 
 *    Rev 1.1   06 May 2013 14:38:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   10 Apr 2013 14:53:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   put in check for FastOCRLicense
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:54:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   27 Feb 2013 11:25:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   put in checks to see if it has license errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   25 Feb 2011 12:22:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   01 Jun 2010 14:57:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   10 Aug 2005 12:40:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   09 Aug 2005 07:51:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   17 Feb 2005 15:01:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   08 Jul 2004 10:38:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  431
 * SCR Title:  Fix problem with using match string files that are zero elements
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   put in check to make sure that if using a match string file that the number of elements is above 0.  if not, the inspection will not be allowed to go online
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   19 Jan 2004 10:02:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update header information to make getInputImage method public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   30 Apr 2003 17:02:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed a couple of small memory leaks in the ::onRun.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   22 Apr 2003 12:42:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   20 Nov 2002 10:05:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   New value objects instead of parameterized string value object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Jul 2001 20:42:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed two class variables: pROIObj and plvImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Jul 2001 20:42:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed two class variables: pROIObj and plvImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Jul 2001 20:39:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Jul 2001 20:38:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed two class variables: pROIObj and plvImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Jul 2001 20:37:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed two class variables: pROIObj and plvImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Jul 2001 20:36:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed two class variables: pROIObj and plvImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Jul 2001 20:35:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed two class variables: pROIObj and plvImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Jul 2001 20:35:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed two class variables: pROIObj and plvImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Jul 2001 20:35:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed two class variables: pROIObj and plvImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Jul 2001 20:34:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed two class variables: pROIObj and plvImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   13 Jun 2001 08:22:46   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: SVOCRAnalyzeResultClass, clearAll, addDefaultInputOutputObjects, CreateObject, GenerateFontModel, LoadMatchString, processMessage, onRun.
 * 
 * Added the following functions: GetFontFileName, GetMatchStringFileName, SetFontFileName, SetMatchStringFileName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   10 May 2001 15:46:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  199
 * SCR Title:  Update perimeter OCR to accept up to 10,000 entries in match string file
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Change to the max number of lines allowed in the match file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   28 Aug 2000 16:05:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  177
 * SCR Title:  Fix Automatic Load Last SEC Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update FastOCR to prevent memory access violations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   18 Jul 2000 15:48:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  169
 * SCR Title:  Add Match Score Information to OCR Tools
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   OCR Match Score Updates
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   15 Jun 2000 09:41:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  162
 * SCR Title:  Upgrade Coreco Sapera from Version 2.10 to Version 3.00
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all instances of obsolete WiT data types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   23 May 2000 10:14:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  149
 * SCR Title:  Fix OCR Geometry Maximum Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed the selection parameter from 2 to 3 in all instances.  Added two new files SVOCRParamStruct.h and SVOCRParamStruct.cpp to move the SVOCRParamStruct to its own file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   16 May 2000 14:43:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   27 Apr 2000 08:48:16   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  130
 * SCR Title:  OCR Tool Modification
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Modified the functions 'LoadMatchString' and 'onRun'.
 * Added 3 new functions 'BuildHashTable',  'InsertValueToTable', 'CheckStringInTable' for hash table creation and lookup.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:23:14   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Nov 09 1999 09:44:52   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add message response to 'GOING_ON_LINE' so match string file can be opened and a possible new match string can be retreived.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Oct 20 1999 07:24:40   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Character blob x and y axis sort.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Oct 18 1999 16:50:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   First changes for multi-axis character blob sorting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Sep 07 1999 08:51:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added OnValidate() and onRun() methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Sep 1999 09:56:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Run logic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Aug 31 1999 19:21:48   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   OCR converted to 3.0 architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Aug 28 1999 10:18:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add OCR Analyzer and Result.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
// EOF //
