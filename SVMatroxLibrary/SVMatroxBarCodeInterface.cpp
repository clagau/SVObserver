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

#include "stdafx.h"
#include "SVMatroxBarCodeInterface.h"
#include "SVMatroxImagingLibrary.h"  // has MIL includes
#include "SVMatroxResourceMonitor.h"
#include "SVBufferResource.h"
#include <assert.h>

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
						::MessageBox(NULL, "SVMEE_STRING_TOO_LARGE", NULL, MB_OK);
					}
				}
				else
				{
					l_Code = SVMEE_INVALID_PARAMETER;
					::MessageBox(NULL, "SVMEE_INVALID_PARAMETER", NULL, MB_OK);
				}
			}
			else
			{
				l_Code = SVMEE_WRONG_PARAMETER;
				::MessageBox(NULL, "SVMEE_WRONG_PARAMETER", NULL, MB_OK);
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
			::MessageBox(NULL, "SVMEE_INVALID_HANDLE", NULL, MB_OK);
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
		::MessageBox(NULL, "Exception",NULL, MB_OK);
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





// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBarCodeInterface.cpp_v  $
 * 
 *    Rev 1.2   23 Oct 2013 08:50:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified types to be compatible with 32 and 64bit librarys.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 11:08:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 14:56:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Jul 2012 12:45:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow gige to be compiled with both MIL 8.0 and 9.0.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   29 Jun 2012 14:06:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code needed to implement MIL 9.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Sep 2011 14:23:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Martox Library to include a new resource management system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   27 Jan 2011 10:31:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   28 May 2010 14:01:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with includes and make them consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   03 Mar 2010 13:03:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with string due to removal of MFC.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Feb 2010 11:32:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added functionality to allow for new OCR training functionality and fixed issues with unicode compiles.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   03 Jun 2008 14:37:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Bar Code Threshold Auto functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 May 2008 14:35:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  627
 * SCR Title:  Add RawData object to the Barcode Analyzer and format read string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added code for Adaptive Threshold ( M_ADAPTIVE )
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 May 2008 14:45:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Bug with buffer resource class that is used for the SVMatroxBarcodeInterface class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Apr 2008 11:38:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix Bug with Barcode String length where the length may not be correct due to matrox reporting the wrong length with multiple strings.  Also removed length limitation by creating a resource class for the MIL_TEXT_CHARs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:17:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
