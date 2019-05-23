// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcrInterface
// * .File Name       : $Workfile:   SVMatroxOcrInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 11:15:30  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMatroxOcrInterface.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxBlobInterface.h"
#include "SVMatroxBufferCreateChildStruct.h"
#include "SVMatroxBufferCreateStruct.h"
#include "SVMatroxBufferInterface.h"
#include "SVMatroxCommandDataImage.h"
#include "SVMatroxErrorEnum.h"
#include "SVMatroxHelper.h"
#include "SVMatroxImageInterface.h"
#include "SVMatroxResourceMonitor.h"
#include "SVMatroxOcrCalibrateStruct.h"
#include "SVMatroxOcrCreateStruct.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVCommandLibrary/SVCommandDataContainer.h"
#include "SVCommandLibrary/SVCommandDataHolder.h"
#include "SVFileSystemLibrary/SVFile.h"
#pragma endregion Includes

/**
@SVOperationName Default Constructor

@SVOperationDescription

*/
SVMatroxOcrInterface::SVMatroxOcrInterface()
{
}

/**
@SVOperationName Destructor

@SVOperationDescription

*/
SVMatroxOcrInterface::~SVMatroxOcrInterface()
{
}

/**
@SVOperationName Convert2MatroxType - SVOcrControlEnum

@SVOperationDescription This function converts a SVOcrControlEnum to matrox constants.

*/
__int64 SVMatroxOcrInterface::Convert2MatroxControlType(SVOcrControlEnum p_eType) 
{
	__int64 l_lControlType = 0;
	switch( p_eType )
	{
		case SVCharCellSizeX:
		{
			l_lControlType = M_CHAR_CELL_SIZE_X;
			break;
		}
		case SVCharCellSizeY:
		{
			l_lControlType = M_CHAR_CELL_SIZE_Y;
			break;
		}
		case SVCharInFont:
		{
			l_lControlType = M_CHAR_IN_FONT;
			break;
		}
		case SVCharNumber:
		{
			l_lControlType = M_CHAR_NUMBER;
			break;
		}
		case SVCharNumberInFont:
		{
			l_lControlType = M_CHAR_NUMBER_IN_FONT;
			break;
		}
		case SVCharOffsetX:
		{
			l_lControlType = M_CHAR_OFFSET_X;
			break;
		}
		case SVCharOffsetY:
		{
			l_lControlType = M_CHAR_OFFSET_Y;
			break;
		}
		case SVCharThickness:
		{
			l_lControlType = M_CHAR_THICKNESS;
			break;
		}
		case SVContext:
		{
			l_lControlType = M_CONTEXT;
			break;
		}
		case SVFontType:
		{
			l_lControlType = M_FONT_TYPE;
			break;
		}
		case SVStringAngle:
		{
			l_lControlType = M_STRING_ANGLE;
			break;
		}
		case SVTargetCharSizeX:
		{
			l_lControlType = M_TARGET_CHAR_SIZE_X;
			break;
		}
		case SVTargetCharSizeY:
		{
			l_lControlType = M_TARGET_CHAR_SIZE_Y;
			break;
		}

		case SVOcrStringSizeMax:
		{
			l_lControlType = M_STRING_SIZE_MAX;
			break;
		}
		case SVOcrForegroundValue:
		{
			l_lControlType = M_FOREGROUND_VALUE;
			break;
		}

		case SVOcrCharErase:
		{
			l_lControlType = M_CHAR_ERASE;
			break;
		}
		case SVOcrStringSize:
		{
			l_lControlType = M_STRING_SIZE;
			break;
		}
		case SVOcrIsFontPreprocessed:
		{
			l_lControlType = M_PREPROCESSED;
			break;
		}
		case SVOcrCharSizeX:
		{
			l_lControlType = M_CHAR_SIZE_X;
			break;
		}
		case SVOcrCharSizeY:
		{
			l_lControlType = M_CHAR_SIZE_Y;
			break;
		}
		case SVOcrStringAcceptance:
		{
			l_lControlType = M_STRING_ACCEPTANCE;
			break;
		}
		case SVOcrCharAcceptance:
		{
			l_lControlType = M_CHAR_ACCEPTANCE;
			break;
		}
		case SVOcrSpeed:
		{
			l_lControlType = M_SPEED;
			break;
		}
		case SVOcrTargetCharSpacing:
		{
			l_lControlType = M_TARGET_CHAR_SPACING;
			break;
		}
		case SVOcrCharInvalid:
		{
			l_lControlType = M_CHAR_INVALID;
			break;
		}
		case SVOcrMorphologicFiltering:
		{
			l_lControlType = M_MORPHOLOGIC_FILTERING;
			break;
		}
		case SVOcrCharPositionVariationX:
		{
			l_lControlType = M_CHAR_POSITION_VARIATION_X;
			break;
		}
		case SVOcrCharPositionVariationY:
		{
			l_lControlType = M_CHAR_POSITION_VARIATION_Y;
			break;
		}
		case SVOcrStringAngleDeltaNeg:
		{
			l_lControlType = M_STRING_ANGLE_DELTA_NEG;
			break;
		}
		case SVOcrStringAngleDeltaPos:
		{
			l_lControlType = M_STRING_ANGLE_DELTA_POS;
			break;
		}
		case SVOcrStringAngleInterpolationMode:
		{
			l_lControlType = M_STRING_ANGLE_INTERPOLATION_MODE;
			break;
		}
		case SVOcrBrokenChar:
		{
			l_lControlType = M_BROKEN_CHAR;
			break;
		}
		case SVOcrSkipStringLocation:
		{
			l_lControlType = M_SKIP_STRING_LOCATION;
			break;
		}
		case SVOcrThickenChar:
		{
			l_lControlType = M_THICKEN_CHAR;
			break;
		}
		case SVOcrTouchingChar:
		{
			l_lControlType = M_TOUCHING_CHAR;
			break;
		}
		case SVOcrStringNumber:
		{
			l_lControlType = M_STRING_NUMBER;
			break;
		}
		default:
		{
			assert(0);
		}

	}
	return l_lControlType;
}

/**
@SVOperationName Convert2MatroxType - SVOcrResultEnum

@SVOperationDescription This function converts a SVOcrResultEnum to matrox constants.

*/
__int64 SVMatroxOcrInterface::Convert2MatroxResultType(SVOcrResultEnum p_eType) 
{
	__int64 l_lControlType = 0;
	switch( p_eType )
	{

		// Result types
		case SVOcrResultText:
		{
			l_lControlType = M_TEXT;
			break;
		}
		case SVOcrResultTextLen:
		{
			l_lControlType = M_TEXT_LENGTH;
			break;
		}
		case SVOcrTextScore:
		{
			l_lControlType = M_TEXT_SCORE;
			break;
		}
		case SVOcrThreshold:
		{
			l_lControlType = M_THRESHOLD;
			break;
		}
		case SVOcrNbrString:
		{
			l_lControlType = M_NB_STRING;
			break;
		}
		case SVOcrString:
		{
			l_lControlType = M_STRING;
			break;
		}
		case SVOcrStringAllocSize:
		{
			l_lControlType = M_STRING_ALLOC_SIZE;
			break;
		}
		case SVOcrStringScore:
		{
			l_lControlType = M_STRING_SCORE;
			break;
		}
		case SVOcrResultStringSize:
		{
			l_lControlType = M_STRING_SIZE;
			break;
		}
		case SVOcrStringValidFlag:
		{
			l_lControlType = M_STRING_VALID_FLAG;
			break;
		}
		case SVOcrSum:
		{
			l_lControlType = M_SUM;
			break;
		}
		case SVOcrCharPosX:
		{
			l_lControlType = M_CHAR_POSITION_X;
			break;
		}
		case SVOcrCharPosY:
		{
			l_lControlType = M_CHAR_POSITION_Y;
			break;
		}
		case SVOcrCharScore:
		{
			l_lControlType = M_CHAR_SCORE;
			break;
		}
		case SVOcrCharSizeX:
		{
			l_lControlType = M_CHAR_SIZE_X;
			break;
		}
		case SVOcrCharSizeY:
		{
			l_lControlType = M_CHAR_SIZE_Y;
			break;
		}
		case SVOcrCharSpacing:
		{
			l_lControlType = M_CHAR_SPACING;
			break;
		}
		case SVOcrCharValidFlag:
		{
			l_lControlType = M_CHAR_VALID_FLAG;
			break;
		}
		default:
		{
			assert(0);
		}

	}
	return l_lControlType;
}


