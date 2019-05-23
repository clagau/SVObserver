// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxPatternInterface
// * .File Name       : $Workfile:   SVMatroxPatternInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   26 Jun 2014 11:28:44  $
// ******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <cmath>

#pragma region Includes
#include "SVMatroxPatternInterface.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxErrorEnum.h"
#include "SVMatroxHelper.h"
#include "SVMatroxResourceMonitor.h"
#pragma endregion Includes

/**
@SVOperationName Default Constructor

@SVOperationDescription

*/
SVMatroxPatternInterface::SVMatroxPatternInterface()
{
}

/**
@SVOperationName Destructor

@SVOperationDescription

*/
SVMatroxPatternInterface::~SVMatroxPatternInterface()
{
}

/**
@SVOperationName ConvertModelType2MatroxType - SVPatternModelAllocationTypeEnum combination

@SVOperationDescription This function converts a SVPatternModelAllocationTypeEnum Combination to matrox constants.

*/

long SVMatroxPatternInterface::ConvertModelType2MatroxType( long lType)
{
	// these are allowed to be OR'ed together
	long l_lMatroxType = 0;

	if (lType & SVPatModelTypeNormalized)
	{
		l_lMatroxType |= M_NORMALIZED; // think this is the base type and must be present
	}

	if (lType & SVPatModelTypeCircularOverscan)
	{
		l_lMatroxType |= M_CIRCULAR_OVERSCAN;
	}

	if (l_lMatroxType == 0)
	{
		l_lMatroxType = M_UNINITIALIZED;
	}
	return l_lMatroxType;
}

/**
@SVOperationName Convert2MatroxType - SVPatternTypeEnum

@SVOperationDescription This function converts a SVPatternTypeEnum to matrox constants.

*/
long SVMatroxPatternInterface::Convert2MatroxType( SVPatternTypeEnum eType )
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch( eType & SVPatBaseType)
	{
		case SVPatSearchAngle:
		{
			l_lMatroxType = M_SEARCH_ANGLE;
			break;
		}
		case SVPatSearchAngleDeltaNeg:
		{
			l_lMatroxType = M_SEARCH_ANGLE_DELTA_NEG;
			break;
		}
		case SVPatSearchAngleDeltaPos:
		{
			l_lMatroxType = M_SEARCH_ANGLE_DELTA_POS;
			break;
		}
		case SVPatSearchAngleMode:
		{
			l_lMatroxType = M_SEARCH_ANGLE_MODE;
			break;
		}
		case SVPatSearchAngleAccuracy:
		{
			l_lMatroxType = M_SEARCH_ANGLE_ACCURACY;
			break;
		}
		case SVPatSearchAngleInterpMode:
		{
			l_lMatroxType = M_SEARCH_ANGLE_INTERPOLATION_MODE;
			break;
		}
		case SVPatSearchAngleTolerance:
		{
			l_lMatroxType = M_SEARCH_ANGLE_TOLERANCE;
			break;
		}

		default:
		{
			assert(0);
			break;
		}

	}
	return l_lMatroxType;
}

/**
@SVOperationName Convert2MatroxType - SVPatternTypeEnum

@SVOperationDescription This function converts a SVPatternTypeEnum to matrox constants.

*/
long SVMatroxPatternInterface::Convert2MatroxType( SVPatternSearchParameterTypeEnum eType )
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch( eType)
	{
		case SVPatFirstLevel:
		{
			l_lMatroxType = M_FIRST_LEVEL;
			break;
		}
		case SVPatLastLevel:
		{
			l_lMatroxType = M_LAST_LEVEL;
			break;
		}
		case SVPatModelStep:
		{
			l_lMatroxType = M_MODEL_STEP;
			break;
		}
		case SVPatFastFind:
		{
			l_lMatroxType = M_FAST_FIND;
			break;
		}
		case SVPatMinSpacingX:
		{
			l_lMatroxType = M_MIN_SEPARATION_X;
			break;
		}
		case SVPatMinSpacingY:
		{
			l_lMatroxType = M_MIN_SEPARATION_Y;
			break;
		}
		case SVPatCoarseSearchAcceptance:
		{
			l_lMatroxType = M_COARSE_SEARCH_ACCEPTANCE;
			break;
		}
		case SVPatExtraCandidates:
		{
			l_lMatroxType = M_EXTRA_CANDIDATES;
			break;
		}
		default:
		{
			assert(0);
			break;
		}
	}
	return l_lMatroxType;
}

