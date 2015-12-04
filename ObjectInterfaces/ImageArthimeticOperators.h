//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Arthimetic Image Operators that are not direct MIL defines.
//******************************************************************************

#pragma once

// Special Image Operators...
#define SV_IMGOP_AVERAGE		0xfff	// Support Average Of Two Images - SVO Image Arithmetic Operation 
										// NOTE: Not defined by MIL and should not interfere with 
										//		 any MIL's M_... define !!! ( check MIL.h )

#define SV_IMGOP_DOUBLE_HEIGHT	0xffe	// for Half-Field camera input...
										// Note: this operator will scale by a factor of 2 in the y axis
										//	Not defined by MIL and should not interfere with 
										//	any MIL's M_... define !!! ( check MIL.h )

#define SV_IMGOP_FLIP_VERTICAL	0xffd	// for image flipping
										// Note: this operator will only use the first image
										//	Not defined by MIL and should not interfere with 
										//	any MIL's M_... define !!! ( check MIL.h )

#define SV_IMGOP_FLIP_HORIZONTAL 0xffc	// for image flipping
										// Note: this operator will only use the first image
										//	Not defined by MIL and should not interfere with 
										//	any MIL's M_... define !!! ( check MIL.h )
