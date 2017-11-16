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
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "SVMessage\SVMessage.h"
#include "Definitions/StringTypeDef.h"
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
MatroxType SVMatroxBarCodeInterface::Convert2MatroxType(SVBarCodeControlTypeEnum p_eType) 
{
	MatroxType l_lControlType = M_UNINITIALIZED;
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
			l_lControlType = M_THRESHOLD_VALUE;
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
MatroxType SVMatroxBarCodeInterface::Convert2MatroxType(SVBarCodeTypesEnum p_eType) 
{
	MatroxType l_lControlType = M_UNINITIALIZED;
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
			l_lControlType = M_GS1_DATABAR;
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
		case SVQRCode:
		{
			l_lControlType = M_QRCODE;
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
HRESULT SVMatroxBarCodeInterface::Set( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const double& p_dValue )
{
	HRESULT Result(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rCodeID.empty() )
		{
				// Two matrox constants..
				if( p_eType == SVBCAdaptiveThreshold )
				{
					McodeControl( p_rCodeID.m_BarCodeId, M_THRESHOLD_MODE ,M_ADAPTIVE );
				}
				else if ( p_eType == SVBCAutoThreshold )
				{
					McodeControl( p_rCodeID.m_BarCodeId, M_THRESHOLD_MODE ,M_DEFAULT );
				}
				else if ( p_eType == SVBCUnEvenGrid )
				{
					if ( p_dValue )
					{
						McodeControl( p_rCodeID.m_BarCodeId, M_DISTORTION, M_UNEVEN_GRID_STEP );
					}
					else
					{
						McodeControl( p_rCodeID.m_BarCodeId, M_DISTORTION, M_DEFAULT );
					}
				}
				else
				{
					// One Matrox constant and value..
					MatroxType l_lControlType = Convert2MatroxType( p_eType );
					if( l_lControlType != M_UNINITIALIZED )
					{
						McodeControl( p_rCodeID.m_BarCodeId, l_lControlType ,p_dValue);
					}
					else
					{
						Result = SVMEE_INVALID_PARAMETER;
					}
				}
				Result = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			Result = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		Result = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return Result;
}



/**
@SVOperationName Set - long

@SVOperationDescription Uses McodeControl to set the control type with value.

*/
HRESULT SVMatroxBarCodeInterface::Set( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const long& p_lValue )
{
	return Set( p_rCodeID, p_eType, static_cast<double>( p_lValue ) );
}


/**
@SVOperationName Set - SVMatroxString

@SVOperationDescription Uses McodeControl to set the control type with value.

*/HRESULT SVMatroxBarCodeInterface::Set( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, const std::string& p_dValue )
{
	// Currently there are no strings to set.
	return SVMEE_INVALID_PARAMETER;
}



/**
@SVOperationName Get - double

@SVOperationDescription Uses McodeInquire to get the control type with value.

*/
HRESULT SVMatroxBarCodeInterface::Get( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, double& p_dValue ) 
{
	HRESULT Result( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rCodeID.empty() )
		{
			if( (p_eType & SVBCStringType) != SVBCStringType )
			{
				MatroxType l_lControlType = Convert2MatroxType( p_eType );
				if( l_lControlType != M_UNINITIALIZED )
				{
					McodeInquire( p_rCodeID.m_BarCodeId, l_lControlType , &p_dValue);
					Result = SVMatroxApplicationInterface::GetLastStatus();
				}
				else
				{
					Result = SVMEE_INVALID_PARAMETER;
				}
			}
			else
			{
				Result = SVMEE_INVALID_PARAMETER;
			}
		}
		else
		{
			Result = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		Result = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == Result);
	return Result;
}


/**
@SVOperationName Get - long

@SVOperationDescription Get returns the selected control value from the provided SVMatroxBarCode handle.

*/
HRESULT SVMatroxBarCodeInterface::Get( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, long& p_rlValue ) 
{
	HRESULT l_Code;
	double l_dValue = 0.0;
	l_Code = Get( p_rCodeID, p_eType, l_dValue);
	p_rlValue = static_cast<long>( l_dValue );
	return l_Code;
}


/**
@SVOperationName Get - String

@SVOperationDescription Get returns the selected control value from the provided SVMatroxBarCode handle.

*/
HRESULT SVMatroxBarCodeInterface::Get( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, std::string& p_rstrValue ) 
{
	// Currently there are no strings to Get.
	return SVMEE_INVALID_PARAMETER;
}


/**
@SVOperationName GetResult - double

@SVOperationDescription

*/
bool SVMatroxBarCodeInterface::GetResult( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, double& p_rdValue, SvStl::MessageContainerVector *pErrorMessages)
{
	bool RetValue = true;

#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rCodeID.empty() )
		{
			MatroxType l_lControlType = Convert2MatroxType( p_eType );
			if( l_lControlType != M_UNINITIALIZED )
			{
				McodeGetResult( p_rCodeID.m_BarCodeId, l_lControlType , &p_rdValue);
				HRESULT MatroxCode = SVMatroxApplicationInterface::GetLastStatus();
				if( S_OK != MatroxCode )
				{
					RetValue = false;
					if (nullptr != pErrorMessages)
					{
						SVMatroxStatusInformation l_info;
						MatroxCode = SVMatroxApplicationInterface::GetLastStatus(l_info);
						SvDef::StringVector messageList;
						messageList.push_back(l_info.m_StatusString);
						SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_ErrorMcodeGetResult, messageList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10250);//, GetUniqueObjectID() );
						pErrorMessages->push_back(Msg);
					}
				}
			}
			else
			{
				RetValue = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_StringTooLarge, SvStl::SourceFileParams(StdMessageParams));
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			RetValue = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_StringTooLarge, SvStl::SourceFileParams(StdMessageParams));
				pErrorMessages->push_back(Msg);
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		RetValue = false;
		SVMatroxApplicationInterface::LogMatroxException();
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams));;
			pErrorMessages->push_back(Msg);
		}
	}
