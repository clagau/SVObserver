// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBlobInterface
// * .File Name       : $Workfile:   SVMatroxBlobInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   12 Nov 2014 08:24:36  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxBlobInterface.h"

#include "SVCommandLibrary/SVCommandDataContainer.h"
#include "SVCommandLibrary/SVCommandDataHolder.h"
#include "SVCommandLibrary/SVCommandDataValue.h"

#include "SVMatroxCommandDataImage.h"
#include "SVMatroxImagingLibrary.h"  // has MIL includes
#include "SVMatroxBufferInterface.h"
#include "SVMatroxResourceMonitor.h"

/**
@SVOperationName Default Constructor

@SVOperationDescription

*/
SVMatroxBlobInterface::SVMatroxBlobInterface()
{
}

/**
@SVOperationName Destructor

@SVOperationDescription

*/
SVMatroxBlobInterface::~SVMatroxBlobInterface()
{
}

/**
@SVOperationName Convert2MatroxControlType - SVBlobSelectionEnum

@SVOperationDescription This function converts a SVresearch contol type to a Matrox control type.

*/
long SVMatroxBlobInterface::Convert2MatroxControlType(SVBlobSelectionEnum p_lControlType) 
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch( p_lControlType )
	{
		
		case SVEBlobArea:
		{
			l_lMatroxType = M_AREA;
			break;
		}
		case SVEBlobBoxXMax:
		{
			l_lMatroxType = M_BOX_X_MAX;
			break;
		}
		case SVEBlobBoxXMin:
		{
			l_lMatroxType = M_BOX_X_MIN;
			break;
		}
		case SVEBlobBoxYMax:
		{
			l_lMatroxType = M_BOX_Y_MAX;
			break;
		}
		case SVEBlobBoxYMin:
		{
			l_lMatroxType = M_BOX_Y_MIN;
			break;
		}
		case SVEBlobBreadth:
		{
			l_lMatroxType = M_BREADTH;
			break;
		}
		case SVEBlobCompactness:
		{
			l_lMatroxType = M_COMPACTNESS;
			break;
		}
		case SVEBlobConvexParameter:
		{
			l_lMatroxType = M_CONVEX_PERIMETER;
			break;
		}
		case SVEBlobElongation:
		{
			l_lMatroxType = M_ELONGATION;
			break;
		}
		case SVEBlobEulerNumber:
		{
			l_lMatroxType = M_EULER_NUMBER;
			break;
		}
		case SVEBlobFeretElongation:
		{
			l_lMatroxType = M_FERET_ELONGATION;
			break;
		}
		case SVEBlobFeretMaxAngle:
		{
			l_lMatroxType = M_FERET_MAX_ANGLE;
			break;
		}
		case SVEBlobFeretMaxDia:
		{
			l_lMatroxType = M_FERET_MAX_DIAMETER;
			break;
		}
		case SVEBlobFeretMeanDia:
		{
			l_lMatroxType = M_FERET_MEAN_DIAMETER;
			break;
		}
		case SVEBlobFeretMinAngle:
		{
			l_lMatroxType = M_FERET_MIN_ANGLE;
			break;
		}
		case SVEBlobFeretMinDia:
		{
			l_lMatroxType = M_FERET_MIN_DIAMETER;
			break;
		}
		case SVEBlobFeretX:
		{
			l_lMatroxType = M_FERET_X;
			break;
		}
		case SVEBlobFeritY:
		{
			l_lMatroxType = M_FERET_Y;
			break;
		}
		case SVEBlobFirstPointX:
		{
			l_lMatroxType = M_FIRST_POINT_X;
			break;
		}
		case SVEBlobFirstPointY:
		{
			l_lMatroxType = M_FIRST_POINT_Y;
			break;
		}
		case SVEBlobGeneralFeret:
		{
			l_lMatroxType = M_GENERAL_FERET;
			break;
		}
		case SVEBlobGeneralMoment:
		{
			l_lMatroxType = M_GENERAL_MOMENT;
			break;
		}
		case SVEBlobIntercept:
		{
			l_lMatroxType = M_INTERCEPT_0;
			break;
		}
		case SVEBlobIntercept45:
		{
			l_lMatroxType = M_INTERCEPT_45;
			break;
		}
		case SVEBlobIntercept90:
		{
			l_lMatroxType = M_INTERCEPT_90;
			break;
		}
		case SVEBlobIntercept135:
		{
			l_lMatroxType = M_INTERCEPT_135;
			break;
		}
		case SVEBlobLabelValue:
		{
			l_lMatroxType = M_LABEL_VALUE;
			break;
		}
		case SVEBlobLength:
		{
			l_lMatroxType = M_LENGTH;
			break;
		}
		case SVEBlobNumberOfChainedPixels:
		{
			l_lMatroxType = M_NUMBER_OF_CHAINED_PIXELS;
			break;
		}
		case SVEBlobNumberOfHoles:
		{
			l_lMatroxType = M_NUMBER_OF_HOLES;
			break;
		}
		case SVEBlobNumberOfRuns:
		{
			l_lMatroxType = M_NUMBER_OF_RUNS;
			break;
		}
		case SVEBlobPerimeter:
		{
			l_lMatroxType = M_PERIMETER;
			break;
		}
		case SVEBlobRoughness:
		{
			l_lMatroxType = M_ROUGHNESS;
			break;
		}
		case SVEBlobXMaxAtYMax:
		{
			l_lMatroxType = M_X_MAX_AT_Y_MAX;
			break;
		}
		case SVEBlobXMinAtYMin:
		{
			l_lMatroxType = M_X_MIN_AT_Y_MIN;
			break;
		}
		case SVEBlobYMaxAtXMin:
		{
			l_lMatroxType = M_Y_MAX_AT_X_MIN;
			break;
		}
		case SVEBlobYMinAtXMax:
		{
			l_lMatroxType = M_Y_MIN_AT_X_MAX;
			break;
		}
		case SVEBlobSumPixel:
		{
			l_lMatroxType = M_SUM_PIXEL;
			break;
		}
		case SVEBlobAxisPrincipalAngle:
		{
			l_lMatroxType = M_AXIS_PRINCIPAL_ANGLE;
			break;
		}
		case SVEBlobAxisSecondaryAngle:
		{
			l_lMatroxType = M_AXIS_SECONDARY_ANGLE;
			break;
		}
		case SVEBlobCenterOfGravityX:
		{
			l_lMatroxType = M_CENTER_OF_GRAVITY_X;
			break;
		}
		case SVEBlobCenterOfGravityY:
		{
			l_lMatroxType = M_CENTER_OF_GRAVITY_Y;
			break;
		}
		case SVEBlobMomentX0Y1:
		{
			l_lMatroxType = M_MOMENT_X0_Y1;
			break;
		}
		case SVEBlobMomentX1Y0:
		{
			l_lMatroxType = M_MOMENT_X1_Y0;
			break;
		}
		case SVEBlobMomentX1Y1:
		{
			l_lMatroxType = M_MOMENT_X1_Y1;
			break;
		}
		case SVEBlobMomentX0Y2:
		{
			l_lMatroxType = M_MOMENT_X0_Y2;
			break;
		}
		case SVEBlobMomentX2Y0:
		{
			l_lMatroxType = M_MOMENT_X2_Y0;
			break;
		}
		case SVEBlobMomentCentralX0Y2:
		{
			l_lMatroxType = M_MOMENT_CENTRAL_X0_Y2;
			break;
		}
		case SVEBlobMomentCentralX2Y0:
		{
			l_lMatroxType = M_MOMENT_CENTRAL_X2_Y0;
			break;
		}
		case SVEBlobMomentCentralX1Y1:
		{
			l_lMatroxType = M_MOMENT_CENTRAL_X1_Y1;
			break;
		}

	}
	return l_lMatroxType;
}


