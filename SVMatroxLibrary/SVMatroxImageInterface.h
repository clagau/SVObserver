// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxImageInterface
// * .File Name       : $Workfile:   SVMatroxImageInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:18:30  $
// ******************************************************************************

#ifndef SV_MATROX_IMAGE_INTERFACE_H
#define SV_MATROX_IMAGE_INTERFACE_H

#include <vector>
#include "SVMatroxEnums.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxImageResult.h"
#include "SVMatroxPolarTransformStruct.h"
#include "SVMatroxImageRotateStruct.h"

class SVCommandDataHolder;

/**
@SVObjectName Matrox Image Interface

@SVObjectOverview This class provides an interface for the Matrox image operations.

@SVObjectOperations 
The Create function uses the Matrox MimAllocResult. This function allocates a result buffer with the specified number of entries, for use with the image processing module's statistical functions
The Destroy function uses the Matrox MimFree. This function deallocates a SVMatroxImageResult previously allocated with Create.
The Arithmatic function uses the Matrox MimArith or the MimArithMultiple. This function performs the specified point-to-point operation on two images, storing results in the specified destination image buffer. 
The Binarize function uses the Matrox MimBinarize.  This function performs binary thresholding on the specified image. Each pixel that meets the specified condition is set to the highest unsigned destination buffer value, while other pixels are set to 0.
The Close function uses the Matrox MimClose. This function performs a binary or grayscale closing operation on the given source image for the specified number of iterations. A closing is a dilation followed by an erosion  
The Convert function uses the Matrox MimConvert. This function performs a color conversion on the source image and places the result in the destination buffer. 
The Convolv function uses the Matrox MimConvolv. This function performs a general convolution operation on the source buffer using the specified filter, storing results in the specified destination buffer.  
The Dilate function uses the Matrox MimDilate. This function performs a binary or grayscale dilation on the given source image for the specified number of iterations.
The Erode function uses the Matrox MimErode. This function performs a binary or grayscale erosion on the given source image for the specified number of iterations. 
The Fip function uses the Matrox MimFlip. This function flips the source image to the destination image according to the specified operation.
The Histogram function uses the Matrox MimHistogram. This function calculates the histogram (or pixel intensity distribution) of the specified source buffer and stores results in the specified histogram result buffer.
The LutMap function uses the Matrox MimLutMap. This function maps each pixel in the specified source image to values determined by the specified lookup table (LUT). 
The Open function uses the Matox MimOpen. This function performs a binary or grayscale opening operation on the given source image for the specified number of iterations. An opening is an erosion followed by a dilation.
The PolarTransform uses the Matrox MimPolarTransform. This function performs a rectangular-to-polar or polar-to-rectangular transformation.
The Project uses the Matrox MimProject. This function projects a two-dimensional buffer into a one-dimensional buffer from the specified angle, and writes results to the specified projection result buffer. The results are generated by adding all pixel values along each diagonal in the image at the specified projection angle. The 90� projection of the image is known as the row profile; the 0� projection as the column profile.
The Rank function uses the Matrox MimRank. This function performs a rank filter operation on the specified source buffer. It replaces each pixel with the pixel in its neighborhood whose value is the specified rankth value relative to others. It uses the specified structuring element as a mask. This mask determines the neighborhood size and which pixels in the neighborhood to ignore.
The Rotate function uses the Matrox MimRotate. This function rotates an image by the specified angle of rotation, using the specified interpolation mode. The center of rotation in the source image is determined by the specified X and Ysource rotation-center coordinates. The rotated image will then be clipped to fit the destination buffer. It will be placed in the destination buffer with its center positioned at the specified X and Y destination center coordinates.
The Thick function uses the Matrox MimThick. This function performs a binary or grayscale thickening on the specified source image for the specified number of iterations. MimThick() is optimized for packed binary buffers. 
The Thin function uses the Matrox MimThin. This function performs a binary or grayscale thinning on the specified source image for the specified number of iterations. MimThin() is optimized for packed binary buffers. 
The Warp function uses the Matrox MimWarp. This function warps an image through lookup tables (LUTs). 
The Watershed uses the Matrox MimWatershed. This function performs a watershed transformation on the specified source buffer. See the Matrox Manual for details.
*/
class SVMatroxImageInterface
{
public:
	typedef SVMatroxApplicationInterface::SVStatusCode SVStatusCode;

	SVMatroxImageInterface();

	virtual ~SVMatroxImageInterface();

	// This function replaces MimAllocResult.
	static SVStatusCode Create( SVMatroxImageResult& p_rResultId, const long NbEntries, SVImageOperationTypeEnum ResultType);
	// This function replaces MimFree.
	static SVStatusCode Destroy( SVMatroxImageResult& p_rResultId );
	