#endif
	assert(RetValue);
	return RetValue;
}

/**
@SVOperationName GetResult - long

@SVOperationDescription

*/
bool SVMatroxBarCodeInterface::GetResult( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, long& p_rlValue, SvStl::MessageContainerVector *pErrorMessages)
{
	double l_dValue = 0.0;
	bool retValue = GetResult( p_rCodeID, p_eType, l_dValue, pErrorMessages);
	p_rlValue = static_cast<long>( l_dValue );
	return retValue;
}

/**
@SVOperationName GetResult - string

@SVOperationDescription

*/
bool SVMatroxBarCodeInterface::GetResult( const SVMatroxBarCode& p_rCodeID, SVBarCodeControlTypeEnum p_eType, std::string& p_rstrValue, SvStl::MessageContainerVector *pErrorMessages)
{
	bool retValue = true;
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rCodeID.empty() )
		{
			if( (p_eType & SVBCStringType) == SVBCStringType )
			{
				MatroxType l_lControlType = Convert2MatroxType( p_eType );
				if( l_lControlType != M_UNINITIALIZED )
				{
					long l_lSize;
					SVBufferResource l_Resource;

					McodeGetResult( p_rCodeID.m_BarCodeId, M_STRING_SIZE | M_TYPE_LONG, &l_lSize );
					if( S_OK == l_Resource.Resize( l_lSize + 256 )  )
					{
						MIL_TEXT_CHAR* l_pChars = nullptr;
						l_Resource.GetBuffer( l_pChars );
						McodeGetResult( p_rCodeID.m_BarCodeId, l_lControlType , l_pChars );
						HRESULT MatroxCode = SVMatroxApplicationInterface::GetLastStatus();
						if( S_OK == MatroxCode )
						{
							p_rstrValue = l_pChars;
						}
						else
						{
							retValue = false;
							if (nullptr != pErrorMessages)
							{
								SVMatroxStatusInformation l_info;
								MatroxCode = SVMatroxApplicationInterface::GetLastStatus(l_info);
								SvDef::StringVector messageList;
								messageList.push_back(l_info.m_StatusString);
								SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_ErrorMcodeGetResult, messageList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10250);//, GetUniqueObjectID() );
								pErrorMessages->push_back(Msg);
							}
						}
					}
					else
					{
						retValue = false;
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_StringTooLarge, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10250);//, GetUniqueObjectID() );
							pErrorMessages->push_back(Msg);
						}
					}
				}
				else
				{
					retValue = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10251);//, GetUniqueObjectID() );
						pErrorMessages->push_back(Msg);
					}
				}
			}
			else
			{
				retValue = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10252);//, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			retValue = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10253);//, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		SVMatroxApplicationInterface::LogMatroxException();
		retValue = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10253);//, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
