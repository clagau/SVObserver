// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxImageInterface
// * .File Name       : $Workfile:   SVMatroxImageInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   23 Oct 2013 08:50:50  $
// ******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "SVMatroxImageInterface.h"

#include "SVCommandLibrary/SVCommandDataHolder.h"
#include "SVCommandLibrary/SVCommandDataValue.h"
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVMatroxBufferInterface.h"
#include "SVMatroxCommandDataImage.h"
#include "SVMatroxImagingLibrary.h"  // has MIL includes
#include "SVMatroxResourceMonitor.h"


SVInterpolationModeOptions::SVInterpolationModeOptionsEnumMap SVInterpolationModeOptions::m_Convertor = boost::assign::map_list_of<>
	(SVInterpolationModeOptions::InterpolationModeAuto, M_INTERPOLATE)
	(SVInterpolationModeOptions::InterpolationModeBilinear, M_BILINEAR)
	(SVInterpolationModeOptions::InterpolationModeBicubic, M_BICUBIC)
	(SVInterpolationModeOptions::InterpolationModeAverage, M_AVERAGE)
	(SVInterpolationModeOptions::InterpolationModeNearestNeighbor, M_NEAREST_NEIGHBOR);

SVOverscanOptions::SVOverscanOptionsEnumMap SVOverscanOptions::m_Convertor = boost::assign::map_list_of<>
	(SVOverscanOptions::OverscanEnable, M_OVERSCAN_ENABLE)
	(SVOverscanOptions::OverscanDisable, M_OVERSCAN_DISABLE);

SVPerformanceOptions::SVPerformanceOptionsEnumMap SVPerformanceOptions::m_Convertor = boost::assign::map_list_of<>
	(SVPerformanceOptions::PerformanceFast, M_FAST)
	(SVPerformanceOptions::PerformancePresice, M_REGULAR);

/**
@SVOperationName Default Constructor

@SVOperationDescription

*/
SVMatroxImageInterface::SVMatroxImageInterface()
{
}

/**
@SVOperationName Destructor

@SVOperationDescription

*/
SVMatroxImageInterface::~SVMatroxImageInterface()
{
}

/**
@SVOperationName Convert2MatroxType - SVImageOperationTypeEnum

@SVOperationDescription This function converts a SVImageOperationTypeEnum to a Matrox constant.

*/
long SVMatroxImageInterface::Convert2MatroxType( SVImageOperationTypeEnum p_eType) 
{
	long l_lMatroxType = 0;
	switch( p_eType &~ SVSaturation)
	{

		case SVImageMultipleAccumulate:
		{
			l_lMatroxType = M_MULTIPLY_ACCUMULATE_2;
			break;
		}
		case SVImageAdd:
		{
			l_lMatroxType = M_ADD;
			break;
		}
		case SVImageAnd:
		{
			l_lMatroxType = M_AND;
			break;
		}
		case SVImageDiv:
		{
			l_lMatroxType = M_DIV;
			break;
		}
		case SVImageMax:
		{
			l_lMatroxType = M_MAX;
			break;
		}
		case SVImageMin:
		{
			l_lMatroxType = M_MIN;
			break;
		}
		case SVImageMult:
		{
			l_lMatroxType = M_MULT;
			break;
		}
		case SVImageNand:
		{
			l_lMatroxType = M_NAND;
			break;
		}
		case SVImageNor:
		{
			l_lMatroxType = M_NOR;
			break;
		}
		case SVImageOr:
		{
			l_lMatroxType = M_OR;
			break;
		}
		case SVImageSub:
		{
			l_lMatroxType = M_SUB;
			break;
		}
		case SVImageSubAbs:
		{
			l_lMatroxType = M_SUB_ABS;
			break;
		}
		case SVImageXOr:
		{
			l_lMatroxType = M_XOR;
			break;
		}
		case SVImageXNor:
		{
			l_lMatroxType = M_XNOR;
			break;
		}
		case SVImagePass:
		{
			l_lMatroxType = M_PASS;
			break;
		}
		case SVImageNot:
		{
			l_lMatroxType = M_NOT;
			break;
		}
	}

	switch( p_eType )
	{
		case SVImageCountList:
		{
			l_lMatroxType = M_COUNT_LIST;
			break;
		}
		case SVImageEventList:
		{
			l_lMatroxType = M_EVENT_LIST;
			break;
		}
		case SVImageExtreemList:
		{
			l_lMatroxType = M_EXTREME_LIST;
			break;
		}
		case SVImageHistList:
		{
			l_lMatroxType = M_HIST_LIST;
			break;
		}
		case SVImageProjList:
		{
			l_lMatroxType = M_PROJ_LIST;
			break;
		}

		case SVImageGrayScale:
		{
			l_lMatroxType = M_GRAYSCALE;
			break;
		}
		case SVImageBinary:
		{
			l_lMatroxType = M_BINARY;
			break;
		}


		case SVImageDefault:
		{
			l_lMatroxType = M_DEFAULT;
			break;
		}

		case SVNearestNeighbor:
		{
			l_lMatroxType = M_NEAREST_NEIGHBOR;
			break;
		}
		case SVNearestNeighOverScanClear:
		{
			l_lMatroxType = M_NEAREST_NEIGHBOR | M_OVERSCAN_CLEAR;
			break;
		}
		case SVNearestNeighOverScanEnable:
		{
			l_lMatroxType = M_NEAREST_NEIGHBOR | M_OVERSCAN_ENABLE;
			break;
		}
		case SVNearestNeighOverScanDisable:
		{
			l_lMatroxType = M_NEAREST_NEIGHBOR | M_OVERSCAN_DISABLE;
			break;
		}

		case SVBilinear:
		{
			l_lMatroxType = M_BILINEAR;
			break;
		}
		case SVBilinearOverScanClear:
		{
			l_lMatroxType = M_BILINEAR | M_OVERSCAN_CLEAR;
			break;
		}
		case SVBilinearOverScanEnable:
		{
			l_lMatroxType = M_BILINEAR | M_OVERSCAN_ENABLE;
			break;
		}
		case SVBilinearOverScanDisable:
		{
			l_lMatroxType = M_BILINEAR | M_OVERSCAN_DISABLE;
			break;
		}

		case SVBiCubic:
		{
			l_lMatroxType = M_BICUBIC;
			break;
		}
		case SVBiCubicOverScanClear:
		{
			l_lMatroxType = M_BICUBIC | M_OVERSCAN_CLEAR;
			break;
		}
		case SVBiCubicOverScanEnable:
		{
			l_lMatroxType = M_BICUBIC | M_OVERSCAN_ENABLE;
			break;
		}
		case SVBiCubicOverScanDisable:
		{
			l_lMatroxType = M_BICUBIC | M_OVERSCAN_DISABLE;
			break;
		}
 	}
	
	// Combination Constant
	if( (p_eType & SVImageFloatType) == SVImageFloatType )
	{
		l_lMatroxType |= M_FLOAT;
	}

	if( (p_eType & SVSaturation) == SVSaturation )
	{
		l_lMatroxType |= M_SATURATION;
	}

	return l_lMatroxType;
}


