//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGlobalHandles
//* .File Name       : $Workfile:   SVGlobalHandles.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   11 Jun 2013 15:26:10  $
//******************************************************************************

#pragma once

// These should go in svglobal.h but had compilation problems
  // Bit masking constants
#pragma region Includes
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVDataBufferHandleStruct 
// -----------------------------------------------------------------------------
// .Description : Is used as a multi system data buffer handle container
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :09.09.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
struct SVDataBufferHandleStruct
{
	SVDataBufferHandleStruct()
	{
		DwSize = sizeof( SVDataBufferHandleStruct );
		PHostBuffer = nullptr;
	};

	DWORD DwSize;					// Size of this structure...
	SVMatroxImageResult	milResult;	// MIL Buffer Handle, if available...
	LPVOID PHostBuffer;				// Buffer Pointer ( Process Address Space ), if available...
};

struct SVBarCodeBufferHandleStruct
{
	SVBarCodeBufferHandleStruct()
	{
		DwSize = sizeof( SVBarCodeBufferHandleStruct );
	};

	DWORD DwSize;					// Size of this structure...
	SVMatroxBarCode	milBarCode;		// MIL Buffer Handle, if available...
};