/**
@SVOperationName Convert2MatroxType - SVPatternResultEnum

@SVOperationDescription This function converts a SVPatternResultEnum to matrox constants.

*/
long SVMatroxPatternInterface::Convert2MatroxType( SVPatternResultEnum eType )
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch( eType & SVPatBaseType)
	{
		// Results
		case SVCountList:
		{
			l_lMatroxType = M_COUNT_LIST;
			break;
		}
		case SVEventList:
		{
			l_lMatroxType = M_EVENT_LIST;
			break;
		}
		case SVExtremeList:
		{
			l_lMatroxType = M_EXTREME_LIST;
			break;
		}
		case SVHistList:
		{
			l_lMatroxType = M_HIST_LIST;
			break;
		}
		case SVProjList:
		{
			l_lMatroxType = M_PROJ_LIST;
			break;
		}
		case SVPatAngle:
		{
			l_lMatroxType = M_ANGLE;
			break;
		}
		case SVPatIndex:
		{
			l_lMatroxType = M_INDEX;
			break;
		}
		case SVPatPosX:
		{
			l_lMatroxType = M_POSITION_X;
			break;
		}
		case SVPatPosY:
		{
			l_lMatroxType = M_POSITION_Y;
			break;
		}
		case SVPatScore:
		{
			l_lMatroxType = M_SCORE;
			break;
		}
		default:
		{
			assert(0);
			break;
		}
	}
	return l_lMatroxType;
}

