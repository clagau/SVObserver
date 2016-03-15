// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcrInterface
// * .File Name       : $Workfile:   SVMatroxOcrInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:24:52  $
// ******************************************************************************

#ifndef SV_MATROX_OCR_INTERFACE_H
#define SV_MATROX_OCR_INTERFACE_H

#include <comdef.h>
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVMatroxEnums.h"
#include "SVMatroxStatusInformation.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxOcr.h"
#include "SVMatroxOcrResult.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxOcrCreateStruct.h"
#include "SVMatroxOcrCalibrateStruct.h"

class SVCommandDataHolder;

/**
@SVObjectName Matrox Ocr Interface

@SVObjectOverview This class supports Ocr functions.

@SVObjectOperations The Create function uses the Matrox MocrAllocFont. This function allocates a SVMatroxOcr. A SVMatroxOcr contains the OCR font, target image information, and processing controls.
The CreateResult function uses the Matrox MocrAllocResult. This function allocates a result buffer for use with other OCR functions.
The Destroy function uses the Matrox MocrFree. This function deletes the specified SVMatroxOcr or result buffer identifier, and releases any memory associated with it.
The CalibrateFont function uses the Matrox MocrCalibrateFont. This function automatically calibrates the X and Y size and spacing of the font of a specified SVMatroxOcr to match that of a string in the sample target image.
The Execute function uses the MocrReadString. This function reads an unknown string from the specified target image using the specified SVMatroxOcr.
The CopyFont function uses the MocrCopyFont. This function copies a character representation of one, or many, font characters to/from the specified image buffer. This buffer can then be used to initialize, change or obtain a visual representation of the font's characters.
The PreProcess function uses the MocrPreprocess. This function preprocesses the specified SVMatroxOcr. This function must be called before any read or verify operation and after all the control and constraints are set to speed up all subsequent read or verify operations. Note that preprocessing will be done automatically during the first read or verify operation if this function is not explicitly called.
The Set functions use the Matrox MocrControl. This function also sets various controls that affect how results are retrieved by MocrGetResult(). The Set functions have three variations which are double&, long&, and SVMatrox&.
The Get functions use the Matrox MocrInquire and MocrGetResult. This function retrieves the result(s) of the specified type from an OCR result buffer or inquires about an SVMatroxOcr or an OCR result buffer setting. The Get functions have three variations which are double&, long&, and SVMatrox&.
The SetResult functions use the Matrox MocrControl. This function also sets various controls that affect how results are retrieved by MocrGetResult(). The SetResult functions have three variations which are double&, long&, and SVMatrox&.
The GetResult functions use the Matrox MocrGetResult. This function retrieves the result(s) of the specified type from an OCR result buffer. The GetResult functions have three variations which are double&, long&, and SVMatrox&.
The RestoreFont function uses the Matrox MocrRestoreFont. This function restores an SVMatroxOcr, previously saved with SaveFont() from a file. Alternatively, this function can load only font constraint or control data from the file into the specified SVMatroxOcr. 
The SaveFont function uses the Matrox MocrSaveFont. This function saves an existing SVMatroxOcr to disk using the MIL font file format. The SVMatroxOcr's control, constraint, and/or font character data can all be saved; which data is saved depends on the value of the Operation parameter.
*/
class SVMatroxOcrInterface
{
public:
	typedef SVMatroxApplicationInterface::SVStatusCode SVStatusCode;

	SVMatroxOcrInterface();

	virtual ~SVMatroxOcrInterface();

	// This function replaces MocrAllocFont
	static SVStatusCode Create( SVMatroxOcr& p_rFontId, const SVMatroxOcrCreateStruct& p_rCreateStruct );
	// This function replaces MocrAllocResult
	static SVStatusCode Create( SVMatroxOcrResult& p_rFontResult);
	// This function replaces MocrAllocFont
	static SVStatusCode Destroy( SVMatroxOcr& p_rId );
	static SVStatusCode Destroy( SVMatroxOcrResult& p_rId );

	// This function replaces MocrCalibrateFont
	static SVStatusCode CalibrateFont( const SVMatroxOcr& p_rFontId, const SVMatroxOcrCalibrateStruct& p_rCalStruct );
	static HRESULT CalibrateFontCommand( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );

