//******************************************************************************
/// COPYRIGHT (c) 2019 by Seidenader Maschinenbau GmbH
/// \file BandEnums.h
/// All Rights Reserved
//******************************************************************************
/// Bands for color threshold RGB or hue/saturation/luminance 
//******************************************************************************

#pragma once

namespace SvDef
{
//! From the Matrox Imaging Library Help file for MbufChildColor2d, the Band parameter specifies the index of the band to use. 
//! Valid index values are from 0 to (number of bands of the buffer - 1). 
//! Band 0 corresponds to: the red band (for RGB parent buffers), the hue band (for HSI parent buffers) 
//! Band 1 corresponds to: the green band (for RGB parent buffers), the saturation band (for HSI parent buffers)
//! Band 2 corresponds to: the blue band (for RGB parent buffers), the luminance band (for HSI parent buffers)
enum BandEnum
{
	Band0 = 0,				//! Red or Hue band
	Band1 = 1,				//! Green or Saturation Band 
	Band2 = 2,				//! Blue or Luminance Band
	BandNumber = 3			//! The number of bands
};

constexpr BandEnum BandList[BandEnum::BandNumber] = { BandEnum::Band0, BandEnum::Band1, BandEnum::Band2 };
}