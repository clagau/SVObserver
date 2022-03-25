// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxImageInterface
// * .File Name       : $Workfile:   SVMatroxImageInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   23 Oct 2013 08:50:50  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMatroxImageInterface.h"
#include "SVMatroxEnums.h"

#include "SVMatroxApplicationInterface.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxBufferInterface.h"
#include "SVMatroxErrorEnum.h"
#include "SVMatroxImageRotateStruct.h"
#include "SVMatroxPolarTransformStruct.h"
#include "SVMatroxResourceMonitor.h"
#include "SVCommandLibrary/SVCommandDataHolder.h"
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "LookUp.h"
#pragma endregion Includes

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

long SVMatroxImageInterface::Convert2MatroxType(SVImageOperationTypeEnum p_eType)
{
	long l_lMatroxType = 0;
	switch (p_eType &~SVSaturation)
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

	switch (p_eType)
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
	if ((p_eType & SVImageFloatType) == SVImageFloatType)
	{
		l_lMatroxType |= M_FLOAT;
	}

	if ((p_eType & SVSaturation) == SVSaturation)
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
	switch (p_eControlType)
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
long SVMatroxImageInterface::Convert2MatroxType(SVFilterOperationEnum p_eType)
{
	long l_lMatroxType = 0;
	switch (p_eType)
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
long SVMatroxImageInterface::Convert2MatroxType(SVImageWaterShedEnum p_eType)
{
	long l_lMatroxType = 0;
	if ((p_eType & SVImageWSBasin) == SVImageWSBasin)
	{
		l_lMatroxType |= M_BASIN;
	}
	else
		if ((p_eType & SVImageWSWatershed) == SVImageWSWatershed)
		{
			l_lMatroxType |= M_WATERSHED;
		}

	if ((p_eType & SVImageWSMaximaFill) == SVImageWSMaximaFill)
	{
		l_lMatroxType |= M_MAXIMA_FILL;
	}
	else
		if ((p_eType & SVImageWSMinimaFill) == SVImageWSMinimaFill)
		{
			l_lMatroxType |= M_MINIMA_FILL;
		}

	if ((p_eType & SVImageWSRegular) == SVImageWSRegular)
	{
		l_lMatroxType |= M_REGULAR;
	}
	else
		if ((p_eType & SVImageWSStraight) == SVImageWSStraight)
		{
			l_lMatroxType |= M_STRAIGHT_WATERSHED;
		}

	if ((p_eType & SVImage4Connected) == SVImage4Connected)
	{
		l_lMatroxType |= M_4_CONNECTED;
	}
	else
		if ((p_eType & SVImage8Connected) == SVImage8Connected)
		{
			l_lMatroxType |= M_8_CONNECTED;
		}

	if ((p_eType & SVImageSkipLastLevel) == SVImageSkipLastLevel)
	{
		l_lMatroxType |= M_SKIP_LAST_LEVEL;
	}


	if ((p_eType & SVImageFillSource) == SVImageFillSource)
	{
		l_lMatroxType |= M_FILL_SOURCE;
	}
	return l_lMatroxType;
}


/**
@SVOperationName Create

@SVOperationDescription This function creates a Result object and return the of it SVMatroxIdentifier.

*/
HRESULT SVMatroxImageInterface::Create(__int64& rResultId, const long p_lNbEntries, SVImageOperationTypeEnum p_eResultType)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lResultType = Convert2MatroxType(p_eResultType);




		MIL_ID l_NewId = MimAllocResult(M_DEFAULT_HOST,
			p_lNbEntries,
			l_lResultType,
			M_NULL);


		l_Code = SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
		{
			SVMatroxResourceMonitor::InsertIdentifier(SVImageID, l_NewId);
			l_Code = Destroy(rResultId);
			rResultId = l_NewId;
		}


	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Destroy

@SVOperationDescription This function destroys a Result object.

*/
HRESULT SVMatroxImageInterface::Destroy(__int64& rResultId)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (M_NULL != rResultId)
		{

			MimFree(rResultId);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if (l_Code == S_OK)
			{
				SVMatroxResourceMonitor::EraseIdentifier(SVImageID, rResultId);

				rResultId = M_NULL;
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
	assert(l_Code == S_OK);
	return l_Code;
}

HRESULT SVMatroxImageInterface::ArithmeticGainOffset(const SVMatroxBuffer& p_rDestId, double gain, double offset)
{

	if (p_rDestId.empty())
	{
		return SVMEE_INVALID_HANDLE;
	}
	if (gain != 1.0)
	{
		if (offset == 0.0)
		{
			MimArith(p_rDestId.GetIdentifier(), gain, p_rDestId.GetIdentifier(), M_MULT_CONST + M_SATURATION + M_FLOAT_PROC);
		}
		else
		{
			MimArith(p_rDestId.GetIdentifier(), gain, p_rDestId.GetIdentifier(), M_MULT_CONST + M_FLOAT_PROC + M_SATURATION);
			MimArith(p_rDestId.GetIdentifier(), offset, p_rDestId.GetIdentifier(), M_ADD_CONST + M_FLOAT_PROC + M_SATURATION);
		}
	}
	else if (offset != 0.0)
	{
		MimArith(p_rDestId.GetIdentifier(), offset, p_rDestId.GetIdentifier(), M_ADD_CONST + M_SATURATION + M_FLOAT_PROC);
	}

	HRESULT l_Code = SVMatroxApplicationInterface::GetLastStatus();
	assert(l_Code == S_OK);
	return l_Code;
}


HRESULT SVMatroxImageInterface::ArithmeticGainOffsetClip(const SVMatroxBuffer& p_rDestId, const SVMatroxBuffer& p_dSource, double gain, double offset)
{

	if (p_rDestId.empty())
	{
		return SVMEE_INVALID_HANDLE;
	}

	HRESULT hres = ArithmeticGainOffset(p_dSource, gain, offset);
	if (hres == S_OK)
	{
		MimClip(p_dSource.GetIdentifier(), p_rDestId.GetIdentifier(), M_SATURATION, M_NULL, M_NULL, M_NULL, M_NULL);
		hres = SVMatroxApplicationInterface::GetLastStatus();
	}
	
	return hres;
}

/**
@SVOperationName Image Arithmatic

@SVOperationDescription This function performs the specified point-to-point operation on two images, an image and a constant, an image, or a constant, storing results in the specified destination image buffer.

*/
HRESULT SVMatroxImageInterface::Arithmetic(const SVMatroxBuffer& p_rDestId,
	const SVMatroxBuffer& p_dSource1,
	const SVMatroxBuffer& p_dSource2,
	SVImageOperationTypeEnum p_eOperation)
{
	// This function Replaces MimArith ( image tool )
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rDestId.empty() && !p_dSource1.empty() && !p_dSource2.empty())
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROX_FUNCTION)		
			TCHAR buf[1024];
			_stprintf_s(buf, 1024, _T("DestId %i  Source1Id %i Source2Id %i \n"), int(p_rDestId.GetIdentifier()), int(p_dSource1.GetIdentifier()), int(p_dSource2.GetIdentifier()));
			::OutputDebugString(buf);
#endif
			long l_lMatroxFlags = Convert2MatroxType(p_eOperation);
			if (l_lMatroxFlags != 0)
			{
				if (p_eOperation == SVImageMultipleAccumulate)
				{
					MimArithMultiple(p_dSource1.GetIdentifier(),	// Source1
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
					if (l_lMatroxFlags != M_PASS)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

HRESULT SVMatroxImageInterface::ArithmeticLut(const SVMatroxBuffer& rDestId, const SVMatroxBuffer& rSource1, const SVMatroxBuffer& rSource2, const std::unique_ptr<LookUp>& lookupPtr)
{

	if (rDestId.empty() || rSource1.empty() || rSource2.empty() || !lookupPtr)
	{
		return SVMEE_INVALID_HANDLE;
	}
	
	MIL_INT Address_1 = MbufInquire(rSource1.GetIdentifier(), M_HOST_ADDRESS, M_NULL);
	MIL_INT Pitch_1 = MbufInquire(rSource1.GetIdentifier(), M_PITCH, M_NULL);
	MIL_INT Height_1 = MbufInquire(rSource1.GetIdentifier(), M_SIZE_Y, M_NULL);
	MIL_INT Width_1 = MbufInquire(rSource1.GetIdentifier(), M_SIZE_X, M_NULL);

	MIL_INT Adresss_2 = MbufInquire(rSource2.GetIdentifier(), M_HOST_ADDRESS, M_NULL);
	MIL_INT Pitch_2 = MbufInquire(rSource2.GetIdentifier(), M_PITCH, M_NULL);
	MIL_INT Height_2 = MbufInquire(rSource2.GetIdentifier(), M_SIZE_Y, M_NULL);
	MIL_INT Width_2 = MbufInquire(rSource2.GetIdentifier(), M_SIZE_X, M_NULL);

	MIL_INT Dest = MbufInquire(rDestId.GetIdentifier(), M_HOST_ADDRESS, M_NULL);
	MIL_INT PitchDest = MbufInquire(rDestId.GetIdentifier(), M_PITCH, M_NULL);
	MIL_INT Height_Dest = MbufInquire(rDestId.GetIdentifier(), M_SIZE_Y, M_NULL);
	MIL_INT Width_Dest = MbufInquire(rDestId.GetIdentifier(), M_SIZE_X, M_NULL);

	

	auto Width = std::min(Width_1, Width_2);
	Width = std::min(Width, Width_Dest);

	auto Height = std::min(Height_1, Height_2);
	Height = std::min(Height, Height_Dest);
	
	//use reference because of performance 
	const BYTE* const &rData = lookupPtr->getDataRef();
	int depth = lookupPtr->getDepth();
	
	for (int y = 0; y < Height; y++)
	{
		BYTE* lineptr_1 = &((BYTE*)Address_1)[Pitch_1 * (y)];
		BYTE*  lineptr_2 = &((BYTE*)Adresss_2)[Pitch_2 * (y)];
		BYTE* lineptr_Dest = &((BYTE*)Dest)[PitchDest * (y)];
		for (int x = 0; x < Width; x++)
		{
			//*lineptrDest = lookupPtr->get(*lineptr_B1,*lineptr_B2);
			*lineptr_Dest = rData[depth * (*lineptr_2) + (*lineptr_1)];
			lineptr_Dest++;
			lineptr_1++;
			lineptr_2++;
		}
	}
	return S_OK;
}
/**
@SVOperationName Image Binarize

@SVOperationDescription This function is used to threshold an image.

*/
HRESULT SVMatroxImageInterface::Binarize(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	SVConditionEnum p_eCondition,
	double p_dCondLow,
	double p_dCondHigh)
{
	HRESULT l_Code(S_OK);
	SVMatroxStatusInformation	l_StatusInfo;

#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType(p_eCondition);
			if (l_lMatroxFlags != 0)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		//		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	//-	JAB110708
	if (l_Code != S_OK)
	{
		l_StatusInfo.m_StatusCode = l_Code;
		l_StatusInfo.m_StatusString = "SVMatroxImageInterface::Binarize error";
		SVMatroxApplicationInterface::Log(l_StatusInfo);
		assert(0);
	}

	return l_Code;
}

/**
@SVOperationName Image Close

@SVOperationDescription This function performs a binary or grayscale closing operation on the given source image for the specified number of iterations.

*/
HRESULT SVMatroxImageInterface::Close(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	long p_lItters,
	SVImageOperationTypeEnum p_eOp)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType(p_eOp);
			if (l_lMatroxFlags != 0)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Convert

@SVOperationDescription This function is used to convert image types.

*/
HRESULT SVMatroxImageInterface::Convert(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	SVImageOperationTypeEnum p_eOperation)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = 0;
			if (p_eOperation == SVImageRGBToHLS)
			{
				l_lMatroxFlags = M_RGB_TO_HLS;
			}
			else
				if (p_eOperation == SVImageHLSToRGB)
				{
					l_lMatroxFlags = M_HLS_TO_RGB;
				}
				else if (p_eOperation == SVImageRGBToL)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}



/**
@SVOperationName Image Convolve - Kernel

@SVOperationDescription This function performs a general convolution operation on the source buffer using the supplied kernel.

*/
HRESULT SVMatroxImageInterface::Convolve(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	const SVMatroxBuffer& p_Kernel)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty() && !p_Kernel.empty())
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Convolve - Pre-defined Filter

@SVOperationDescription This function performs a general convolution operation on the source buffer using the specified filter.

*/
HRESULT SVMatroxImageInterface::Convolve(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, SVFilterOperationEnum p_eFilterOperation)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxFlags = Convert2MatroxType(p_eFilterOperation);

		if (!p_rSource.empty() && !p_rDest.empty())
		{
			MimConvolve(p_rSource.GetIdentifier(),
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Dilate

@SVOperationDescription This function performs a binary or grayscale dilation on the given source image for the specified number of iterations.

*/
HRESULT SVMatroxImageInterface::Dilate(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	long p_lItters,
	SVImageOperationTypeEnum p_eOp)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType(p_eOp);
			if (l_lMatroxFlags != 0)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Erode

@SVOperationDescription This function performs a binary or grayscale erosion on the given source image for the specified number of iterations.

*/
HRESULT SVMatroxImageInterface::Erode(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	long p_lItters,
	SVImageOperationTypeEnum p_eOp)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType(p_eOp);
			if (l_lMatroxFlags != 0)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Flip

@SVOperationDescription This function flips the source image to the destination image according to the specified operation.

*/
HRESULT SVMatroxImageInterface::Flip(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	SVImageFlipEnum p_eOperation)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = M_FLIP_HORIZONTAL;

			if (p_eOperation == SVImageFlipHorizontal)
			{
				l_lMatroxFlags = M_FLIP_HORIZONTAL;
			}
			else
				if (p_eOperation == SVImageFlipVertical)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}


HRESULT SVMatroxImageInterface::GetResult(const __int64& rResultID, std::vector<double>& p_adArray)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		MIL_INT l_lSize = 0;
		if (M_NULL != rResultID)
		{
			MimInquire(rResultID, M_RESULT_SIZE, &l_lSize);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if (l_Code == S_OK && l_lSize > 0)
			{
				p_adArray.resize(l_lSize);
				MimGetResult(rResultID, M_VALUE | M_TYPE_DOUBLE, &p_adArray[0]);
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
	assert(l_Code == S_OK);
	return l_Code;
}

HRESULT SVMatroxImageInterface::GetResult(const __int64& rResultID, std::vector<long>& p_alArray)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		MIL_INT l_lSize = 0;
		if (M_NULL != rResultID)
		{
			MimInquire(rResultID, M_RESULT_SIZE, &l_lSize);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if (l_Code == S_OK && l_lSize > 0)
			{
				p_alArray.resize(l_lSize);
				MimGetResult(rResultID, M_VALUE | M_TYPE_LONG, &p_alArray[0]);
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
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Histogram

@SVOperationDescription This function calculates the histogram (or pixel intensity distribution) of the specified source buffer and stores results in the specified histogram result buffer.

*/
HRESULT SVMatroxImageInterface::Histogram(const __int64& rHistResult, const SVMatroxBuffer& p_rSource)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && M_NULL != rHistResult)
		{
			MimHistogram(p_rSource.GetIdentifier(), rHistResult);

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
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Lut Map

@SVOperationDescription This function maps each pixel in the specified source image to values determined by the specified lookup table (LUT).

*/
HRESULT SVMatroxImageInterface::LutMap(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	const SVMatroxBuffer& p_rLut)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty() && !p_rLut.empty())
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Open

@SVOperationDescription This function performs a binary or grayscale opening operation on the given source image for the specified number of iterations.

*/
HRESULT SVMatroxImageInterface::Open(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType(p_eOp);
			if (l_lMatroxFlags != 0)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Polar Transform

@SVOperationDescription This function performs a rectangular-to-polar or polar-to-rectangular transformation.

*/
HRESULT SVMatroxImageInterface::PolarTransform(const SVMatroxBuffer& p_rDest,
	SVMatroxPolarTransformStruct& p_rTransformStruct)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rDest.empty() && !p_rTransformStruct.m_rSource.empty())
		{
			long l_lMatroxInterpMode = Convert2MatroxType(p_rTransformStruct.m_eInterpMode);
			long l_lMatroxOperationMode = M_RECTANGULAR_TO_POLAR;

			if (p_rTransformStruct.m_eOpMode == SVRectToPolar)
			{
				l_lMatroxOperationMode = M_RECTANGULAR_TO_POLAR;
			}
			else
			{
				l_lMatroxOperationMode = M_POLAR_TO_RECTANGULAR;
			}

			if (l_lMatroxInterpMode != 0 && l_lMatroxOperationMode != 0)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Project

@SVOperationDescription This function projects a two-dimensional buffer into a one-dimensional buffer from the specified angle, and writes results to the specified projection result buffer.

*/
HRESULT SVMatroxImageInterface::Project(const __int64& rResultId,
	const SVMatroxBuffer& p_rSource,
	double p_dAngle)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && M_NULL != rResultId)
		{
			MimProject(p_rSource.GetIdentifier(), rResultId, p_dAngle);
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
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Rank

@SVOperationDescription This function performs a rank filter operation on the specified source buffer.

*/
HRESULT SVMatroxImageInterface::Rank(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	const SVMatroxBuffer& p_rStructElem,
	long p_lRank)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty() && !p_rStructElem.empty())
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Rotate

@SVOperationDescription This function rotates an image by the specified angle of rotation, using the specified interpolation mode.

*/
HRESULT SVMatroxImageInterface::Rotate(const SVMatroxBuffer& p_rProjDest,
	const SVMatroxImageRotateStruct& p_RotateStruct)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rProjDest.empty() && !p_RotateStruct.m_rSource.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType(p_RotateStruct.m_eInterpolation);
			if (l_lMatroxFlags != 0)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}


/**
@SVOperationName Image Thick

@SVOperationDescription This function performs a binary or grayscale thickening on the specified source image for the specified number of iterations.

*/
HRESULT SVMatroxImageInterface::Thick(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	long p_lItters,
	SVImageOperationTypeEnum p_eOp)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType(p_eOp);
			if (l_lMatroxFlags != 0)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Thin

@SVOperationDescription This function performs a binary or grayscale thinning on the specified source image for the specified number of iterations.

*/
HRESULT SVMatroxImageInterface::Thin(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	long p_lItters,
	SVImageOperationTypeEnum p_eOp)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType(p_eOp);
			if (l_lMatroxFlags != 0)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Warp

@SVOperationDescription This function warps an image through lookup tables (LUTs).

*/
HRESULT SVMatroxImageInterface::Warp(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	const SVMatroxBuffer& p_rLut1,
	const SVMatroxBuffer& p_rLut2,
	SVImageOperationTypeEnum p_eInterpMode)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty() && !p_rLut1.empty() && !p_rLut2.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType(p_eInterpMode);
			if (l_lMatroxFlags != 0)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Watershed

@SVOperationDescription This function performs a watershed transformation on the specified source buffer.

*/
HRESULT SVMatroxImageInterface::Watershed(const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	const SVMatroxBuffer& p_rMarker,
	long p_lMinVariation,
	SVImageWaterShedEnum p_lControlFlag)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rSource.empty() && !p_rDest.empty())
		{
			long l_lMatroxFlags = Convert2MatroxType(p_lControlFlag);
			if (l_lMatroxFlags != 0)
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
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Image Resize

@SVOperationDescription This function performs a scaling transformation on the specified source buffer.

*/
HRESULT SVMatroxImageInterface::Resize(
	const SVMatroxBuffer& p_rDest,
	const SVMatroxBuffer& p_rSource,
	const double				p_dScaleFactorX,
	const double				p_dScaleFactorY,
	long						interpolationMode,
	long						overscanMode)

{
	HRESULT l_Code(S_OK);

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

			MimResize(p_rSource.GetIdentifier(),
				p_rDest.GetIdentifier(),
				p_dScaleFactorX,
				p_dScaleFactorY,
				static_cast<MIL_INT64> (interpolationMode | overscanMode));

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			break;
		} // while (true)
}

#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

SVMatroxFileTypeEnum SVMatroxImageInterface::getFileType(LPCTSTR FileExt)
{
	SVMatroxFileTypeEnum Result(SVFileUnknown);

	std::string strExtension(FileExt);

	if (SvUl::CompareNoCase(strExtension, std::string(_T(".mim"))) == 0)
	{
		Result = SVFileMIL;
	}
	if (SvUl::CompareNoCase(strExtension, std::string(_T(".tif"))) == 0)
	{
		Result = SVFileTiff;
	}
	if (SvUl::CompareNoCase(strExtension, std::string(_T(".bmp"))) == 0)
	{
		Result = SVFileBitmap;
	}

	return Result;
}

bool SVMatroxImageInterface::isFloatBufferAllowed(long operation)
{
	switch (operation & ~SVSaturation)
	{
		case    SVImageAdd: // M_ADD 
		case	SVImageDiv:  // M_DIV 
		case	SVImageMult:  // M_MULT
		case	SVImageSub:  // M_SUB
		case	SVImageSubAbs: // M_SUB_ABS 
			return true;
			break;
		default:
			return false;
	}

}