/**
@SVOperationName Convert2MatroxControlType - SVBlobControlEnum

@SVOperationDescription This function converts a SVresearch contol type to a Matrox control type.

*/
long SVMatroxBlobInterface::Convert2MatroxControlType(SVBlobControlEnum p_lControlType) 
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch( p_lControlType )
	{
		case SVEBlobForeground:
		{
			l_lMatroxType = M_FOREGROUND_VALUE;
			break;
		}
		case SVEBlobIdentifier:
		{
			l_lMatroxType = M_IDENTIFIER_TYPE;
			break;
		}
		case SVEBlobSaveRuns:
		{
			l_lMatroxType = M_SAVE_RUNS;
			break;
		}

		case SVEBlobAll:
		{
			l_lMatroxType = M_ALL_BLOBS;
			break;
		}
		case SVEBlobExcludeBlobs:
		{
			l_lMatroxType = M_EXCLUDED_BLOBS;
			break;
		}
		case SVEBlobIncludeBlobs:
		{
			l_lMatroxType = M_INCLUDED_BLOBS;
			break;
		}
	}
	return l_lMatroxType;
}

/**
@SVOperationName Convert2MatroxControlType - SVBlobOperationEnum

@SVOperationDescription This function converts a SVresearch contol type to a Matrox control type.

*/
long SVMatroxBlobInterface::Convert2MatroxControlType(SVBlobOperationEnum p_lControlType) 
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch( p_lControlType )
	{
		// Operation
		case SVEBlobExclude:
		{
			l_lMatroxType = M_EXCLUDE;
			break;
		}
		case SVEBlobExcludeOnly:
		{
			l_lMatroxType = M_EXCLUDE_ONLY;
			break;
		}
		case SVEBlobInclude:
		{
			l_lMatroxType = M_INCLUDE;
			break;
		}
	}
	return l_lMatroxType;
}


