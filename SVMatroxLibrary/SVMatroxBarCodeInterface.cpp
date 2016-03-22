// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBarCodeInterface
// * .File Name       : $Workfile:   SVMatroxBarCodeInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   23 Oct 2013 08:50:48  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVMatroxBarCodeInterface.h"
#include "SVMatroxImagingLibrary.h"  // has MIL includes
#include "SVMatroxResourceMonitor.h"
#include "SVBufferResource.h"
#include "SVStatusLibrary\MessageManagerResource.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#pragma endregion Includes

/**
@SVOperationName Default Constructor

@SVOperationDescription

*/
SVMatroxBarCodeInterface::SVMatroxBarCodeInterface()
{
	;
}

/**
@SVOperationName Destructor

@SVOperationDescription

*/
SVMatroxBarCodeInterface::~SVMatroxBarCodeInterface()
{
	;
}


/**
@SVOperationName Convert2MatroxType - SVBarCodeControlTypeEnum

@SVOperationDescription This function converts the SVObserver SVBarCodeControlTypeEnum to a matrox Enum.

*/
long SVMatroxBarCodeInterface::Convert2MatroxType(SVBarCodeControlTypeEnum p_eType) 
{
	long l_lControlType = M_UNINITIALIZED;
	switch( p_eType )
	{
		case SVBCForeGroundValue:
		{
			l_lControlType = M_FOREGROUND_VALUE;
			break;
		}
		case SVBCCellNumberX:
		{
			l_lControlType = M_CELL_NUMBER_X;
			break;
		}
		case SVBCCellNumberY:
		{
			l_lControlType = M_CELL_NUMBER_Y;
			break;
		}
		case SVBCEncoding:
		{
			l_lControlType = M_ENCODING;
			break;
		}
		case SVBCErrorCorrection:
		{
			l_lControlType = M_ERROR_CORRECTION;
			break;
		}
		case SVBCCellSizeMin:
		{
			l_lControlType = M_CELL_SIZE_MIN;
			break;
		}
		case SVBCCellSizeMax:
		{
			l_lControlType = M_CELL_SIZE_MAX;
			break;
		}
		case SVBCSearchAngle:
		{
			l_lControlType = M_SEARCH_ANGLE;
			break;
		}
		case SVBCSearchAngleDeltaNeg:
		{
			l_lControlType = M_SEARCH_ANGLE_DELTA_NEG;
			break;
		}
		case SVBCSearchAngleDeltaPos:
		{
			l_lControlType = M_SEARCH_ANGLE_DELTA_POS;
			break;
		}
		case SVBCThreshold:
		{
			l_lControlType = M_THRESHOLD;
			break;
		}
		case SVBCCodeTimeout:
		{
			l_lControlType = M_TIMEOUT;
			break;
		}
		case SVBCBarCodeStatus:
		{
			l_lControlType = M_STATUS;
			break;
		}
		case SVBCBarCodeString:
		{
			l_lControlType = M_STRING;
			break;
		}
		case SVBCBarcodeStringSize:
		{
			l_lControlType = M_STRING_SIZE;
			break;
		}
		case SVBCBarcodeScore:
		{
			l_lControlType = M_SCORE;
			break;
		}
		case SVBCSpeed:
		{
			l_lControlType = M_SPEED;
			break;
		}
		case SVBCAdaptiveThreshold:
		{
			l_lControlType = M_ADAPTIVE;
			break;
		}
		default:
		{
			break;
		}
	}

	// Combination long data type..
	if( p_eType & SVBCTypeLong )
	{
		l_lControlType |= M_TYPE_LONG;
	}

	return l_lControlType;
}


