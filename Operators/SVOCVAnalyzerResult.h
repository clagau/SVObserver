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
#include "SVMatroxLibrary/SVMatroxOcr.h"
#include "SVSystemLibrary/SVLockableClass.h"
#include "SVOCVBlobRecord.h"
#include "SVResult.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVFileNameValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

#define  OCV_MAX_ENTREES       10000  // Max number of lines allowed in the match file

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVOCVAnalyzerResult 
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
////////////////////////////////////////////////////////////////////////////////
class SVOCVAnalyzerResult : public SVResult
{
protected:
	SV_DECLARE_CLASS

public:
	SVOCVAnalyzerResult( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOCVANALYZERRESULT );

	virtual ~SVOCVAnalyzerResult();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	bool GenerateFontModel();
	HRESULT LoadMatchString();
	void HideResults();

	bool BuildHashTable(char *pBuffer);
	void InsertValueToTable(short nValue, int index);
	long CheckStringInTable(const std::string& rMatchString);


	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

protected:
	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
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

	long     m_lTotalBytes = 0;
	char     *m_pBuffer = nullptr;
	long     m_lLowValue = 0;
	long     m_lHighValue = 0;
	int      m_nTotalCount = 0;
	TCHAR    *m_pDataArr[OCV_MAX_ENTREES] {};
	short    *m_pIndexTable = nullptr;
	double   m_dFactor = 1.;

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
	__int64 m_milResultID = 0LL;
	long m_lFontStringLength;
	long m_lFontStringLengthMax;
	long m_lMatchStringLength;

private:
	bool m_bHasLicenseError;
};

} //namespace SvOp