/**
@SVOperationName Convert2MatroxType - SVConditionEnum

@SVOperationDescription This function converts a SVConditionEnum to a Matrox constant.

*/
long SVMatroxImageInterface::Convert2MatroxType(SVConditionEnum p_eControlType) 
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch( p_eControlType )
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
@SVOperationName Convert2MatroxType - SVFilterOperationEnum

@SVOperationDescription This function converts a SVFilterOperationEnum to a Matrox constant.

*/
long SVMatroxImageInterface::Convert2MatroxType( SVFilterOperationEnum p_eType) 
{
	long l_lMatroxType = 0;
	switch( p_eType  )
	{
		case SVFilterOpEdge:
		{
			l_lMatroxType = M_EDGE_DETECT;
			break;
		}
		case SVFilterOpEdgeDet2:
		{
			l_lMatroxType = M_EDGE_DETECT2;
			break;
		}
		case SVFilterOpHorizEdge:
		{
			l_lMatroxType = M_HORIZ_EDGE;
			break;
		}
		case SVFilterOpLaplacianEdge:
		{
			l_lMatroxType = M_LAPLACIAN_EDGE;
			break;
		}
		case SVFilterOpLaplacianEdge2:
		{
			l_lMatroxType = M_LAPLACIAN_EDGE2;
			break;
		}
		case SVFilterOpSharpen:
		{
			l_lMatroxType = M_SHARPEN;
			break;
		}
		case SVFilterOpSharpen2:
		{
			l_lMatroxType = M_SHARPEN2;
			break;
		}
		case SVFilterOpSmooth:
		{
			l_lMatroxType = M_SMOOTH;
			break;
		}
		case SVFilterOpVertEdge:
		{
			l_lMatroxType = M_VERT_EDGE;
			break;
		}
	}
	return l_lMatroxType;
}


/**
@SVOperationName Convert2MatroxType - SVImageWaterShedEnum

@SVOperationDescription This function converts a SVImageWaterShedEnum to a Matrox constant.

*/
long SVMatroxImageInterface::Convert2MatroxType( SVImageWaterShedEnum p_eType) 
{
	long l_lMatroxType = 0;
	if( (p_eType & SVImageWSBasin) == SVImageWSBasin )
	{
		l_lMatroxType |= M_BASIN;
	}
	else
	if( (p_eType & SVImageWSWatershed) == SVImageWSWatershed )
	{
		l_lMatroxType |= M_WATERSHED;
	}

	if( (p_eType & SVImageWSMaximaFill) == SVImageWSMaximaFill )
	{
		l_lMatroxType |= M_MAXIMA_FILL;
	}
	else
	if( (p_eType & SVImageWSMinimaFill) == SVImageWSMinimaFill )
	{
		l_lMatroxType |= M_MINIMA_FILL;
	}

	if( (p_eType & SVImageWSRegular) == SVImageWSRegular )
	{
		l_lMatroxType |= M_REGULAR;
	}
	else
	if( (p_eType & SVImageWSStraight) == SVImageWSStraight )
	{
		l_lMatroxType |= M_STRAIGHT_WATERSHED;
	}

	if( (p_eType & SVImage4Connected) == SVImage4Connected )
	{
		l_lMatroxType |= M_4_CONNECTED;
	}
	else
	if( (p_eType & SVImage8Connected) == SVImage8Connected )
	{
		l_lMatroxType |= M_8_CONNECTED;
	}

	if( (p_eType & SVImageSkipLastLevel) == SVImageSkipLastLevel )
	{
		l_lMatroxType |= M_SKIP_LAST_LEVEL;
	}


	if( (p_eType & SVImageFillSource) == SVImageFillSource )
	{
		l_lMatroxType |= M_FILL_SOURCE;
	}
	return l_lMatroxType;
}


