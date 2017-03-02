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
#include "SVUtilityLibrary\SVString.h"
#include "SVMatroxEnums.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxBarCode.h"
#include "SVMatroxBuffer.h"
#include "SVStatusLibrary\MessageContainer.h"

#pragma endregion Includes

/**
@SVObjectName Matrox Bar Code Interface

@SVObjectOverview This class provides an interface for the Matrox Bar Code operations.  It does not have any data objects.  The data object is contained in the SVMatroxBarCode class.

@SVObjectOperations The create function creates a barcode object. The type must be of a SVCodeTypeEnum.
The Destroy function frees the handle created in the create function.
The Execute function calls the Matrox McodeRead. This function searches for a specific type of code in an image. The control settings of the specified code object determine how to perform the operation.
The Set functions include double&, long& and SVMatroxString& types.  Each of these functions take a SVMatroxBarCode reference, a SVBarcodeControlTypeEnum, and the value reference.
The Get functions include double&, long&, and SVMatroxString& types. Each of these functions take a SVMatroxBarCode reference, a SVBarcodeControlTypeEnum, and a value reference;
The GetResult functions include double&, long&, and SVMatroxString& types. Each of these functions take a SVMatroxBarCode reference, a SVBarcodeControlTypeEnum, and a value reference;
*/
class SVMatroxBarCodeInterface
{
public:

	typedef SVMatroxApplicationInterface::SVStatusCode SVStatusCode;

	SVMatroxBarCodeInterface();

	virtual ~SVMatroxBarCodeInterface();

	// Use this instead of McodeControl
	static SVStatusCode Set( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const double& p_dValue );
	static SVStatusCode Set( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const long& p_lValue );
	static SVStatusCode Set( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const SVString& p_dValue );

	// Use this instead of McodeInquire.
	static SVStatusCode Get( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, double& p_dValue ) ;
	static SVStatusCode Get( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, long& p_dValue ) ;
	static SVStatusCode Get( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, SVString& p_dValue ) ;

	// The following functions call McodeGetResult.
	static bool GetResult( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, double& p_rdValue, SvStl::MessageContainerVector *pErrorMessages=nullptr);
	static bool GetResult( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, long& p_rlValue, SvStl::MessageContainerVector *pErrorMessages=nullptr);
	static bool GetResult( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, SVString& p_rsValue, SvStl::MessageContainerVector *pErrorMessages=nullptr);

	// Does the work of McodeAllocate 
	static SVStatusCode Create( SVMatroxBarCode& p_rCodeID, SVBarCodeTypesEnum p_eType );
	static SVStatusCode Create( SVMatroxBarCode& p_rCodeID, const SVMatroxBarCode& p_eFromCodeID );

	// Does the work of McodeFree 
	static SVStatusCode Destroy( SVMatroxBarCode& p_rCodeID );

	// McodeRead
	static bool Execute( const SVMatroxBarCode& p_rCodeID, const SVMatroxBuffer& p_rSourceId, SvStl::MessageContainerVector *pErrorMessages=nullptr );
private:
	static MatroxType Convert2MatroxType(SVBarCodeControlTypeEnum p_eType) ;
	static MatroxType Convert2MatroxType(SVBarCodeTypesEnum p_eType) ;
};

