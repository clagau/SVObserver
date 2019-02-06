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
#pragma once

#pragma region Includes
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVImageAnalyzerClass.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#pragma endregion Includes

namespace SvOp
{
class SVBarCodeResultClass;
}

namespace SvAo
{

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
	std::string m_RegExpressionValue;
	std::string m_StringFileName;

	SvVol::SVBoolValueObjectClass m_bWarnOnFailedRead;
	SvVol::SVStringValueObjectClass msv_szBarCodeValue;
	SvVol::SVStringValueObjectClass msv_szRegExpressionValue;
	SvVol::SVLongValueObjectClass msv_lBarCodeType;
	SvVol::SVDoubleValueObjectClass msv_dOrientation;
	SvVol::SVDoubleValueObjectClass msv_dSkewNegative;
	SvVol::SVDoubleValueObjectClass msv_dSkewPositive;
	SvVol::SVDoubleValueObjectClass msv_dCellMinSize;
	SvVol::SVDoubleValueObjectClass msv_dCellMaxSize;
	SvVol::SVDoubleValueObjectClass msv_dCellNumberX;
	SvVol::SVDoubleValueObjectClass msv_dCellNumberY;
	SvVol::SVDoubleValueObjectClass msv_dSpeed;
	SvVol::SVDoubleValueObjectClass msv_dThreshold;
	SvVol::SVDoubleValueObjectClass msv_dStringSize;
	SvVol::SVDoubleValueObjectClass msv_dErrorCorrection;
	SvVol::SVDoubleValueObjectClass msv_dEncoding;
	SvVol::SVDoubleValueObjectClass msv_dForegroundColor;
	SvVol::SVBoolValueObjectClass msv_bSaveStringInFile;
	SvVol::SVFileNameValueObjectClass msv_szStringFileName;
	SvVol::SVLongValueObjectClass msv_lBarcodeTimeout;
	// To support special DMCs May 2008.
	SvVol::SVByteValueObjectClass msv_RawData;
	SvVol::SVEnumerateValueObjectClass msv_eStringFormat;
	SvVol::SVLongValueObjectClass msv_lThresholdType;

	//for MIL 9.0 - New only for DataMatrix Codes
	SvVol::SVBoolValueObjectClass msv_bUnEvenGrid;
	
	SVMatroxIdentifier m_MilCodeId = M_NULL;

	SVBarCodeAnalyzerClass(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBARCODEANALYZER);
	virtual ~SVBarCodeAnalyzerClass();

	void CloseMil();
	bool InitMil(SvStl::MessageContainerVector *pErrorMessages=nullptr);
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	static bool CharIsControl(TCHAR p_Char);

protected:
	void init();
	virtual bool onRun(SVRunStatusClass &rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

private:
	bool SaveRegExpression( SvStl::MessageContainerVector *pErrorMessages=nullptr );
	bool LoadRegExpression( bool DisplayErrorMessage = true, SvStl::MessageContainerVector *pErrorMessages=nullptr );

	bool m_bHasLicenseError;

	SvOp::SVBarCodeResultClass* m_pBarCodeResult{nullptr};
};

} //namespace SvAo