/**
@SVOperationName Convert2MatroxType - SVOcrTypeEnum

@SVOperationDescription This function converts a SVOcrTypeEnum to matrox constants.

*/
__int64 SVMatroxOcrInterface::Convert2MatroxCreateType( SVOcrTypeEnum p_eType ) 
{
	__int64 l_lCode = 0;
	switch( p_eType & SVBasics )
	{
		case SVSemiM1292:
		{
			l_lCode = M_SEMI_M12_92 ;
			break;
		}
		case SVSemiM1388:
		{
			l_lCode = M_SEMI_M13_88 ;
			break;
		}
		case SVUserDefined:
		{
			l_lCode = M_USER_DEFINED ;
			break;
		}
	}

	switch( p_eType & SVCombine1 )
	{
		case SVConstrained:
		{
			l_lCode |= M_CONSTRAINED ;
			break;
		}
		case SVGeneral:
		{
			l_lCode |= M_GENERAL ;
			break;
		}
	}
	return l_lCode;
}

/**
@SVOperationName Convert2MatroxType - SVOcrOperationEnum

@SVOperationDescription This function converts a SVOcrOperationEnum to matrox constants.

*/
__int64 SVMatroxOcrInterface::Convert2MatroxOperationType( SVOcrOperationEnum p_eType ) 
{
	__int64 l_lCode = 0;

	// Operation Constants
	if( (p_eType & SVOcrLoadConstraint) == SVOcrLoadConstraint)
	{
		l_lCode |= M_LOAD_CONSTRAINT;
	}

	if( (p_eType & SVOcrLoadControl) == SVOcrLoadControl)
	{
		l_lCode |= M_LOAD_CONTROL;
	}

	if( (p_eType & SVOcrDefault) == SVOcrDefault)
	{
		l_lCode |= M_DEFAULT;
	}

	if( (p_eType & SVOcrSave) == SVOcrSave)
	{
		l_lCode |= M_SAVE;
	}

	if( (p_eType & SVOcrSaveConstraint) == SVOcrSaveConstraint)
	{
		l_lCode |= M_SAVE_CONSTRAINT;
	}

	if( (p_eType & SVOcrSaveControl) == SVOcrSaveControl)
	{
		l_lCode |= M_SAVE_CONTROL;
	}

	// Combination Constants...
	if( (p_eType & SVOcrAllChar) == SVOcrAllChar )
	{
		l_lCode |= M_ALL_CHAR;
	}

	if( (p_eType  & SVOcrSort) == SVOcrSort )
	{
		l_lCode |= M_SORT;
	}

	if( (p_eType  & SVOcrRestore) == SVOcrRestore )
	{
		l_lCode |= M_RESTORE;
	}

	return l_lCode;
}


/**
@SVOperationName Create - Font

@SVOperationDescription Creates a SVMatroxOcr font object.

*/
HRESULT SVMatroxOcrInterface::Create( SVMatroxOcr& p_rFontId, const SVMatroxOcrCreateStruct& p_rCreateStruct )
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == S_OK )
		{
			MIL_ID l_NewID = M_NULL;
			__int64 l_lCreateType = Convert2MatroxCreateType( p_rCreateStruct.m_eFontType );
			if( l_lCreateType != 0 )
			{
				l_NewID = MocrAllocFont( M_DEFAULT_HOST, 
					l_lCreateType, 
					p_rCreateStruct.m_lCharNumber, 
					p_rCreateStruct.m_lCharCellSizeX, 
					p_rCreateStruct.m_lCharCellSizeY,
					p_rCreateStruct.m_lCharOffsetX,
					p_rCreateStruct.m_lCharOffsetY,
					p_rCreateStruct.m_lCharSizeX,
					p_rCreateStruct.m_lCharSizeY,
					p_rCreateStruct.m_lCharThickness,
					p_rCreateStruct.m_lStringLength,
					p_rCreateStruct.m_eInitFlag == SVOcrForegroundBlack ? M_FOREGROUND_BLACK : M_FOREGROUND_WHITE,
					M_NULL);

				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == S_OK )
				{
					SVMatroxResourceMonitor::InsertIdentifier( SVOCRID, l_NewID );

					// If one already exists then destroy the old.
					if( !p_rFontId.empty() )
					{
						Destroy( p_rFontId );
					}
					p_rFontId.m_OcrFontID = l_NewID;
				}
			}
			else
			{
				l_Code = SVMEE_WRONG_PARAMETER;
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
	return l_Code;
}

