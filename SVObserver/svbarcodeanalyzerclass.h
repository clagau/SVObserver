//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeAnalyzerClass
//* .File Name       : $Workfile:   svbarcodeanalyzerclass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   19 Dec 2014 03:59:32  $
//******************************************************************************

#ifndef SVBARCODEANALYZERCLASS_H
#define SVBARCODEANALYZERCLASS_H

#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVAnalyzer.h"

enum 
{
	SVBCStringFormatRemoveCharacters = 0,
	SVBCStringFormatTranslateCharacters,
	SVBCStringFormatReplaceCharacters,
};
class SVBarCodeAnalyzerClass : public SVImageAnalyzerClass  
{
	friend class SVSetupDialogManager;

	SV_DECLARE_CLASS (SVBarCodeAnalyzerClass);

public:
	CString m_csRegExpressionValue;
	CString m_csStringFileName;

	SVBoolValueObjectClass m_bWarnOnFailedRead;
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	SVStringValueObjectClass msv_szBarCodeValue;
	SVStringValueObjectClass msv_szRegExpressionValue;
	SVLongValueObjectClass msv_lBarCodeType;
	SVDoubleValueObjectClass msv_dOrientation;
	SVDoubleValueObjectClass msv_dSkewNegative;
	SVDoubleValueObjectClass msv_dSkewPositive;
	SVDoubleValueObjectClass msv_dCellMinSize;
	SVDoubleValueObjectClass msv_dCellMaxSize;
	SVDoubleValueObjectClass msv_dCellNumberX;
	SVDoubleValueObjectClass msv_dCellNumberY;
	SVDoubleValueObjectClass msv_dSpeed;
	SVDoubleValueObjectClass msv_dThreshold;
	SVDoubleValueObjectClass msv_dStringSize;
	SVDoubleValueObjectClass msv_dErrorCorrection;
	SVDoubleValueObjectClass msv_dEncoding;
	SVDoubleValueObjectClass msv_dForegroundColor;
	SVBoolValueObjectClass msv_bSaveStringInFile;
	SVFileNameValueObjectClass msv_szStringFileName;
	SVLongValueObjectClass msv_lBarcodeTimeout;
	// To support special DMCs May 2008.
	SVByteValueObjectClass msv_RawData;
	SVEnumerateValueObjectClass msv_eStringFormat;
	SVLongValueObjectClass msv_lThresholdType;

	//for MIL 9.0 - New only for DataMatrix Codes
	SVBoolValueObjectClass msv_bUnEvenGrid;
	
	SVMatroxBarCode m_MilCodeId;

	SVBarCodeAnalyzerClass (BOOL BCreateDefaultTaskList = TRUE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVBARCODEANALYZER);
	void CloseMil ();
	BOOL InitMil ();
	virtual BOOL CreateObject (SVObjectLevelCreateStruct* PCreateStructure);
	virtual BOOL OnValidate ();

	SVResultClass* GetResultObject();

	virtual ~SVBarCodeAnalyzerClass ();
	virtual BOOL CloseObject ();

	virtual HRESULT ResetObject();
	static bool CharIsControl( TCHAR p_Char);
protected:
	void init ();
	virtual BOOL onRun (SVRunStatusClass &RRunStatus);
private:
	BOOL SaveRegExpression ( BOOL DisplayErrorMessage = TRUE );
	BOOL LoadRegExpression ( BOOL DisplayErrorMessage = TRUE );

	CString errStr;							// for errorEvent

	bool m_bHasLicenseError;
};

#endif // !defined(AFX_SVBARCODEANALYZERCLASS_H__A8960E74_74C9_11D3_A7C8_00106F010A93__INCLUDED_)