	// This function replaces MocrVerifyString and MocrReadString
	static SVStatusCode Execute( const SVMatroxOcrResult& p_rResultId, const SVMatroxOcr& p_rFontId, const SVMatroxBuffer& p_rImage );
	static HRESULT ReadString( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	static HRESULT VerifyString( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	
	// This function replaces MocrCopyFont
	static SVStatusCode CopyFont(const SVMatroxOcr& p_rFontId, const SVMatroxBuffer& p_rImageBufId,
		SVOcrOperationEnum p_eOperation, SVMatroxString& p_String);

	// This function replaces MocrPreprocess
	static SVStatusCode Preprocess( const SVMatroxOcr& p_rFontId );

	static SVStatusCode SetVerify( SVMatroxOcr& p_rFontId, const SVMatroxString& p_strVerifyString, const bool p_bVerifyOn );

	// These functions use MocrControl
	static SVStatusCode Set( const SVMatroxOcr& p_rFontId, SVOcrControlEnum, const double& p_dValue );
	static SVStatusCode Set( const SVMatroxOcr& p_rFontId, SVOcrControlEnum, const long& p_lValue );

	// This function replaces MocrInquire
	static SVStatusCode Get( const SVMatroxOcr& p_rFontId, SVOcrControlEnum InquireType, double& p_rdValue) ;
	static SVStatusCode Get( const SVMatroxOcr& p_rFontId, SVOcrControlEnum InquireType, long& p_rlValue) ;
	static SVStatusCode Get( const SVMatroxOcr& p_rFontId, SVOcrControlEnum InquireType, SVMatroxString& p_rStrValue) ;

	// These functions use MocrControl
	static SVStatusCode SetResult( const SVMatroxOcrResult& p_rFontId, SVOcrControlEnum, const double& p_dValue );
	static SVStatusCode SetResult( const SVMatroxOcrResult& p_rFontId, SVOcrControlEnum, const long& p_lValue );

	// This function replaces MocrInquire
	static SVStatusCode GetResult( const SVMatroxOcrResult& p_rFontId, SVOcrResultEnum InquireType, double& p_rdValue) ;
	static SVStatusCode GetResult( const SVMatroxOcrResult& p_rFontId, SVOcrResultEnum InquireType, long& p_rlValue) ;
	static SVStatusCode GetResult( const SVMatroxOcrResult& p_rFontId, SVOcrResultEnum InquireType, SVMatroxString& p_rStrValue) ;
	static SVStatusCode GetResult( const SVMatroxOcrResult& p_rFontId, SVOcrResultEnum InquireType, SVMatroxDoubleArray& p_adValues ) ;

	// This function replaces MocrRestoreFont
	static SVStatusCode RestoreFont( SVMatroxOcr& p_rFontId, const SVMatroxString& p_sFileName, SVOcrOperationEnum p_eOperation);
	static HRESULT RestoreFont( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );

	// This function replaces MocrSaveFont
	static SVStatusCode SaveFont( const SVMatroxOcr& p_rFontId, const SVMatroxString& p_sFileName, SVOcrOperationEnum p_eOperation);
	static HRESULT SaveFont( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );

	static HRESULT FindFontCharacters( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	static HRESULT CharacterThickness( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );

private:
	enum SVImageBackgroundEnum
	{
		DARK  = 0,
		LIGHT = 1,
	};

	static long Convert2MatroxControlType(SVOcrControlEnum p_eType) ;
	static long Convert2MatroxCreateType(SVOcrTypeEnum p_eType) ;
	static long Convert2MatroxOperationType( SVOcrOperationEnum p_eType ) ;
	static long Convert2MatroxResultType( SVOcrResultEnum p_eType) ;

	static HRESULT CreateTempFileName( SVMatroxString& p_rFileName );

	static HRESULT ConvertFileToByteVector( const SVMatroxString& p_rFileName, SVByteVector& p_rFileContents );
	static HRESULT ConvertByteVectorToTempFile( const SVByteVector& p_rFileContents, SVMatroxString& p_rFileName );

	static HRESULT CreateFontDataFromFontId( const SVMatroxOcr& p_rFontId, SVCommandDataHolder& p_rFontData );
	static HRESULT CreateFontIdFromFontData( const SVCommandDataHolder& p_rFontData, SVMatroxOcr& p_rFontId );

	static HRESULT UpdateCharacterListFromFontId( const SVMatroxOcr& p_rFontId, SVCommandDataHolder& p_rResults );
	static HRESULT UpdateFontIdFromCharacterList( const SVCommandDataHolder& p_rAttributes, SVMatroxOcr& p_rFontId );

	static HRESULT ThicknessCalculation( SVMatroxBuffer p_ThresholdImage, BYTE p_BackgroundType, long& p_rThickness );

};

#endif // SV_MATROX_OCR_INTERFACE_H