/**
@SVOperationName Convert2MatroxType - SVBarCodeTypesEnum

@SVOperationDescription This function converts the SVObserver SVBarCodeTypesEnumto a matrox Enum.

*/
// Bar Code Types......
long SVMatroxBarCodeInterface::Convert2MatroxType(SVBarCodeTypesEnum p_eType) 
{
	long l_lControlType = M_UNINITIALIZED;
	switch( p_eType )
	{
		case SVBC412:
		{
			l_lControlType = M_BC412;
			break;
		}
		case SVCodeABar:
		{
			l_lControlType = M_CODABAR ;
			break;
		}
		case SVCode39:
		{
			l_lControlType = M_CODE39 ;
			break;
		}
		case SVCode128:
		{
			l_lControlType = M_CODE128;
			break;
		}
		case SVDataMatrix:
		{
			l_lControlType = M_DATAMATRIX;
			break;
		}
		case SVEan13:
		{
			l_lControlType = M_EAN13;
			break;
		}
		case SVInterleaved25:
		{
			l_lControlType = M_INTERLEAVED25;
			break;
		}
		case SVMaxiCode:
		{
			l_lControlType = M_MAXICODE;
			break;
		}
		case SVPDF417:
		{
			l_lControlType = M_PDF417;
			break;
		}
		case SVPharmaCode:
		{
			l_lControlType = M_PHARMACODE;
			break;
		}
		case SVPlanet:
		{
			l_lControlType = M_PLANET;
			break;
		}
		case SVPostNet:
		{
			l_lControlType = M_POSTNET;
			break;
		}
		case SVRssCode:
		{
			l_lControlType = M_RSSCODE;
			break;
		}
		case SVUpcA:
		{
			l_lControlType = M_UPC_A;
			break;
		}
		case SVUpcE:
		{
			l_lControlType = M_UPC_E;
			break;
		}
		default:
		{
			break;
		}
	}

	// Combination long data type..
	if( p_eType & SVBCTypeLong )
	{
		l_lControlType |= M_TYPE_LONG;
	}

	return l_lControlType;
}



/**
@SVOperationName Set double

@SVOperationDescription Uses McodeControl to set the control type with value.

*/
SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::Set( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const double& p_dValue )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rCodeID.empty() )
		{
				// Two matrox constants..
				if( p_eType == SVBCAdaptiveThreshold )
				{
					McodeControl( p_rCodeID.m_BarCodeId, M_THRESHOLD ,M_ADAPTIVE );
				}
				else if ( p_eType == SVBCAutoThreshold )
				{
					McodeControl( p_rCodeID.m_BarCodeId, M_THRESHOLD ,M_DEFAULT );
				}
				else if ( p_eType == SVBCUnEvenGrid )
				{
#if SV_CURRENT_MIL_VERSION == 0x0900
					if ( p_dValue )
					{
						McodeControl( p_rCodeID.m_BarCodeId, M_DISTORTION, M_UNEVEN_GRID_STEP );
					}
					else
					{
						McodeControl( p_rCodeID.m_BarCodeId, M_DISTORTION, M_DEFAULT );
					}
#endif
				}
				else
				{
					// One Matrox constant and value..
					long l_lControlType = Convert2MatroxType( p_eType );
					if( l_lControlType != M_UNINITIALIZED )
					{
						McodeControl( p_rCodeID.m_BarCodeId, l_lControlType ,p_dValue);
					}
					else
					{
						l_Code = SVMEE_INVALID_PARAMETER;
					}
				}
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}



/**
@SVOperationName Set - long

@SVOperationDescription Uses McodeControl to set the control type with value.

*/
SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::Set( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const long& p_lValue )
{
	return Set( p_rCodeID, p_eType, static_cast<double>( p_lValue ) );
}


/**
@SVOperationName Set - SVMatroxString

@SVOperationDescription Uses McodeControl to set the control type with value.

*/SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::Set( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const SVMatroxString& p_dValue )
{
	// Currently there are no strings to set.
	return SVMEE_INVALID_PARAMETER;
}



