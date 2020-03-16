// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBarCodeInterface
// * .File Name       : $Workfile:   SVMatroxBarCodeInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:56:48  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxSimpleEnums.h"
#include "SVStatusLibrary\MessageContainer.h"
#pragma endregion Includes

class SVMatroxBuffer;

/**
@SVObjectName Matrox Bar Code Interface

@SVObjectOverview This class provides an interface for the Matrox Bar Code operations.  It does not have any data objects.  The data object is contained in the SVMatroxBarCode class.

@SVObjectOperations The create function creates a barcode object. The type must be of a SVCodeTypeEnum.
The Destroy function frees the handle created in the create function.
The Execute function calls the Matrox McodeRead. This function searches for a specific type of code in an image. The control settings of the specified code object determine how to perform the operation.
The Set functions include double&, long& and SVMatroxString& types.  Each of these functions take a SVMatroxIdentifier reference, a SVBarcodeControlTypeEnum, and the value reference.
The Get functions include double&, long&, and SVMatroxString& types. Each of these functions take a SVMatroxIdentifier reference, a SVBarcodeControlTypeEnum, and a value reference;
The GetResult functions include double&, long&, and SVMatroxString& types. Each of these functions take a SVMatroxIdentifier reference, a SVBarcodeControlTypeEnum, and a value reference;
*/
class SVMatroxBarCodeInterface
{
public:

	SVMatroxBarCodeInterface();

	virtual ~SVMatroxBarCodeInterface();

	// Use this instead of McodeControl
	static HRESULT Set( const __int64& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const double& p_dValue );
	static HRESULT Set( const __int64& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const long& p_lValue );
	static HRESULT Set( const __int64& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const std::string& p_dValue );

	// Use this instead of McodeInquire.
	static HRESULT Get( const __int64& p_rCodeID, SVBarCodeControlTypeEnum p_eType, double& p_dValue ) ;
	static HRESULT Get( const __int64& p_rCodeID, SVBarCodeControlTypeEnum p_eType, long& p_dValue ) ;
	static HRESULT Get( const __int64& p_rCodeID, SVBarCodeControlTypeEnum p_eType, std::string& p_dValue ) ;

	// The following functions call McodeGetResult.
	static bool GetResult( const __int64& p_rCodeID, SVBarCodeControlTypeEnum p_eType, double& p_rdValue, SvStl::MessageContainerVector *pErrorMessages=nullptr);
	static bool GetResult( const __int64& p_rCodeID, SVBarCodeControlTypeEnum p_eType, long& p_rlValue, SvStl::MessageContainerVector *pErrorMessages=nullptr);
	static bool GetResult( const __int64& p_rCodeID, SVBarCodeControlTypeEnum p_eType, std::string& p_rsValue, SvStl::MessageContainerVector *pErrorMessages=nullptr);

	// Does the work of McodeAllocate 
	static HRESULT Create(__int64& p_rCodeID, SVBarCodeTypesEnum p_eType );
	static HRESULT Create(__int64& p_rCodeID, const __int64& p_eFromCodeID );

	// Does the work of McodeFree 
	static HRESULT Destroy(__int64& p_rCodeID );

	// McodeRead
	static bool Execute( const __int64& p_rCodeID, const SVMatroxBuffer& p_rSourceId, SvStl::MessageContainerVector *pErrorMessages=nullptr );
private:
	static __int64 Convert2MatroxType(SVBarCodeControlTypeEnum p_eType) ;
};