	// This function Replaces MimArith and MimArithMultiple( image tool )
	static SVStatusCode Arithmetic( const SVMatroxBuffer& p_rDestId, const SVMatroxBuffer& p_dSource1, const SVMatroxBuffer& p_dSource2, SVImageOperationTypeEnum p_lOperation);
	// This function replaces MimBinarize ( threshold )
	static SVStatusCode Binarize( const SVMatroxBuffer& p_rDestID, const SVMatroxBuffer p_rSource, SVConditionEnum p_eCondition, double p_dCondLow, double p_dCondHigh );
	static HRESULT AdaptiveThreshold( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	static HRESULT AutoThreshold( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	static HRESULT FixedThreshold( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );

	// This function replaces MimClose ( filter)
	static SVStatusCode Close( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp);
	// This function replaces MimConvert ( RGB to HSI)
	static SVStatusCode Convert( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, SVImageOperationTypeEnum p_eOperation);
	// This function replaces MimConvolve ( filter)
	static SVStatusCode Convolve( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, const SVMatroxBuffer& p_Kernel );
	static SVStatusCode Convolve( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, SVFilterOperationEnum p_eFilterOperation );
	// This function replaces MimDilate
	static SVStatusCode Dilate( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp);
	// This function replaces MimErode
	static SVStatusCode Erode( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp);
	// This function replaces MimFlip
	static SVStatusCode Flip( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, SVImageFlipEnum p_lOperation);

	// This function replaces MimGetResult
	static SVStatusCode GetResult( const SVMatroxImageResult& p_rResultID, SVMatroxDoubleArray& p_adArray);
	static SVStatusCode GetResult( const SVMatroxImageResult& p_rResultID, SVMatroxLongArray& p_alArray);
	static SVStatusCode GetResult( const SVMatroxImageResult& p_rResultID, SVMatroxByteArray& p_acArray);
	static SVStatusCode GetResult( const SVMatroxImageResult& p_rResultID, void * p_pArray);

	// This function replaces MimHistogram
	static SVStatusCode Histogram( const SVMatroxImageResult& p_rHistResult, const SVMatroxBuffer& p_rSource);
	// This function replaces MimLutMap
	static SVStatusCode LutMap( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, const SVMatroxBuffer& p_rLut);
	// This function replaces MimOpen
	static SVStatusCode Open( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp);
	// This function replaces MimPolarTransform
	static SVStatusCode PolarTransform( const SVMatroxBuffer& p_rDest, SVMatroxPolarTransformStruct& p_rTransformStruct);
	// This function replaces MimProject (Linear tool)
	static SVStatusCode Project( const SVMatroxImageResult& p_rProjDest, const SVMatroxBuffer& p_rSource, double p_dAngle);
	// This function replaces MimRank
	static SVStatusCode Rank( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, const SVMatroxBuffer& p_rStructElem, long p_lRank);
	// This function replaces MimRotate
	static SVStatusCode Rotate( const SVMatroxBuffer& p_rProjDest, const SVMatroxImageRotateStruct& p_RotateStruct );
	// This function replaces MimThick (filter)
	static SVStatusCode Thick( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp);
	// This function replaces MimThin (filter)
	static SVStatusCode Thin( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, long p_lItters, SVImageOperationTypeEnum p_eOp);
	// This function replaces MimWarp (Warp Tools)
	static SVStatusCode Warp( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, const SVMatroxBuffer& p_rLut1, const SVMatroxBuffer& p_rLut2, SVImageOperationTypeEnum p_eInterpMode);
	// This function replaces MimWatershed
	static SVStatusCode Watershed( const SVMatroxBuffer& p_rDest, const SVMatroxBuffer& p_rSource, const SVMatroxBuffer& p_rMarker, long p_lMinVariation, SVImageWaterShedEnum p_lControlFlag);
	// This function replaces MimResize
	// It is assumed that parameters are verified before entering Resize.  They are not checked again inside Resize.
	static SVStatusCode Resize( const SVMatroxBuffer& p_rDest, 
								const SVMatroxBuffer& p_rSource, 
								const double p_dScaleFactorX, 
								const double p_dScaleFactorY, 
								const SVInterpolationModeOptions::SVInterpolationModeOptionsEnum interpolationMode,
								const SVOverscanOptions::SVOverscanOptionsEnum overscan,
								const SVPerformanceOptions::SVPerformanceOptionsEnum performance);
		
private:
	static long Convert2MatroxType( SVImageOperationTypeEnum p_eDisp) ;
	static long Convert2MatroxType( SVImageWaterShedEnum p_eType) ;
	static long Convert2MatroxType( SVFilterOperationEnum p_eType) ;
	static long Convert2MatroxType(SVConditionEnum p_eControlType) ;

	static void AdaptiveThreshold( unsigned char* input, unsigned char* bin, int width, int height, int stride, float interval );

};

#endif // SV_MATROX_IMAGE_INTERFACE_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxImageInterface.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:18:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Oct 2010 15:00:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include new command functionality using the matrox library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 May 2010 14:01:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with includes and make them consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jun 2009 15:50:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Resize method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jul 2008 11:55:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  631
 * SCR Title:  Upgrade SVObserver to 5.01
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed the second parameter to a reference rather than pass by value for the PolarTransform function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:20:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
