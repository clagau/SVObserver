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
#include "SVMatroxBufferInterface.h"
#include "SVMatroxErrorEnum.h"
#include "SVMatroxHelper.h"
#include "SVMatroxImageInterface.h"
#include "SVMatroxResourceMonitor.h"
#include "SVMatroxOcrCalibrateStruct.h"
#include "SVMatroxOcrCreateStruct.h"
#include "SVCommandLibrary/SVCommandDataHolder.h"
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
	switch (p_eType)
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
	switch (p_eType)
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
__int64 SVMatroxOcrInterface::Convert2MatroxCreateType(SVOcrTypeEnum p_eType)
{
	__int64 l_lCode = 0;
	switch (p_eType & SVBasics)
	{
		case SVSemiM1292:
		{
			l_lCode = M_SEMI_M12_92;
			break;
		}
		case SVSemiM1388:
		{
			l_lCode = M_SEMI_M13_88;
			break;
		}
		case SVUserDefined:
		{
			l_lCode = M_USER_DEFINED;
			break;
		}
	}

	switch (p_eType & SVCombine1)
	{
		case SVConstrained:
		{
			l_lCode |= M_CONSTRAINED;
			break;
		}
		case SVGeneral:
		{
			l_lCode |= M_GENERAL;
			break;
		}
	}
	return l_lCode;
}

/**
@SVOperationName Convert2MatroxType - SVOcrOperationEnum

@SVOperationDescription This function converts a SVOcrOperationEnum to matrox constants.

*/
__int64 SVMatroxOcrInterface::Convert2MatroxOperationType(SVOcrOperationEnum p_eType)
{
	__int64 l_lCode = 0;

	// Operation Constants
	if ((p_eType & SVOcrLoadConstraint) == SVOcrLoadConstraint)
	{
		l_lCode |= M_LOAD_CONSTRAINT;
	}

	if ((p_eType & SVOcrLoadControl) == SVOcrLoadControl)
	{
		l_lCode |= M_LOAD_CONTROL;
	}

	if ((p_eType & SVOcrDefault) == SVOcrDefault)
	{
		l_lCode |= M_DEFAULT;
	}

	if ((p_eType & SVOcrSave) == SVOcrSave)
	{
		l_lCode |= M_SAVE;
	}

	if ((p_eType & SVOcrSaveConstraint) == SVOcrSaveConstraint)
	{
		l_lCode |= M_SAVE_CONSTRAINT;
	}

	if ((p_eType & SVOcrSaveControl) == SVOcrSaveControl)
	{
		l_lCode |= M_SAVE_CONTROL;
	}

	// Combination Constants...
	if ((p_eType & SVOcrAllChar) == SVOcrAllChar)
	{
		l_lCode |= M_ALL_CHAR;
	}

	if ((p_eType  & SVOcrSort) == SVOcrSort)
	{
		l_lCode |= M_SORT;
	}

	if ((p_eType  & SVOcrRestore) == SVOcrRestore)
	{
		l_lCode |= M_RESTORE;
	}

	return l_lCode;
}


