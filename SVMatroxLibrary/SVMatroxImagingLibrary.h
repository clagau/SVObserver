// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxImagingLibrary
// * .File Name       : $Workfile:   SVMatroxImagingLibrary.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   16 Jan 2014 10:40:58  $
// ******************************************************************************

#ifndef SV_MATROX_IMAGING_LIBRARY_H
#define SV_MATROX_IMAGING_LIBRARY_H

#define SV_DESIRED_MIL_VERSION 0x0900

#if SV_DESIRED_MIL_VERSION == 0x0900
	#define M_MIL_USE_SAFE_TYPE 0
#endif

#include <mil.h>

#pragma comment( lib, "mil" )
#pragma comment( lib, "milblob" )
#pragma comment( lib, "milcode" )
#pragma comment( lib, "milim" )
#pragma comment( lib, "milocr" )
#pragma comment( lib, "milpat" )

#ifdef M_MIL_CURRENT_ASCII_VERSION
 	#pragma message ( "MIL Version " M_MIL_CURRENT_ASCII_VERSION )
#else
 	#pragma message ( "MIL Version UNKNOWN" )
#endif

#ifdef M_MIL_CURRENT_INT_VERSION
	#ifdef SV_CURRENT_MIL_VERSION
		#undef SV_CURRENT_MIL_VERSION
	#endif

	#define SV_CURRENT_MIL_VERSION M_MIL_CURRENT_INT_VERSION
#endif

#ifndef SV_CURRENT_MIL_VERSION
	#define SV_CURRENT_MIL_VERSION 0x0000
#endif

#if SV_CURRENT_MIL_VERSION != SV_DESIRED_MIL_VERSION
	#error ****** INVALID MIL VERSION : Current version does not match desired version.
#endif

#endif // SV_MATROX_IMAGING_LIBRARY_H

