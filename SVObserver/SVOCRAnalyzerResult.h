//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRAnalyzerResult
//* .File Name       : $Workfile:   SVOCRAnalyzerResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 10:44:26  $
//******************************************************************************
#ifndef SV_OCR_ANALYZER_RESULT_H
#define SV_OCR_ANALYZER_RESULT_H

//******************************************************************************
// Include(s):
//******************************************************************************

#include "SVSystemLibrary/SVLockableClass.h"
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

	SvSyl::SVLockableClass lock;

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

