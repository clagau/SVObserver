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

#if !defined(AFX_SVBARCODERESULT_H__3A85515D_7B17_11D3_A7CE_00106F010A93__INCLUDED_)
#define AFX_SVBARCODERESULT_H__3A85515D_7B17_11D3_A7CE_00106F010A93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVResultString.h"

#define  BC_MAX_ENTREES       10000  // Max number of lines allowed in the match file
#define  BC_CAR_RETURN        0x0D

class SVBarCodeResultClass : public SVStringResultClass  
{
	SV_DECLARE_CLASS(SVBarCodeResultClass);

public:
	bool m_bFailedToRead;
	BOOL onRun( SVRunStatusClass& RRunStatus );
	BOOL OnValidate();
	SVStringValueObjectClass* getRegExpression();
	BOOL CloseObject();
	BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	SVBarCodeResultClass(BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVBARCODEANALYZERESULT);
	virtual ~SVBarCodeResultClass();

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	HRESULT LoadMatchStringFile();
	BOOL BuildHashTable(char *szBuffer);
	void InsertValueToTable(short nValue, int index);

	int CheckStringInTable(CString MatchString);

	SVBoolValueObjectClass msv_bUseSingleMatchString;
	SVBoolValueObjectClass msv_bUseMatchStringFile;
	SVFileNameValueObjectClass msv_szMatchStringFileName;
	SVLongValueObjectClass msv_lMatchStringLine;
	SVDoubleValueObjectClass m_dReadScore;
	virtual HRESULT ResetObject();

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

#endif // !defined(AFX_SVBARCODERESULT_H__3A85515D_7B17_11D3_A7CE_00106F010A93__INCLUDED_)

