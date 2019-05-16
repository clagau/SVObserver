//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeResultClass
//* .File Name       : $Workfile:   SVBarCodeResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 10:19:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVResultString.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVFileNameValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{
#define  BC_MAX_ENTREES       10000  // Max number of lines allowed in the match file
#define  BC_CAR_RETURN        0x0D

class SVBarCodeResultClass : public SVStringResultClass  
{
	SV_DECLARE_CLASS(SVBarCodeResultClass);

public:
	bool m_bFailedToRead = false;
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	SVBarCodeResultClass(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBARCODEANALYZERESULT);
	virtual ~SVBarCodeResultClass();

	HRESULT LoadMatchStringFile();
	bool BuildHashTable(char *szBuffer);
	void InsertValueToTable(short nValue, int index);

	int CheckStringInTable( const std::string& rMatchString );

	SvVol::SVBoolValueObjectClass msv_bUseSingleMatchString;
	SvVol::SVBoolValueObjectClass msv_bUseMatchStringFile;
	SvVol::SVFileNameValueObjectClass msv_szMatchStringFileName;
	SvVol::SVLongValueObjectClass msv_lMatchStringLine;
	SvVol::SVDoubleValueObjectClass m_dReadScore;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

private:
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

private:
	SvOl::SVInObjectInfoStruct m_SVRegExpressionObjectInfo;

	long     m_lTotalBytes;
	int      m_nTotalCount;
	TCHAR*   m_pBuffer;
	long     m_lLowValue;
	long     m_lHighValue;
	TCHAR*   m_pDataArr[BC_MAX_ENTREES];
	short*   m_pIndexTable;
	double   m_dFactor;
};

} //namespace SvOp