/**
@SVOperationName Get - double

@SVOperationDescription Uses McodeInquire to get the control type with value.

*/
SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::Get( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, double& p_dValue ) 
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rCodeID.empty() )
		{
			if( (p_eType & SVBCStringType) != SVBCStringType )
			{
				MIL_INT l_lControlType = Convert2MatroxType( p_eType );
				if( l_lControlType != M_UNINITIALIZED )
				{
					McodeInquire( p_rCodeID.m_BarCodeId, l_lControlType , &p_dValue);
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
				}
				else
				{
					l_Code = SVMEE_INVALID_PARAMETER;
				}
			}
			else
			{
				l_Code = SVMEE_INVALID_PARAMETER;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


/**
@SVOperationName Get - long

@SVOperationDescription Get returns the selected control value from the provided SVMatroxBarCode handle.

*/
SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::Get( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, long& p_rlValue ) 
{
	SVStatusCode l_Code;
	double l_dValue = 0.0;
	l_Code = Get( p_rCodeID, p_eType, l_dValue);
	p_rlValue = static_cast<long>( l_dValue );
	return l_Code;
}


/**
@SVOperationName Get - String

@SVOperationDescription Get returns the selected control value from the provided SVMatroxBarCode handle.

*/
SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::Get( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, SVMatroxString& p_rstrValue ) 
{
	// Currently there are no strings to Get.
	return SVMEE_INVALID_PARAMETER;
}


/**
@SVOperationName GetResult - double

@SVOperationDescription

*/
SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::GetResult( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, double& p_rdValue)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rCodeID.empty() )
		{
			long l_lControlType = Convert2MatroxType( p_eType );
			if( l_lControlType != M_UNINITIALIZED )
			{
				McodeGetResult( p_rCodeID.m_BarCodeId, l_lControlType , &p_rdValue);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_PARAMETER;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName GetResult - long

@SVOperationDescription

*/
SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::GetResult( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, long& p_rlValue)
{
	SVStatusCode l_Code;
	double l_dValue = 0.0;
	l_Code = GetResult( p_rCodeID, p_eType, l_dValue);
	p_rlValue = static_cast<long>( l_dValue );
	return l_Code;
}

/**
@SVOperationName GetResult - string

@SVOperationDescription

*/
SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::GetResult( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, SVMatroxString& p_rstrValue)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rCodeID.empty() )
		{
			if( (p_eType & SVBCStringType) == SVBCStringType )
			{
				long l_lControlType = Convert2MatroxType( p_eType );
				if( l_lControlType != M_UNINITIALIZED )
				{
					long l_lSize;
					SVBufferResource l_Resource;

					McodeGetResult( p_rCodeID.m_BarCodeId, M_STRING_SIZE | M_TYPE_LONG, &l_lSize );
					if( l_Resource.Resize( l_lSize + 256 ) == S_OK )
					{
						MIL_TEXT_CHAR* l_pChars = NULL;
						l_Resource.GetBuffer( l_pChars );
						McodeGetResult( p_rCodeID.m_BarCodeId, l_lControlType , l_pChars );
						l_Code = SVMatroxApplicationInterface::GetLastStatus();
						if( l_Code == SVMEE_STATUS_OK )
						{
							p_rstrValue = l_pChars;
						}
					}
					else
					{
						l_Code = SVMEE_STRING_TOO_LARGE;
						SvStl::MessageMgrStdDisplay Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, "SVMEE_STRING_TOO_LARGE", StdMessageParams, SvOi::Err_10250 );
					}
				}
				else
				{
					l_Code = SVMEE_INVALID_PARAMETER;
					SvStl::MessageMgrStdDisplay Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, "SVMEE_INVALID_PARAMETER", StdMessageParams, SvOi::Err_10251 );
				}
			}
			else
			{
				l_Code = SVMEE_WRONG_PARAMETER;
				SvStl::MessageMgrStdDisplay Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, "SVMEE_WRONG_PARAMETER", StdMessageParams, SvOi::Err_10252 );
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
			SvStl::MessageMgrStdDisplay Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, "SVMEE_INVALID_HANDLE", StdMessageParams, SvOi::Err_10253 );
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
		SvStl::MessageMgrStdDisplay Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, "Exception", StdMessageParams, SvOi::Err_10253 );
	}
#endif
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Execute

@SVOperationDescription Execute function calls the mil function McodeRead.