/**
@SVOperationName Create - Result

@SVOperationDescription Creates a SVMatroxIdentifier.

*/
HRESULT SVMatroxOcrInterface::CreateResult(__int64& p_rFontResult )
{
	// This function replaces MocrAllocResult
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == S_OK )
		{
			MIL_ID l_NewID;
			l_NewID = MocrAllocResult( M_DEFAULT_HOST, M_DEFAULT, M_NULL );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if( l_Code == S_OK )
			{
				SVMatroxResourceMonitor::InsertIdentifier( SVOCRResultID, l_NewID );

				DestroyResult( p_rFontResult );
				p_rFontResult = l_NewID;
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
	return l_Code;
}

/**
@SVOperationName Destroy - SVMatroxOcr

@SVOperationDescription Destroys a SVMatroxOcr.

*/
HRESULT SVMatroxOcrInterface::Destroy( SVMatroxOcr& p_rId )
{
	HRESULT l_Code = S_OK;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rId.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == S_OK )
			{
				SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

				l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

				if( l_Code == S_OK )
				{
					MocrFree( p_rId.m_OcrFontID );
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
					if( l_Code == S_OK )
					{
						SVMatroxResourceMonitor::EraseIdentifier( SVOCRID, p_rId.m_OcrFontID );

						p_rId.m_OcrFontID = M_NULL;
					}
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
	return l_Code;
}

/**
@SVOperationName DestroyResult

@SVOperationDescription Destroys a MatroxOcrResult.

*/
HRESULT SVMatroxOcrInterface::DestroyResult(__int64& rId )
{
	return DestroyMatroxId(rId, MocrFree, SVOCRResultID);
}



/**
@SVOperationName Ocr Calibrate Font

@SVOperationDescription This function automatically calibrates the X and Y size and spacing of the font of a specified OCR font context to match that of a string in the sample target image.

*/
HRESULT SVMatroxOcrInterface::CalibrateFont( const SVMatroxOcr& p_rFontId, const SVMatroxOcrCalibrateStruct& p_rCalStruct )
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		// This function replaces MocrCalibrateFont
		if( !p_rFontId.empty() && !p_rCalStruct.m_ImageBuff.empty() )
		{
			MocrCalibrateFont( p_rCalStruct.m_ImageBuff.GetIdentifier(),
				p_rFontId.m_OcrFontID,
				const_cast<MIL_TEXT_CHAR*>(p_rCalStruct.m_strCalString.c_str()), 
				p_rCalStruct.m_TargetCharSizeXMin, 
				p_rCalStruct.m_TargetCharSizeXMax,
				p_rCalStruct.m_TargetCharSizeXStep,
				p_rCalStruct.m_TargetCharSizeYMin,
				p_rCalStruct.m_TargetCharSizeYMax,
				p_rCalStruct.m_TargetCharSizeYStep,
				M_DEFAULT);
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

HRESULT SVMatroxOcrInterface::CalibrateFontCommand( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	SVMatroxOcr l_Font;

	p_rResults.clear();

	l_Status = CreateFontIdFromFontData( p_rAttributes, l_Font );

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		SVMatroxCommandDataImage l_Image;
		SVMatroxOcrCalibrateStruct l_CalStruct;

		if( S_OK == l_Status )
		{
			std::vector<unsigned char> l_TempImage;

			l_Status = p_rAttributes.GetImage( _T( "Calibrate Image" ), l_TempImage );

			if( S_OK == l_Status )
			{
				l_Status = l_Image.SetData( l_TempImage );
			}

			if( S_OK == l_Status )
			{
				l_CalStruct.m_ImageBuff = l_Image.m_Buffer;
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rAttributes.GetValue( _T( "Calibrate String" ), l_Temp );

			if( S_OK == l_Status )
			{
				l_CalStruct.m_strCalString = static_cast< char* >( _bstr_t( l_Temp ) );
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rAttributes.GetValue( _T( "Maximum Character Size Width" ), l_Temp );

			if( S_OK == l_Status )
			{
				l_CalStruct.m_TargetCharSizeXMax = l_Temp;
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rAttributes.GetValue( _T( "Minimum Character Size Width" ), l_Temp );

			if( S_OK == l_Status )
			{
				l_CalStruct.m_TargetCharSizeXMin = l_Temp;
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rAttributes.GetValue( _T( "Maximum Character Size Height" ), l_Temp );

			if( S_OK == l_Status )
			{
				l_CalStruct.m_TargetCharSizeYMax = l_Temp;
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rAttributes.GetValue( _T( "Minimum Character Size Height" ), l_Temp );

			if( S_OK == l_Status )
			{
				l_CalStruct.m_TargetCharSizeYMin = l_Temp;
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rAttributes.GetValue( _T( "Character Step Size Width" ), l_Temp );

			if( S_OK == l_Status )
			{
				l_CalStruct.m_TargetCharSizeXStep = l_Temp;
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rAttributes.GetValue( _T( "Character Step Size Height" ), l_Temp );

			if( S_OK == l_Status )
			{
				l_CalStruct.m_TargetCharSizeYStep = l_Temp;
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = SVMatroxOcrInterface::Set( l_Font, SVOcrStringSize, static_cast< double >( l_CalStruct.m_strCalString.size() ) );
		}

		if( S_OK == l_Status )
		{
			l_Status = SVMatroxOcrInterface::CalibrateFont( l_Font, l_CalStruct );
		}

		if( S_OK == l_Status )
		{
			l_Status = CreateFontDataFromFontId( l_Font, p_rResults );
		}
	}

	Destroy( l_Font );

	return l_Status;
}


/**
@SVOperationName Ocr Copy Font

@SVOperationDescription This function copies a character representation of one, or many, font characters to/from the specified image buffer.

*/
HRESULT SVMatroxOcrInterface::CopyFont(const SVMatroxOcr& p_rFontId, const SVMatroxBuffer& p_rImageBufId,
		SVOcrOperationEnum p_eOperation, std::string& p_String)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rFontId.empty() && !p_rImageBufId.empty() )
		{
			char buf[128];
			strcpy_s( buf, 128, const_cast<MIL_TEXT_CHAR*>( p_String.c_str()));

			long l_lOperation = p_eOperation == SVOcrCopytoFont ? M_COPY_TO_FONT : M_COPY_FROM_FONT;
			
			MocrCopyFont(p_rImageBufId.GetIdentifier(), 
				p_rFontId.m_OcrFontID, 
				l_lOperation,
				buf );

			if( p_eOperation == SVOcrCopyFromFont )
			{
				p_String = buf;
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
@SVOperationName Ocr Preprocess

@SVOperationDescription This function preprocesses the specified OCR font context.

*/
HRESULT SVMatroxOcrInterface::Preprocess( const SVMatroxOcr& p_rFontId )
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rFontId.empty() )
		{
			MocrPreprocess( p_rFontId.m_OcrFontID, M_DEFAULT );
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
@SVOperationName Ocr Restore Font

@SVOperationDescription This function restores an OCR font context, previously saved with SaveFont(), from a file. 

*/
HRESULT SVMatroxOcrInterface::RestoreFont( SVMatroxOcr& p_rFontId, const std::string& p_sFileName, SVOcrOperationEnum p_eOperation)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		MIL_ID l_NewId = M_NULL;
		__int64 l_lOperation = 0;
		if( p_eOperation == SVOcrRestore)
		{
			l_lOperation = Convert2MatroxOperationType( p_eOperation );
			l_NewId = MocrRestoreFont( const_cast<MIL_TEXT_CHAR*>( p_sFileName.c_str()), l_lOperation, M_DEFAULT_HOST, M_NULL );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if( l_Code == S_OK )
			{
				SVMatroxResourceMonitor::InsertIdentifier( SVOCRID, l_NewId );

				// If restored a new code successfully then destroy the old if there is one.
				if( !p_rFontId.empty() )
				{
					Destroy( p_rFontId );
				}
				p_rFontId.m_OcrFontID = l_NewId;
			}
		}
		else
		{
			if( !p_rFontId.empty() )
			{
				l_lOperation = Convert2MatroxOperationType( p_eOperation );
				MocrRestoreFont( const_cast<MIL_TEXT_CHAR*>(p_sFileName.c_str()), l_lOperation, M_DEFAULT_HOST, &p_rFontId.m_OcrFontID );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
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
	return l_Code;
}

HRESULT SVMatroxOcrInterface::RestoreFont( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	std::vector<unsigned char> l_CharacterFileContents;
	std::vector<unsigned char> l_ControlFileContents;
	std::vector<unsigned char> l_ConstraintFileContents;
	std::string l_CharacterFileName;
	std::string l_ControlFileName;
	std::string l_ConstraintFileName;

	l_Status = p_rAttributes.GetBlock( _T( "Character File Contents" ), l_CharacterFileContents );

	if( S_OK == l_Status )
	{
		l_Status = ConvertByteVectorToTempFile( l_CharacterFileContents, l_CharacterFileName );
	}

	if( S_OK == p_rAttributes.GetBlock( _T( "Control File Contents" ), l_ControlFileContents ) )
	{
		l_Status = ConvertByteVectorToTempFile( l_ControlFileContents, l_ControlFileName );
	}

	if( S_OK == p_rAttributes.GetBlock( _T( "Constraint File Contents" ), l_ConstraintFileContents ) )
	{
		l_Status = ConvertByteVectorToTempFile( l_ConstraintFileContents, l_ConstraintFileName );
	}

	if( S_OK == l_Status )
	{
		SVMatroxOcr lFontHandle;

		l_Status = RestoreFont( lFontHandle, l_CharacterFileName, SVOcrRestore );

		if( S_OK == l_Status && 0 < l_ControlFileName.size() )
		{
			l_Status = RestoreFont( lFontHandle, l_ControlFileName, SVOcrLoadControl );
		}

		if( S_OK == l_Status && 0 < l_ConstraintFileName.size() )
		{
			l_Status = RestoreFont( lFontHandle, l_ConstraintFileName, SVOcrLoadConstraint );
		}

		if( S_OK == l_Status )
		{
			l_Status = CreateFontDataFromFontId( lFontHandle, p_rResults );
		}

		SVMatroxOcrInterface::Destroy( lFontHandle );
	}

	if( 0 < l_CharacterFileName.size() )
	{
		::remove( l_CharacterFileName.c_str() );
	}

	if( 0 < l_ControlFileName.size() )
	{
		::remove( l_ControlFileName.c_str() );
	}

	if( 0 < l_ConstraintFileName.size() )
	{
		::remove( l_ConstraintFileName.c_str() );
	}

	return l_Status;
}

/**
@SVOperationName Ocr Save Font

@SVOperationDescription This function saves an existing OCR font context to disk using the MIL font file format.

*/
HRESULT SVMatroxOcrInterface::SaveFont( const SVMatroxOcr& p_rFontId, const std::string& p_sFileName, SVOcrOperationEnum p_eOperation)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lOperation ;
		if( !p_rFontId.empty() )
		{
			l_lOperation = Convert2MatroxOperationType( p_eOperation );
			if( l_lOperation != 0 )
			{
				MocrSaveFont( const_cast<MIL_TEXT_CHAR*>(p_sFileName.c_str()), l_lOperation, p_rFontId.m_OcrFontID );
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
	return l_Code;
}

HRESULT SVMatroxOcrInterface::SaveFont( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	std::string l_CharacterFileName;
	std::string l_ControlFileName;
	std::string l_ConstraintFileName;

	SVMatroxOcr l_Font;

	l_Status = CreateFontIdFromFontData( p_rAttributes, l_Font );

	if( S_OK == l_Status )
	{
		l_Status = CreateTempFileName( l_CharacterFileName );
	}

	if( S_OK == l_Status )
	{
		l_Status = CreateTempFileName( l_ControlFileName );
	}

	if( S_OK == l_Status )
	{
		l_Status = CreateTempFileName( l_ConstraintFileName );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::SaveFont( l_Font, l_CharacterFileName, SVOcrSave );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::SaveFont( l_Font, l_ControlFileName, SVOcrSaveControl );
	}
	
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::SaveFont( l_Font, l_ConstraintFileName, SVOcrSaveConstraint );
	}

	SVMatroxOcrInterface::Destroy( l_Font );

	if( S_OK == l_Status )
	{
		std::vector<unsigned char> l_CharacterFileContents;
		std::vector<unsigned char> l_ControlFileContents;
		std::vector<unsigned char> l_ConstraintFileContents;

		if( S_OK == l_Status )
		{
			l_Status = ConvertFileToByteVector( l_CharacterFileName, l_CharacterFileContents );
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rResults.SetBlock( _T( "Character File Contents" ), l_CharacterFileContents, true );
		}

		if( S_OK == l_Status )
		{
			l_Status = ConvertFileToByteVector( l_ControlFileName, l_ControlFileContents );
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rResults.SetBlock( _T( "Control File Contents" ), l_ControlFileContents, true );
		}

		if( S_OK == l_Status )
		{
			l_Status = ConvertFileToByteVector( l_ConstraintFileName, l_ConstraintFileContents );
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rResults.SetBlock( _T( "Constraint File Contents" ), l_ConstraintFileContents, true );
		}
	}

	if( 0 < l_CharacterFileName.size() )
	{
		::remove( l_CharacterFileName.c_str() );
	}

	if( 0 < l_ControlFileName.size() )
	{
		::remove( l_ControlFileName.c_str() );
	}

	if( 0 < l_ConstraintFileName.size() )
	{
		::remove( l_ConstraintFileName.c_str() );
	}

	return l_Status;
}

HRESULT SVMatroxOcrInterface::FindFontCharacters( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_BlobResultsPtr;
	SIZE imageSize = { 0, 0 };
	_variant_t l_OffsetX = 0;
	_variant_t l_OffsetY = 0;
	_variant_t l_Width = 0;
	_variant_t l_Height = 0;
	_variant_t l_IsBlack = true;
	SVMatroxCommandDataImage l_SourceImage;
	SVCommandDataContainer* l_pBlobResults = nullptr;
	SVMatroxBlobInterface::SVBlobList l_Blobs;
	SVMatroxBlobInterface::SVBlobOffsetList l_FilteredBlobs;

	l_Status = p_rAttributes.GetValue( _T( "Offset X" ), l_OffsetX );

	if( S_OK == l_Status )
	{
		l_Status = p_rAttributes.GetValue( _T( "Offset Y" ), l_OffsetY );
	}

	if( S_OK == l_Status )
	{
		l_Status = p_rAttributes.GetValue( _T( "Width" ), l_Width );
	}

	if( S_OK == l_Status )
	{
		l_Status = p_rAttributes.GetValue( _T( "Height" ), l_Height );
	}

	if( S_OK == l_Status )
	{
		l_Status = p_rAttributes.GetValue( _T( "Is Black Background" ), l_IsBlack );
	}

	if( S_OK == l_Status )
	{
		std::vector<unsigned char> l_TempImage;

		l_Status = p_rAttributes.GetImage( _T( "Source Image" ), l_TempImage );

		if( S_OK == l_Status )
		{
			l_Status = l_SourceImage.SetData( l_TempImage );
		}
	}

	if( S_OK == l_Status )
	{
		long l_Temp = 0;
		l_Status = SVMatroxBufferInterface::Get( l_SourceImage.m_Buffer, SVSizeX, l_Temp );
		imageSize.cx = static_cast< LONG >( l_Temp );
	}

	if( S_OK == l_Status )
	{
		long l_Temp = 0;
		l_Status = SVMatroxBufferInterface::Get( l_SourceImage.m_Buffer, SVSizeY, l_Temp );
		imageSize.cy = static_cast< LONG >( l_Temp );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBlobInterface::FindSizedBlobs( l_SourceImage.m_Buffer, !( bool( l_IsBlack ) ), l_Blobs );
	}

	if( S_OK == l_Status )
	{
		SIZE size = { l_Width, l_Height };
		POINT offset = { l_OffsetX, l_OffsetY };

		// Detemine suggested Blob Size
		if (size.cx == 0)
		{
			size.cx = SVMatroxBlobInterface::CalculateOptimalWidth(l_Blobs, static_cast<unsigned short>(offset.x));
			size.cx += offset.x;
		}
		if (size.cy == 0)
		{
			size.cy = SVMatroxBlobInterface::CalculateOptimalHeight(l_Blobs, static_cast<unsigned short>(offset.y));
			size.cy += offset.y;
		}

		// Convert SVBlobList to SVBlobOffsetList
		l_Status = SVMatroxBlobInterface::CreateBlobOffsetResultList(l_Blobs, l_FilteredBlobs, size, offset, imageSize);

		if( S_OK == l_Status )
		{
			l_Status = p_rResults.SetValue( _T( "Width" ), size.cx, true );
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rResults.SetValue( _T( "Height" ), size.cy, true );
		}
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBlobInterface::FillResults( l_FilteredBlobs, p_rResults );
	}

	return l_Status;
}

HRESULT SVMatroxOcrInterface::CharacterThickness( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	long l_Thickness = 0;

	SVCommandDataFacadePtr l_BlobResultsPtr;
	_variant_t l_BackgroundType = DARK;
	SVMatroxCommandDataImage l_ThresholdImage;
	SVMatroxBlobInterface::SVBlobList l_Blobs;
	long lSizeX = 0;
	long lSizeY = 0;

	l_Status = p_rAttributes.GetValue( _T( "Background Type" ), l_BackgroundType );

	if( S_OK == l_Status )
	{
		std::vector<unsigned char> l_TempImage;

		l_Status = p_rAttributes.GetImage( _T( "Threshold Image" ), l_TempImage );

		if( S_OK == l_Status )
		{
			l_Status = l_ThresholdImage.SetData( l_TempImage );
		}
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBufferInterface::Get( l_ThresholdImage.m_Buffer, SVSizeX, lSizeX );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBufferInterface::Get( l_ThresholdImage.m_Buffer, SVSizeY, lSizeY );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBlobInterface::FindSizedBlobs( l_ThresholdImage.m_Buffer, ( static_cast< BYTE >( l_BackgroundType ) != DARK ), l_Blobs );
	}

	if( S_OK == l_Status )
	{
		if( l_Blobs.size() > 0 )
		{
			std::vector<long> l_aItterations ;
			//l_aItterations.resize(results.size());

			for( size_t x = 0 ; x < l_Blobs.size() ; x++ )
			{
				const SVMatroxBlobInterface::SVBlob& l_rBlob = l_Blobs[ x ];

				// Only try to deal with objects that are large enough to filter.
				if( l_rBlob.area > 20 
					&& (l_rBlob.boundingRect.bottom - l_rBlob.boundingRect.top)>3 
					&& (l_rBlob.boundingRect.right - l_rBlob.boundingRect.left)>3)
				{
					// create child buffer around character
					SVMatroxBufferCreateChildStruct l_CreateStruct(l_ThresholdImage.m_Buffer);
					l_CreateStruct.m_data.m_lBand = 0;
					l_CreateStruct.m_data.m_lOffX = l_rBlob.boundingRect.left - 1 >= 0 ? l_rBlob.boundingRect.left - 1 : l_rBlob.boundingRect.left;
					l_CreateStruct.m_data.m_lOffY = l_rBlob.boundingRect.top - 1 >= 0 ? l_rBlob.boundingRect.top - 1 : l_rBlob.boundingRect.top;
					long lNewSize = l_rBlob.boundingRect.right - l_rBlob.boundingRect.left + 3;
					l_CreateStruct.m_data.m_lSizeX = lNewSize < lSizeX - l_CreateStruct.m_data.m_lOffX ? lNewSize : l_rBlob.boundingRect.right - l_rBlob.boundingRect.left + 1;
					lNewSize = l_rBlob.boundingRect.bottom - l_rBlob.boundingRect.top + 3;
					l_CreateStruct.m_data.m_lSizeY = lNewSize < lSizeY - l_CreateStruct.m_data.m_lOffY ? lNewSize : l_rBlob.boundingRect.bottom - l_rBlob.boundingRect.top + 1;
					l_CreateStruct.m_data.m_lParentBandCount = 1;

					SVMatroxBuffer ROIBuf;
					HRESULT l_Code = SVMatroxBufferInterface::Create( ROIBuf, l_CreateStruct);

					long l_tmp;
					if( S_OK == ThicknessCalculation( ROIBuf, l_BackgroundType, l_tmp ) )
					{
						l_aItterations.push_back( l_tmp);
					}

					ROIBuf.clear();
				}
			}

			// calculate average thickness
			if( l_aItterations.size() > 0 )
			{
				long sum = 0;
				for( size_t i = 0 ; i < l_aItterations.size() ; i++ )
				{
					sum += l_aItterations[i];
				}
				l_Thickness = sum / static_cast< long >( l_aItterations.size() );
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rResults.SetValue( _T( "Thickness" ), l_Thickness, true );
		}
	}

	return l_Status;
}

/**
@SVOperationName Set Verify

@SVOperationDescription This function sets the verify string and the verify flag which determine the operation is used during the execute.

*/
HRESULT SVMatroxOcrInterface::SetVerify( SVMatroxOcr& p_rFontId, const std::string& p_strVerifyString, const bool p_bVerifyOn )
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rFontId.empty() )
		{
			p_rFontId.m_bVerify = p_bVerifyOn;
			p_rFontId.m_VerifyString = p_strVerifyString;
			l_Code = S_OK;
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
@SVOperationName Set - double&

@SVOperationDescription This function sets various OCR controls for a read/verify operation.

*/
HRESULT SVMatroxOcrInterface::Set( const SVMatroxOcr& p_rFontId, SVOcrControlEnum p_eControlType, const double& p_dValue )
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lControlType = Convert2MatroxControlType( p_eControlType );
		if( l_lControlType != 0 )
		{
			if(!p_rFontId.empty())
			{
				MocrControl(p_rFontId.m_OcrFontID, l_lControlType, p_dValue);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
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
@SVOperationName Set - long&

@SVOperationDescription This function sets various OCR controls for a read/verify operation.

*/
HRESULT SVMatroxOcrInterface::Set( const SVMatroxOcr& p_rFontId, SVOcrControlEnum p_eControlType, const long& p_lValue )
{
	return Set(p_rFontId, p_eControlType, static_cast<double>(p_lValue));
}


/**
@SVOperationName Get - double&

@SVOperationDescription This function inquires about an OCR font context or an OCR result buffer setting.

*/
HRESULT SVMatroxOcrInterface::Get( const SVMatroxOcr& p_rFontId, SVOcrControlEnum  p_eControlType, double& p_rdValue) 
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lControlType = Convert2MatroxControlType( p_eControlType );
		if( l_lControlType != 0 )
		{
			if(!p_rFontId.empty())
			{
				MocrInquire(p_rFontId.m_OcrFontID, l_lControlType, &p_rdValue);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
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
@SVOperationName Get - long&

@SVOperationDescription This function inquires about an OCR font context or an OCR result buffer setting.

*/
HRESULT SVMatroxOcrInterface::Get( const SVMatroxOcr& p_rFontId, SVOcrControlEnum  p_eControlType, long& p_rlValue) 
{
	double l_dValue;
	HRESULT l_Code = Get( p_rFontId, p_eControlType, l_dValue);
	if( l_Code == S_OK )
	{
		p_rlValue = static_cast<long>(l_dValue);
	}
	return l_Code;
}


/**
@SVOperationName Get - SVMatroxString&

@SVOperationDescription This function inquires about an OCR font context or an OCR result buffer setting.

*/
HRESULT SVMatroxOcrInterface::Get( const SVMatroxOcr& p_rFontId, SVOcrControlEnum  p_eControlType, std::string& p_rStrValue) 
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		double l_dSize=0;
		if( p_eControlType == SVOcrString )
		{
			if(!p_rFontId.empty())
			{
				MocrInquire( p_rFontId.m_OcrFontID, M_STRING_SIZE, &l_dSize );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == S_OK && l_dSize > 0)
				{
					p_rStrValue.resize( static_cast<long>(l_dSize) );
					MocrInquire(p_rFontId.m_OcrFontID, M_STRING, &p_rStrValue[0]);
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
				}
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
		else if( p_eControlType == SVCharInFont )
		{
			if(!p_rFontId.empty())
			{
				MocrInquire( p_rFontId.m_OcrFontID, M_CHAR_NUMBER_IN_FONT, &l_dSize );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == S_OK && l_dSize > 0 )
				{
					p_rStrValue.resize( static_cast<long>(l_dSize) );
					MocrInquire(p_rFontId.m_OcrFontID, M_CHAR_IN_FONT, &p_rStrValue[0]);
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
				}
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
		else
		{
			l_Code = SVMEE_WRONG_PARAMETER;
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
@SVOperationName SetResult - double

@SVOperationDescription This function sets various OCR controls for a read/verify operation.

*/
HRESULT SVMatroxOcrInterface::SetResult( const __int64& rResultId, SVOcrControlEnum p_eControlType, const double& p_dValue )
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lControlType = Convert2MatroxControlType( p_eControlType );
		if( l_lControlType != 0 )
		{
			if(M_NULL != rResultId)
			{
				MocrControl(rResultId, l_lControlType, p_dValue);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
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
@SVOperationName SetResult - long&

@SVOperationDescription This function sets various OCR controls for a read/verify operation.

*/
HRESULT SVMatroxOcrInterface::SetResult( const __int64& rResultId, SVOcrControlEnum p_eControlType, const long& p_lValue )
{
	return SetResult(rResultId, p_eControlType, static_cast<double>(p_lValue));
}



/**
@SVOperationName SetResult - double&

@SVOperationDescription This function retrieves the result(s) of the specified type from an OCR

*/
HRESULT SVMatroxOcrInterface::GetResult( const __int64& rResultId, SVOcrResultEnum  p_eControlType, double& p_rdValue)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lControlType = Convert2MatroxResultType( p_eControlType );
		if( l_lControlType != 0 )
		{
			if(M_NULL != rResultId)
			{
				MocrGetResult(rResultId, l_lControlType, &p_rdValue);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
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
@SVOperationName SetResult - long&

@SVOperationDescription This function retrieves the result(s) of the specified type from an OCR

*/
HRESULT SVMatroxOcrInterface::GetResult( const __int64& rResultId, SVOcrResultEnum  p_eControlType, long& p_rlValue)
{
	double l_dValue;
	HRESULT l_Code = GetResult(rResultId, p_eControlType, l_dValue);
	if( l_Code == S_OK )
	{
		p_rlValue = static_cast<long>(l_dValue);
	}
	return l_Code;
}

/**
@SVOperationName SetResult - SVMatroxString&

@SVOperationDescription This function retrieves the result(s) of the specified type from an OCR

*/
HRESULT SVMatroxOcrInterface::GetResult( const __int64& rResultId, SVOcrResultEnum  p_eControlType, std::string& p_rStrValue)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lControlType = Convert2MatroxResultType( p_eControlType );
		if( l_lControlType != 0 )
		{
			if( M_NULL != rResultId)
			{
				MIL_TEXT_CHAR l_cBuf[256];
				MocrGetResult(rResultId, l_lControlType, &l_cBuf);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == S_OK )
				{
					p_rStrValue = l_cBuf;
				}
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
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

HRESULT SVMatroxOcrInterface::GetResult( const __int64& rResultId, SVOcrResultEnum InquireType, std::vector<double>& p_adValues )
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lControlType = Convert2MatroxResultType( InquireType );
		if( l_lControlType != 0 )
		{
			if( M_NULL != rResultId)
			{
				MocrGetResult(rResultId, l_lControlType, &p_adValues[0] );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
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
@SVOperationName Execute

@SVOperationDescription If the verify flag is off this function reads an unknown string from the specified target image using the specified OCR font context. If the verify flag is on this function determines whether a known string is present in the target image, and evaluates the quality of the string. 

*/
HRESULT SVMatroxOcrInterface::Execute( const __int64& rResultId, const SVMatroxOcr& p_rFontId, const SVMatroxBuffer& p_rImage )
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( M_NULL != rResultId && !p_rFontId.empty() && !p_rImage.empty() )
		{
			if( p_rFontId.m_bVerify )
			{
				// MocrVerify
				MocrVerifyString(p_rImage.GetIdentifier(), 
					p_rFontId.m_OcrFontID, 
					const_cast<MIL_TEXT_CHAR*>(p_rFontId.m_VerifyString.c_str()), 
					rResultId);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				// MocrRead
				MocrReadString( p_rImage.GetIdentifier(), p_rFontId.m_OcrFontID, rResultId);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
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
	return l_Code;
}

HRESULT SVMatroxOcrInterface::ReadString( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	SVMatroxOcr l_Font;

	p_rResults.clear();

	l_Status = CreateFontIdFromFontData( p_rAttributes, l_Font );

	if( S_OK == l_Status )
	{
		SVMatroxCommandDataImage l_Image;
		__int64 milResult = M_NULL;

		l_Font.m_bVerify = false;

		if( S_OK == l_Status )
		{
			std::vector<unsigned char> l_TempImage;

			l_Status = p_rAttributes.GetImage( _T( "Read Image" ), l_TempImage );

			if( S_OK == l_Status )
			{
				l_Status = l_Image.SetData( l_TempImage );
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = SVMatroxOcrInterface::CreateResult( milResult );
		}

		if( S_OK == l_Status )
		{
			l_Status = SVMatroxOcrInterface::Execute( milResult, l_Font, l_Image.m_Buffer );
		}

		if( S_OK == l_Status )
		{
			// Process the OCR chars returned from MocrReadString();
			long l_Length = 0L;
			double l_MatchScore = 0.0;
			std::string l_ReadString;

			l_Status = SVMatroxOcrInterface::GetResult( milResult, SVOcrResultStringSize, l_Length );

			if( l_Length != 0 )
			{
				if( S_OK == l_Status )
				{
					l_Status = SVMatroxOcrInterface::GetResult(milResult, SVOcrStringScore, l_MatchScore );
				}

				if( S_OK == l_Status )
				{
					l_Status = SVMatroxOcrInterface::GetResult(milResult, SVOcrString, l_ReadString );
				}
			}

			if( S_OK == l_Status )
			{
				l_Status = p_rResults.SetValue( _T( "Found String" ), _bstr_t( l_ReadString.c_str() ), true );
			}

			if( S_OK == l_Status )
			{
				l_Status = p_rResults.SetValue( _T( "Match Score" ), l_MatchScore, true );
			}
		}

		DestroyResult( milResult );
	}

	Destroy( l_Font );

	return l_Status;
}

HRESULT SVMatroxOcrInterface::VerifyString( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	SVMatroxOcr l_Font;

	p_rResults.clear();

	l_Status = CreateFontIdFromFontData( p_rAttributes, l_Font );

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		SVMatroxCommandDataImage l_Image;
		__int64 milResult = M_NULL;

		l_Font.m_bVerify = true;

		if( S_OK == l_Status )
		{
			std::vector<unsigned char> l_TempImage;

			l_Status = p_rAttributes.GetImage( _T( "Verify Image" ), l_TempImage );

			if( S_OK == l_Status )
			{
				l_Status = l_Image.SetData( l_TempImage );
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = p_rAttributes.GetValue( _T( "Verify String" ), l_Temp );

			if( S_OK == l_Status )
			{
				l_Font.m_VerifyString = static_cast< char* >( _bstr_t( l_Temp ) );
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = SVMatroxOcrInterface::Set( l_Font, SVOcrStringSize, static_cast< double >( l_Font.m_VerifyString.size() ) );
		}

		if( S_OK == l_Status )
		{
			l_Status = SVMatroxOcrInterface::CreateResult(milResult);
		}

		if( S_OK == l_Status )
		{
			l_Status = SVMatroxOcrInterface::Execute( milResult, l_Font, l_Image.m_Buffer );
		}

		if( S_OK == l_Status )
		{
			// Process the OCR chars returned from MocrReadString();
			long l_Length = 0L;
			double l_MatchScore = 0.0;
			std::string l_ReadString;

			l_Status = SVMatroxOcrInterface::GetResult( milResult, SVOcrResultStringSize, l_Length );

			if( l_Length != 0 )
			{
				if( S_OK == l_Status )
				{
					l_Status = SVMatroxOcrInterface::GetResult(milResult, SVOcrStringScore, l_MatchScore );
				}
			}

			if( S_OK == l_Status  )
			{
				l_Status = p_rResults.SetValue( _T( "Match Score" ), l_MatchScore, true );
			}
		}

		DestroyResult( milResult );
	}

	Destroy( l_Font );

	return l_Status;
}

HRESULT SVMatroxOcrInterface::CreateTempFileName( std::string& p_rFileName )
{
	HRESULT l_Status = S_OK;

	p_rFileName.clear();

	TCHAR l_szFileName[ MAX_PATH ];

	if( ::GetTempFileName( _T( "C:\\TEMP" ), _T( "FTF" ), 0, l_szFileName ) != 0 )
	{
		p_rFileName = l_szFileName;
		p_rFileName += _T( ".mfo" );

		::rename( l_szFileName, p_rFileName.c_str() );
	}
	else
	{
		l_Status = ::GetLastError();
	}

	return l_Status;
}

HRESULT SVMatroxOcrInterface::ConvertFileToByteVector(const std::string& rFileName, std::vector<unsigned char>& rFileContents)
{
	HRESULT l_Status = S_OK;

	rFileContents.clear();

	SVFile l_File;

	l_File.Open( rFileName.c_str(), SVFile::modeRead );
	l_Status = l_File.ReadContents( rFileContents );
	l_File.Close();

	return l_Status;
}

HRESULT SVMatroxOcrInterface::ConvertByteVectorToTempFile(const std::vector<unsigned char>& rFileContents, std::string& rFileName)
{
	HRESULT l_Status = S_OK;

	std::string l_FileName;

	l_Status = CreateTempFileName( l_FileName );

	if( S_OK == l_Status )
	{
		SVFile l_File;

		l_File.Open( l_FileName.c_str(), SVFile::modeWrite );
		l_Status = l_File.WriteContents( rFileContents );
		l_File.Close();

		if( S_OK == l_Status )
		{
			rFileName = l_FileName;
		}
	}

	return l_Status;
}

HRESULT SVMatroxOcrInterface::CreateFontDataFromFontId( const SVMatroxOcr& p_rFontId, SVCommandDataHolder& p_rFontData )
{
	HRESULT l_Status = S_OK;

	long lForeground;
	double dCharBoxSizeX;
	double dCharBoxSizeY;
	double dCharOffsetX;
	double dCharOffsetY;
	long lCharThickness;
	long lCharInCurrFont;
	long lStringLength;
	long lMaxStringLength;

	double l_dInvalidCharacter;
	double l_dCharacterAcceptance;
	double l_dStringAcceptance;
	double l_dOcrSpeed;
	double l_dSpacing;
	double l_dStringAngle;
	double l_dAngleDeltaNeg;
	double l_dAngleDeltaPos;
	double l_dInterpolation;
	double l_dPositionVariationX;
	double l_dPositionVariationY;
	double l_dSkipStringLocation;
	double l_dBrokenChar;
	double l_dMorphologicFiltering;
	double l_dThickenChar;
	double l_dTouchingChar;
	double l_dStringNumber;

	SVCommandDataHolder l_FontData;

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVCharNumberInFont, lCharInCurrFont );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVCharThickness, lCharThickness );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrForegroundValue, lForeground );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrStringSize, lStringLength );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrStringSizeMax, lMaxStringLength );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVCharCellSizeX, dCharBoxSizeX );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVCharCellSizeY, dCharBoxSizeY );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVCharOffsetX, dCharOffsetX );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVCharOffsetY, dCharOffsetY );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrCharAcceptance, l_dCharacterAcceptance );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrStringAcceptance, l_dStringAcceptance );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrSpeed, l_dOcrSpeed );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrTargetCharSpacing, l_dSpacing );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrCharInvalid, l_dInvalidCharacter );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVStringAngle, l_dStringAngle );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrStringAngleDeltaNeg, l_dAngleDeltaNeg );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrStringAngleDeltaPos, l_dAngleDeltaPos );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrStringAngleInterpolationMode, l_dInterpolation );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrCharPositionVariationX, l_dPositionVariationX );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrCharPositionVariationY, l_dPositionVariationY );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrSkipStringLocation, l_dSkipStringLocation );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrBrokenChar, l_dBrokenChar );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrMorphologicFiltering, l_dMorphologicFiltering );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrThickenChar, l_dThickenChar );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrTouchingChar, l_dTouchingChar );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVOcrStringNumber, l_dStringNumber );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Number Of Characters" ), lCharInCurrFont, true );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Width" ), dCharBoxSizeX, true );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Height" ), dCharBoxSizeY, true );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Character Offset X" ), dCharOffsetX, true );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Character Offset Y" ), dCharOffsetY, true );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Character Thickness" ), lCharThickness, true );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Is Foreground White" ), bool( lForeground == SVOcrForegroundWhite ), true );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "String Length" ), lStringLength, true );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Max String Length" ), lMaxStringLength, true );
	}

	if( S_OK == l_Status )
	{
		l_FontData.SetValue( _T( "Character Acceptance" ), l_dCharacterAcceptance );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "String Acceptance" ), l_dStringAcceptance );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Speed" ), l_dOcrSpeed );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Spacing" ), l_dSpacing );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Invalid Character" ), l_dInvalidCharacter );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Interpolation Mode" ), l_dInterpolation );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "String Angle" ), l_dStringAngle );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Angle Delta Neg" ), l_dAngleDeltaNeg );
	}

	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Angle Delta Pos" ), l_dAngleDeltaPos );
	}
	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Position Variation X" ), l_dPositionVariationX );
	}
	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Position Variation Y" ), l_dPositionVariationY );
	}
	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Skip String Location" ), l_dSkipStringLocation );
	}
	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Broken Char" ), l_dBrokenChar );
	}
	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Morphologic Filtering" ), l_dMorphologicFiltering );
	}
	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Thicken Char" ), l_dThickenChar );
	}
	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "Touching Char" ), l_dTouchingChar );
	}
	if( S_OK == l_Status )
	{
		l_Status = l_FontData.SetValue( _T( "String Number" ), l_dStringNumber );
	}

	if( S_OK == l_Status )
	{
		l_Status = UpdateCharacterListFromFontId( p_rFontId, l_FontData );
	}

	if( S_OK == l_Status )
	{
		l_Status = p_rFontData.SetContainer( _T( "Font" ), l_FontData, true );
	}

	return l_Status;
}

