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
	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override;
	virtual BOOL OnValidate() override;
	SVStringValueObjectClass* getRegExpression();
	virtual BOOL CloseObject() override;
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	SVBarCodeResultClass(BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBARCODEANALYZERESULT);
	virtual ~SVBarCodeResultClass();

	virtual bool resetAllObjects( bool shouldNotifyFriends, bool silentReset ) override;

	HRESULT LoadMatchStringFile();
	BOOL BuildHashTable(char *szBuffer);
	void InsertValueToTable(short nValue, int index);

	int CheckStringInTable(CString MatchString);

	SVBoolValueObjectClass msv_bUseSingleMatchString;
	SVBoolValueObjectClass msv_bUseMatchStringFile;
	SVFileNameValueObjectClass msv_szMatchStringFileName;
	SVLongValueObjectClass msv_lMatchStringLine;
	SVDoubleValueObjectClass m_dReadScore;
	virtual HRESULT ResetObject() override;

protected:
	SVStringValueObjectClass* getInputString();
	SVInObjectInfoStruct m_SVRegExpressionObjectInfo;

	long     m_lTotalBytes;
	int      m_nTotalCount;
	char     *m_pBuffer;
	long     m_lLowValue;
	long     m_lHighValue;
	char     *m_pDataArr[BC_MAX_ENTREES];
	short    *m_pIndexTable;
	double   m_dFactor;
};
