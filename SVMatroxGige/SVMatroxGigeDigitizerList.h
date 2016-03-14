//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeDigitizerList.h
//* .File Name       : $Workfile:   SVMatroxGigeDigitizerList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:44:06  $
//******************************************************************************

#ifndef SVMATROXGIGEDIGITIZERLIST_H
#define SVMATROXGIGEDIGITIZERLIST_H

#include "SVMatroxGigeDigitizer.h"
#include "SVMatroxGigeHandleList.h"

namespace
{
	enum
	{
		INVALID_SVMATROXGIGEDIGITIZER_HANDLE = 0xFF
	};
}

// allow up to 32 Matrox Gige Digitizer per System 
typedef SVMatroxGigeHandleList<32, unsigned char, INVALID_SVMATROXGIGEDIGITIZER_HANDLE, SVMatroxGigeDigitizer> SVMatroxGigeDigitizerList;

#endif