/**
@SVOperationName Convert2MatroxControlType - SVConditionEnum

@SVOperationDescription This function converts a SVresearch contol type to a Matrox control type.

*/
long SVMatroxBlobInterface::Convert2MatroxControlType(SVConditionEnum p_lControlType) 
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch( p_lControlType )
	{
		// Condition
		case SVECondInRange:
		{
			l_lMatroxType = M_IN_RANGE;
			break;
		}
		case SVECondOutRange:
		{
			l_lMatroxType = M_OUT_RANGE;
			break;
		}
		case SVECondEqual:
		{
			l_lMatroxType = M_EQUAL;
			break;
		}
		case SVECondGreater:
		{
			l_lMatroxType = M_GREATER;
			break;
		}
		case SVECondGreaterOrEqual:
		{
			l_lMatroxType = M_GREATER_OR_EQUAL;
			break;
		}
		case SVECondLess:
		{
			l_lMatroxType = M_LESS;
			break;
		}
		case SVECondLessOrEqual:
		{
			l_lMatroxType = M_LESS_OR_EQUAL;
			break;
		}
		case SVECondNotEqual:
		{
			l_lMatroxType = M_NOT_EQUAL;
			break;
		}
	}
	return l_lMatroxType;
}


/**
@SVOperationName Create - Blob Feature List

@SVOperationDescription This function creates a Blob Feature List.

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::Create( SVMatroxBlobFeatureList& p_rFeatureListId )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVStatusCode l_Code( SVMEE_STATUS_OK );
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == SVMEE_STATUS_OK )
		{
			SVMatroxIdentifier l_NewId = MblobAllocFeatureList(M_DEFAULT_HOST, M_NULL );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if( l_Code == SVMEE_STATUS_OK )
			{
				SVMatroxResourceMonitor::InsertIdentifier( SVBlobFeatureListID, l_NewId );

				if( p_rFeatureListId.empty() )
				{
					p_rFeatureListId.m_BlobFeatureListID = l_NewId;
				}
				else
				{
					l_Code = Destroy(p_rFeatureListId );
					p_rFeatureListId.m_BlobFeatureListID = l_NewId;
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
@SVOperationName Blob Create - Blob Result

@SVOperationDescription This function creates a blob result object.

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::Create( SVMatroxBlobResult& p_rResultId )
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
			SVMatroxIdentifier l_NewId = MblobAllocResult(M_DEFAULT_HOST, M_NULL );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if( l_Code == SVMEE_STATUS_OK )
			{
				SVMatroxResourceMonitor::InsertIdentifier( SVBlobResultID, l_NewId );

				if( p_rResultId.empty() )
				{
					p_rResultId.m_BlobResultId = l_NewId;
				}
				else
				{
					l_Code = Destroy(p_rResultId );
					p_rResultId.m_BlobResultId = l_NewId;
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
@SVOperationName Destroy - Blob Feature List

@SVOperationDescription This function frees the blob feature list.

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::Destroy( SVMatroxBlobFeatureList& p_rBlobId )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rBlobId.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MblobFree( p_rBlobId.m_BlobFeatureListID );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
				{
					SVMatroxResourceMonitor::EraseIdentifier( SVBlobFeatureListID, p_rBlobId.m_BlobFeatureListID );

					p_rBlobId.m_BlobFeatureListID = M_NULL;
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
@SVOperationName Destroy - Blob Result

@SVOperationDescription This function frees the blob result resource.

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::Destroy( SVMatroxBlobResult& p_rResultId )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rResultId.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MblobFree( p_rResultId.m_BlobResultId );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
				{
					SVMatroxResourceMonitor::EraseIdentifier( SVBlobResultID, p_rResultId.m_BlobResultId );

					p_rResultId.m_BlobResultId = M_NULL;
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
@SVOperationName Execute

@SVOperationDescription This function does the work of finding blobs in the "Blob Identifier Image".

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::Execute( const SVMatroxBlobResult& p_rResult, const SVMatroxBuffer& p_rIdentId,  const SVMatroxBlobFeatureList& p_rFeatureListId )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rIdentId.empty() && !p_rFeatureListId.empty() && !p_rFeatureListId.empty() )
		{
			MblobCalculate( p_rIdentId.GetIdentifier(), M_NULL, p_rFeatureListId.m_BlobFeatureListID, p_rResult.m_BlobResultId);
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
@SVOperationName Get -  double&

@SVOperationDescription This function gets the selected control type in the form of a long.

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::Get( const SVMatroxBlobResult& p_rResultId, SVBlobControlEnum p_lControlType, double& p_dControlValue) 
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rResultId.empty() )
		{
			long l_lMatroxType = Convert2MatroxControlType( p_lControlType );
			if( l_lMatroxType != M_UNINITIALIZED )
			{
				double l_dValue;
				MblobInquire( p_rResultId.m_BlobResultId, l_lMatroxType, &l_dValue );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
				{
					p_dControlValue = l_dValue;
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
	return l_Code;
}

/**
@SVOperationName GetResult 

@SVOperationDescription This function gets an array of doubles for the results.  The number of positions in the array will be the same as the value returned by GetNumber.

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::GetResult( const SVMatroxBlobResult& p_rResultId, SVBlobSelectionEnum p_lControlType, double* p_pdResultsArray ) 
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rResultId.empty() )
		{
			long l_lMatroxType = Convert2MatroxControlType( p_lControlType );
			if( l_lMatroxType != M_UNINITIALIZED )
			{
				MblobGetResult( p_rResultId.m_BlobResultId, 
					l_lMatroxType | M_BINARY,
					p_pdResultsArray );

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

/**
@SVOperationName GetResult 

@SVOperationDescription This function gets an array of longs for the results.  The number of positions in the array will be the same as the value returned by GetNumber.

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::GetResult( const SVMatroxBlobResult& p_rResultId, SVBlobSelectionEnum p_lControlType, long* p_plResultsArray ) 
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rResultId.empty() )
		{
			long l_lMatroxType = Convert2MatroxControlType( p_lControlType );
			if( l_lMatroxType != M_UNINITIALIZED )
			{
				MblobGetResult( p_rResultId.m_BlobResultId, 
					l_lMatroxType + M_TYPE_LONG,
					p_plResultsArray );

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


/**
@SVOperationName GetNumber 

@SVOperationDescription This function gets the number of currently included blobs.

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::GetNumber( const SVMatroxBlobResult& p_rResultId, long& p_rNumber ) 
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rResultId.empty() )
		{
			SVMatroxInt l_Number = 0;
			long l_lValue;
			//MblobGetNumber( p_rResultId.m_BlobResultId, &l_lValue );
			MblobGetNumber( p_rResultId.m_BlobResultId, &l_Number );
			l_lValue = SVMatroxApplicationInterface::SVMatroxIntToHRESULT( l_Number );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if( l_Code == SVMEE_STATUS_OK )
			{
				p_rNumber = l_lValue;
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
@SVOperationName Get - long&

@SVOperationDescription This function gets the selected control type in the form of a double.

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::Get( const SVMatroxBlobResult& p_rResultId, SVBlobControlEnum p_lControlType, long& p_lControlValue) 
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		double l_dValue;
		if( !p_rResultId.empty() )
		{
			l_Code = Get(p_rResultId, p_lControlType, l_dValue);
			if( l_Code == SVMEE_STATUS_OK )
			{
				p_lControlValue = static_cast<long>( l_dValue );
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
@SVOperationName Set - double

@SVOperationDescription This function sets the selected control type with the supplied double value

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::Set( const SVMatroxBlobResult& p_rResultId, SVBlobControlEnum p_lControlType, const double p_lControlValue)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rResultId.empty() )
		{
			long l_lMatroxType = Convert2MatroxControlType( p_lControlType );
			if( l_lMatroxType != M_UNINITIALIZED )
			{
				MblobControl( p_rResultId.m_BlobResultId, l_lMatroxType, p_lControlValue );
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

/**
@SVOperationName Set - long

@SVOperationDescription This function sets the selected control type with the supplied long value

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::Set( const SVMatroxBlobResult& p_rResultId, SVBlobControlEnum p_lControlType, const long p_lControlValue)
{
	return Set( p_rResultId, p_lControlType, static_cast<double>( p_lControlValue ));
}

/**
@SVOperationName GetForeground - bool

@SVOperationDescription This function gets the Foreground control value

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::GetForeground( const SVMatroxBlobResult& p_rResultId, bool& rIsDarkForeground)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rResultId.empty() )
		{
			long value;
			MblobInquire( p_rResultId.m_BlobResultId, M_FOREGROUND_VALUE, &value);
			rIsDarkForeground = (value) ? true : false;
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
@SVOperationName SetForeground - bool

@SVOperationDescription This function sets the Foreground control value

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::SetForeground( const SVMatroxBlobResult& p_rResultId, bool isDarkForeground)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rResultId.empty() )
		{
			MblobControl( p_rResultId.m_BlobResultId, M_FOREGROUND_VALUE, (isDarkForeground) ? M_ZERO: M_NONZERO );
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
@SVOperationName GetIdentifier - bool

@SVOperationDescription This function gets the Blob Identifier Type control value

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::GetIdentifier( const SVMatroxBlobResult& p_rResultId, bool& p_bBinary)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rResultId.empty() )
		{
			long value;
			MblobInquire( p_rResultId.m_BlobResultId, M_IDENTIFIER_TYPE, &value);
			p_bBinary = (value) ? true : false;
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
@SVOperationName SetIdentifier - bool

@SVOperationDescription This function sets the Blob Identifier Type control value

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::SetIdentifier( const SVMatroxBlobResult& p_rResultId, bool p_bBinary)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rResultId.empty() )
		{
			MblobControl( p_rResultId.m_BlobResultId, M_IDENTIFIER_TYPE, (p_bBinary) ? M_BINARY : M_GRAYSCALE );
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
@SVOperationName Blob Fill

@SVOperationDescription This function draws the found blobs in an image that meet certian criteria with a specific color.

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::BlobFill( const SVMatroxBlobResult& p_rResult, const SVMatroxBuffer& p_rImageID, SVBlobControlEnum p_eCriterion, const long& p_lColor)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		// This function replaces MblobFill
		if( !p_rResult.empty() )
		{
			long l_lMatroxType = Convert2MatroxControlType( p_eCriterion );
			if( l_lMatroxType != M_UNINITIALIZED )
			{
				MblobFill( p_rResult.m_BlobResultId, 
					p_rImageID.GetIdentifier(), 
					l_lMatroxType, 
					p_lColor );
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

/**
@SVOperationName Blob Select

@SVOperationDescription This function selects blobs that meet a specified criterion. These blobs will be included in or excluded from future operations (calculations or result retrieval), or deleted entirely from the result buffer. 

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::BlobSelect( const SVMatroxBlobResult& p_rResult, SVBlobOperationEnum p_eOperation, SVBlobSelectionEnum p_eSelection, SVConditionEnum p_eCondition, const double& p_dCondLow, const double& p_dCondHigh)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rResult.empty() )
		{
			long l_lOperation = Convert2MatroxControlType( p_eOperation );
			long l_lSelect = Convert2MatroxControlType( p_eSelection );
			long l_lCondition = Convert2MatroxControlType( p_eCondition );

			if( l_lSelect != M_UNINITIALIZED && l_lOperation != M_UNINITIALIZED && l_lCondition != M_UNINITIALIZED )
			{
				MblobSelect( p_rResult.m_BlobResultId,
					l_lOperation,
					l_lSelect,
					l_lCondition,
					p_dCondLow,
					p_dCondHigh );
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

/**
@SVOperationName Blob Select Feature

@SVOperationDescription This function selects the feature(s) to be calculated by Execute() when using the specified feature list. 

*/
SVMatroxBlobInterface::SVStatusCode SVMatroxBlobInterface::BlobSelectFeature( const SVMatroxBlobFeatureList& p_rFeatureList, SVBlobSelectionEnum p_eFeature)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		// This function replaces MblobSelectFeature
		if( !p_rFeatureList.empty() )
		{
			long l_lFeature = Convert2MatroxControlType( p_eFeature );
			if( l_lFeature != M_UNINITIALIZED )
			{
				MblobSelectFeature( p_rFeatureList.m_BlobFeatureListID,
					l_lFeature);

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

HRESULT SVMatroxBlobInterface::FillResults( const SVBlobOffsetList& p_rBlobList, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	long l_lNumberBlobs = static_cast< long >( p_rBlobList.size() );

	l_Status = p_rResults.SetValue( _T( "Number of Blobs" ), l_lNumberBlobs, true );

	if( l_Status == S_OK )
	{
		for( int i = 0; i < l_lNumberBlobs; ++i)
		{
			SVCommandDataHolder l_Blob;
			const POINT& l_rPoint = p_rBlobList[ i ];

			l_Status = l_Blob.SetValue( _T( "Offset X" ), l_rPoint.x, true );

			if( l_Status == S_OK )
			{
				l_Status = l_Blob.SetValue( _T( "Offset Y" ), l_rPoint.y, true );
			}

			if( l_Status == S_OK )
			{
				SVString l_Name;

				l_Name = SvUl_SF::Format( _T( "Blob Result %d" ), i );

				l_Status = p_rResults.SetContainer( _bstr_t(l_Name.c_str()), l_Blob, true );
			}
		}
	}

	return l_Status;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
class SortBlobs: std::binary_function<const SVMatroxBlobInterface::SVBlob&, const SVMatroxBlobInterface::SVBlob&, bool>
{
private:
	int m_type;

public:
	SortBlobs(int sortCriteria) : m_type(sortCriteria) {}

	bool operator()(const SVMatroxBlobInterface::SVBlob& lBlob, const SVMatroxBlobInterface::SVBlob& rBlob)
	{
		return SVMatroxBlobInterface::Compare(m_type, lBlob, rBlob);
	}
};

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
void SVMatroxBlobInterface::Sort(SVBlobList& list, int sortCriteria)
{
	// Sort by specified sort criteria
	std::sort(list.begin(), list.end(), SortBlobs(sortCriteria));
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
bool SVMatroxBlobInterface::Compare(int criteria, const SVBlob& lBlob, const SVBlob& rBlob)
{
	bool bRetVal = true;
	if (criteria & SortAscending)
	{
		if (criteria & SortBy_XY)
		{
			// sort by x(major), y(minor)
			if (lBlob.center.x > rBlob.center.x ||
				(lBlob.center.x == rBlob.center.x &&
				lBlob.center.y >= rBlob.center.y))
				bRetVal = false;
		}
		else if (criteria & SortBy_YX)
		{
			// sort by y(major), x(minor)
			if (lBlob.center.y > rBlob.center.y ||
				(lBlob.center.y == rBlob.center.y &&
				lBlob.center.x >= rBlob.center.x))
				bRetVal = false;
		}
		else if (criteria & SortBy_X)
		{
			if (lBlob.center.x >= rBlob.center.x)
				bRetVal = false;
		}
		else if (criteria & SortBy_Y)
		{
			if (lBlob.center.y >= rBlob.center.y)
				bRetVal = false;
		}
		else if (criteria & SortBy_Area)
		{
			if (lBlob.area >= rBlob.area)
				bRetVal = false;
		}
	}
	else
	{
		bRetVal = false;

		if (criteria & SortBy_XY)
		{
			// sort by x(major), y(minor)
			if (lBlob.center.x > rBlob.center.x ||
				(lBlob.center.x == rBlob.center.x &&
				lBlob.center.y > rBlob.center.y))
				bRetVal = true;
		}
		else if (criteria & SortBy_YX)
		{
			// sort by y(major), x(minor)
			if (lBlob.center.y > rBlob.center.y ||
				(lBlob.center.y == rBlob.center.y &&
				lBlob.center.x > rBlob.center.x))
				bRetVal = true;
		}
		else if (criteria & SortBy_X)
		{
			if (lBlob.center.x > rBlob.center.x)
				bRetVal = true;
		}
		else if (criteria & SortBy_Y)
		{
			if (lBlob.center.y > rBlob.center.y)
				bRetVal = true;
		}
		else if (criteria & SortBy_Area)
		{
			if (lBlob.area > rBlob.area)
				bRetVal = true;
		}
	}
	return bRetVal;
}

long SVMatroxBlobInterface::CalculateOptimalWidth(const SVBlobList& rList, unsigned short padding)
{
	long avgWidth = 0;

	for (SVBlobList::const_iterator it = rList.begin(); it != rList.end();++it)
	{
		const SVBlob& blob = (*it);
		int width = blob.boundingRect.right - blob.boundingRect.left;
		if (IsValidDimension(width + padding))
		{
			if (!avgWidth)
			{
				avgWidth = width;
			}
			else
			{
				avgWidth += width;
				avgWidth /= 2;
			}
		}
	}
	return avgWidth; 
}

HRESULT SVMatroxBlobInterface::FindSizedBlobs( SVMatroxBuffer& p_rThresholdImage, bool p_LightBackground, SVBlobList& rList )
{
	HRESULT l_Status = S_OK;

	SVMatroxBlobResult blobResult;
	// Note: Special Condition if Max Width and Height is 0
	// We must try and suggest the width and height
	// Create feature list
	SVMatroxBlobFeatureList blobFeatureList;

	l_Status = SVMatroxBlobInterface::Create(blobFeatureList);
	
	if (l_Status == SVMEE_STATUS_OK)
	{
		l_Status = SVMatroxBlobInterface::Create(blobResult);
	}

	// Set Control Values
	if (l_Status == SVMEE_STATUS_OK)
	{
		/*
		#define M_NONZERO               0x80L
		#define M_ZERO                 0x100L
		#define M_NON_ZERO          M_NONZERO
		#define M_FOREGROUND_WHITE      0x80L
		#define M_FOREGROUND_BLACK     0x100L
		*/

		l_Status = SVMatroxBlobInterface::SetForeground(blobResult, p_LightBackground );
	}

	if (l_Status == SVMEE_STATUS_OK)
	{
		/* MIL Blob Identifier Type
		#define M_BINARY          0x00001000L
		#define M_GREYSCALE           0x0200L
		#define M_GRAYSCALE       M_GREYSCALE
		*/

		l_Status = SVMatroxBlobInterface::Set(blobResult, SVEBlobIdentifier, 0x00001000L);
	}

	// Set features
	if (l_Status == SVMEE_STATUS_OK)
	{
		l_Status = SVMatroxBlobInterface::BlobSelectFeature(blobFeatureList, SVEBlobArea);
	}
	if (l_Status == SVMEE_STATUS_OK)
	{
		l_Status = SVMatroxBlobInterface::BlobSelectFeature(blobFeatureList, SVEBlobBoxXMin);
	}
	if (l_Status == SVMEE_STATUS_OK)
	{
		l_Status = SVMatroxBlobInterface::BlobSelectFeature(blobFeatureList, SVEBlobBoxXMax);
	}
	if (l_Status == SVMEE_STATUS_OK)
	{
		l_Status = SVMatroxBlobInterface::BlobSelectFeature(blobFeatureList, SVEBlobBoxYMin);
	}
	if (l_Status == SVMEE_STATUS_OK)
	{
		l_Status = SVMatroxBlobInterface::BlobSelectFeature(blobFeatureList, SVEBlobBoxYMax);
	}
	if (l_Status == SVMEE_STATUS_OK)
	{
		l_Status = SVMatroxBlobInterface::BlobSelectFeature(blobFeatureList, SVEBlobCenterOfGravityX);
	}
	if (l_Status == SVMEE_STATUS_OK)
	{
		l_Status = SVMatroxBlobInterface::BlobSelectFeature(blobFeatureList, SVEBlobCenterOfGravityY);
	}
	if (l_Status == SVMEE_STATUS_OK)
	{   
		// Find Blobs
		l_Status = SVMatroxBlobInterface::Execute(blobResult, p_rThresholdImage, blobFeatureList);
	}

	if (l_Status == SVMEE_STATUS_OK)
	{
		long l_lNumberBlobs = 0;

		if (l_Status == SVMEE_STATUS_OK)
		{
			l_Status = SVMatroxBlobInterface::GetNumber( blobResult, l_lNumberBlobs );
		}

		if (l_Status == S_OK && l_lNumberBlobs > 0)
		{
			SVBlobResultCollection l_lBoxXMax;
			SVBlobResultCollection l_lBoxXMin;
			SVBlobResultCollection l_lBoxYMax;
			SVBlobResultCollection l_lBoxYMin;
			SVBlobResultCollection l_lCenterX;
			SVBlobResultCollection l_lCenterY;
			SVBlobResultCollection l_lArea;
			
			l_lBoxXMax.resize(l_lNumberBlobs);
			l_lBoxXMin.resize(l_lNumberBlobs);
			l_lBoxYMax.resize(l_lNumberBlobs);
			l_lBoxYMin.resize(l_lNumberBlobs);
			l_lCenterX.resize(l_lNumberBlobs);
			l_lCenterY.resize(l_lNumberBlobs);
			l_lArea.resize(l_lNumberBlobs);
			
			// Get all Blobs Found
			l_Status = SVMatroxBlobInterface::GetResult(blobResult, SVEBlobBoxXMin, &l_lBoxXMin[0]);
			if (l_Status == SVMEE_STATUS_OK)
			{
				l_Status = SVMatroxBlobInterface::GetResult(blobResult, SVEBlobBoxXMax, &l_lBoxXMax[0]);
			}
			if (l_Status == SVMEE_STATUS_OK)
			{
				l_Status = SVMatroxBlobInterface::GetResult(blobResult, SVEBlobBoxYMin, &l_lBoxYMin[0]);
			}
			if (l_Status == SVMEE_STATUS_OK)
			{
				l_Status = SVMatroxBlobInterface::GetResult(blobResult, SVEBlobBoxYMax, &l_lBoxYMax[0]);
			}
			if (l_Status == SVMEE_STATUS_OK)
			{
				l_Status = SVMatroxBlobInterface::GetResult(blobResult, SVEBlobCenterOfGravityX, &l_lCenterX[0]);
			}
			if (l_Status == SVMEE_STATUS_OK)
			{
				l_Status = SVMatroxBlobInterface::GetResult(blobResult, SVEBlobCenterOfGravityY, &l_lCenterY[0]);
			}
			if (l_Status == SVMEE_STATUS_OK)
			{
				l_Status = SVMatroxBlobInterface::GetResult(blobResult, SVEBlobArea, &l_lArea[0]);
			}
			
			if (l_Status == SVMEE_STATUS_OK)
			{
				// populate blob rect list
				for (int i = 0;i < l_lNumberBlobs;i++)
				{
					SVBlob blob;
					RECT rect = { l_lBoxXMin[i], l_lBoxYMin[i], l_lBoxXMax[i], l_lBoxYMax[i] };
					POINT pos = { l_lCenterX[i], l_lCenterY[i] };
					blob.boundingRect = rect;
					blob.centerOfGravity = pos;
					blob.center.x = rect.left + ((rect.right - rect.left) / 2);
					blob.center.y = rect.top + ((rect.bottom - rect.top) / 2);
					blob.area = l_lArea[i];
			
					rList.push_back(blob);
				}
				// sort Results
				// Sort the Blob List by the X Axis, Ascending
				Sort(rList, SortAscending | SortBy_XY);
			}
		}
	}

	// Cleanup
	SVMatroxBlobInterface::Destroy(blobFeatureList);
	SVMatroxBlobInterface::Destroy(blobResult);

	return l_Status;
}

HRESULT SVMatroxBlobInterface::NumBlobs( SVMatroxBuffer& p_rThresholdImage, BYTE p_Background, long& p_rBlobCount )
{
	HRESULT l_Status = S_OK;

	SVMatroxBlobFeatureList blobFeatureList;
	SVMatroxBlobResult blobResult;

	l_Status = SVMatroxBlobInterface::Create(blobFeatureList);

	if (l_Status == S_OK)
	{
		l_Status = SVMatroxBlobInterface::Create(blobResult);
	}

	// Set Control Values
	if (l_Status == S_OK)
	{
		bool bLightGround = ( p_Background == DARK );
		l_Status = SVMatroxBlobInterface::SetForeground(blobResult, bLightGround);
	}

	if (l_Status == S_OK)
	{
		l_Status = SVMatroxBlobInterface::Set(blobResult, SVEBlobIdentifier, 0x00001000L);
	}

	// Set features
	if (l_Status == S_OK)
	{
		l_Status = SVMatroxBlobInterface::BlobSelectFeature(blobFeatureList, SVEBlobArea);
	}

	if (l_Status == S_OK)
	{   
		// Find Blobs
		l_Status = SVMatroxBlobInterface::Execute(blobResult, p_rThresholdImage, blobFeatureList);
	}

	if (l_Status == S_OK)
	{
		long l_lNumberBlobs = 0;
		l_Status = SVMatroxBlobInterface::GetNumber(blobResult, l_lNumberBlobs);

		if (l_Status == S_OK)
		{
			p_rBlobCount = l_lNumberBlobs;
		}
	}

	// Cleanup
	SVMatroxBlobInterface::Destroy(blobFeatureList);
	SVMatroxBlobInterface::Destroy(blobResult);

	return l_Status;
}

long SVMatroxBlobInterface::CalculateOptimalHeight(const SVBlobList& rList, unsigned short padding)
{
	long avgHeight = 0;

	for (SVBlobList::const_iterator it = rList.begin(); it != rList.end();++it)
	{
		const SVBlob& blob = (*it);
		int height = blob.boundingRect.bottom - blob.boundingRect.top;

		if (IsValidDimension(height + padding))
		{
			if (!avgHeight)
			{
				avgHeight = height;
			}
			else
			{
				avgHeight += height;
				avgHeight /= 2;
			}
		}
	}
	return avgHeight;
}

bool SVMatroxBlobInterface::IsValidSize(int width, int height)
{
	bool bRetVal = false;
	
	// Reject Blobs that are 1 pixel wide or high
	// Reject Blobs that are greater than 255
	if (IsValidDimension(width) && IsValidDimension(height))
	{
		bRetVal = true;
	}
	return bRetVal;
}

bool SVMatroxBlobInterface::IsValidDimension(int value)
{
	bool bRetVal = false;

	// Reject Blobs that are less than 6 pixels wide or high
	// Reject Blobs that are greater than 256 pixel wide or high
	if (value > 5 && value < 257)
	{
		bRetVal = true;
	}
	return bRetVal;
}

bool SVMatroxBlobInterface::IsValidCandidate(const SVBlob& blob, const SIZE& size, const POINT& offset)
{
	bool bRetVal = false;

	RECT rect = blob.boundingRect;
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// Reject Blobs based on minimum/maximum width and height
	if (IsValidSize(width + offset.x, height + offset.y))
	{
		// Reject Blobs wider or higher than optimal ?
		//if (width <= size.cx && height <= size.cy)
		{
			bRetVal = true;
		}
	}
	return bRetVal;
}

HRESULT SVMatroxBlobInterface::CreateBlobOffsetResultList(const SVBlobList& results, SVBlobOffsetList& offsetList, const SIZE& size, const POINT& offset, const SIZE& imageSize)
{
	HRESULT hr = S_OK;

	//
	offsetList.clear();

	// Get the offset(s) of the blobs
	for (SVBlobList::const_iterator it = results.begin();it != results.end();++it)
	{
		const SVBlob& blob = (*it);
		if (IsValidCandidate(blob, size, offset))
		{
			// Center ROI's on Blobs
			SIZE blobSize = { (blob.boundingRect.right - blob.boundingRect.left) + offset.x, (blob.boundingRect.bottom - blob.boundingRect.top) + offset.y };
			POINT centerPos = { blob.boundingRect.left + (blobSize.cx / 2), blob.boundingRect.top + (blobSize.cy / 2) };
			POINT pos = { centerPos.x - (size.cx / 2), centerPos.y - (size.cy / 2) };

			// Ensure Blob Rects are within the Image Size
			// Check if off the Right Edge
			if (pos.x + size.cx > imageSize.cx)
			{
				// move it
				pos.x -= ((pos.x + size.cx) - imageSize.cx);
			}
			// Check if off the Bottom Edge
			if (pos.y + size.cy > imageSize.cy)
			{
				// move it
				pos.y -= ((pos.y + size.cy) - imageSize.cy);
			}
			// Check if off the Left Edge
			if (pos.x < 0)
			{
				// move it
				pos.x = 0;
			}
			// Check if off the Top Edge
			if (pos.y < 0)
			{
				// move it
				pos.y = 0;
			}
			
			// 
			offsetList.push_back(pos);
		}
	}
	return hr;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBlobInterface.cpp_v  $
 * 
 *    Rev 1.2   12 Nov 2014 08:24:36   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  938
 * SCR Title:  Add Black Blob Mode to Blob Analyzer (SVO-336)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   rename parameter name for methode Set/GetForeground from light background to isBlackForeground
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
 *    Rev 1.0   22 Apr 2013 14:59:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Sep 2011 14:23:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Martox Library to include a new resource management system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   11 Mar 2011 13:36:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver to handle new ocr commands from remote systems via SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jan 2011 10:31:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Oct 2010 15:00:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include new command functionality using the matrox library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Feb 2010 11:32:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added functionality to allow for new OCR training functionality and fixed issues with unicode compiles.
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
