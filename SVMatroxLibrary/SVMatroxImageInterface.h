// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxImageInterface
// * .File Name       : $Workfile:   SVMatroxImageInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:18:30  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "Definitions\SVMatroxSimpleEnums.h"
#pragma endregion Includes
#include "LookUp.h"

class SVCommandDataHolder;
struct SVMatroxImageRotateStruct;
struct SVMatroxPolarTransformStruct;
class SVMatroxBuffer;


class SVMatroxImageInterface
{
public:
	SVMatroxImageInterface();

	virtual ~SVMatroxImageInterface();

	// This function replaces MimAllocResult.
	//The Create function uses the Matrox MimAllocResult. 
	//This function allocates a result buffer with the specified number of entries, for use with the image processing module's statistical functions
	static HRESULT Create(__int64& rResultId, const long NbEntries, SVImageOperationTypeEnum ResultType);

	// This function replaces MimFree.
	//The Destroy function uses the Matrox MimFree.This function deallocates a SVMatroxIdentifier previously allocated with Create.
	static HRESULT Destroy(__int64& rResultId);

	//The Arithmatic function uses the Matrox MimArith or the MimArithMultiple. This function performs the specified point-to-point operation on two images, storing results in the specified destination image buffer. 
	// This function Replaces MimArith and MimArithMultiple( image tool )
	static HRESULT Arithmetic(const SVMatroxBuffer& p_rDestId, const SVMatroxBuffer& p_dSource1, const SVMatroxBuffer& p_dSource2, SVImageOperationTypeEnum p_lOperation);

	// This function Replaces MimArith and MimArithMultiple( image tool )
	static HRESULT ArithmeticLut(const SVMatroxBuffer& p_rDestId, const SVMatroxBuffer& p_dSource1, const SVMatroxBuffer& p_dSource2, const std::unique_ptr<LookUp>& lookupPtr);

	//ArithmeticGainOffset + clip  clip is useful when destination buffer and source buffer have different type or depth 
	static HRESULT  ArithmeticGainOffsetClip(const SVMatroxBuffer& p_rDestId, const SVMatroxBuffer& p_dSource, double gain, double offset);

	//buffer[i][j] =  buffer[i][j] *gain +offset  
	static HRESULT  ArithmeticGainOffset(const SVMatroxBuffer& p_dSource, double gain, double offset);

	// This function replaces MimBinarize ( threshold )
	//The Binarize function uses the Matrox MimBinarize.  This function performs binary thresholding on the specified image. 
	//Each pixel that meets the specified condition is set to the highest unsigned destination buffer value, while other pixels are set to 0.
	static HRESULT Binarize(const SVMatroxBuffer& p_rDestID, const SVMatroxBuffer& p_rSource, SVConditionEnum p_eCondition, double p_dCondLow, double p_dCondHigh);

	// This function replaces MimClose ( filter)
	//The Close function uses the Matrox MimClose.This function performs a binary or grayscale closing operation on the given source image for the specified number of iterations.
	//A closing is a dilation followed by an erosion
	static HRESULT Close(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp);

	// This function replaces MimConvert ( RGB to HSI)
	//The Convert function uses the Matrox MimConvert. This function performs a color conversion on the source image and places the result in the destination buffer. 
	static HRESULT Convert(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, SVImageOperationTypeEnum p_eOperation);

	// This function replaces MimConvolve ( filter)
	//The Convolv function uses the Matrox MimConvolv. This function performs a general convolution operation on the source buffer using the specified filter, storing results in the specified destination buffer.  
	static HRESULT Convolve(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, const SVMatroxBuffer& p_Kernel);
	static HRESULT Convolve(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, SVFilterOperationEnum p_eFilterOperation);

	// This function replaces MimDilate
	//The Dilate function uses the Matrox MimDilate. This function performs a binary or grayscale dilation on the given source image for the specified number of iterations.
	static HRESULT Dilate(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp);

	// This function replaces MimErode
	//The Erode function uses the Matrox MimErode. This function performs a binary or grayscale erosion on the given source image for the specified number of iterations. 
	static HRESULT Erode(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp);

	// This function replaces MimFlip
	//The Flip function uses the Matrox MimFlip. This function flips the source image to the destination image according to the specified operation.
	static HRESULT Flip(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, SVImageFlipEnum p_lOperation);

	// This function replaces MimGetResult
	static HRESULT GetResult(const __int64& rResultId, std::vector<double>& p_adArray);
	static HRESULT GetResult(const __int64& rResultId, std::vector<long>& p_alArray);

