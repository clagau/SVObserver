//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayAnalyzerResult
//* .File Name       : $Workfile:   SVOCRGrayAnalyzerResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   23 Oct 2014 09:53:40  $
//******************************************************************************

#pragma once

#include "SVSystemLibrary/SVLockableClass.h"
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

	SvSyl::SVLockableClass		lock;

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

private:
	bool m_bHasLicenseError;
    
};