HRESULT SVMatroxOcrInterface::CreateFontIdFromFontData( const SVCommandDataHolder& p_rFontData, SVMatroxOcr& p_rFontId )
{
	HRESULT l_Status = S_OK;

	long l_Count = 0;
	long l_lStringLength;

	SVMatroxOcrCreateStruct l_Create;
	std::string l_CharacterFileName;
	std::string l_ControlFileName;
	std::string l_ConstraintFileName;

	double l_dCharacterAcceptance;
	double l_dStringAcceptance;
	double l_dOcrSpeed;
	double l_dSpacing;
	double l_dInvalidCharacter;
	double l_dStringAngle;
	double l_dAngleDeltaNeg;
	double l_dAngleDeltaPos;
	double l_dInterpolationMode;
	double l_dPositionVariationX;
	double l_dPositionVariationY;
	double l_dSkipStringLocation;
	double l_dBrokenChar;
	double l_dMorphologicFiltering;
	double l_dThickenChar;
	double l_dTouchingChar;
	double l_dStringNumber;

	SVCommandDataHolder l_FontData;

	l_Status = p_rFontData.GetContainer( _T( "Font" ), l_FontData );

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Number Of Characters" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_Create.m_lCharNumber = ( static_cast< long >( l_Temp ) < 256 ) ? 256 : l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Width" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_Create.m_lCharCellSizeX = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Height" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_Create.m_lCharCellSizeY = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Character Offset X" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_Create.m_lCharOffsetX = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Character Offset Y" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_Create.m_lCharOffsetY = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Character Thickness" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_Create.m_lCharThickness = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Is Foreground White" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_Create.m_eInitFlag = ( static_cast< bool >( l_Temp ) ) ? SVOcrForegroundWhite : SVOcrForegroundBlack;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Max String Length" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_Create.m_lStringLength = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "String Length" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_lStringLength = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Character Acceptance" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dCharacterAcceptance = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "String Acceptance" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dStringAcceptance = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Speed" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dOcrSpeed = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Invalid Character" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dInvalidCharacter = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Spacing" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dSpacing = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "String Angle" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dStringAngle = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Angle Delta Neg" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dAngleDeltaNeg = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Angle Delta Pos" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dAngleDeltaPos = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Interpolation Mode" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dInterpolationMode = l_Temp;
		}
	}
	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Position Variation X" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dPositionVariationX = l_Temp;
		}
	}
	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Position Variation Y" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dPositionVariationY = l_Temp;
		}
	}
	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Skip String Location" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dSkipStringLocation = l_Temp;
		}
	}
	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Broken Char" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dBrokenChar = l_Temp;
		}
	}
	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Morphologic Filtering" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dMorphologicFiltering = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Thicken Char" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dThickenChar = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "Touching Char" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dTouchingChar = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		_variant_t l_Temp;

		l_Status = l_FontData.GetValue( _T( "String Number" ), l_Temp );

		if( S_OK == l_Status )
		{
			l_dStringNumber = l_Temp;
		}
	}

	if( S_OK == l_Status )
	{
		l_Create.m_eFontType = SVUserDefinedConstrained;
		l_Create.m_lCharSizeX = l_Create.m_lCharCellSizeX - ( 2 * l_Create.m_lCharOffsetX );
		l_Create.m_lCharSizeY = l_Create.m_lCharCellSizeY - ( 2 * l_Create.m_lCharOffsetY );
	}

	if( !( p_rFontId.empty() ) )
	{
		Destroy( p_rFontId );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Create( p_rFontId, l_Create );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrStringSize, l_lStringLength );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrCharAcceptance, l_dCharacterAcceptance );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrStringAcceptance, l_dStringAcceptance );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrSpeed, l_dOcrSpeed );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrTargetCharSpacing, l_dSpacing );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrCharInvalid, l_dInvalidCharacter );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVStringAngle, l_dStringAngle );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrStringAngleDeltaNeg, l_dAngleDeltaNeg );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrStringAngleDeltaPos, l_dAngleDeltaPos );
	}
	if( S_OK == l_Status  )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrStringAngleInterpolationMode, l_dInterpolationMode );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrCharPositionVariationX, l_dPositionVariationX );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrCharPositionVariationY, l_dPositionVariationY );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrSkipStringLocation, l_dSkipStringLocation );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrBrokenChar, l_dBrokenChar );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrMorphologicFiltering, l_dMorphologicFiltering );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrThickenChar, l_dThickenChar );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrTouchingChar, l_dTouchingChar );
	}
	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Set( p_rFontId, SVOcrStringNumber, l_dStringNumber );
	}

	if( S_OK == l_Status )
	{
		l_Status = UpdateFontIdFromCharacterList( l_FontData, p_rFontId );

		if( S_OK != l_Status && !( p_rFontId.empty() ) )
		{
			Destroy( p_rFontId );
		}
	}

	return l_Status;
}