/**
@SVOperationName CreateContext - Pattern Model

@SVOperationDescription This function creates a model, using data from the specified area of the model's source image.

*/
HRESULT SVMatroxPatternInterface::CreateContext(__int64& rModelId, SVMatroxBuffer& rSrcImageId,long lOffX, long lOffY, long lSizeX, long lSizeY, long lType )
{
	HRESULT l_Code( S_OK );

#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == S_OK )
		{
			MIL_ID l_NewId = MpatAlloc(M_DEFAULT_HOST, M_NORMALIZED, M_DEFAULT, M_NULL);
			MIL_INT64 ModelType = M_REGULAR_MODEL;
			if (lType & SVPatModelTypeCircularOverscan)
			{
				ModelType += M_CIRCULAR_OVERSCAN;
			}
			MpatDefine(l_NewId, M_REGULAR_MODEL, rSrcImageId.GetIdentifier(),
				lOffX, lOffY, lSizeX, lSizeY, M_DEFAULT);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if( l_Code == S_OK )
			{
				SVMatroxResourceMonitor::InsertIdentifier( SVPatternModelID, l_NewId );

				if (M_NULL != rModelId)
				{
					l_Code = DestroyContext(rModelId);
				}	
				rModelId = l_NewId;
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
@SVOperationName Create - Pattern Result

@SVOperationDescription This function creates a result buffer with the specified number of entries.

*/
HRESULT SVMatroxPatternInterface::CreateResult(__int64& rResultId, long& rlNbrEntries )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == S_OK )
		{
			MIL_ID l_NewId = MpatAllocResult(M_DEFAULT_HOST, M_DEFAULT, nullptr);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if( l_Code == S_OK )
			{
				SVMatroxResourceMonitor::InsertIdentifier( SVPatternResultID, l_NewId );

				if( M_NULL != rResultId )
				{
					l_Code = DestroyResult(rResultId);
				}
				rResultId = l_NewId;
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
@SVOperationName DestroyContext - Pattern Model

@SVOperationDescription This function destroys the supplied pattern model.

*/
HRESULT SVMatroxPatternInterface::DestroyContext(__int64& rContextId)
{
	return DestroyMatroxId(rContextId, MpatFree, SVPatternModelID);
}

/**
@SVOperationName Destroy - Pattern Result

@SVOperationDescription This function destroys the supplied pattern result.

*/
HRESULT SVMatroxPatternInterface::DestroyResult(__int64& rResultId )
{
	return DestroyMatroxId(rResultId, MpatFree, SVPatternResultID);
}

/**
@SVOperationName SVPatPreProcModel

@SVOperationDescription This function preprocesses the specified model.

*/
HRESULT SVMatroxPatternInterface::PreProcModel(const __int64& rModelId, const SVMatroxBuffer& rImageId)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( M_NULL != rModelId && !rImageId.empty() )
		{
			MpatPreprocess(rModelId, M_DEFAULT, rImageId.GetIdentifier());
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
@SVOperationName Execute

@SVOperationDescription This function finds occurrences of the specified model in the given image and returns the position of each occurrence.

*/
HRESULT SVMatroxPatternInterface::Execute(const __int64& rResultId, const SVMatroxBuffer& rSourceImageId, const __int64& rModelId)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (M_NULL != rResultId && !rSourceImageId.empty() && M_NULL != rModelId)
		{
			MpatFind(rModelId, rSourceImageId.GetIdentifier(), rResultId);
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
@SVOperationName Get - double

@SVOperationDescription This function returns information about the specified model or result buffer and stores it in the supplied double.

*/
HRESULT SVMatroxPatternInterface::Get( const __int64& rModelId, SVPatternTypeEnum eType, double& rdValue )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType( eType );
		if( l_lMatroxType != M_UNINITIALIZED )
		{
			if( M_NULL != rModelId )
			{
				double l_dValue;
				MpatInquire(rModelId, l_lMatroxType, &l_dValue );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == S_OK )
				{
					rdValue = l_dValue;
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

/**
@SVOperationName Get - long

@SVOperationDescription This function returns information about the specified model or result buffer and stores it in the supplied long.

*/
HRESULT SVMatroxPatternInterface::Get( const __int64& rModelId, const SVPatternTypeEnum eType, long& rlValue )
{
	double l_dValue;
	HRESULT l_Code = Get(rModelId, eType, l_dValue);
	if( l_Code == S_OK )
	{
		rlValue = static_cast<long>( l_dValue );
	}
	return l_Code;
}

/**
@SVOperationName Set double

@SVOperationDescription This function can be used to set the model's internal search parameters.

*/
HRESULT SVMatroxPatternInterface::Set( const __int64& rModelId, const SVPatternTypeEnum eType, const double& rdValue )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType( eType );
		if( l_lMatroxType != M_UNINITIALIZED )
		{
			if( M_NULL != rModelId )
			{
				MpatControl(rModelId, M_DEFAULT, l_lMatroxType, rdValue);
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
@SVOperationName Set - long

@SVOperationDescription This function can be used to set the model's internal search parameters.

*/
HRESULT SVMatroxPatternInterface::Set( const __int64& rModelId, const SVPatternTypeEnum eType, const long& rlValue )
{
	return Set( rModelId, eType, static_cast<double>(rlValue));
}

/**
@SVOperationName Set double

@SVOperationDescription This function can be used to set the model's internal advanced search parameters.

*/
HRESULT SVMatroxPatternInterface::Set( const __int64& rModelId, const SVPatternSearchParameterTypeEnum eType, const double& rdValue )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType( eType );
		if( l_lMatroxType != M_UNINITIALIZED )
		{
			if( M_NULL != rModelId )
			{
				MpatControl(rModelId, M_DEFAULT, l_lMatroxType, rdValue);
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
@SVOperationName Set - long

@SVOperationDescription This function can be used to set the model's internal advanced search parameters.

*/
HRESULT SVMatroxPatternInterface::Set( const __int64& rModelId, const SVPatternSearchParameterTypeEnum eType, const long& rlValue )
{
	return Set( rModelId, eType, static_cast<double>(rlValue));
}

/**
@SVOperationName GetResult - double*

@SVOperationDescription This function retrieves the result(s) of the specified type from a specified pattern matching result buffer.

*/
HRESULT SVMatroxPatternInterface::GetResult(const __int64& rResultId, const SVPatternResultEnum eType, double* pdValue)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType( eType );
		if( l_lMatroxType != M_UNINITIALIZED )
		{
			if( M_NULL != rResultId )
			{
				MpatGetResult(rResultId, l_lMatroxType,	pdValue );
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
@SVOperationName SetCenter

@SVOperationDescription This function sets the reference position of the specified model.

*/
HRESULT SVMatroxPatternInterface::SetCenter( const __int64& rModelId, long lXOffset, long lYOffset)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( M_NULL != rModelId )
		{
			MpatControl(rModelId, M_DEFAULT, M_REFERENCE_X, lXOffset);
			MpatControl(rModelId, M_DEFAULT, M_REFERENCE_Y, lYOffset);
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

HRESULT SVMatroxPatternInterface::SetDontCare( const SVMatroxBuffer& rDontCareImageId, const __int64& rModelId )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !rDontCareImageId.empty() && M_NULL != rModelId )
		{
			MpatMask(rModelId, M_DEFAULT, rDontCareImageId.GetIdentifier(), M_DONT_CARE, M_DEFAULT);
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
@SVOperationName SetNumber

@SVOperationDescription This function sets the number of occurrences of a model for which to search.

*/
HRESULT SVMatroxPatternInterface::SetNumber( const __int64& rModelId, long lNumber )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( M_NULL != rModelId )
		{
			MpatControl(rModelId, M_DEFAULT, M_NUMBER, lNumber);
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
@SVOperationName GetNumber

@SVOperationDescription This function retrieves the number of matches found after searching for a model.

*/
HRESULT SVMatroxPatternInterface::GetNumber(const __int64& rResultId, long& rlValue)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( M_NULL != rResultId )
		{
			MIL_INT number = 0;
			MpatGetResult(rResultId, M_DEFAULT, M_NUMBER+ M_TYPE_MIL_INT, &number);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if (l_Code == S_OK)
			{
				rlValue = static_cast<long>(number);
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

/**
@SVOperationName SetPosition

@SVOperationDescription This function sets the specified model's search region.

*/
HRESULT SVMatroxPatternInterface::SetPosition( const __int64& rModelId, long lXOffset, long lYOffset, long lSizeX, long lSizeY )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( M_NULL != rModelId )
		{
			MpatControl(rModelId, M_DEFAULT, M_SEARCH_OFFSET_X, lXOffset);
			MpatControl(rModelId, M_DEFAULT, M_SEARCH_OFFSET_Y, lYOffset);
			MpatControl(rModelId, M_DEFAULT, M_SEARCH_SIZE_X, lSizeX);
			MpatControl(rModelId, M_DEFAULT, M_SEARCH_SIZE_Y, lSizeY);
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
@SVOperationName SetAcceptance

@SVOperationDescription This function sets the acceptance level for a match made with the specified model when it is sought in an image.

*/
HRESULT SVMatroxPatternInterface::SetAcceptance( const __int64& rModelId, const double dValue )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( M_NULL != rModelId )
		{
			MpatControl(rModelId, M_DEFAULT, M_ACCEPTANCE, dValue);
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
@SVOperationName SetCertainty

@SVOperationDescription This function sets the certainty level for a match made with the specified model when it is sought in an image.

*/
HRESULT SVMatroxPatternInterface::SetCertainty( const __int64& rModelId, const double dValue )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( M_NULL != rModelId )
		{
			MpatControl(rModelId, M_DEFAULT, M_CERTAINTY, dValue);
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
@SVOperationName SetAccuracy

@SVOperationDescription This function sets the specified model's search parameter for positional accuracy and the complexity of the image.

*/
HRESULT SVMatroxPatternInterface::SetAccuracy( const __int64& rModelId, const long lValue )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( M_NULL != rModelId )
		{
			MpatControl(rModelId, M_DEFAULT, M_ACCURACY, lValue);
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
@SVOperationName SetSpeed

@SVOperationDescription This function specifies the required search speed.

*/
HRESULT SVMatroxPatternInterface::SetSpeed( const __int64& rModelId, const long lValue )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( M_NULL != rModelId )
		{
			MpatControl(rModelId, M_DEFAULT, M_SPEED, lValue);
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
@SVOperationName SetAngle - double

@SVOperationDescription This function specifies the required search angle parameters.

*/
HRESULT SVMatroxPatternInterface::SetAngle( const __int64& rModelId, SVPatternTypeEnum eType, double dValue )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( M_NULL != rModelId )
		{
			long l_lMatroxType = Convert2MatroxType( eType );
			MpatControl(rModelId, M_DEFAULT, l_lMatroxType, dValue);
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
@SVOperationName SetAngle - bool

@SVOperationDescription This function enables or disables the search angle functionality.

*/
HRESULT SVMatroxPatternInterface::SetAngle( const __int64& rModelId, bool bEnable )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( M_NULL != rModelId )
		{
			MIL_DOUBLE value = bEnable ? M_ENABLE : M_DISABLE;
			MpatControl(rModelId, M_DEFAULT, M_SEARCH_ANGLE_MODE, value);
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

/*static*/ RECT SVMatroxPatternInterface::CalculateOverscanOuterRect( const POINT& pos, const SIZE& size )
{
	POINT centerPos = { pos.x + (size.cx / 2), pos.y + (size.cy / 2) };
	//dx and dy is half the size of the rectangle
	float dx = static_cast<float>(0.5*size.cx);
	float dy = static_cast<float>(0.5*size.cy);
	//The 0.5 is needed for the rounding from float to long
	long radius = static_cast<long>( sqrt( (dx * dx) + (dy * dy) ) + 0.5 );

	RECT rect = { centerPos.x - radius, centerPos.y - radius, centerPos.x + radius, centerPos.y + radius };
	return rect;
}

/*static*/ RECT SVMatroxPatternInterface::CalculateOverscanInnerRect( const POINT& pos, const SIZE& size )
{
	RECT boundingRect = { pos.x, pos.y, pos.x + size.cx, pos.y + size.cy };
	long width = boundingRect.right - boundingRect.left;
	long height = boundingRect.bottom - boundingRect.top;
	POINT centerPos = { boundingRect.left + (width / 2), boundingRect.top + (height / 2) };
	long radius = std::min( width / 2, height / 2 );
	long val = static_cast<long>(static_cast<float>(radius) * (sqrt(2.0f) / 2.0f));

	RECT rect = { centerPos.x - val, centerPos.y - val, centerPos.x + val, centerPos.y + val };
	return rect;
}

