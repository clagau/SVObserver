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

#include "SVMatroxPatternInterface.h"
#include "SVMatroxImagingLibrary.h"  // has MIL includes
#include "SVMatroxResourceMonitor.h"

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

long SVMatroxPatternInterface::ConvertModelType2MatroxType( long p_lType)
{
	// these are allowed to be OR'ed together
	long l_lMatroxType = 0;

	if (p_lType & SVPatModelTypeNormalized)
	{
		l_lMatroxType |= M_NORMALIZED; // think this is the base type and must be present
	}

	if (p_lType & SVPatModelTypeCircularOverscan)
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
long SVMatroxPatternInterface::Convert2MatroxType( SVPatternTypeEnum p_eType )
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch( p_eType & SVPatBaseType)
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
long SVMatroxPatternInterface::Convert2MatroxType( SVPatternSearchParameterTypeEnum p_eType )
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch( p_eType)
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
			l_lMatroxType = M_MIN_SPACING_X;
			break;
		}
		case SVPatMinSpacingY:
		{
			l_lMatroxType = M_MIN_SPACING_Y;
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
long SVMatroxPatternInterface::Convert2MatroxType( SVPatternResultEnum p_eType )
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch( p_eType & SVPatBaseType)
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
			l_lMatroxType = M_MODEL_INDEX;
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
@SVOperationName Create - Pattern Model

@SVOperationDescription This function creates a model, using data from the specified area of the model's source image.

*/
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::Create( SVMatroxPatternModel& p_rModel, SVMatroxBuffer& p_rSrcImageId,long p_lOffX, long p_lOffY, long p_lSizeX, long p_lSizeY, long p_lType )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
	long l_lMatroxType = ConvertModelType2MatroxType(p_lType);

#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == SVMEE_STATUS_OK )
		{
			MIL_ID l_NewId = MpatAllocModel(M_DEFAULT_HOST,
				p_rSrcImageId.GetIdentifier(),
				p_lOffX,
				p_lOffY,
				p_lSizeX,
				p_lSizeY,
				l_lMatroxType,
				M_NULL);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if( l_Code == SVMEE_STATUS_OK )
			{
				SVMatroxResourceMonitor::InsertIdentifier( SVPatternModelID, l_NewId );

				if( p_rModel.empty() )
				{
					p_rModel.m_ModelId = l_NewId;
				}
				else
				{
					l_Code = Destroy( p_rModel );
					p_rModel.m_ModelId = l_NewId;
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
@SVOperationName Create - Pattern Result

@SVOperationDescription This function creates a result buffer with the specified number of entries.

*/
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::Create( SVMatroxPatResult& p_rResultId, long& p_rlNbrEntries )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == SVMEE_STATUS_OK )
		{
			MIL_ID l_NewId = MpatAllocResult(M_DEFAULT_HOST,
				p_rlNbrEntries,
				NULL);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if( l_Code == SVMEE_STATUS_OK )
			{
				SVMatroxResourceMonitor::InsertIdentifier( SVPatternResultID, l_NewId );

				if( p_rResultId.empty() )
				{
					p_rResultId.m_PatResultId = l_NewId;
				}
				else
				{
					l_Code = Destroy( p_rResultId );
					p_rResultId.m_PatResultId = l_NewId;
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
@SVOperationName Destroy - Pattern Model

@SVOperationDescription This function destroys the supplied pattern model.

*/
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::Destroy( SVMatroxPatternModel& p_rId )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rId.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MpatFree( p_rId.m_ModelId );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
				{
					SVMatroxResourceMonitor::EraseIdentifier( SVPatternModelID, p_rId.m_ModelId );

					p_rId.m_ModelId = M_NULL;
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
@SVOperationName Destroy - Pattern Result

@SVOperationDescription This function destroys the supplied pattern result.

*/
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::Destroy( SVMatroxPatResult& p_rId )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rId.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MpatFree( p_rId.m_PatResultId );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
				{
					SVMatroxResourceMonitor::EraseIdentifier( SVPatternResultID, p_rId.m_PatResultId );

					p_rId.m_PatResultId = M_NULL;
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
@SVOperationName PatCopy

@SVOperationDescription This function copies the pattern model into a SVMatroxBuffer.
 Do Not use with Circular Overscan to try and restore the Image from the Model Pattern.

*/
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::PatCopy( const SVMatroxBuffer& p_rDestImageId, const SVMatroxPatternModel& p_rSourceModelId )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rDestImageId.empty() && !p_rSourceModelId.empty() )
		{
			MpatCopy( p_rSourceModelId.m_ModelId, p_rDestImageId.GetIdentifier(), M_DEFAULT );
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
@SVOperationName SVPatPreProcModel

@SVOperationDescription This function preprocesses the specified model.

*/
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::PreProcModel( const SVMatroxPatternModel& p_rModelId, const SVMatroxBuffer& p_rImageId)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rModelId.empty() && !p_rImageId.empty() )
		{
			MpatPreprocModel( p_rImageId.GetIdentifier(), p_rModelId.m_ModelId, M_DEFAULT);
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::Execute( const SVMatroxPatResult& p_rResultId, const SVMatroxBuffer& p_rSourceImageId, const SVMatroxPatternModel& p_rModelId )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rResultId.empty() && !p_rSourceImageId.empty() && !p_rModelId.empty() )
		{
			MpatFindModel(p_rSourceImageId.GetIdentifier(), 
				p_rModelId.m_ModelId,
				p_rResultId.m_PatResultId );
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::Get( const SVMatroxPatternModel& p_rModelId, SVPatternTypeEnum p_eType, double& p_rdValue )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType( p_eType );
		if( l_lMatroxType != M_UNINITIALIZED )
		{
			if( !p_rModelId.empty() )
			{
				double l_dValue;
				MpatInquire(p_rModelId.m_ModelId, 
					l_lMatroxType,
					&l_dValue );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
				{
					p_rdValue = l_dValue;
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::Get( const SVMatroxPatternModel& p_rModelId, const SVPatternTypeEnum p_eType, long& p_rlValue )
{
	double l_dValue;
	SVStatusCode l_Code = Get(p_rModelId, p_eType, l_dValue);
	if( l_Code == SVMEE_STATUS_OK )
	{
		p_rlValue = static_cast<long>( l_dValue );
	}
	return l_Code;
}

/**
@SVOperationName Set double

@SVOperationDescription This function can be used to set the model's internal search parameters.

*/
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::Set( const SVMatroxPatternModel& p_rModelId, const SVPatternTypeEnum p_eType, const double& p_rdValue )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType( p_eType );
		if( l_lMatroxType != M_UNINITIALIZED )
		{
			if( !p_rModelId.empty() )
			{
				MpatSetSearchParameter(p_rModelId.m_ModelId, 
					l_lMatroxType,
					p_rdValue );
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::Set( const SVMatroxPatternModel& p_rModelId, const SVPatternTypeEnum p_eType, const long& p_rlValue )
{
	return Set( p_rModelId, p_eType, static_cast<double>(p_rlValue));
}

/**
@SVOperationName Set double

@SVOperationDescription This function can be used to set the model's internal advanced search parameters.

*/
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::Set( const SVMatroxPatternModel& p_rModelId, const SVPatternSearchParameterTypeEnum p_eType, const double& p_rdValue )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType( p_eType );
		if( l_lMatroxType != M_UNINITIALIZED )
		{
			if( !p_rModelId.empty() )
			{
				MpatSetSearchParameter(p_rModelId.m_ModelId, 
					l_lMatroxType,
					p_rdValue );
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::Set( const SVMatroxPatternModel& p_rModelId, const SVPatternSearchParameterTypeEnum p_eType, const long& p_rlValue )
{
	return Set( p_rModelId, p_eType, static_cast<double>(p_rlValue));
}

/**
@SVOperationName GetResult - double*

@SVOperationDescription This function retrieves the result(s) of the specified type from a specified pattern matching result buffer.

*/
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::GetResult( const SVMatroxPatResult& p_rResultId, const SVPatternResultEnum p_eType, double* p_pdValue )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType( p_eType );
		if( l_lMatroxType != M_UNINITIALIZED )
		{
			if( !p_rResultId.empty() )
			{
				MpatGetResult(p_rResultId.m_PatResultId, 
					l_lMatroxType,
					p_pdValue );
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::SetCenter( const SVMatroxPatternModel& p_rModelId, long p_lXOffset, long p_lYOffset)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rModelId.empty() )
		{
			MpatSetCenter(p_rModelId.m_ModelId, 
				p_lXOffset,
				p_lYOffset );
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::SetNumber( const SVMatroxPatternModel& p_rModelId, long p_lNumber )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rModelId.empty() )
		{
			MpatSetNumber(p_rModelId.m_ModelId, 
				p_lNumber);
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::GetNumber( const SVMatroxPatResult& p_rResultId, long& p_rlValue )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rResultId.empty() )
		{
			SVMatroxInt l_Temp = 0;
			long l_lNumber = 0;
			MpatGetNumber( p_rResultId.m_PatResultId, &l_Temp );
			l_lNumber = SVMatroxApplicationInterface::SVMatroxIntToHRESULT( l_Temp );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if( l_Code == SVMEE_STATUS_OK )
			{
				p_rlValue = l_lNumber;
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::SetPosition( const SVMatroxPatternModel& p_rModelId, long p_lXOffset, long p_lYOffset, long p_lSizeX, long p_lSizeY )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rModelId.empty() )
		{
			MpatSetPosition(p_rModelId.m_ModelId, 
				p_lXOffset,
				p_lYOffset,
				p_lSizeX,
				p_lSizeY);
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::SetAcceptance( const SVMatroxPatternModel& p_rModelId, const double p_dValue )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rModelId.empty() )
		{
			MpatSetAcceptance(p_rModelId.m_ModelId, 
				p_dValue);
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::SetCertainty( const SVMatroxPatternModel& p_rModelId, const double p_dValue )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rModelId.empty() )
		{
			MpatSetCertainty(p_rModelId.m_ModelId, 
				p_dValue);
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::SetAccuracy( const SVMatroxPatternModel& p_rModelId, const long p_lValue )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rModelId.empty() )
		{
			MpatSetAccuracy(p_rModelId.m_ModelId, 
				p_lValue);
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::SetSpeed( const SVMatroxPatternModel& p_rModelId, const long p_lValue )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rModelId.empty() )
		{
			MpatSetSpeed(p_rModelId.m_ModelId, 
				p_lValue);
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::SetAngle( const SVMatroxPatternModel& p_rModelId, SVPatternTypeEnum p_eType, double p_dValue )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rModelId.empty() )
		{
			long l_lMatroxType = Convert2MatroxType( p_eType );
			MpatSetAngle(p_rModelId.m_ModelId, 
				l_lMatroxType,
				p_dValue);
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
SVMatroxPatternInterface::SVStatusCode SVMatroxPatternInterface::SetAngle( const SVMatroxPatternModel& p_rModelId, bool p_bEnable )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rModelId.empty() )
		{
			if( p_bEnable )
			{
				MpatSetAngle(p_rModelId.m_ModelId, 
					M_SEARCH_ANGLE_MODE ,
					M_ENABLE );
			}
			else
			{
				MpatSetAngle(p_rModelId.m_ModelId, 
					M_SEARCH_ANGLE_MODE ,
					M_DISABLE );
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

