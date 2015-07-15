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

#ifndef SV_MATROX_BAR_CODE_INTERFACE_H
#define SV_MATROX_BAR_CODE_INTERFACE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxEnums.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxBarCode.h"
#include "SVMatroxBuffer.h"


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
	static SVStatusCode Set( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const SVMatroxString& p_dValue );

	// Use this instead of McodeInquire.
	static SVStatusCode Get( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, double& p_dValue ) ;
	static SVStatusCode Get( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, long& p_dValue ) ;
	static SVStatusCode Get( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, SVMatroxString& p_dValue ) ;

	// The following functions call McodeGetResult.
	static SVStatusCode GetResult( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, double& p_rdValue);
	static SVStatusCode GetResult( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, long& p_rlValue);
	static SVStatusCode GetResult( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, SVMatroxString& p_rsValue);

	// Does the work of McodeAllocate 
	static SVStatusCode Create( SVMatroxBarCode& p_rCodeID, SVBarCodeTypesEnum p_eType );
	static SVStatusCode Create( SVMatroxBarCode& p_rCodeID, const SVMatroxBarCode& p_eFromCodeID );

	// Does the work of McodeFree 
	static SVStatusCode Destroy( SVMatroxBarCode& p_rCodeID );

	// McodeRead
	static SVStatusCode Execute( const SVMatroxBarCode& p_rCodeID, const SVMatroxBuffer& p_rSourceId );
private:
	static long Convert2MatroxType(SVBarCodeControlTypeEnum p_eType) ;
	static long Convert2MatroxType(SVBarCodeTypesEnum p_eType) ;

};

#endif // SV_MATROX_BAR_CODE_INTERFACE_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBarCodeInterface.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:56:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:17:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
