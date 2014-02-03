//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVAnalyzerResult
//* .File Name       : $Workfile:   SVOCVAnalyzerResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 11:55:22  $
//******************************************************************************

#ifndef SVOCVANALYZERESULTCLASS_H
#define SVOCVANALYZERESULTCLASS_H

#include "SVObjectLibrary/SVLockableClass.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVOCVBlobRecord.h"
#include "SVResult.h"

#define  OCV_MAX_ENTREES       10000  // Max number of lines allowed in the match file

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVOCVAnalyzeResultClass 
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
class SVOCVAnalyzeResultClass : public SVResultClass
{
protected:
	SV_DECLARE_CLASS( SVOCVAnalyzeResultClass )

public:
	SVOCVAnalyzeResultClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVOCVANALYZERESULT );

	~SVOCVAnalyzeResultClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual CRect Draw( HDC DC, CRect R );
	virtual BOOL OnValidate();

	void GetOCVResultString( CString & csResult );
	void WriteToArchive(CFile & fileArchive);
	
	BOOL GenerateFontModel();
	HRESULT LoadMatchString();
	void HideResults();

	BOOL BuildHashTable(char *pBuffer);
	void InsertValueToTable(short nValue, int index);
	int CheckStringInTable(CString MatchString);
	BOOL UpdateResultFigure( long p_lIndex );

	SVImageClass* getInputImage();

	virtual HRESULT ResetObject();

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual LONG_PTR processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext );
	virtual HRESULT onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructCArray& p_rMultiLineArray );

private:
	void clearAll();

//******************************************************************************
// Attribute(s):
//******************************************************************************
public:
	// Results
	SVOCVCharacterResultArray arrayOCVCharacterResults;

protected:

	long     m_lTotalBytes;
	char     *m_pBuffer;
	long     m_lLowValue;
	long     m_lHighValue;
	int      m_nTotalCount;
	char     *m_pDataArr[OCV_MAX_ENTREES];
	short    *m_pIndexTable;
	double   m_dFactor;

	SVLockableClass		lock;

	// Input: Image
	SVInObjectInfoStruct inputImageObjectInfo;
	long m_lCurrentFoundStringLength;

public:
	// *** // ***
	// New value objects instead of parameterized string value object
	SVBoolValueObjectClass		m_bvoUseMatchFile;
	SVFileNameValueObjectClass	m_fnvoFontFileName;
	SVFileNameValueObjectClass	m_fnvoConstraintsFileName;
	SVFileNameValueObjectClass	m_fnvoControlsFileName;
	SVFileNameValueObjectClass	m_fnvoMatchStringFileName;
	SVBoolValueObjectClass		m_bvoPerformOCR;
	// *** // ***

	// Output
	SVStringValueObjectClass	m_svoMatchString;
	SVStringValueObjectClass	m_svoFoundString;
	SVLongValueObjectClass		m_lvoMatchLineNumber;

	// Exposing OCV Match Scores
	SVDoubleValueObjectClass	m_dvoHighestMatchScore;
	SVDoubleValueObjectClass	m_dvoLowestMatchScore;
	SVDoubleValueObjectClass	m_dvoAverageMatchScore;

	//	MIL object ids used in the OCR/OCV operation
	SVMatroxOcr m_milFontID;
	SVMatroxOcrResult m_milResultID;
	long m_lFontStringLength;
	long m_lFontStringLengthMax;
	long m_lMatchStringLength;

private:
	bool m_bHasLicenseError;
};

#endif //#ifndef SVOCVANALYZERESULTCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOCVAnalyzerResult.h_v  $
 * 
 *    Rev 1.1   01 Feb 2014 11:55:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:01:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   27 Feb 2013 11:26:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Put in checks to see if it has licnese errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   02 Nov 2012 12:43:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  797
 * SCR Title:  Fix Analyzer Overlay Issues
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnCollectOverlays Override.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   25 Jul 2012 14:39:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removal of dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   25 Feb 2011 12:22:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 Feb 2011 15:05:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Jan 2011 11:42:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Jun 2010 14:57:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Jul 2007 13:37:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Feb 2005 07:33:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Jul 2004 10:38:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  431
 * SCR Title:  Fix problem with using match string files that are zero elements
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   put in check to make sure that if using a match string file that the number of elements is above 0.  if not, the inspection will not be allowed to go online
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 May 2004 11:27:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  420
 * SCR Title:  Allow SVObserver to update the string length for the Matrox OCR
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new member variables to monitor string length and matchstring length.
 * Updated OnValidate, LoadMatchString, HideResults, and the constructor to initialize and use the new member variables.
 * Updated GenerateFontModel to use new member variable and to set string length to match string length.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Jan 2004 10:02:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update header information to make getInputImage method public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Sep 2003 13:07:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