/**
@SVOperationName Create

@SVOperationDescription This function creates a SVMatroxImageResult.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Create( SVMatroxImageResult& p_rResultId, const long p_lNbEntries, SVImageOperationTypeEnum p_eResultType)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lResultType = Convert2MatroxType( p_eResultType );

		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == SVMEE_STATUS_OK )
		{
			MIL_ID l_NewId = MimAllocResult(M_DEFAULT_HOST,
				p_lNbEntries,
				l_lResultType,
				M_NULL);


			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if( l_Code == SVMEE_STATUS_OK )
			{
				SVMatroxResourceMonitor::InsertIdentifier( SVImageID, l_NewId );

				if( p_rResultId.empty() )
				{
					p_rResultId.m_ImageResultID = l_NewId;
				}
				else
				{
					l_Code = Destroy( p_rResultId );
					p_rResultId.m_ImageResultID = l_NewId;
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Destroy

@SVOperationDescription This function destroys a SVMatroxImageResult.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Destroy( SVMatroxImageResult& p_rResultId )
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
				SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

				l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

				if( l_Code == SVMEE_STATUS_OK )
				{
					MimFree( p_rResultId.m_ImageResultID );
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
					if( l_Code == SVMEE_STATUS_OK )
					{
						SVMatroxResourceMonitor::EraseIdentifier( SVImageID, p_rResultId.m_ImageResultID );

						p_rResultId.m_ImageResultID = M_NULL;
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


/**
@SVOperationName Image Arithmatic

@SVOperationDescription This function performs the specified point-to-point operation on two images, an image and a constant, an image, or a constant, storing results in the specified destination image buffer.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Arithmetic( const SVMatroxBuffer& p_rDestId, 
																		const SVMatroxBuffer& p_dSource1, 
																		const SVMatroxBuffer& p_dSource2, 
																		SVImageOperationTypeEnum p_eOperation)
{
	// This function Replaces MimArith ( image tool )
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rDestId.empty() && !p_dSource1.empty() && !p_dSource2.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType( p_eOperation );
			if( l_lMatroxFlags !=0 )
			{
				if( p_eOperation == SVImageMultipleAccumulate )
				{
					MimArithMultiple( p_dSource1.GetIdentifier(),	// Source1
						1,												// Source2
						p_dSource2.GetIdentifier(),					// Source3
						1,												// Source4
						2,												// Source5
						p_rDestId.GetIdentifier(),					// Destination
						l_lMatroxFlags,
						M_DEFAULT);
				}
				else
				{
					if ( l_lMatroxFlags != M_PASS ) 
					{
						MimArith(p_dSource1.GetIdentifier(), 
							p_dSource2.GetIdentifier(), 
							p_rDestId.GetIdentifier(),
							l_lMatroxFlags);
					}
					else // for M_PASS operation the second image has to be M_NULL
					{
						MimArith(p_dSource1.GetIdentifier(), 
							M_NULL, 
							p_rDestId.GetIdentifier(),
							l_lMatroxFlags);
					}
				}

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Binarize

@SVOperationDescription This function is used to threshold an image.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Binarize( const SVMatroxBuffer& p_rDest,
																	  const SVMatroxBuffer p_rSource, 
																	  SVConditionEnum p_eCondition, 
																	  double p_dCondLow, 
																	  double p_dCondHigh )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
	SVMatroxStatusInformation	l_StatusInfo;

#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType( p_eCondition );
			if( l_lMatroxFlags !=0 )
			{
				MimBinarize(p_rSource.GetIdentifier(), 
					p_rDest.GetIdentifier(), 
					l_lMatroxFlags,
					p_dCondLow,
					p_dCondHigh);
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
//		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

//-	JAB110708
	if (l_Code != SVMEE_STATUS_OK)
	{
		l_StatusInfo.m_StatusCode = l_Code;
		l_StatusInfo.m_StatusString = "SVMatroxImageInterface::Binarize error";
		SVMatroxApplicationInterface::Log(l_StatusInfo);
		assert(0);
	}

	return l_Code;
}

HRESULT SVMatroxImageInterface::AdaptiveThreshold( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	_variant_t l_IsBlack;
	SVMatroxCommandDataImage l_SourceImage;
	long l_Width = 0;
	long l_Height = 0;
	long l_Stride = 0;
	unsigned char* l_pSource = nullptr;

	l_Status = p_rAttributes.GetValue( _T( "Is Black Background" ), l_IsBlack );

	if( S_OK == l_Status )
	{
		SVByteVector l_TempImage;

		l_Status = p_rAttributes.GetImage( _T( "Source Image" ), l_TempImage );

		if( S_OK == l_Status )
		{
			l_Status = l_SourceImage.SetData( l_TempImage );
		}
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBufferInterface::Get( l_SourceImage.m_Buffer, SVSizeX, l_Width );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBufferInterface::Get( l_SourceImage.m_Buffer, SVSizeY, l_Height );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBufferInterface::Get( l_SourceImage.m_Buffer, SVPitchByte, l_Stride );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBufferInterface::GetHostAddress( &l_pSource, l_SourceImage.m_Buffer );
	}

	if( S_OK == l_Status )
	{
		// Create the Destination Buffer
		SVMatroxBuffer dstBuffer;
		unsigned char* l_pDest = nullptr;

		SVMatroxBufferCreateStruct createStruct;
		createStruct.m_lSizeBand = 1;
		createStruct.m_lSizeX = l_Width;
		createStruct.m_lSizeY = l_Height;
		createStruct.m_eType = SV8BitUnsigned;
		createStruct.m_eAttribute = SVBufAttImageProcDib;

		l_Status = SVMatroxBufferInterface::Create(dstBuffer, createStruct);

		if( S_OK == l_Status )
		{
			l_Status = SVMatroxBufferInterface::GetHostAddress( &l_pDest, dstBuffer );
		}

		if( S_OK == l_Status )
		{
			if( (bool)l_IsBlack )
			{
				l_Status = SVMatroxImageInterface::Arithmetic(l_SourceImage.m_Buffer, l_SourceImage.m_Buffer, l_SourceImage.m_Buffer, SVImageNot);
			}

			if( S_OK == l_Status )
			{
				AdaptiveThreshold(l_pSource, l_pDest, static_cast< int >( l_Width ), static_cast< int >( l_Height ), static_cast< int >( l_Stride ), 0.15f);
			}

			if( (bool)l_IsBlack )
			{
				l_Status = SVMatroxImageInterface::Arithmetic(dstBuffer, dstBuffer, dstBuffer, SVImageNot);
			}

			if( S_OK == l_Status )
			{
				SVCommandDataFacadePtr l_DestPtr = new SVMatroxCommandDataImage( dstBuffer, true ); 

				if( ! l_DestPtr.empty() )
				{
					l_Status = p_rResults.SetData( _T( "Result Image" ), l_DestPtr );
				}
				else
				{
					l_Status = E_FAIL;
				}
			}

			dstBuffer.clear();
		}
	}

	return l_Status;
}

HRESULT SVMatroxImageInterface::AutoThreshold( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	_variant_t l_Multiplier;
	_variant_t l_IsBlack;
	SVMatroxCommandDataImage l_SourceImage;
	long l_Width = 0;
	long l_Height = 0;

	l_Status = p_rAttributes.GetValue( _T( "Threshold Multiplier" ), l_Multiplier );

	if( S_OK == l_Status )
	{
		l_Status = p_rAttributes.GetValue( _T( "Is Black Background" ), l_IsBlack );
	}

	if( S_OK == l_Status )
	{
		SVByteVector l_TempImage;

		l_Status = p_rAttributes.GetImage( _T( "Source Image" ), l_TempImage );

		if( S_OK == l_Status )
		{
			l_Status = l_SourceImage.SetData( l_TempImage );
		}
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBufferInterface::Get( l_SourceImage.m_Buffer, SVSizeX, l_Width );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBufferInterface::Get( l_SourceImage.m_Buffer, SVSizeY, l_Height );
	}

	if( S_OK == l_Status )
	{
		// Create the Destination Buffer
		SVMatroxBuffer dstBuffer;
		unsigned char l_Value = 0;

		SVMatroxBufferCreateStruct createStruct;
		createStruct.m_lSizeBand = 1;
		createStruct.m_lSizeX = l_Width;
		createStruct.m_lSizeY = l_Height;
		createStruct.m_eType = SV8BitUnsigned;
		createStruct.m_eAttribute = SVBufAttImageProcDib;

		l_Status = SVMatroxBufferInterface::Create(dstBuffer, createStruct);

		if( S_OK == l_Status )
		{
			// Read histogram...
			SVMatroxImageResult histResult;
				
			SVMatroxImageInterface::SVStatusCode l_RetCode = SVMatroxImageInterface::Create(histResult, 256, SVImageHistList);

			if (l_RetCode == SVMEE_STATUS_OK)
			{
				l_RetCode = SVMatroxImageInterface::Histogram(histResult, l_SourceImage.m_Buffer);
			}
			if (l_RetCode == SVMEE_STATUS_OK)
			{
				SVMatroxLongArray histValueArray;

				__int64 numPixels = l_Width * l_Height;

				l_RetCode = SVMatroxImageInterface::GetResult(histResult, histValueArray);
				if (l_RetCode == SVMEE_STATUS_OK)
				{
					if (histValueArray.size() && numPixels)
					{
						unsigned char l_Lower( 0 );
						unsigned char l_Upper( 255 );

						__int64 sum = 0;
						for (size_t i = 0;i < histValueArray.size(); ++i)
						{
							sum += ((__int64)histValueArray[i]) * ((__int64)i);
						}

						// Calculate average and auto threshold value...
						unsigned char autoThreshold = (unsigned char)((double)(sum / numPixels) * (double)l_Multiplier);

						if( (bool)l_IsBlack )
						{
							l_Lower = autoThreshold;
						}
						else
						{
							l_Upper = autoThreshold;
						}
						//
						l_Value = autoThreshold;

						//
						if( (unsigned char)l_Lower <= (unsigned char)l_Upper )
						{
							l_RetCode = SVMatroxImageInterface::Binarize(dstBuffer,
																		l_SourceImage.m_Buffer,
																		SVECondInRange,
																		l_Lower,
																		l_Upper);
							if (l_RetCode != SVMEE_STATUS_OK)
							{
								l_Status = l_RetCode;
							}
						}
						else
						{
							l_RetCode = SVMatroxImageInterface::Binarize(dstBuffer,
																		l_SourceImage.m_Buffer,
																		SVECondOutRange,
																		l_Upper,
																		l_Lower);

							if (l_RetCode != SVMEE_STATUS_OK)
							{
								l_Status = l_RetCode;
							}
						}

					}
				}
				else
				{
					l_Status = l_RetCode;
				}
			} 
			else
			{
				l_Status = l_RetCode;
			}

			SVMatroxImageInterface::Destroy(histResult);

			if( S_OK == l_Status )
			{
				l_Status = p_rResults.SetValue( _T( "Threshold Value" ), l_Value, true );
			}

			if( S_OK == l_Status )
			{
				SVCommandDataFacadePtr l_DestPtr = new SVMatroxCommandDataImage( dstBuffer, true );

				if( ! l_DestPtr.empty() )
				{
					l_Status = p_rResults.SetData( _T( "Result Image" ), l_DestPtr );
				}
				else
				{
					l_Status = E_FAIL;
				}
			}

			dstBuffer.clear();
		}
	}

	return l_Status;
}

HRESULT SVMatroxImageInterface::FixedThreshold( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults )
{
	HRESULT l_Status = S_OK;

	_variant_t l_Lower;
	_variant_t l_Upper;
	SVMatroxCommandDataImage l_SourceImage;
	long l_Width = 0;
	long l_Height = 0;

	l_Status = p_rAttributes.GetValue( _T( "Threshold Lower Value" ), l_Lower );

	if( S_OK == l_Status )
	{
		l_Status = p_rAttributes.GetValue( _T( "Threshold Upper Value" ), l_Upper );
	}

	if( S_OK == l_Status )
	{
		SVByteVector l_TempImage;

		l_Status = p_rAttributes.GetImage( _T( "Source Image" ), l_TempImage );

		if( S_OK == l_Status )
		{
			l_Status = l_SourceImage.SetData( l_TempImage );
		}
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBufferInterface::Get( l_SourceImage.m_Buffer, SVSizeX, l_Width );
	}

	if( S_OK == l_Status )
	{
		l_Status = SVMatroxBufferInterface::Get( l_SourceImage.m_Buffer, SVSizeY, l_Height );
	}

	if( S_OK == l_Status )
	{
		// Create the Destination Buffer
		SVMatroxBuffer dstBuffer;

		SVMatroxBufferCreateStruct createStruct;
		createStruct.m_lSizeBand = 1;
		createStruct.m_lSizeX = l_Width;
		createStruct.m_lSizeY = l_Height;
		createStruct.m_eType = SV8BitUnsigned;
		createStruct.m_eAttribute = SVBufAttImageProcDib;

		l_Status = SVMatroxBufferInterface::Create(dstBuffer, createStruct);

		if( S_OK == l_Status )
		{
			SVMatroxImageInterface::SVStatusCode l_RetCode(SVMEE_STATUS_OK);

			if( (unsigned char)l_Lower <= (unsigned char)l_Upper )
			{
				l_RetCode = SVMatroxImageInterface::Binarize(dstBuffer,
															l_SourceImage.m_Buffer,
															SVECondInRange,
															l_Lower,
															l_Upper);
				if (l_RetCode != SVMEE_STATUS_OK)
				{
					l_Status = l_RetCode;
				}
			}
			else
			{
				l_RetCode = SVMatroxImageInterface::Binarize(dstBuffer,
															l_SourceImage.m_Buffer,
															SVECondOutRange,
															l_Upper,
															l_Lower);

				if (l_RetCode != SVMEE_STATUS_OK)
				{
					l_Status = l_RetCode;
				}
			}

			if( S_OK == l_Status )
			{
				SVCommandDataFacadePtr l_DestPtr = new SVMatroxCommandDataImage( dstBuffer, true );

				if( ! l_DestPtr.empty() )
				{
					l_Status = p_rResults.SetData( _T( "Result Image" ), l_DestPtr );
				}
				else
				{
					l_Status = E_FAIL;
				}
			}

			dstBuffer.clear();
		}
	}

	return l_Status;
}

/**
@SVOperationName Image Close 

@SVOperationDescription This function performs a binary or grayscale closing operation on the given source image for the specified number of iterations.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Close( const SVMatroxBuffer& p_rDest, 
																   const SVMatroxBuffer& p_rSource, 
																   long p_lItters, 
																   SVImageOperationTypeEnum p_eOp)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType( p_eOp );
			if( l_lMatroxFlags !=0 )
			{
				MimClose(p_rSource.GetIdentifier(), 
					p_rDest.GetIdentifier(),
					p_lItters,
					l_lMatroxFlags);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Convert

@SVOperationDescription This function is used to convert image types.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Convert( const SVMatroxBuffer& p_rDest, 
																	 const SVMatroxBuffer& p_rSource, 
																	 SVImageOperationTypeEnum p_eOperation)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = 0;
			if( p_eOperation == SVImageRGBToHLS )
			{
				l_lMatroxFlags = M_RGB_TO_HLS;
			}
			else
			if( p_eOperation == SVImageHLSToRGB )
			{
				l_lMatroxFlags = M_HLS_TO_RGB;
			}
			else if( p_eOperation == SVImageRGBToL )
			{
				l_lMatroxFlags = M_RGB_TO_L;
			}

			MimConvert(p_rSource.GetIdentifier(), 
				p_rDest.GetIdentifier(),
				l_lMatroxFlags);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}



/**
@SVOperationName Image Convolve - Kernel

@SVOperationDescription This function performs a general convolution operation on the source buffer using the supplied kernel.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Convolve( const SVMatroxBuffer& p_rDest, 
																	  const SVMatroxBuffer& p_rSource, 
																	  const SVMatroxBuffer& p_Kernel )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty() && !p_Kernel.empty() )
		{
			MimConvolve(p_rSource.GetIdentifier(), 
				p_rDest.GetIdentifier(),
				p_Kernel.GetIdentifier());

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Convolve - Pre-defined Filter

@SVOperationDescription This function performs a general convolution operation on the source buffer using the specified filter.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Convolve( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, SVFilterOperationEnum p_eFilterOperation )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxFlags = Convert2MatroxType( p_eFilterOperation );

		if( !p_rSource.empty() && !p_rDest.empty() )
		{
			MimConvolve(p_rSource.GetIdentifier(), 
				p_rDest.GetIdentifier(),
				l_lMatroxFlags );

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Dilate

@SVOperationDescription This function performs a binary or grayscale dilation on the given source image for the specified number of iterations.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Dilate( const SVMatroxBuffer& p_rDest, 
																	const SVMatroxBuffer& p_rSource, 
																	long p_lItters, 
																	SVImageOperationTypeEnum p_eOp)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType( p_eOp );
			if( l_lMatroxFlags != 0 )
			{
				MimDilate(p_rSource.GetIdentifier(), 
					p_rDest.GetIdentifier(),
					p_lItters,
					l_lMatroxFlags);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Erode

@SVOperationDescription This function performs a binary or grayscale erosion on the given source image for the specified number of iterations.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Erode( const SVMatroxBuffer& p_rDest, 
																   const SVMatroxBuffer& p_rSource, 
																   long p_lItters, 
																   SVImageOperationTypeEnum p_eOp)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType( p_eOp );
			if( l_lMatroxFlags != 0 )
			{
				MimErode(p_rSource.GetIdentifier(), 
					p_rDest.GetIdentifier(),
					p_lItters,
					l_lMatroxFlags);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Flip

@SVOperationDescription This function flips the source image to the destination image according to the specified operation.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Flip( const SVMatroxBuffer& p_rDest, 
																  const SVMatroxBuffer& p_rSource, 
																  SVImageFlipEnum p_eOperation)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = M_FLIP_HORIZONTAL;

			if( p_eOperation == SVImageFlipHorizontal )
			{
				l_lMatroxFlags = M_FLIP_HORIZONTAL;
			}
			else
			if( p_eOperation == SVImageFlipVertical )
			{
				l_lMatroxFlags = M_FLIP_VERTICAL;
			}

			MimFlip(p_rSource.GetIdentifier(), 
				p_rDest.GetIdentifier(),
				l_lMatroxFlags,
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


/**
@SVOperationName Image Get Result - SVMatroxDoubleArray

@SVOperationDescription This function retrieves all the results of the specified type from the specified result buffer and stores them in the specified one-dimensional destination user array.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::GetResult( const SVMatroxImageResult& p_rResultID, 
																	   SVMatroxDoubleArray& p_adArray)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		MIL_INT l_lSize = 0;
		if( !p_rResultID.empty() )
		{
			MimInquire( p_rResultID.m_ImageResultID,
				M_RESULT_SIZE,
				&l_lSize);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if( l_Code == SVMEE_STATUS_OK && l_lSize > 0)
			{
				p_adArray.resize( l_lSize );
				MimGetResult( p_rResultID.m_ImageResultID,
					M_VALUE | M_TYPE_DOUBLE  ,
					&p_adArray[0]);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;	
}

/**
@SVOperationName Image Get Result - SVMatroxLongArray

@SVOperationDescription This function retrieves all the results of the specified type from the specified result buffer and stores them in the specified one-dimensional destination user array.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::GetResult( const SVMatroxImageResult& p_rResultID, 
																	   SVMatroxLongArray& p_alArray)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		MIL_INT l_lSize = 0;
		if( !p_rResultID.empty() )
		{
			MimInquire( p_rResultID.m_ImageResultID,
				M_RESULT_SIZE,
				&l_lSize);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if( l_Code == SVMEE_STATUS_OK && l_lSize > 0)
			{
				p_alArray.resize( l_lSize );
				MimGetResult( p_rResultID.m_ImageResultID,
					M_VALUE | M_TYPE_LONG,
					&p_alArray[0]);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;	
}
/**
@SVOperationName Image Get Result - SVMatroxByteArray

@SVOperationDescription This function retrieves all the results of the specified type from the specified result buffer and stores them in the specified one-dimensional destination user array.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::GetResult( const SVMatroxImageResult& p_rResultID, 
																	   SVMatroxByteArray& p_acArray)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		MIL_INT l_lSize = 0;
		if( !p_rResultID.empty() )
		{
			MimInquire( p_rResultID.m_ImageResultID,
				M_RESULT_SIZE,
				&l_lSize);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if( l_Code == SVMEE_STATUS_OK && l_lSize > 0)
			{
				p_acArray.resize( l_lSize );
				MimGetResult( p_rResultID.m_ImageResultID,
					M_VALUE + M_TYPE_CHAR,
					&p_acArray[0]);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;	
}

/**
@SVOperationName Image Get Result - void*

@SVOperationDescription This function retrieves all the results of the specified type from the specified result buffer and stores them in the specified one-dimensional destination user array.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::GetResult( const SVMatroxImageResult& p_rResultID, void * p_pArray)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lSize = 0;
		if( !p_rResultID.empty() )
		{
			MimGetResult( p_rResultID.m_ImageResultID,
				M_VALUE,
				p_pArray);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;	
}


/**
@SVOperationName Image Histogram

@SVOperationDescription This function calculates the histogram (or pixel intensity distribution) of the specified source buffer and stores results in the specified histogram result buffer.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Histogram( const SVMatroxImageResult& p_rHistResult, 
																	   const SVMatroxBuffer& p_rSource)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rHistResult.empty())
		{
			MimHistogram(p_rSource.GetIdentifier(), p_rHistResult.m_ImageResultID);

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Lut Map

@SVOperationDescription This function maps each pixel in the specified source image to values determined by the specified lookup table (LUT).

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::LutMap( const SVMatroxBuffer& p_rDest, 
																	const SVMatroxBuffer& p_rSource, 
																	const SVMatroxBuffer& p_rLut)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty() && !p_rLut.empty() )
		{
			MimLutMap(p_rSource.GetIdentifier(), 
				p_rDest.GetIdentifier(),
				p_rLut.GetIdentifier());

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Open

@SVOperationDescription This function performs a binary or grayscale opening operation on the given source image for the specified number of iterations.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Open( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType( p_eOp );
			if( l_lMatroxFlags !=0 )
			{
				MimOpen(p_rSource.GetIdentifier(), 
					p_rDest.GetIdentifier(),
					p_lItters,
					l_lMatroxFlags);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Polar Transform

@SVOperationDescription This function performs a rectangular-to-polar or polar-to-rectangular transformation.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::PolarTransform( const SVMatroxBuffer& p_rDest, 
																			SVMatroxPolarTransformStruct& p_rTransformStruct)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rDest.empty() && !p_rTransformStruct.m_rSource.empty() )
		{
			long l_lMatroxInterpMode = Convert2MatroxType( p_rTransformStruct.m_eInterpMode );
			long l_lMatroxOperationMode = M_RECTANGULAR_TO_POLAR;

			if( p_rTransformStruct.m_eOpMode == SVRectToPolar)
			{
				l_lMatroxOperationMode = M_RECTANGULAR_TO_POLAR ;
			}
			else
			{
				l_lMatroxOperationMode = M_POLAR_TO_RECTANGULAR;
			}

			if( l_lMatroxInterpMode !=0 && l_lMatroxOperationMode != 0 )
			{
				MimPolarTransform(p_rTransformStruct.m_rSource.GetIdentifier(), 
					p_rDest.GetIdentifier(),
					p_rTransformStruct.m_dCenterX,
					p_rTransformStruct.m_dCenterY,
					p_rTransformStruct.m_dEndRadius,
					p_rTransformStruct.m_dStartRadius,
					p_rTransformStruct.m_dStartAngle,
					p_rTransformStruct.m_dEndAngle,
					l_lMatroxOperationMode,
					l_lMatroxInterpMode,
					&p_rTransformStruct.m_dDestSizeX,
					&p_rTransformStruct.m_dDestSizeY);

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Project

@SVOperationDescription This function projects a two-dimensional buffer into a one-dimensional buffer from the specified angle, and writes results to the specified projection result buffer.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Project( const SVMatroxImageResult& p_rProjDest, 
																	 const SVMatroxBuffer& p_rSource, 
																	 double p_dAngle)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rProjDest.empty())
		{
			MimProject(p_rSource.GetIdentifier(), 
				p_rProjDest.m_ImageResultID,
				p_dAngle);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Rank

@SVOperationDescription This function performs a rank filter operation on the specified source buffer. 

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Rank( const SVMatroxBuffer& p_rDest, 
																  const SVMatroxBuffer& p_rSource, 
																  const SVMatroxBuffer& p_rStructElem, 
																  long p_lRank)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty() && !p_rStructElem.empty() )
		{
			MimRank(p_rSource.GetIdentifier(), 
				p_rDest.GetIdentifier(),
				p_rStructElem.GetIdentifier(),
				p_lRank,
				M_GRAYSCALE);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Rotate

@SVOperationDescription This function rotates an image by the specified angle of rotation, using the specified interpolation mode.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Rotate( const SVMatroxBuffer& p_rProjDest, 
																	const SVMatroxImageRotateStruct& p_RotateStruct )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rProjDest.empty() && !p_RotateStruct.m_rSource.empty() )
		{
			long l_lMatroxFlags = Convert2MatroxType( p_RotateStruct.m_eInterpolation );
			if( l_lMatroxFlags !=0 )
			{
				MimRotate(p_RotateStruct.m_rSource.GetIdentifier(), 
					p_rProjDest.GetIdentifier(),
					p_RotateStruct.m_dAngle,
					p_RotateStruct.m_dSrcCenX,
					p_RotateStruct.m_dSrcCenY,
					p_RotateStruct.m_dDstCenX,
					p_RotateStruct.m_dDstCenY,
					l_lMatroxFlags);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


/**
@SVOperationName Image Thick

@SVOperationDescription This function performs a binary or grayscale thickening on the specified source image for the specified number of iterations.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Thick( const SVMatroxBuffer& p_rDest, 
																   const SVMatroxBuffer& p_rSource, 
																   long p_lItters, 
																   SVImageOperationTypeEnum p_eOp)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType( p_eOp );
			if( l_lMatroxFlags !=0 )
			{
				MimThick(p_rSource.GetIdentifier(), 
					p_rDest.GetIdentifier(),
					p_lItters,
					l_lMatroxFlags);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Thin

@SVOperationDescription This function performs a binary or grayscale thinning on the specified source image for the specified number of iterations.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Thin( const SVMatroxBuffer& p_rDest, 
																  const SVMatroxBuffer& p_rSource, 
																  long p_lItters, 
																  SVImageOperationTypeEnum p_eOp)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType( p_eOp );
			if( l_lMatroxFlags !=0 )
			{
				MimThin(p_rSource.GetIdentifier(), 
					p_rDest.GetIdentifier(),
					p_lItters,
					l_lMatroxFlags);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Warp

@SVOperationDescription This function warps an image through lookup tables (LUTs). 

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Warp( const SVMatroxBuffer& p_rDest, 
																  const SVMatroxBuffer& p_rSource, 
																  const SVMatroxBuffer& p_rLut1, 
																  const SVMatroxBuffer& p_rLut2, 
																  SVImageOperationTypeEnum p_eInterpMode)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty() && !p_rLut1.empty() && !p_rLut2.empty() )
		{
			long l_lMatroxFlags = Convert2MatroxType( p_eInterpMode );
			if( l_lMatroxFlags !=0 )
			{
				MimWarp(p_rSource.GetIdentifier(), 
					p_rDest.GetIdentifier(),
					p_rLut1.GetIdentifier(),
					p_rLut2.GetIdentifier(),
					M_WARP_LUT | M_FIXED_POINT | 8L,
					l_lMatroxFlags);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Watershed

@SVOperationDescription This function performs a watershed transformation on the specified source buffer.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Watershed( const SVMatroxBuffer& p_rDest, 
																	   const SVMatroxBuffer& p_rSource, 
																	   const SVMatroxBuffer& p_rMarker, 
																	   long p_lMinVariation, 
																	   SVImageWaterShedEnum p_lControlFlag)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rSource.empty() && !p_rDest.empty() )
		{
			long l_lMatroxFlags = Convert2MatroxType( p_lControlFlag );
			if( l_lMatroxFlags !=0 )
			{
				MimWatershed(p_rSource.GetIdentifier(), 
					p_rMarker.GetIdentifier(),
					p_rDest.GetIdentifier(),
					p_lMinVariation,
					l_lMatroxFlags);
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Image Resize

@SVOperationDescription This function performs a scaling transformation on the specified source buffer.

*/
SVMatroxImageInterface::SVStatusCode SVMatroxImageInterface::Resize( const SVMatroxBuffer&		p_rDest,
																	 const SVMatroxBuffer&		p_rSource,
																	 const double				p_dScaleFactorX,
																	 const double				p_dScaleFactorY,
																	 const SVInterpolationModeOptions::SVInterpolationModeOptionsEnum interpolationMode,
																	 const SVOverscanOptions::SVOverscanOptionsEnum	overscan,
																	 const SVPerformanceOptions::SVPerformanceOptionsEnum	performance)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );

