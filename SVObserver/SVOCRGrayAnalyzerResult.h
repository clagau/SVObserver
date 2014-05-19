//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayAnalyzerResult
//* .File Name       : $Workfile:   SVOCRGrayAnalyzerResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:02:50  $
//******************************************************************************
#ifndef SV_OCR_GRAY_ANALYZER_RESULT_H
#define SV_OCR_GRAY_ANALYZER_RESULT_H

//******************************************************************************
// Include(s):
//******************************************************************************

#include "SVObjectLibrary/SVLockableClass.h"
#include "SVLVFastOCR.h"
#include "SVOCRBlobRecord.h"
#include "SVOCRGrayParamStruct.h"
#include "SVResult.h"

//******************************************************************************
// Define(s):
//******************************************************************************

#define  GRAY_MAX_ENTREES       10000  // Max number of lines allowed in the match file

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVOCRGrayAnalyzeResultClass 
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVOCRGrayAnalyzeResultClass : public SVResultClass
{
protected:
	SV_DECLARE_CLASS( SVOCRGrayAnalyzeResultClass )

public:
	SVOCRGrayAnalyzeResultClass( SVObjectClass* POwner = NULL, 
		                           int StringResourceID = IDS_CLASSNAME_SVOCRGRAYANALYZERESULT );

	~SVOCRGrayAnalyzeResultClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual CRect Draw( HDC DC, CRect R );
	virtual BOOL OnValidate();

	BOOL GetFontFileName( CString & csName );
	BOOL GetMatchStringFileName( CString & csName );

	SVOCRGrayParamStruct& GetOCRParameters();
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
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

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

	SVFileNameValueObjectClass msvfnvocGrayFontFileName;
	SVFileNameValueObjectClass msvfnvocGrayMatchStringFileName;

protected:

	long     m_lTotalBytes;
	char     *m_pBuffer;
	long     m_lLowValue;
	long     m_lHighValue;
	int      m_nTotalCount;
	char     *m_pDataArr[GRAY_MAX_ENTREES];
	short    *m_pIndexTable;
	double   m_dFactor;

	SVLockableClass		lock;

	// Input: Image
	SVInObjectInfoStruct inputImageObjectInfo;

  //
	// The OCR parameters
	//
	SVOCRGrayParamStruct ocrParameters;

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
	
	SVLongValueObjectClass		m_voUseMatchFile;
	SVLongValueObjectClass		m_voMaxMatches;
	SVLongValueObjectClass		m_voXVicinity;
	SVLongValueObjectClass		m_voYVicinity;
	SVLongValueObjectClass		m_voScale;
	SVLongValueObjectClass		m_voMaxCandidates;
	SVLongValueObjectClass		m_voCandsInVicinity;
	SVLongValueObjectClass		m_voOutput;

	SVDoubleValueObjectClass	m_voRejectThresh;
	SVDoubleValueObjectClass	m_voAcceptThresh;
	SVDoubleValueObjectClass	m_voRelThresh;
	SVDoubleValueObjectClass	m_voMinContrast;
	SVDoubleValueObjectClass	m_voCandidateThresh;
	SVDoubleValueObjectClass	m_voCandidateRelThresh;
	// *** // ***

	// Output
	SVStringValueObjectClass	matchString;
	SVStringValueObjectClass	foundString;

	// line number of the Match string.
	SVLongValueObjectClass msvlMatchLineNumber;

	//
	//  Wit Objects
	//
	
	//	Generated by call to ocrInitialize.
	CorFontModelGray* pOCRFontModelGray;	

	//  Array of individual characters found by cor_ocrGrayRun.
	CorStringVector *presultStringVector;
	CorFpointVector *plocationVector;
	CorFloatVector	*plvcfvMatchScoreVector;
	CorObj			*pROIObj;

	//  Wit wrapper for noMatchLabel.
	char *witNoMatchLabel;
    
};

#endif //#ifndef SV_OCR_GRAY_ANALYZER_RESULT_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOCRGrayAnalyzerResult.h_v  $
 * 
 *    Rev 1.2   15 May 2014 11:02:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified processMessage signature to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 11:39:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:55:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   25 Feb 2011 12:22:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   01 Jun 2010 14:57:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   10 Aug 2005 12:40:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   09 Aug 2005 07:35:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   17 Feb 2005 15:01:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   08 Jul 2004 10:41:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  431
 * SCR Title:  Fix problem with using match string files that are zero elements
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   put in check to make sure that if using a match string file that the number of elements is above 0.  if not, the inspection will not be allowed to go online
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   19 Jan 2004 10:02:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update header information to make getInputImage method public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   30 Apr 2003 17:02:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed a couple of small memory leaks in the ::onRun.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 Apr 2003 12:41:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   20 Nov 2002 10:12:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added value objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Aug 2001 16:16:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  209
 * SCR Title:  Permit multiple match strings in the Grayscale OCR match string file
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added #define GRAY_MAX_ENTRIES to 10000.
 * 
 * Added attributes used for maintaining match string file information.
 * 
 * Added methods for finding and maintaining match string file information
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   17 Jul 2001 20:34:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  214
 * SCR Title:  Fix OCR analyzer Dr. Watson problem when calculating match score information.
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed two class variables: pROIObj and plvImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Jun 2001 08:51:48   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: SVOCRGrayAnalyzeResultClass, CreateObject, GenerateFontModel,
 * LoadMatchString, processMessage, onRun.
 * .
 * Added the following functions: GetFontFileName, GetMatchStringFileName, SetFontFileName,
 * SetMatchStringFileName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Aug 2000 16:05:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  177
 * SCR Title:  Fix Automatic Load Last SEC Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update FastOCR to prevent memory access violations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Jul 2000 15:48:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  169
 * SCR Title:  Add Match Score Information to OCR Tools
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   OCR Match Score Updates
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jun 2000 09:41:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  162
 * SCR Title:  Upgrade Coreco Sapera from Version 2.10 to Version 3.00
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all instances of obsolete WiT data types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:44:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
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
