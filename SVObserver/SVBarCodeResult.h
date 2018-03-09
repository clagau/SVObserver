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

#pragma endregion Includes

#define  BC_MAX_ENTREES       10000  // Max number of lines allowed in the match file
#define  BC_CAR_RETURN        0x0D

class SVBarCodeResultClass : public SVStringResultClass  
{
	SV_DECLARE_CLASS(SVBarCodeResultClass);

public:
	bool m_bFailedToRead;
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	SVBarCodeResultClass(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBARCODEANALYZERESULT);
	virtual ~SVBarCodeResultClass();

	HRESULT LoadMatchStringFile();
	bool BuildHashTable(char *szBuffer);
	void InsertValueToTable(short nValue, int index);

	int CheckStringInTable( const std::string& rMatchString );

	SVBoolValueObjectClass msv_bUseSingleMatchString;
	SVBoolValueObjectClass msv_bUseMatchStringFile;
	SVFileNameValueObjectClass msv_szMatchStringFileName;
	SVLongValueObjectClass msv_lMatchStringLine;
	SVDoubleValueObjectClass m_dReadScore;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

protected:
	SVStringValueObjectClass* getInputString(bool bRunMode = false);
	SVStringValueObjectClass* getRegExpression(bool bRunMode = false);

private:
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

private:
	SVInObjectInfoStruct m_SVRegExpressionObjectInfo;

	long     m_lTotalBytes;
	int      m_nTotalCount;
	TCHAR*   m_pBuffer;
	long     m_lLowValue;
	long     m_lHighValue;
	TCHAR*   m_pDataArr[BC_MAX_ENTREES];
	short*   m_pIndexTable;
	double   m_dFactor;
};