#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		while (true)
		{
			if (p_rSource.empty())
			{
				l_Code = SVMSG_SVO_5042_COULDNOTGETSOURCEIMAGE;
				break;
			}

			if (p_rDest.empty())
			{
				l_Code = SVMSG_SVO_5043_COULDNOTGETDESTINATIONIMAGE;
				break;
			}

			long matroxInterpolationMode	= 0;
			long matroxOverscan				= 0;
			long matroxPerformance			= 0;

			l_Code = SVInterpolationModeOptions::m_Convertor.ConvertBitSetToMatroxType(interpolationMode, matroxInterpolationMode);

			if (SVMEE_STATUS_OK != l_Code)
			{
				break;
			}

			l_Code = SVOverscanOptions::m_Convertor.ConvertBitSetToMatroxType(overscan, matroxOverscan);

			if (SVMEE_STATUS_OK != l_Code)
			{
				break;
			}

			switch (performance)
			{
			case	SVPerformanceOptions::PerformanceFast:	
			case	SVPerformanceOptions::PerformancePresice:
				{
					// valid parameter
					l_Code = SVPerformanceOptions::m_Convertor.ConvertBitSetToMatroxType(performance, matroxPerformance);
					break;
				}
			default:
				{
					// invalid parameter
					l_Code = SVMSG_SVO_5046_INVALIDPERFORMANCE;
					break;
				}
			}

			if (SVMEE_STATUS_OK != l_Code)
			{
				break;
			}

			MimResize(	p_rSource.GetIdentifier(), 
						p_rDest.GetIdentifier(),
						p_dScaleFactorX,
						p_dScaleFactorY,
						matroxInterpolationMode | matroxOverscan | matroxPerformance);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			break;
		} // while (true)
	}