HRESULT SVMatroxOcrInterface::UpdateCharacterListFromFontId( const SVMatroxOcr& p_rFontId, SVCommandDataHolder& p_rFontData )
{
	HRESULT l_Status = S_OK;

	long l_Count = 0;
	long l_Foreground = SVOcrForegroundBlack;
	double dCharBoxSizeX = 0.0;
	double dCharBoxSizeY = 0.0;
	std::string l_strFontChars;
	SVCommandDataHolder l_Characters;
			
	l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVCharNumberInFont, l_Count );

	if( S_OK == l_Status )
	{
		l_strFontChars.resize( l_Count + 1 );

		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVCharInFont, l_strFontChars );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVCharCellSizeX, dCharBoxSizeX );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxOcrInterface::Get( p_rFontId, SVCharCellSizeY, dCharBoxSizeY );
	}

	for ( size_t i = 0; S_OK == l_Status && i < l_strFontChars.size(); ++i )
	{
		SVCommandDataHolder l_Character;
		SVMatroxBuffer l_CharHandle;
		SVMatroxBufferCreateStruct l_BufferInfo;

		std::string l_FontChar;
		
		l_FontChar += l_strFontChars[ i ];

		l_BufferInfo.m_eAttribute = SVBufAttImageProcDib;
		l_BufferInfo.m_lSizeX = static_cast< long >( dCharBoxSizeX );
		l_BufferInfo.m_lSizeY = static_cast< long >( dCharBoxSizeY );
		l_BufferInfo.m_lSizeBand = 1;
		SetImageDepth(l_BufferInfo, 8 );

		l_Status = SVMatroxBufferInterface::Create( l_CharHandle, l_BufferInfo ); 

		if( !l_CharHandle.empty() )
		{
			l_Status = SVMatroxOcrInterface::CopyFont( p_rFontId, l_CharHandle, SVOcrCopyFromFont, l_FontChar );

			if( S_OK == l_Status )
			{
				SVCommandDataFacadePtr l_DestPtr{ new SVMatroxCommandDataImage(l_CharHandle, true) };

				if( nullptr != l_DestPtr )
				{
					l_Status = l_Character.SetData( _T( "Image" ), l_DestPtr );
				}
				else
				{
					l_Status = E_FAIL;
				}
			}

			l_CharHandle.clear();

			if( S_OK == l_Status )
			{
				l_Status = l_Character.SetValue( _T( "Label" ), _bstr_t( l_FontChar.c_str() ), true );
			}

			if( S_OK == l_Status )
			{
				std::string l_Name = SvUl::Format( _T( "Character %d" ), i );

				l_Status = l_Characters.SetContainer( _bstr_t(l_Name.c_str()), l_Character, true );
			}
		}
		else
		{
			if( S_OK == l_Status )
			{
				l_Status = E_FAIL;
			}
		}
	}

	if( S_OK == l_Status )
	{
		l_Status = p_rFontData.SetContainer( _T( "Character List" ), l_Characters, true );
	}

	if( S_OK == l_Status )
	{
		l_Status = p_rFontData.SetValue( _T( "Number Of Characters" ), l_Count, true );
	}

	return l_Status;
}

