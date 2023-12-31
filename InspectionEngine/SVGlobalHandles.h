//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes

namespace SvIe
{

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVDataBufferHandleStruct 
// -----------------------------------------------------------------------------
// .Description : Is used as a multi system data buffer handle container
////////////////////////////////////////////////////////////////////////////////
struct SVDataBufferHandleStruct
{
	SVDataBufferHandleStruct()
	{
		DwSize = sizeof( SVDataBufferHandleStruct );
		PHostBuffer = nullptr;
	};

	DWORD DwSize = 0;					// Size of this structure...
	__int64	milResult = 0LL;	// MIL Buffer Handle, if available...
	LPVOID PHostBuffer = nullptr;				// Buffer Pointer ( Process Address Space ), if available...
};

struct SVBarCodeBufferHandleStruct
{
	SVBarCodeBufferHandleStruct()
	{
		DwSize = sizeof( SVBarCodeBufferHandleStruct );
	};

	DWORD DwSize = 0;					// Size of this structure...
	__int64	milBarCode = 0LL;		// MIL Buffer Handle, if available...
};

} //namespace SvIe
