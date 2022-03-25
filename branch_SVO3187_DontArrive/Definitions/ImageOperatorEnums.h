//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Arthimetic Image Operators that are not direct MIL defines.
//******************************************************************************

#pragma once

namespace SvDef
{
	enum ImageOperatorEnum
	{
		// Support Average Of Two Images - SVO Image Arithmetic Operation 
		// NOTE: Not defined by MIL and should not interfere with 
		// any MIL's M_... define !!! ( check MIL.h )
		SVImageOperatorAverage = 0xfff,

		// for Half-Field camera input...
		// Note: this operator will scale by a factor of 2 in the y axis
		//	Not defined by MIL and should not interfere with 
		//	any MIL's M_... define !!! ( check MIL.h )
		SVImageOperatorDoubleHeight = 0xffe,

		// for image flipping
		// Note: this operator will only use the first image
		//	Not defined by MIL and should not interfere with 
		//	any MIL's M_... define !!! ( check MIL.h )
		SVImageOperatorFlipVertical = 0xffd,

		// for image flipping
		// Note: this operator will only use the first image
		//	Not defined by MIL and should not interfere with 
		//	any MIL's M_... define !!! ( check MIL.h )
		SVImageOperatorFlipHorizontal = 0xffc,
	};
}