*/
SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::Execute( const SVMatroxBarCode& p_rCodeID, const SVMatroxBuffer& p_rSourceId )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rCodeID.empty() && !p_rSourceId.empty() )
		{
			McodeRead(p_rCodeID.m_BarCodeId, p_rSourceId.GetIdentifier(), M_DEFAULT);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Create - SVBarCodeTypesEnum

@SVOperationDescription This function creates a barcode object from the supplied SVBarCodeTypeEnum. This object is used for reading barcodes.

*/
SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::Create( SVMatroxBarCode& p_rCodeID, SVBarCodeTypesEnum p_eType )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_ID l_NewID = M_NULL;
		SVMatroxIdentifier l_lCodeType = Convert2MatroxType( p_eType );

		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == SVMEE_STATUS_OK )
		{
			l_NewID = McodeAlloc( M_DEFAULT_HOST , l_lCodeType, M_DEFAULT, M_NULL );
			// Check For errors
			l_Code = SVMatroxApplicationInterface::GetLastStatus();


			if( l_Code == SVMEE_STATUS_OK )
			{// Free handle if already filled.
				long l_ID = SVMatroxApplicationInterface::SVMatroxIntToHRESULT( l_NewID );
				SVMatroxResourceMonitor::InsertIdentifier( SVBarCodeID, l_ID );

				if( !p_rCodeID.empty() )	
				{
					Destroy( p_rCodeID );
				}
				// Set new handle
				p_rCodeID.m_BarCodeId = l_NewID;
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


/**
@SVOperationName Create - SVMatroxBarCode

@SVOperationDescription This function creates a barcode object from an existing SVMatroxBarCode.

*/
SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::Create( SVMatroxBarCode& p_rCodeID, const SVMatroxBarCode& p_FromCodeID )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		// This function creates a new barcode from an existing barcode.
		MIL_ID l_NewID = M_NULL;

		if( !p_FromCodeID.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				long l_lValue;

				McodeInquire(p_FromCodeID.m_BarCodeId, M_CODE_TYPE, &l_lValue );
				l_NewID = McodeAlloc(M_DEFAULT_HOST, l_lValue, M_DEFAULT, M_NULL );

				// Check For errors
				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				// If new code is ok then copy parameters from reference to new code.
				if( l_Code == SVMEE_STATUS_OK )
				{
					l_Code = SVMatroxApplicationInterface::GetFirstError();
					McodeInquire( p_FromCodeID.m_BarCodeId, M_ERROR_CORRECTION , &l_lValue );
					McodeControl( l_NewID, M_ERROR_CORRECTION, l_lValue );

					McodeInquire( p_FromCodeID.m_BarCodeId, M_FOREGROUND_VALUE , &l_lValue );
					McodeControl( l_NewID, M_FOREGROUND_VALUE, l_lValue );

					McodeInquire( p_FromCodeID.m_BarCodeId, M_DATAMATRIX_SHAPE  , &l_lValue );
					McodeControl( l_NewID, M_DATAMATRIX_SHAPE , l_lValue );

					McodeInquire( p_FromCodeID.m_BarCodeId, M_CELL_SIZE_MIN  , &l_lValue );
					McodeControl( l_NewID, M_CELL_SIZE_MIN , l_lValue );

					McodeInquire( p_FromCodeID.m_BarCodeId, M_CELL_NUMBER_Y , &l_lValue );
					McodeControl( l_NewID, M_CELL_NUMBER_Y, l_lValue );

					McodeInquire( p_FromCodeID.m_BarCodeId, M_CELL_NUMBER_X  , &l_lValue );
					McodeControl( l_NewID, M_CELL_NUMBER_X , l_lValue );

					McodeInquire( p_FromCodeID.m_BarCodeId, M_16BIT_CHARACTER  , &l_lValue );
					McodeControl( l_NewID, M_16BIT_CHARACTER , l_lValue );
					l_Code = SVMatroxApplicationInterface::GetFirstError();
				}

				if( l_Code == SVMEE_STATUS_OK )
				{// Free handle if already filled.
					SVMatroxResourceMonitor::InsertIdentifier( SVBarCodeID, l_NewID );

					if( !p_rCodeID.empty() )	
					{
						Destroy( p_rCodeID );
					}
					// Set new handle
					p_rCodeID.m_BarCodeId = l_NewID;
				}
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
	
}



/**
@SVOperationName Destroy

@SVOperationDescription Destroys the Bar Code handle.

*/
SVMatroxBarCodeInterface::SVStatusCode SVMatroxBarCodeInterface::Destroy( SVMatroxBarCode& p_rCodeID )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rCodeID.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				McodeFree( p_rCodeID.m_BarCodeId );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
				{
					SVMatroxResourceMonitor::EraseIdentifier( SVBarCodeID, p_rCodeID.m_BarCodeId );

					p_rCodeID.m_BarCodeId = M_NULL;
				}
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
}