#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/* 
From the Journal of graphics, gpu and game tools, issue Volume 12, Number 2, 2007.

Adaptive Thresholding using the Integral Image
Derek Bradley
Carleton University

Gerhard Roth
National Research Council of Canada
*/
void SVMatroxImageInterface::AdaptiveThreshold(unsigned char* input, unsigned char* bin, int width, int height, int stride, float interval)
{
	int s2 = (width / 8) / 2;

	// create the integral image
	unsigned long* integralImg = new unsigned long[(width * height * sizeof(unsigned long *))];

	for (int i = 0;i < width; i++)
	{
		// reset this column sum
		long sum = 0;

		for (int j = 0; j < height; j++)
		{
			int index = (j * width) + i;
			//int index = (j * stride) + i;

			sum += input[index];

			if (i == 0)
			{
				integralImg[index] = sum;
			}
			else
			{
				integralImg[index] = integralImg[index - 1] + sum;
			}
		}
	}

	// perform thresholding
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			int index = j * width + i;

			// set the SxS region
			int x1 = i - s2; 
			int x2 = i + s2;
			int y1 = j - s2; 
			int y2 = j + s2;

			// check the border
			if (x1 < 0) 
			{
				x1 = 0;
			}

			if (x2 >= width) 
			{
				x2 = width - 1;
			}

			if (y1 < 0) 
			{
				y1 = 0;
			}

			if (y2 >= height) 
			{
				y2 = height - 1;
			}
			
			int count = (x2 - x1) * (y2 - y1);

			// I(x,y) = s(x2,y2) - s(x1, y2) - s(x2, y1) + s(x1, x1)
			int sum = integralImg[y2 * width + x2] -
					integralImg[y1 * width + x2] -
					integralImg[y2 * width + x1] +
					integralImg[y1 * width + x1];

			if (static_cast<long>(input[index] * count) < static_cast<long>(sum * (1.0 - interval)))
			{
				bin[index] = 0;
			}
			else
			{
				bin[index] = 255;
			}
		}
	}
	
	delete [] integralImg;
}

SVMatroxFileTypeEnum SVMatroxImageInterface::getFileType( LPCTSTR FileExt )
{
	SVMatroxFileTypeEnum Result( SVFileUnknown );

	SVString strExtension( FileExt );

	if( SvUl_SF::CompareNoCase( strExtension, SVString( _T( ".mim" )) ) == 0 )
	{
		Result = SVFileMIL;
	}
	if( SvUl_SF::CompareNoCase( strExtension, SVString( _T( ".tif" )) ) == 0 )
	{
		Result = SVFileTiff;
	}
	if( SvUl_SF::CompareNoCase( strExtension, SVString( _T( ".bmp" )) ) == 0 )
	{
		Result = SVFileBitmap;
	}

	return Result;
}