#endif
	assert(retValue);
	return retValue;
}

/**
@SVOperationName Execute

@SVOperationDescription Execute function calls the mil function McodeRead.

*/
bool SVMatroxBarCodeInterface::Execute( const SVMatroxBarCode& p_rCodeID, const SVMatroxBuffer& p_rSourceId, SvStl::MessageContainerVector *pErrorMessages )
{
	bool retValue = true;
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rCodeID.empty() && !p_rSourceId.empty() )
		{
			McodeRead(p_rCodeID.m_BarCodeId, p_rSourceId.GetIdentifier(), M_DEFAULT);
			HRESULT MatroxCode = SVMatroxApplicationInterface::GetLastStatus();
			if( S_OK != MatroxCode )
			{
				retValue = false;
				if (nullptr != pErrorMessages)
				{
					SVMatroxStatusInformation l_info;
					MatroxCode = SVMatroxApplicationInterface::GetLastStatus(l_info);
					SvDef::StringVector messageList;
					messageList.push_back(l_info.m_StatusString);
					SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_ErrorMcodeGetResult, messageList, SvStl::SourceFileParams(StdMessageParams));
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			retValue = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams));
				pErrorMessages->push_back(Msg);
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		retValue = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams));
			pErrorMessages->push_back(Msg);
		}
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(retValue);
	return retValue;
}

/**
@SVOperationName Create - SVBarCodeTypesEnum

@SVOperationDescription This function creates a barcode object from the supplied SVBarCodeTypeEnum. This object is used for reading barcodes.

*/
HRESULT SVMatroxBarCodeInterface::Create( SVMatroxBarCode& p_rCodeID, SVBarCodeTypesEnum p_eType )
{
	HRESULT Result( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_ID l_NewID = M_NULL;
		SVMatroxIdentifier l_lCodeType = Convert2MatroxType( p_eType );

		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		Result = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if (S_OK == Result)
		{
			l_NewID = McodeAlloc( M_DEFAULT_HOST , l_lCodeType, M_DEFAULT, M_NULL );
			// Check For errors
			Result = SVMatroxApplicationInterface::GetLastStatus();


			if (S_OK == Result)
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
		Result = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == Result);
	return Result;
}


/**
@SVOperationName Create - SVMatroxBarCode

@SVOperationDescription This function creates a barcode object from an existing SVMatroxBarCode.

*/
HRESULT SVMatroxBarCodeInterface::Create( SVMatroxBarCode& p_rCodeID, const SVMatroxBarCode& p_FromCodeID )
{
	HRESULT Result(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		// This function creates a new barcode from an existing barcode.
		MIL_ID l_NewID = M_NULL;

		if( !p_FromCodeID.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			Result = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if (S_OK == Result)
			{
				long l_lValue;

				McodeInquire(p_FromCodeID.m_BarCodeId, M_CODE_TYPE, &l_lValue );
				l_NewID = McodeAlloc(M_DEFAULT_HOST, l_lValue, M_DEFAULT, M_NULL );

				// Check For errors
				Result = SVMatroxApplicationInterface::GetLastStatus();

				// If new code is ok then copy parameters from reference to new code.
				if (S_OK == Result)
				{
					Result = SVMatroxApplicationInterface::GetFirstError();
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
					Result = SVMatroxApplicationInterface::GetFirstError();
				}

				if (S_OK == Result)
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
			Result = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		Result = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == Result);
	return Result;
	
}



/**
@SVOperationName Destroy

@SVOperationDescription Destroys the Bar Code handle.

*/
HRESULT SVMatroxBarCodeInterface::Destroy( SVMatroxBarCode& p_rCodeID )
{
	HRESULT Result(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rCodeID.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			Result = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if(S_OK == Result)
			{
				McodeFree( p_rCodeID.m_BarCodeId );
				Result = SVMatroxApplicationInterface::GetLastStatus();
				if (S_OK == Result)
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
		Result = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == Result);
	return Result;
}