/**
@SVOperationName Create - Font

@SVOperationDescription Creates a SVMatroxOcr font object.

*/
HRESULT SVMatroxOcrInterface::Create(SVMatroxOcr& p_rFontId, const SVMatroxOcrCreateStruct& p_rCreateStruct)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{

		MIL_ID l_NewID = M_NULL;
		__int64 l_lCreateType = Convert2MatroxCreateType(p_rCreateStruct.m_eFontType);
		if (l_lCreateType != 0)
		{
			l_NewID = MocrAllocFont(M_DEFAULT_HOST,
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
			if (l_Code == S_OK)
			{
				SVMatroxResourceMonitor::InsertIdentifier(SVOCRID, l_NewID);

				// If one already exists then destroy the old.
				if (!p_rFontId.empty())
				{
					Destroy(p_rFontId);
				}
				p_rFontId.m_OcrFontID = l_NewID;
			}
		}
		else
		{
			l_Code = SVMEE_WRONG_PARAMETER;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
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
HRESULT SVMatroxOcrInterface::CreateResult(__int64& p_rFontResult)
{
	// This function replaces MocrAllocResult
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{

		MIL_ID l_NewID;
		l_NewID = MocrAllocResult(M_DEFAULT_HOST, M_DEFAULT, M_NULL);
		l_Code = SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
		{
			SVMatroxResourceMonitor::InsertIdentifier(SVOCRResultID, l_NewID);

			DestroyResult(p_rFontResult);
			p_rFontResult = l_NewID;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
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
HRESULT SVMatroxOcrInterface::Destroy(SVMatroxOcr& p_rId)
{
	HRESULT l_Code = S_OK;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rId.empty())
		{






			MocrFree(p_rId.m_OcrFontID);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if (l_Code == S_OK)
			{
				SVMatroxResourceMonitor::EraseIdentifier(SVOCRID, p_rId.m_OcrFontID);

				p_rId.m_OcrFontID = M_NULL;
			}


		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
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
HRESULT SVMatroxOcrInterface::DestroyResult(__int64& rId)
{
	return DestroyMatroxId(rId, MocrFree, SVOCRResultID);
}



/**
@SVOperationName Ocr Preprocess

@SVOperationDescription This function preprocesses the specified OCR font context.

*/
HRESULT SVMatroxOcrInterface::Preprocess(const SVMatroxOcr& p_rFontId)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rFontId.empty())
		{
			MocrPreprocess(p_rFontId.m_OcrFontID, M_DEFAULT);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();

		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
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
HRESULT SVMatroxOcrInterface::RestoreFont(SVMatroxOcr& p_rFontId, const std::string& p_sFileName, SVOcrOperationEnum p_eOperation)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		MIL_ID l_NewId = M_NULL;
		__int64 l_lOperation = 0;
		if (p_eOperation == SVOcrRestore)
		{
			l_lOperation = Convert2MatroxOperationType(p_eOperation);
			l_NewId = MocrRestoreFont(const_cast<MIL_TEXT_CHAR*>(p_sFileName.c_str()), l_lOperation, M_DEFAULT_HOST, M_NULL);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if (l_Code == S_OK)
			{
				SVMatroxResourceMonitor::InsertIdentifier(SVOCRID, l_NewId);

				// If restored a new code successfully then destroy the old if there is one.
				if (!p_rFontId.empty())
				{
					Destroy(p_rFontId);
				}
				p_rFontId.m_OcrFontID = l_NewId;
			}
		}
		else
		{
			if (!p_rFontId.empty())
			{
				l_lOperation = Convert2MatroxOperationType(p_eOperation);
				MocrRestoreFont(const_cast<MIL_TEXT_CHAR*>(p_sFileName.c_str()), l_lOperation, M_DEFAULT_HOST, &p_rFontId.m_OcrFontID);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
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
HRESULT SVMatroxOcrInterface::Set(const SVMatroxOcr& p_rFontId, SVOcrControlEnum p_eControlType, const double& p_dValue)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lControlType = Convert2MatroxControlType(p_eControlType);
		if (l_lControlType != 0)
		{
			if (!p_rFontId.empty())
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
	catch (...)
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
HRESULT SVMatroxOcrInterface::Set(const SVMatroxOcr& p_rFontId, SVOcrControlEnum p_eControlType, const long& p_lValue)
{
	return Set(p_rFontId, p_eControlType, static_cast<double>(p_lValue));
}


/**
@SVOperationName Get - double&

@SVOperationDescription This function inquires about an OCR font context or an OCR result buffer setting.

*/
HRESULT SVMatroxOcrInterface::Get(const SVMatroxOcr& p_rFontId, SVOcrControlEnum  p_eControlType, double& p_rdValue)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lControlType = Convert2MatroxControlType(p_eControlType);
		if (l_lControlType != 0)
		{
			if (!p_rFontId.empty())
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
	catch (...)
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
HRESULT SVMatroxOcrInterface::Get(const SVMatroxOcr& p_rFontId, SVOcrControlEnum  p_eControlType, long& p_rlValue)
{
	double l_dValue;
	HRESULT l_Code = Get(p_rFontId, p_eControlType, l_dValue);
	if (l_Code == S_OK)
	{
		p_rlValue = static_cast<long>(l_dValue);
	}
	return l_Code;
}


/**
@SVOperationName Get - SVMatroxString&

@SVOperationDescription This function inquires about an OCR font context or an OCR result buffer setting.

*/
HRESULT SVMatroxOcrInterface::Get(const SVMatroxOcr& p_rFontId, SVOcrControlEnum  p_eControlType, std::string& p_rStrValue)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		double l_dSize = 0;
		if (p_eControlType == SVOcrString)
		{
			if (!p_rFontId.empty())
			{
				MocrInquire(p_rFontId.m_OcrFontID, M_STRING_SIZE, &l_dSize);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if (l_Code == S_OK && l_dSize > 0)
				{
					p_rStrValue.resize(static_cast<long>(l_dSize));
					MocrInquire(p_rFontId.m_OcrFontID, M_STRING, &p_rStrValue[0]);
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
				}
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
		else if (p_eControlType == SVCharInFont)
		{
			if (!p_rFontId.empty())
			{
				MocrInquire(p_rFontId.m_OcrFontID, M_CHAR_NUMBER_IN_FONT, &l_dSize);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if (l_Code == S_OK && l_dSize > 0)
				{
					p_rStrValue.resize(static_cast<long>(l_dSize));
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName GetResult - double&

@SVOperationDescription This function retrieves the result(s) of the specified type from an OCR

*/
HRESULT SVMatroxOcrInterface::GetResult(const __int64& rResultId, SVOcrResultEnum  p_eControlType, double& p_rdValue)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lControlType = Convert2MatroxResultType(p_eControlType);
		if (l_lControlType != 0)
		{
			if (M_NULL != rResultId)
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
	catch (...)
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
HRESULT SVMatroxOcrInterface::GetResult(const __int64& rResultId, SVOcrResultEnum  p_eControlType, long& p_rlValue)
{
	double l_dValue;
	HRESULT l_Code = GetResult(rResultId, p_eControlType, l_dValue);
	if (l_Code == S_OK)
	{
		p_rlValue = static_cast<long>(l_dValue);
	}
	return l_Code;
}

/**
@SVOperationName SetResult - SVMatroxString&

@SVOperationDescription This function retrieves the result(s) of the specified type from an OCR

*/
HRESULT SVMatroxOcrInterface::GetResult(const __int64& rResultId, SVOcrResultEnum  p_eControlType, std::string& p_rStrValue)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lControlType = Convert2MatroxResultType(p_eControlType);
		if (l_lControlType != 0)
		{
			if (M_NULL != rResultId)
			{
				MIL_STRING strBuf;
				MocrGetResult(rResultId, l_lControlType, strBuf);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if (l_Code == S_OK)
				{
					p_rStrValue = strBuf;
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

HRESULT SVMatroxOcrInterface::GetResult(const __int64& rResultId, SVOcrResultEnum InquireType, std::vector<double>& p_adValues)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lControlType = Convert2MatroxResultType(InquireType);
		if (l_lControlType != 0)
		{
			if (M_NULL != rResultId)
			{
				MocrGetResult(rResultId, l_lControlType, &p_adValues[0]);
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
	catch (...)
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
HRESULT SVMatroxOcrInterface::Execute(const __int64& rResultId, const SVMatroxOcr& p_rFontId, const SVMatroxBuffer& p_rImage)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (M_NULL != rResultId && !p_rFontId.empty() && !p_rImage.empty())
		{
			if (p_rFontId.m_bVerify)
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
				MocrReadString(p_rImage.GetIdentifier(), p_rFontId.m_OcrFontID, rResultId);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}