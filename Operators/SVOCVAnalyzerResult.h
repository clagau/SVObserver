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
#include "SVOCVBlobRecord.h"
#include "SVResult.h"
#pragma endregion Includes

namespace SvOp
{

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

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	void GetOCVResultString( std::string& rResult );
	
	bool GenerateFontModel();
	HRESULT LoadMatchString();
	void HideResults();

	bool BuildHashTable(char *pBuffer);
	void InsertValueToTable(short nValue, int index);
	long CheckStringInTable(const std::string& rMatchString);


	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	virtual HRESULT onCollectOverlays(SvIe::SVImageClass* pImage, SVExtentMultiLineStructVector& rMultiLineArray ) override;

private:
	void clearAll();

//******************************************************************************
// Attribute(s):
//******************************************************************************
public:
	// Results
	SVOCVCharacterResultPtrVector m_OCVCharacterResults;

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
	SvVol::SVBoolValueObjectClass		m_bvoUseMatchFile;
	SvVol::SVFileNameValueObjectClass	m_fnvoFontFileName;
	SvVol::SVFileNameValueObjectClass	m_fnvoConstraintsFileName;
	SvVol::SVFileNameValueObjectClass	m_fnvoControlsFileName;
	SvVol::SVFileNameValueObjectClass	m_fnvoMatchStringFileName;
	SvVol::SVBoolValueObjectClass		m_bvoPerformOCR;
	// *** // ***

	// Output
	SvVol::SVStringValueObjectClass	m_svoMatchString;
	SvVol::SVStringValueObjectClass	m_svoFoundString;
	SvVol::SVLongValueObjectClass		m_lvoMatchLineNumber;

	// Exposing OCV Match Scores
	SvVol::SVDoubleValueObjectClass	m_dvoHighestMatchScore;
	SvVol::SVDoubleValueObjectClass	m_dvoLowestMatchScore;
	SvVol::SVDoubleValueObjectClass	m_dvoAverageMatchScore;

	//	MIL object ids used in the OCR/OCV operation
	SVMatroxOcr m_milFontID;
	SVMatroxIdentifier m_milResultID = M_NULL;
	long m_lFontStringLength;
	long m_lFontStringLengthMax;
	long m_lMatchStringLength;

private:
	bool m_bHasLicenseError;
};

} //namespace SvOp