	// This function replaces MimHistogram
	//The Histogram function uses the Matrox MimHistogram. This function calculates the histogram (or pixel intensity distribution) of the specified source buffer and stores results in the specified histogram result buffer.
	static HRESULT Histogram(const __int64& rResultId, const SVMatroxBuffer& p_rSource);

	// This function replaces MimLutMap
	//The LutMap function uses the Matrox MimLutMap. This function maps each pixel in the specified source image to values determined by the specified lookup table (LUT). 
	static HRESULT LutMap(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, const SVMatroxBuffer& p_rLut);

	// This function replaces MimOpen
	//The Open function uses the Matox MimOpen. This function performs a binary or grayscale opening operation on the given source image for the specified number of iterations. An opening is an erosion followed by a dilation.
	static HRESULT Open(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp);

	// This function replaces MimPolarTransform
	//The PolarTransform uses the Matrox MimPolarTransform.This function performs a rectangular - to - polar or polar - to - rectangular transformation.
	static HRESULT PolarTransform(const SVMatroxBuffer& p_rDest, SVMatroxPolarTransformStruct& p_rTransformStruct);

	// This function replaces MimProject (Linear tool)
	//The Project uses the Matrox MimProject.This function projects a two - dimensional buffer into a one - dimensional buffer from the specified angle, and writes results to the specified projection result buffer.The results are generated by adding all pixel values along each diagonal in the image at the specified projection angle.The 90° projection of the image is known as the row profile; the 0° projection as the column profile.
	static HRESULT Project(const __int64& rResultId, const SVMatroxBuffer& p_rSource, double p_dAngle);

	// This function replaces MimRank
	//	The Rank function uses the Matrox MimRank.This function performs a rank filter operation on the specified source buffer.It replaces each pixel with the pixel in its neighborhood whose value is the specified rankth value relative to others.It uses the specified structuring element as a mask.This mask determines the neighborhood sizeand which pixels in the neighborhood to ignore.
	static HRESULT Rank(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, const SVMatroxBuffer& p_rStructElem, long p_lRank);

	// This function replaces MimRotate
	//The Rotate function uses the Matrox MimRotate.This function rotates an image by the specified angle of rotation, using the specified interpolation mode.The center of rotation in the source image is determined by the specified Xand Ysource rotation - center coordinates.The rotated image will then be clipped to fit the destination buffer.It will be placed in the destination buffer with its center positioned at the specified X and Y destination center coordinates.
	static HRESULT Rotate(const SVMatroxBuffer& p_rProjDest, const SVMatroxImageRotateStruct& p_RotateStruct);

	// This function replaces MimThick (filter)
	//The Thick function uses the Matrox MimThick.This function performs a binary or grayscale thickening on the specified source image for the specified number of iterations.MimThick() is optimized for packed binary buffers.
	static HRESULT Thick(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp);

	// This function replaces MimThin (filter)
	//The Thin function uses the Matrox MimThin.This function performs a binary or grayscale thinning on the specified source image for the specified number of iterations.MimThin() is optimized for packed binary buffers.
	static HRESULT Thin(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp);

	// This function replaces MimWarp (Warp Tools)
	//The Warp function uses the Matrox MimWarp.This function warps an image through lookup tables(LUTs).
	static HRESULT Warp(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, const SVMatroxBuffer& p_rLut1, const SVMatroxBuffer& p_rLut2, SVImageOperationTypeEnum p_eInterpMode);

	// This function replaces MimWatershed
	//The Watershed uses the Matrox MimWatershed.This function performs a watershed transformation on the specified source buffer.See the Matrox Manual for details.
	static HRESULT Watershed(const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, const SVMatroxBuffer& p_rMarker, long p_lMinVariation, SVImageWaterShedEnum p_lControlFlag);

	// This function replaces MimResize
	// It is assumed that parameters are verified before entering Resize.  They are not checked again inside Resize.
	static HRESULT Resize(const SVMatroxBuffer& p_rDest,
								const SVMatroxBuffer& p_rSource,
								const double p_dScaleFactorX,
								const double p_dScaleFactorY,
								long interpolationMode,
								long overscanMode);

	static bool isFloatBufferAllowed(long  op);
private:
	static long Convert2MatroxType(SVImageOperationTypeEnum p_eDisp);
	static long Convert2MatroxType(SVImageWaterShedEnum p_eType);
	static long Convert2MatroxType(SVFilterOperationEnum p_eType);
	static long Convert2MatroxType(SVConditionEnum p_eControlType);
};

