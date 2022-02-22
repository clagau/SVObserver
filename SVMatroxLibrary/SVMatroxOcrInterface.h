// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcrInterface
// * .File Name       : $Workfile:   SVMatroxOcrInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:24:52  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#include "SVMatroxSimpleEnums.h"
#include "SVMatroxOcr.h"
#pragma endregion Includes

class SVCommandDataHolder;
class SVMatroxBuffer;
struct SVMatroxOcrCalibrateStruct;
struct SVMatroxOcrCreateStruct;

/**
@SVObjectName Matrox Ocr Interface

@SVObjectOverview This class supports Ocr functions.

@SVObjectOperations The Create function uses the Matrox MocrAllocFont. This function allocates a SVMatroxOcr. A SVMatroxOcr contains the OCR font, target image information, and processing controls.
The CreateResult function uses the Matrox MocrAllocResult. This function allocates a result buffer for use with other OCR functions.
The Destroy function uses the Matrox MocrFree. This function deletes the specified SVMatroxOcr or result buffer identifier, and releases any memory associated with it.
The Execute function uses the MocrReadString. This function reads an unknown string from the specified target image using the specified SVMatroxOcr.
The PreProcess function uses the MocrPreprocess. This function preprocesses the specified SVMatroxOcr. This function must be called before any read or verify operation and after all the control and constraints are set to speed up all subsequent read or verify operations. Note that preprocessing will be done automatically during the first read or verify operation if this function is not explicitly called.
The Set functions use the Matrox MocrControl. This function also sets various controls that affect how results are retrieved by MocrGetResult(). The Set functions have three variations which are double&, long&, and SVMatrox&.
The Get functions use the Matrox MocrInquire and MocrGetResult. This function retrieves the result(s) of the specified type from an OCR result buffer or inquires about an SVMatroxOcr or an OCR result buffer setting. The Get functions have three variations which are double&, long&, and SVMatrox&.
The GetResult functions use the Matrox MocrGetResult. This function retrieves the result(s) of the specified type from an OCR result buffer. The GetResult functions have three variations which are double&, long&, and SVMatrox&.
The RestoreFont function uses the Matrox MocrRestoreFont. This function restores an SVMatroxOcr, previously saved with SaveFont() from a file. Alternatively, this function can load only font constraint or control data from the file into the specified SVMatroxOcr. 
*/
class SVMatroxOcrInterface
{
public:
	SVMatroxOcrInterface();

	virtual ~SVMatroxOcrInterface();

	// This function replaces MocrAllocFont
	static HRESULT Create( SVMatroxOcr& p_rFontId, const SVMatroxOcrCreateStruct& p_rCreateStruct );
	// This function replaces MocrAllocResult
	static HRESULT CreateResult(__int64& p_rFontResult);
	// This function replaces MocrAllocFont
	static HRESULT Destroy( SVMatroxOcr& p_rId );
	static HRESULT DestroyResult(__int64& p_rId );

	// This function replaces MocrVerifyString and MocrReadString
	static HRESULT Execute( const __int64& rResultId, const SVMatroxOcr& p_rFontId, const SVMatroxBuffer& p_rImage );
	
	// This function replaces MocrPreprocess
	static HRESULT Preprocess( const SVMatroxOcr& p_rFontId );

	// These functions use MocrControl
	static HRESULT Set( const SVMatroxOcr& p_rFontId, SVOcrControlEnum, const double& p_dValue );
	static HRESULT Set( const SVMatroxOcr& p_rFontId, SVOcrControlEnum, const long& p_lValue );

	// This function replaces MocrInquire
	static HRESULT Get( const SVMatroxOcr& p_rFontId, SVOcrControlEnum InquireType, double& p_rdValue) ;
	static HRESULT Get( const SVMatroxOcr& p_rFontId, SVOcrControlEnum InquireType, long& p_rlValue) ;
	static HRESULT Get( const SVMatroxOcr& p_rFontId, SVOcrControlEnum InquireType, std::string& p_rStrValue) ;

	// This function replaces MocrInquire
	static HRESULT GetResult( const __int64& rResultId, SVOcrResultEnum InquireType, double& p_rdValue) ;
	static HRESULT GetResult( const __int64& rResultId, SVOcrResultEnum InquireType, long& p_rlValue) ;
	static HRESULT GetResult( const __int64& rResultId, SVOcrResultEnum InquireType, std::string& p_rStrValue) ;
	static HRESULT GetResult( const __int64& rResultId, SVOcrResultEnum InquireType, std::vector<double>& p_adValues ) ;

	// This function replaces MocrRestoreFont
	static HRESULT RestoreFont( SVMatroxOcr& p_rFontId, const std::string& p_sFileName, SVOcrOperationEnum p_eOperation);

private:
	enum SVImageBackgroundEnum
	{
		DARK  = 0,
		LIGHT = 1,
	};

	static __int64 Convert2MatroxControlType(SVOcrControlEnum p_eType) ;
	static __int64 Convert2MatroxCreateType(SVOcrTypeEnum p_eType) ;
	static __int64 Convert2MatroxOperationType( SVOcrOperationEnum p_eType ) ;
	static __int64 Convert2MatroxResultType( SVOcrResultEnum p_eType) ;
};