HRESULT SVMatroxOcrInterface::UpdateFontIdFromCharacterList( const SVCommandDataHolder& p_rFontData, SVMatroxOcr& p_rFontId )
{
	HRESULT l_Status = S_OK;

	_variant_t l_NumberOfCharacters;

	l_Status = p_rFontData.GetValue( _T( "Number Of Characters" ), l_NumberOfCharacters );

	if( S_OK == l_Status )
	{
		SVCommandDataHolder l_Characters;

		l_Status = p_rFontData.GetContainer( _T( "Character List" ), l_Characters );

		for( size_t i = 0; S_OK == l_Status && i < static_cast< size_t >( l_NumberOfCharacters ); ++i )
		{
			SVCommandDataHolder l_Character;
			std::string l_Name = SvUl::Format( _T( "Character %d" ), i );

			l_Status = l_Characters.GetContainer( _bstr_t(l_Name.c_str()), l_Character );

			if( S_OK == l_Status )
			{
				_bstr_t l_Label;
				SVMatroxCommandDataImage l_MatroxImage;

				if( S_OK == l_Status )
				{
					_variant_t l_TempLabel;

					l_Status = l_Character.GetValue( _T( "Label" ), l_TempLabel );

					l_Label = l_TempLabel;

					if( l_Label.length() <= 0 )
					{
						l_Status = E_FAIL;
					}
				}

				if( S_OK == l_Status )
				{
					std::vector<unsigned char> l_TempImage;

					l_Status = l_Character.GetImage( _T( "Image" ), l_TempImage );

					if( S_OK == l_Status )
					{
						l_Status = l_MatroxImage.SetData( l_TempImage );
					}
				}

				if( S_OK == l_Status )
				{
					std::string l_FontChar;

					l_FontChar += ( static_cast< char* >( l_Label ) )[ 0 ];

					if( !( ( l_MatroxImage.m_Buffer ).empty() ) )
					{
						l_Status = SVMatroxOcrInterface::CopyFont( p_rFontId, l_MatroxImage.m_Buffer, SVOcrCopytoFont, l_FontChar );
					}
				}
			}
			else
			{
				if( S_OK == l_Status )
				{
					l_Status = E_FAIL;
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVMatroxOcrInterface::ThicknessCalculation( SVMatroxBuffer p_ThresholdImage, BYTE p_BackgroundType, long& p_rThickness )
{
	HRESULT l_Status = S_OK;

	long ItterationsBefore = 0;
	long ItterationsAfter = 0;

	// create working buffer
	SVMatroxBuffer l_WorkingBuffer;

	// copy image to buffer
	HRESULT l_Code = SVMatroxBufferInterface::Create( l_WorkingBuffer, p_ThresholdImage);
	SVMatroxBufferInterface::CopyBuffer( l_WorkingBuffer, p_ThresholdImage );

	if( l_Code == S_OK)
	{
		bool bEroding = true;
		bool bEnableBeforeCount = true;
		do
		{
			// Erode or dialate
			if( p_BackgroundType == LIGHT )
			{
				l_Status = SVMatroxImageInterface::Erode(l_WorkingBuffer, l_WorkingBuffer, 1, SVImageBinary);
			}
			else
			{
				l_Status = SVMatroxImageInterface::Dilate( l_WorkingBuffer, l_WorkingBuffer, 1, SVImageBinary);
			}

			if( bEnableBeforeCount )
			{
				// Itterations until the blob seperates.
				ItterationsBefore++;
			}

			// Itterations to end.
			ItterationsAfter++;

			// blob analysis
			long lNumBlobs = 0;

			// if blobs increase then 2nd count
			l_Status = SVMatroxBlobInterface::NumBlobs( l_WorkingBuffer, p_BackgroundType, lNumBlobs );

			if( lNumBlobs > 1 )
			{
				bEnableBeforeCount = false;
			}
			if( lNumBlobs== 0 )
			{
				break;
			}

		// until gone. No Blobs.
		}while(ItterationsAfter < 20 && ItterationsBefore < 20);
	}

	// thickness calculation.
	p_rThickness = (ItterationsBefore + ItterationsAfter);

	return l_Status;
}

