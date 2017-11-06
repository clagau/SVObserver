//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file GlobalConst.h
//*****************************************************************************
/// Global constants used in interfaces
//******************************************************************************
#pragma once

#pragma region Includes
#include "Windef.h"
#pragma endregion Includes

namespace SvDef
{
#pragma region Declarations
	const int			cRingBufferDepthMin = 2;
	const int			cRingBufferDepthMax = 30;
	const int			cRingBufferNumberOutputImages = 2;
	const int			cTableMaxRowMin = 1;
	const int			cTableMaxRowMax = 10000;

	const long			cDefaultToolUpperThreshold = 255L;
	const long			cDefaultToolLowerThreshold = 40L;
	const bool			cDefaultToolThresholdActivate = true;
	const bool			cDefaultToolUpperThresholdActivate = true;
	const bool			cDefaultToolLowerThresholdActivate = true;			 

	// Default view color settings...
	const COLORREF		cDefaultImageViewBackgroundColor = RGB( 250, 255, 250 );

	// Default image settings...
	const long			cDefaultSourceImageDepth = 15L;
	const long			cDefaultResultImageDepth = 5L;
	const long			cDefaultCameraImageWidth = 640L;
	const long			cDefaultCameraImageHeight = 480L;
	const long			cDefaultImageViewWidth = cDefaultCameraImageWidth;
	const long			cDefaultImageViewHeight = cDefaultCameraImageHeight;
	const long			cDefaultToolsetViewWidth = 200L;
	const long			cDefaultToolsetViewHeight = 400L;
	const long			cDefaultResultViewWidth = 840L;
	const long			cDefaultResultViewHeight = 200L;

	// Default tool extents
	const double		cDefaultWindowToolLeft = 10.0;
	const double		cDefaultWindowToolTop = 10.0;
	const double		cDefaultWindowToolWidth = 100.0;
	const double		cDefaultWindowToolHeight  = 100.0;
	const double		cDefaultWindowToolWidthScaleFactor = 1.0;
	const double		cDefaultWindowToolHeightScaleFactor = 1.0;

	const long			cDefaultPolarTransformCenterX = 200L;
	const long			cDefaultPolarTransformCenterY =  50L;
	const long			cDefaultPolarTransformStartRadius = 200L;
	const long			cDefaultPolarTransformEndRadius = 100L;
	const long			cDefaultPolarTransformStartAngle = 210L;
	const long			cDefaultPolarTransformEndAngle = 330L;

	const int			cMaximumCameras = 4;
	const int			cMinPatternModelNewSize = 8;
	const int			cMinPatternModelLoadSize = 4;
#pragma endregion Declarations
} //namespace SvOi