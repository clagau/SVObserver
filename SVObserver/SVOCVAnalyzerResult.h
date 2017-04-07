//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVAnalyzerResult
//* .File Name       : $Workfile:   SVOCVAnalyzerResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:30  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVSystemLibrary/SVLockableClass.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVOCVBlobRecord.h"
#include "SVResult.h"
#pragma endregion Includes

#define  OCV_MAX_ENTREES       10000  // Max number of lines allowed in the match file

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVOCVAnalyzeResultClass 
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
////////////////////////////////////////////////////////////////////////////////
class SVOCVAnalyzeResultClass : public SVResultClass
{
protected:
	SV_DECLARE_CLASS( SVOCVAnalyzeResultClass )

public:
	SVOCVAnalyzeResultClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOCVANALYZERESULT );

	virtual ~SVOCVAnalyzeResultClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;

	void GetOCVResultString( SVString& rResult );
	
	BOOL GenerateFontModel();
	HRESULT LoadMatchString();
	void HideResults();

	BOOL BuildHashTable(char *pBuffer);
	void InsertValueToTable(short nValue, int index);
	long CheckStringInTable(const SVString& rMatchString);
	BOOL UpdateResultFigure( long p_lIndex );

	SVImageClass* getInputImage();

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	virtual HRESULT onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructVector& p_rMultiLineArray ) override;

private:
	void clearAll();

//******************************************************************************
// Attribute(s):
//******************************************************************************
public:
	// Results
	SVOCVCharacterResultPtrVector arrayOCVCharacterResults;

protected:

	long     m_lTotalBytes;
	char     *m_pBuffer;
	long     m_lLowValue;
	long     m_lHighValue;
	int      m_nTotalCount;
	TCHAR    *m_pDataArr[OCV_MAX_ENTREES];
	short    *m_pIndexTable;
	double   m_dFactor;

	SvSyl::SVLockableClass		lock;

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

