//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeSystemList
//* .File Name       : $Workfile:   SVMatroxGigeSystemList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:43:50  $
//******************************************************************************

#pragma once

#include "SVMatroxGigeSystem.h"
#include "SVMatroxGigeHandleList.h"

namespace
{
	enum
	{
		INVALID_SVMATROXGIGESYSTEM_HANDLE = 0xFF
	};
}

// allow up to 16 Matrox Gige Systems 
typedef SVMatroxGigeHandleList< 16, unsigned char, INVALID_SVMATROXGIGESYSTEM_HANDLE, SVMatroxGigeSystem> SVMatroxGigeSystemList;

