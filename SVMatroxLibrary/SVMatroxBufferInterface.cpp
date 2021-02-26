// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBufferInterface
// * .File Name       : $Workfile:   SVMatroxBufferInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   29 Jan 2014 10:23:24  $
// ******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include <boost/format.hpp>
#include "MatroxImageProps.h"
#include "SVMatroxBufferInterface.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxBufferCreateChildStruct.h"
#include "SVMatroxBufferCreateLineStruct.h"
#include "SVMatroxBufferCreateStruct.h"
#include "SVMatroxErrorEnum.h"
#include "SVMatroxImageBuffer.h"
#include "SVMatroxImageChildBuffer.h"
#include "SVMatroxSystem.h"
#include "SVSystemLibrary/SVImageConvertorGDI.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVUtilityLibrary/SVBitmapInfo.h"
#pragma endregion Includes

enum ImageBands
{
	SingleBand = 1,
	MaxColorBands = 3
};

enum PIxelFormats
{
	Pixel1 = 1,
	Pixel8 = 8,
	Pixel16 = 16,
	Pixel24 = 24,
	Pixel32 = 32
};

/**
@SVOperationName Default Constructor

@SVOperationDescription

*/
SVMatroxBufferInterface::~SVMatroxBufferInterface()
{
}

/**
@SVOperationName Destructor

@SVOperationDescription

*/
SVMatroxBufferInterface::SVMatroxBufferInterface()
{
}

/**
@SVOperationName Convert2MatroxType - SVMatroxBufferTypeEnum

@SVOperationDescription This function converts the SVMatroxBufferTypeEnum to a Matrox Type.

*/
long SVMatroxBufferInterface::Convert2MatroxType(SVMatroxBufferTypeEnum p_eType)
{
	long l_lMatroxType = 0;

	if (SVBufferSigned == (SVBufferSigned & p_eType))
	{
		l_lMatroxType |= M_SIGNED;
	}

	if (SVBufferUnsigned == (SVBufferUnsigned & p_eType))
	{
		l_lMatroxType |= M_UNSIGNED;
	}

	if (SVBufferFloat == (SVBufferFloat & p_eType))
	{
		l_lMatroxType |= M_FLOAT;
	}

	if (SV1BitUnsigned == (SV1BitUnsigned & p_eType))
	{
		l_lMatroxType |= Pixel1;
	}
	if (SV8BitUnsigned == (SV8BitUnsigned & p_eType))
	{
		l_lMatroxType |= Pixel8;
	}
	if (SV16BitUnsigned == (SV16BitUnsigned & p_eType))
	{
		l_lMatroxType |= Pixel16;
	}
	if (SV32BitUnsigned == (SV32BitUnsigned & p_eType))
	{
		l_lMatroxType |= Pixel32;
	}
	return l_lMatroxType;
}

/**
@SVOperationName Convert2MatroxType - SVMatroxBufferAttributeEnum

@SVOperationDescription This function converts the SVMatroxBufferTypeEnum to a Matrox Type.

*/
__int64 SVMatroxBufferInterface::Convert2MatroxType(SVMatroxBufferAttributeEnum p_eType)
{
	__int64 l_lMatroxType = 0;

	// Specifies the buffer usage
	if (SVBufAttLut == (p_eType & SVBufAttLut))
	{
		l_lMatroxType |= M_LUT;
	}

	if (SVBufAttImage == (p_eType & SVBufAttImage))
	{
		l_lMatroxType |= M_IMAGE;
	}

	if (SVBufAttKernel == (p_eType & SVBufAttKernel))
	{
		l_lMatroxType |= M_KERNEL;
	}

	if (SVBufAttStructElement == (p_eType & SVBufAttStructElement))
	{
		l_lMatroxType |= M_STRUCT_ELEMENT;
	}

	// Buffer Type
	if (SVBufAttCompress == (p_eType & SVBufAttCompress))
	{
		l_lMatroxType |= M_COMPRESS;
	}

	if (SVBufAttDisp == (p_eType & SVBufAttDisp))
	{
		l_lMatroxType |= M_DISP;
	}

	if (SVBufAttGrab == (p_eType & SVBufAttGrab))
	{
		l_lMatroxType |= M_GRAB;
	}
	if (SVBufAttProc == (p_eType & SVBufAttProc))
	{
		l_lMatroxType |= M_PROC;
	}

	switch (p_eType & SVBufJpeg)
	{
		// Attributes that go with SVBufAttCompress
		case SVBufAttJpegLossless:
		{
			l_lMatroxType |= M_JPEG_LOSSLESS;
			break;
		}
		case SVBufAttJpeg2000Lossless:
		{
			l_lMatroxType |= M_JPEG2000_LOSSLESS;
			break;
		}
		case SVBufAttJpegLosslessInterlaced:
		{
			l_lMatroxType |= M_JPEG_LOSSLESS_INTERLACED;
			break;
		}
		case SVBufAttJpegLossy:
		{
			l_lMatroxType |= M_JPEG_LOSSY;
			break;
		}
		case SVBufAttJpeg2000Lossy:
		{
			l_lMatroxType |= M_JPEG2000_LOSSY;
			break;
		}
		case SVBufAttJpegLossyInterlaced:
		{
			l_lMatroxType |= M_JPEG_LOSSY_INTERLACED;
			break;
		}
	}

	if (SVBufAttDib == (p_eType & SVBufAttDib))
	{
		l_lMatroxType |= M_DIB;
	}

	if (SVBufAttDirectX == (p_eType & SVBufAttDirectX))
	{
		l_lMatroxType |= M_DIRECTX;
	}

	switch (p_eType & SVBufLoc)
	{
		// Location for buffer
		case SVBufAttOffBoard:
		{
			l_lMatroxType |= M_OFF_BOARD;
			break;
		}
		case SVBufAttOnBoard:
		{
			l_lMatroxType |= M_ON_BOARD;
			break;
		}
	}

	switch (p_eType & SVBufMemType)
	{
		// Type of memory
		case SVBufAttNonPaged:
		{
			l_lMatroxType |= M_NON_PAGED;
			break;
		}
		case SVBufAttPaged:
		{
			l_lMatroxType |= M_PAGED;
			break;
		}
	}

	if (SVBufAttPacked == (p_eType & SVBufAttPacked))
	{
		l_lMatroxType |= M_PACKED;
	}

	if (SVBufAttPlanar == (p_eType & SVBufAttPlanar))
	{
		l_lMatroxType |= M_PLANAR;
	}

	if (SVBufAttNoFlip == (p_eType & SVBufAttNoFlip))
	{
		l_lMatroxType |= M_NO_FLIP;
	}

	if (SVBufAttRgb16 == (p_eType & SVBufAttRgb16))
	{
		l_lMatroxType |= M_RGB16;
	}

	if (SVBufAttBgr24 == (p_eType & SVBufAttBgr24))
	{
		l_lMatroxType |= M_BGR24;
	}

	if (SVBufAttBgr32 == (p_eType & SVBufAttBgr32))
	{
		l_lMatroxType |= M_BGR32;
	}

	return l_lMatroxType;
}

/**
@SVOperationName Convert2MatroxType - SVMatroxBufferInfoEnum

@SVOperationDescription This function converts the SVMatroxBufferTypeEnum to a Matrox Type.

*/
long SVMatroxBufferInterface::Convert2MatroxType(SVMatroxBufferInfoEnum p_eType)
{
	long l_lMatroxType = 0;

	if (SVWindowDibHeader == (p_eType &  SVWindowDibHeader))
	{
		l_lMatroxType |= M_BITMAPINFO;
	}
	if (SVHostAddress == (p_eType & SVHostAddress))
	{
		l_lMatroxType |= M_HOST_ADDRESS;
	}

	if (SVPhysicalAddress == (p_eType & SVPhysicalAddress))
	{
		l_lMatroxType |= M_PHYSICAL_ADDRESS;

	}
	if (SVSizeX == (p_eType & SVSizeX))
	{
		l_lMatroxType |= M_SIZE_X;

	}
	if (SVSizeY == (p_eType & SVSizeY))
	{
		l_lMatroxType |= M_SIZE_Y;

	}
	if (SVSizeBand == (p_eType & SVSizeBand))
	{
		l_lMatroxType |= M_SIZE_BAND;

	}
	if (SVType == (p_eType & SVType))
	{
		l_lMatroxType |= M_TYPE;

	}
	if (SVWindowDC == (p_eType & SVWindowDC))
	{
		l_lMatroxType |= M_DC_HANDLE;

	}
	if (SVAttribute == (p_eType & SVAttribute))
	{
		l_lMatroxType |= M_ATTRIBUTE;

	}
	if (SVDataFormat == (p_eType & SVDataFormat))
	{
		l_lMatroxType |= M_DATA_FORMAT;
	}

	// Kernel Control Flags
	if (SVKernelDefault == (p_eType & SVKernelDefault))
	{
		l_lMatroxType |= M_DEFAULT;

	}
	if (SVAbsoluteValue == (p_eType & SVAbsoluteValue))
	{
		l_lMatroxType |= M_ABSOLUTE_VALUE;

	}
	if (SVNormalization == (p_eType & SVNormalization))
	{
		l_lMatroxType |= M_NORMALIZATION_FACTOR;

	}
	if (SVBufOverScan == (p_eType & SVBufOverScan))
	{
		l_lMatroxType |= M_OVERSCAN;

	}
	if (SVBufSaturation == (p_eType & SVBufSaturation))
	{
		l_lMatroxType |= M_SATURATION;

	}
	if (SVBufWindowDCAlloc == (p_eType & SVBufWindowDCAlloc))
	{
		l_lMatroxType |= M_DC_ALLOC;

	}
	if (SVBufWindowDCFree == (p_eType & SVBufWindowDCFree))
	{
		l_lMatroxType |= M_DC_FREE;

	}
	if (SVBufModified == (p_eType & SVBufModified))
	{
		l_lMatroxType |= M_MODIFIED;
	}
	if (SVDirectXSurface == (p_eType & SVDirectXSurface))
	{
		l_lMatroxType |= M_DDRAW_SURFACE;
	}

	// Combination Constants.
	if (SVPitch == (SVPitch & p_eType))
	{
		l_lMatroxType |= M_PITCH;
	}
	if (SVPitchByte == (SVPitchByte & p_eType))
	{
		l_lMatroxType |= M_PITCH_BYTE;
	}

	// Parent Info
	if (SVParentID == (SVParentID & p_eType))
	{
		l_lMatroxType |= M_PARENT_ID;
	}
	if (SVParentOffsetX == (SVParentOffsetX & p_eType))
	{
		l_lMatroxType |= M_PARENT_OFFSET_X;
	}
	if (SVParentOffsetY == (SVParentOffsetY & p_eType))
	{
		l_lMatroxType |= M_PARENT_OFFSET_Y;
	}
	return l_lMatroxType;
}

/**
@SVOperationName Create - SVMatroxBufferCreateLineStruct

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxBufferCreateLineStruct.

*/
HRESULT SVMatroxBufferInterface::Create(SVMatroxBuffer& rBuffer, const SVMatroxBufferCreateLineStruct& p_CreateLineStruct)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_lType = Convert2MatroxType(p_CreateLineStruct.m_eType);
		__int64 l_lAttrib = Convert2MatroxType(p_CreateLineStruct.m_eAttribute);

		if (0 != l_lType && 0 != l_lAttrib)
		{
			MIL_ID l_NewID = MbufAlloc1d(M_DEFAULT_HOST,
				p_CreateLineStruct.m_lSizeX,
				l_lType,
				l_lAttrib,
				M_NULL);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK == l_Code)
			{
				createImageBufferPtr(rBuffer, l_NewID, std::string(_T("SVMatroxBufferInterface::Create-CreateLine")));
			}

		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBufferCreateStruct

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxSystem and a SVMatroxBufferCreateStruct.

*/
HRESULT SVMatroxBufferInterface::Create(const SVMatroxSystem& p_rSystem, SVMatroxBuffer& rBuffer, const SVMatroxBufferCreateStruct& p_CreateStruct)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_lType = Convert2MatroxType(p_CreateStruct.m_eType);
		__int64 l_lAttrib = Convert2MatroxType(p_CreateStruct.m_eAttribute);

		if (0 != l_lType && 0 != l_lAttrib)
		{

			MIL_ID MilSystemID = p_rSystem.empty() ? M_DEFAULT_HOST : p_rSystem.m_SystemIdentifier;
			MIL_ID l_NewID = M_NULL;
			if (MaxColorBands == p_CreateStruct.m_lSizeBand)
			{
				if (M_LUT == (l_lAttrib & M_LUT))	// Mil help states that LUT must be stored in planar format.
				{
					l_lAttrib |= M_PLANAR;
					if (M_DIB == (l_lAttrib & M_DIB))
					{
						// Remove M_DIB as it is not allowed with M_PLANAR
						l_lAttrib ^= M_DIB;
					}
				}

				if (M_DIB == (l_lAttrib & M_DIB))
				{
					l_lAttrib |= M_PACKED + M_BGR32;
				}

				l_NewID = MbufAllocColor(MilSystemID,
					p_CreateStruct.m_lSizeBand,
					p_CreateStruct.m_lSizeX,
					p_CreateStruct.m_lSizeY,
					l_lType,
					l_lAttrib,
					M_NULL);
			}
			else
			{
				l_NewID = MbufAlloc2d(MilSystemID,
					p_CreateStruct.m_lSizeX,
					p_CreateStruct.m_lSizeY,
					l_lType,
					l_lAttrib,
					M_NULL);
			}
			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK == l_Code)
			{
				createImageBufferPtr(rBuffer, l_NewID, std::string(_T("SVMatroxBufferInterface::Create-SystemID,BufferCreate")));
			}

		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBufferCreateStruct

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxBufferCreateStruct.

*/
HRESULT SVMatroxBufferInterface::Create(SVMatroxBuffer& rBuffer, const SVMatroxBufferCreateStruct& p_CreateStruct)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_lType = Convert2MatroxType(p_CreateStruct.m_eType);
		__int64 l_lAttrib = Convert2MatroxType(p_CreateStruct.m_eAttribute);

		if (0 != l_lType && 0 != l_lAttrib)
		{
			MIL_ID l_NewID = M_NULL;
			if (MaxColorBands == p_CreateStruct.m_lSizeBand)
			{
				if (M_LUT == (l_lAttrib & M_LUT))	// Mil help states that LUT must be stored in planar format.
				{
					l_lAttrib |= M_PLANAR;
					// Planar cannot be used with DIB
					l_lAttrib &= ~M_DIB;
				}
				l_NewID = MbufAllocColor(M_DEFAULT_HOST,
					p_CreateStruct.m_lSizeBand,
					p_CreateStruct.m_lSizeX,
					p_CreateStruct.m_lSizeY,
					l_lType,
					l_lAttrib,
					M_NULL);
			}
			else
			{
				l_NewID = MbufAlloc2d(M_DEFAULT_HOST,
					p_CreateStruct.m_lSizeX,
					p_CreateStruct.m_lSizeY,
					l_lType,
					l_lAttrib,
					M_NULL);
			}
			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK == l_Code)
			{
				createImageBufferPtr(rBuffer, l_NewID, std::string(_T("SVMatroxBufferInterface::Create-BufferCreate")));
			}

		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBufferCreateChildStruct

@SVOperationDescription This function creates a Matrox child buffer using a SVMatroxBufferCreateChildStruct.

*/
HRESULT SVMatroxBufferInterface::Create(SVMatroxBuffer& rBuffer, const SVMatroxBufferCreateChildStruct& p_CreateChildStruct)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_CreateChildStruct.m_rParentBufId.empty())
		{
			MIL_ID childBufID(M_NULL);
			MIL_ID parentBufID = p_CreateChildStruct.m_rParentBufId.GetIdentifier();

			if (p_CreateChildStruct.m_data.m_lParentBandCount > 1)
			{
				// From the Matrox Imaging Library Help file for MbufChildColor2d, the Band parameter specifies the index of the band to use. 
				// Valid index values are from 0 to (number of bands of the buffer - 1). 
				// Band 0 corresponds to: the red band (for RGB parent buffers), the hue band (for HSL parent buffers), and the Y band (for YUV parent buffers). 
				// Band 1 corresponds to: the green band (for RGB parent buffers), the saturation band (for HSL parent buffers), and the U band (for YUV parent buffers). 
				// Band 2 corresponds to: the blue band (for RGB parent buffers), the luminance band (for HSL parent buffers), and the V band (for YUV parent buffers). 
				childBufID = MbufChildColor2d(parentBufID,
					p_CreateChildStruct.m_data.m_lBand,
					p_CreateChildStruct.m_data.m_lOffX,
					p_CreateChildStruct.m_data.m_lOffY,
					p_CreateChildStruct.m_data.m_lSizeX,
					p_CreateChildStruct.m_data.m_lSizeY,
					M_NULL);
			}
			else
			{
				childBufID = MbufChild2d(parentBufID,
					p_CreateChildStruct.m_data.m_lOffX,
					p_CreateChildStruct.m_data.m_lOffY,
					p_CreateChildStruct.m_data.m_lSizeX,
					p_CreateChildStruct.m_data.m_lSizeY,
					M_NULL);
			}

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK == l_Code)
			{
				if (!rBuffer.empty())
				{
					rBuffer.clear();
				}
				rBuffer.m_BufferPtr = SVMatroxBufferPtr {std::make_shared<SVMatroxImageChildBuffer>(p_CreateChildStruct.m_rParentBufId.m_BufferPtr, childBufID, "SVMatroxBufferInterface::Create-CreateChildBuffer")};
			}

		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBufferCreateExtStruct

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxBufferCreateExtStruct.  This type is used for interfacing to other libraries such as Intek.

*/
// MbufCreateColor -  Caution	 : Uses external data buffer that you must manage.
HRESULT SVMatroxBufferInterface::Create(SVMatroxBuffer& rBuffer, SVMatroxBufferCreateExtStruct p_CreateColorStruct)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_lType = Convert2MatroxType(p_CreateColorStruct.m_eType);
		__int64 l_lAttrib = Convert2MatroxType(p_CreateColorStruct.m_eAttribute);

		if (0 != l_lType && 0 != l_lAttrib)
		{

			MIL_ID l_NewID = M_NULL;
			if (p_CreateColorStruct.m_lSizeBand > 1)
			{
				l_NewID = MbufCreateColor(M_DEFAULT_HOST,
					p_CreateColorStruct.m_lSizeBand,
					p_CreateColorStruct.m_lSizeX,
					p_CreateColorStruct.m_lSizeY,
					l_lType,
					l_lAttrib,
					M_HOST_ADDRESS + M_PITCH,
					p_CreateColorStruct.m_PitchWidth,
					reinterpret_cast<void**>(p_CreateColorStruct.m_ppArrayOfDataPtr),
					M_NULL);
			}
			else
			{
				l_lAttrib &= ~M_PACKED;  //  Cannot have M_PACKED with a mono buffer.
				if (M_IS_FORMAT_RGB_BGR(l_lAttrib))
				{
					l_lAttrib &= ~0xff00;  // remove rgb or bgr attribute
				}
				l_NewID = MbufCreate2d(M_DEFAULT_HOST,
					p_CreateColorStruct.m_lSizeX,
					p_CreateColorStruct.m_lSizeY,
					l_lType,
					l_lAttrib,
					M_HOST_ADDRESS + M_PITCH,
					p_CreateColorStruct.m_PitchWidth,
					reinterpret_cast<void*>(*p_CreateColorStruct.m_ppArrayOfDataPtr),
					M_NULL);
			}

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK == l_Code)
			{
				createImageBufferPtr(rBuffer, l_NewID, std::string(_T("SVMatroxBufferInterface::Create-BufferCreateExt")));
			}

		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBuffer

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxBuffer.

*/
HRESULT SVMatroxBufferInterface::Create(SVMatroxBuffer& rBuffer, const SVMatroxBuffer& p_CreateFrom, bool addDibFlag)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT l_lType;
		MIL_INT64 l_lAttrib;
		MIL_INT l_lBandSize;
		MIL_INT l_lSizeX;
		MIL_INT l_lSizeY;

		// Clear first error...
		/*l_Code = */SVMatroxApplicationInterface::GetFirstError();
		MbufInquire(p_CreateFrom.GetIdentifier(), M_SIZE_BAND, &l_lBandSize);
		MbufInquire(p_CreateFrom.GetIdentifier(), M_SIZE_X, &l_lSizeX);
		MbufInquire(p_CreateFrom.GetIdentifier(), M_SIZE_Y, &l_lSizeY);
		MbufInquire(p_CreateFrom.GetIdentifier(), M_TYPE, &l_lType);
		MbufInquire(p_CreateFrom.GetIdentifier(), M_EXTENDED_ATTRIBUTE, &l_lAttrib);
		if (addDibFlag)
		{
			l_lAttrib |= M_DIB;
		}

		l_Code = SVMatroxApplicationInterface::GetFirstError();

		if (S_OK == l_Code)
		{
			MIL_ID l_NewID = M_NULL;
			if (MaxColorBands == l_lBandSize)
			{
				l_NewID = MbufAllocColor(M_DEFAULT_HOST,
					l_lBandSize,
					l_lSizeX,
					l_lSizeY,
					l_lType,
					l_lAttrib,
					M_NULL);
			}
			else
			{
				l_lAttrib &= ~M_PACKED;  //  Cannot have M_PACKED with a mono buffer.
				if (M_IS_FORMAT_RGB_BGR(l_lAttrib))
				{
					l_lAttrib &= ~0xff00;  // remove rgb or bgr attribute
				}
				l_NewID = MbufAlloc2d(M_DEFAULT_HOST,
					l_lSizeX,
					l_lSizeY,
					l_lType,
					l_lAttrib,
					M_NULL);
			}

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK == l_Code)
			{
				createImageBufferPtr(rBuffer, l_NewID, std::string(_T("SVMatroxBufferInterface::Create-MatroxBuffer")));
			}

		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create

@SVOperationDescription This function creates a SVMatroxBuffer from a HBitmap.

*/
HRESULT SVMatroxBufferInterface::Create(SVMatroxBuffer& rBuffer, HBITMAP& p_rHbm)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (nullptr != p_rHbm)
		{
			long l_Width(0);
			long l_Height(0);
			long l_SizeImage(0);
			unsigned short l_BitCount(0);

			DIBSECTION dib;
			memset(&dib, 0, sizeof(dib));
			dib.dsBmih.biSize = sizeof(BITMAPINFOHEADER);

			int cnt = ::GetObject(p_rHbm, sizeof(dib), &dib);
			if (0 == cnt)
			{
				return S_FALSE;
			}

			if (cnt > sizeof(BITMAP))
			{
				BITMAPINFOHEADER* pbmhInfo = &dib.dsBmih;

				// Calculate the absolute height
				l_Height = pbmhInfo->biHeight;
				l_Width = pbmhInfo->biWidth;
				l_BitCount = pbmhInfo->biBitCount;

				// Make sure image size is calculated
				if (0 == pbmhInfo->biSizeImage)
				{
					pbmhInfo->biSizeImage = ((((l_Width * l_BitCount) + 31) & ~31) >> 3) * abs(l_Height);
				}
				l_SizeImage = pbmhInfo->biSizeImage;
			}
			else // Non DIB created bitmap
			{
				// Calculate the absolute height
				l_Height = dib.dsBm.bmHeight;
				l_Width = dib.dsBm.bmWidth;
				l_BitCount = dib.dsBm.bmBitsPixel;
				l_SizeImage = dib.dsBm.bmWidthBytes * abs(l_Height);
			}
			MIL_INT64 lAttrib = M_PROC + M_IMAGE + M_DIB;

			// we always use 8 bit per plane
			long type = Pixel8 + M_UNSIGNED;

			if (Pixel32 == l_BitCount)
			{
				lAttrib += (M_BGR32 + M_PACKED);
			}
			else if (Pixel24 == l_BitCount)
			{
				lAttrib += (M_BGR24 + M_PACKED);
			}
			else if (Pixel16 == l_BitCount)
			{
				lAttrib += (M_RGB16 + M_PACKED);
			}




			MIL_ID l_NewID = M_NULL;

			// Allocate a multi or single band image buffer
			MbufAllocColor(M_DEFAULT_HOST,
				l_BitCount <= Pixel8 ? SingleBand : MaxColorBands,
				l_Width,
				l_Height,
				type,
				lAttrib,
				&l_NewID);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			l_Code = (l_NewID != M_NULL) ? S_OK : S_FALSE;

			if (S_OK == l_Code)
			{
				 LPVOID pHostBuffer = (LPVOID)MbufInquire(l_NewID, M_HOST_ADDRESS, M_NULL);

				createImageBufferPtr(rBuffer, l_NewID, std::string(_T("SVMatroxBufferInterface::Create-HBITMAP")));

				// Set the bits of our new bitmap
				if (dib.dsBm.bmBits)
				{
					memcpy(pHostBuffer, dib.dsBm.bmBits, l_SizeImage);
				}
				else
				{
					/*long cnt = */::GetBitmapBits(p_rHbm, l_SizeImage, pHostBuffer);
				}
			}

		}// end if (nullptr != p_rHbm)
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

HRESULT SVMatroxBufferInterface::createToHBitmap(SVMatroxBuffer& rNewMilId, const HBITMAP& rHbm)
{
	MIL_ID milImage = M_NULL;
	MIL_INT64 bufInternalFormat = 0;
	BITMAP bmpObj;  // Bitmap Objects contain Width, Height and WidthBytes

					// Fetch the object using the bitmap handle
	GetObject(rHbm, sizeof(bmpObj), &bmpObj);

	// Select the buffer format, we have to match the data that was loaded from the bitmap
	// The bitmap format will change depending on desktops color depth
	if (bmpObj.bmBitsPixel == 16)
	{
		bufInternalFormat = M_RGB16;
	}
	else if (bmpObj.bmBitsPixel == 24)
	{
		bufInternalFormat = M_RGB24;
	}
	else if (bmpObj.bmBitsPixel == 32)
	{
		bufInternalFormat = M_BGR32;
	}

	MbufCreateColor(M_DEFAULT_HOST, (0 == bufInternalFormat) ? 1 : 3, bmpObj.bmWidth, bmpObj.bmHeight, 8, M_IMAGE + M_DIB + bufInternalFormat, M_HOST_ADDRESS + M_PITCH_BYTE, bmpObj.bmWidthBytes, (void **)&bmpObj.bmBits/*bmpBuffer*/, &milImage);
	if (M_NULL != milImage)
	{
		createImageBufferPtr(rNewMilId, milImage, std::string(_T("SVMatroxBufferInterface::CreateToHBITMAP")));
		return S_OK;
	}
	return E_FAIL;
}

HRESULT SVMatroxBufferInterface::IsParent(const SVMatroxBuffer& p_rParentBuffer, const SVMatroxBuffer& p_rChildBuffer)
{
	HRESULT l_Status(S_OK);

	if (!(p_rParentBuffer.empty()) && !(p_rChildBuffer.empty()))
	{
		long l_ParentID = 0;

		l_Status = SVMatroxBufferInterface::Get(p_rChildBuffer, SVParentID, l_ParentID);

		if (S_OK == l_Status)
		{
			if (l_ParentID != p_rParentBuffer.GetIdentifier())
			{
				l_Status = SVMEE_INVALID_HANDLE;
			}
		}
	}
	else
	{
		l_Status = SVMEE_INVALID_HANDLE;
	}

	return l_Status;
}

HRESULT SVMatroxBufferInterface::GetPositionPoint(POINT& rPoint, const SVMatroxBuffer& p_rBuffer)
{
	HRESULT l_Status(S_OK);

	SVPoint<long> point;

	l_Status = GetPositionPoint(point, p_rBuffer);

	rPoint = static_cast<POINT> (point);

	return l_Status;
}

HRESULT SVMatroxBufferInterface::GetPositionPoint(SVPoint<long>& rPoint, const SVMatroxBuffer& p_rBuffer)
{
	HRESULT l_Status(S_OK);

	rPoint = SVPoint<long> {};

	if (!(p_rBuffer.empty()))
	{
		long l_ParentID = 0;

		l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVParentID, l_ParentID);

		if (S_OK == l_Status)
		{
			if (0 != l_ParentID)
			{
				if (l_ParentID != p_rBuffer.GetIdentifier())
				{
					long offsetX {0L};
					long offsetY {0L};

					l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVParentOffsetX, offsetX);

					if (S_OK == l_Status)
					{
						l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVParentOffsetY, offsetY);
					}

					if (S_OK == l_Status)
					{
						rPoint = SVPoint<long> {offsetX, offsetY};
					}
				}
				else
				{
					l_Status = SVMEE_INVALID_HANDLE;
				}
			}
		}
	}
	else
	{
		l_Status = SVMEE_INVALID_HANDLE;
	}

	return l_Status;
}

/**
@SVOperationName GetBitmapInfo

@SVOperationDescription This function returns a pointer (LPBITMAPINFO) to the header of the DIB associated with the MIL buffer (if any) or NULL.

*/
HRESULT SVMatroxBufferInterface::GetBitmapInfo(LPBITMAPINFO& p_rpBitmapInfo, const SVMatroxBuffer& p_rBuffer)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rBuffer.empty())
		{
			MbufInquire(p_rBuffer.GetIdentifier(), M_BITMAPINFO, &p_rpBitmapInfo);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

HRESULT SVMatroxBufferInterface::GetBitmapInfo(SVBitmapInfo& p_rBitmapInfo, const SVMatroxBuffer& p_rBuffer, bool* pIsMilInfo)
{
	bool isMilInfo = false;

	BITMAPINFO* l_pBitmapInfo = nullptr;

	p_rBitmapInfo.clear();

	HRESULT l_Status = GetBitmapInfo(l_pBitmapInfo, p_rBuffer);

	if (S_OK == l_Status && nullptr != l_pBitmapInfo)
	{
		p_rBitmapInfo.Assign(*l_pBitmapInfo);
		isMilInfo = true;
	}
	else if (!(p_rBuffer.empty())) // no DIB available...
	{
		long l_Width = 0;
		long l_Height = 0;
		int l_BitCount = 0;

		l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVSizeX, l_Width);

		if (S_OK == l_Status)
		{
			l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVSizeY, l_Height);
		}

		if (S_OK == l_Status)
		{
			long l_PixelDepth = 0;
			long l_BandSize = 0;

			l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVType, l_PixelDepth);

			if (S_OK == l_Status)
			{
				l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVSizeBand, l_BandSize);
			}

			if (S_OK == l_Status)
			{
				long l_DataFormat = 0;

				__int64 parentId = M_NULL;
				MbufInquire(p_rBuffer.GetIdentifier(), M_PARENT_ID, &parentId);

				if (S_OK == SVMatroxBufferInterface::Get(p_rBuffer, SVDataFormat, l_DataFormat))
				{
					NB_OF_BITS_PER_PIXEL(l_DataFormat, l_BitCount);
				}

				if (l_BitCount < 1 || p_rBuffer.GetIdentifier() != parentId)
				{
					l_BitCount = static_cast<int>(l_PixelDepth * l_BandSize);
				}
			}
		}

		if (S_OK == l_Status)
		{
			p_rBitmapInfo = SVBitmapInfo(static_cast<long>(l_Width), static_cast<long>(-l_Height), l_BitCount, SVBitmapInfo::GetDefaultColorTable(l_BitCount));
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if (nullptr != pIsMilInfo)
	{
		*pIsMilInfo = isMilInfo;
	}

	return l_Status;
}

/**
@SVOperationName CopyBuffer (SVMatroxBuffer to SVMatroxBuffer)

@SVOperationDescription This function copies the data from a SVMatroxBuffer to a SVMatroxBuffer.

*/
HRESULT SVMatroxBufferInterface::CopyBuffer(const SVMatroxBuffer& p_rTo, const SVMatroxBuffer& p_rFrom)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rTo.empty() && !p_rFrom.empty())
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROX_FUNCTION)		
			TCHAR buf[1024];
			_stprintf_s(buf, 1024, _T("copy   From  %i To  %i \n"), int(p_rFrom.GetIdentifier()), int(p_rTo.GetIdentifier()));
			::OutputDebugString(buf);
#endif 			
			MbufCopy(p_rFrom.GetIdentifier(),
				p_rTo.GetIdentifier());

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

HRESULT  SVMatroxBufferInterface::InquireBufferProperties(const SVMatroxBuffer& rBuffer, MatroxImageProps& rImageProps)
{
	/*Code = */SVMatroxApplicationInterface::GetFirstError();
	rImageProps.PitchByte = MbufInquire(rBuffer.GetIdentifier(), M_PITCH_BYTE, M_NULL);
	rImageProps.Pitch = MbufInquire(rBuffer.GetIdentifier(), M_PITCH, M_NULL);
	rImageProps.Bandsize = MbufInquire(rBuffer.GetIdentifier(), M_SIZE_BAND, M_NULL);
	rImageProps.sizeX = MbufInquire(rBuffer.GetIdentifier(), M_SIZE_X, M_NULL);
	rImageProps.sizeY = MbufInquire(rBuffer.GetIdentifier(), M_SIZE_Y, M_NULL);
	rImageProps.Matrox_type = MbufInquire(rBuffer.GetIdentifier(), M_TYPE, M_NULL);
	MIL_INT64 ExtendedAttributes;
	MbufInquire(rBuffer.GetIdentifier(), M_EXTENDED_ATTRIBUTE, &ExtendedAttributes);
	rImageProps.Attrib = ExtendedAttributes;
	rImageProps.Bytesize = MbufInquire(rBuffer.GetIdentifier(), M_SIZE_BYTE, M_NULL);
	HRESULT Code = SVMatroxApplicationInterface::GetLastStatus();
	return Code;
}

HRESULT SVMatroxBufferInterface::CreateBuffer(SVMatroxBuffer& rBuffer, MatroxImageProps& rImageProps, void *pMemory)
{
	HRESULT Code(S_OK);
	if (pMemory == nullptr)
	{
		SvStl::MessageContainer message(SVMSG_SVO_5079_CREATEBUFFERFAILED, SvStl::Tid_InvalidMemoryPointer, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16226);
		throw message;
	}
	else if (rImageProps.Matrox_type == 0)
	{
		SvStl::MessageContainer message(SVMSG_SVO_5079_CREATEBUFFERFAILED, SvStl::Tid_InvalidMatroxType, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16226);
		throw message;
	}
	else if (rImageProps.Attrib == 0)
	{
		SvStl::MessageContainer message(SVMSG_SVO_5079_CREATEBUFFERFAILED, SvStl::Tid_InvalidMatroxAttribute, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16226);
		throw message;
	}

	bool RGB = M_IS_FORMAT_RGB_BGR(rImageProps.Attrib);
	bool yuv = M_IS_FORMAT_YUV(rImageProps.Attrib);
	MIL_ID NewID = M_NULL;
	if (MaxColorBands == rImageProps.Bandsize || RGB || yuv)
	{
		void* pa[3];
		pa[0] = pMemory;
		pa[1] = nullptr;
		pa[2] = nullptr;

		NewID = MbufCreateColor(M_DEFAULT_HOST,
			MaxColorBands
			, rImageProps.sizeX,
			rImageProps.sizeY,
			rImageProps.Matrox_type,
			rImageProps.Attrib,
			M_HOST_ADDRESS + M_PITCH_BYTE,
			rImageProps.PitchByte,
			pa,
			M_NULL);
	}
	else
	{
		NewID = MbufCreate2d(M_DEFAULT_HOST
			, rImageProps.sizeX,
			rImageProps.sizeY,
			rImageProps.Matrox_type,
			rImageProps.Attrib,
			M_HOST_ADDRESS + M_PITCH_BYTE,
			rImageProps.PitchByte,
			pMemory,
			M_NULL);
	}
	Code = SVMatroxApplicationInterface::GetLastStatus();

	if (S_OK == Code)
	{
		createImageBufferPtr(rBuffer, NewID, std::string(_T("SVMatroxBufferInterface::CreateBuffer")));
	}
	else
	{
		SvDef::StringVector msg;
		msg.push_back(boost::str(boost::format("Matrox Code: 0X%08X") % Code));
		SvStl::MessageContainer message(SVMSG_SVO_5079_CREATEBUFFERFAILED, SvStl::Tid_Default, msg, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16226);
		throw message;
	}


	assert(S_OK == Code);
	return Code;
}


HRESULT SVMatroxBufferInterface::CopyBuffer(const SVMatroxBuffer& p_rTo, __int64 p_From)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rTo.empty() && 0 != p_From)
		{
			MbufCopy(p_From, p_rTo.GetIdentifier());

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

HRESULT SVMatroxBufferInterface::CopyBuffer(__int64 p_To, const SVMatroxBuffer& p_rFrom)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (0 != p_To && !p_rFrom.empty())
		{
			MbufCopy(p_rFrom.GetIdentifier(), p_To);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName CopyBuffer (SVMatroxBuffer and offsets to SVMatroxBuffer)

@SVOperationDescription This function copies the data from a SVMatroxBuffer to a SVMatroxBuffer with x and y offsets.  If the destination is not large enough to put the source with its offsets then it is clipped.

*/
HRESULT SVMatroxBufferInterface::CopyBuffer(const SVMatroxBuffer& p_rTo, const SVMatroxBuffer& p_rFrom, long p_lXOffset, long p_lYOffset)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rTo.empty() && !p_rFrom.empty())
		{
			MbufCopyClip(p_rFrom.GetIdentifier(),
				p_rTo.GetIdentifier(),
				p_lXOffset,
				p_lYOffset);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName CopyBuffer (SVMatroxBuffer band to SVMatroxBuffer)

@SVOperationDescription This function copies the data from a SVMatroxBuffer to a SVMatroxBuffer with x and y offsets.  If the destination is not large enough to put the source with its offsets then it is clipped.

*/
HRESULT SVMatroxBufferInterface::CopyBuffer(const SVMatroxBuffer& p_rTo, const SVMatroxBuffer& p_rFrom, long p_lBand)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rTo.empty() && !p_rFrom.empty())
		{
			MbufCopyColor(p_rFrom.GetIdentifier(),
				p_rTo.GetIdentifier(),
				p_lBand);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName CopyBuffer (HBITMAP to SVMatroxBuffer)

@SVOperationDescription This function copies the data from a HBitmap to a SVMatroxBuffer.

*/
HRESULT SVMatroxBufferInterface::CopyBuffer(const SVMatroxBuffer& p_rMilId, HBITMAP& p_rHbm)
{	// HBitmapToMilHandle
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		DIBSECTION dib;
		memset(&dib, 0, sizeof(dib));

		if (p_rMilId.empty())
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}

		if (S_OK == l_Code)
		{
			if (nullptr != p_rHbm)
			{
				if (0 != ::GetObject(p_rHbm, sizeof(DIBSECTION), &dib))
				{
					// see article Q186586; GetObject(DIBSECTION) always returns positive height
					// assume source is standard orientation (negative)
					dib.dsBmih.biHeight = -(abs(dib.dsBmih.biHeight));
				}
				else
				{
					l_Code = S_FALSE;
				}
			}
			else
			{
				l_Code = S_FALSE;
			}
		}

		if (S_OK == l_Code)
		{
			// Get the BITMAPINFO from MIL
			BITMAPINFO* pbmInfoMil = reinterpret_cast<LPBITMAPINFO>(MbufInquire(p_rMilId.GetIdentifier(), M_BITMAPINFO, M_NULL));
			LPVOID pHostBuffer = reinterpret_cast<LPVOID>(MbufInquire(p_rMilId.GetIdentifier(), M_HOST_ADDRESS, M_NULL));
			if (pbmInfoMil && pHostBuffer)
			{
				l_Code = SVImageConvertorGDI::CopyDIBits(&(dib.dsBmih), dib.dsBm.bmBits, &(pbmInfoMil->bmiHeader), pHostBuffer);
			}
			else
			{
				// No BitmapInfo, either it wasn't created with M_DIB or it is a child buffer on a color/mono image
				// Don't copy into a child buffer, as it will also update the parent ?
				l_Code = S_FALSE;
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

HRESULT SVMatroxBufferInterface::CopyBufferToFileDIB(std::string& rTo, SVBitmapInfo& rBitMapInfo, const SVMatroxBuffer& rFromId, bool addFileHeader)
{
	MIL_ID milId = rFromId.GetIdentifier();
	bool isMilInfo = false;
	HRESULT hres = GetBitmapInfo(rBitMapInfo, rFromId, &isMilInfo);
	if (S_OK == hres)
	{
		void* pHostBuffer = nullptr;
		std::vector<BYTE> bufferVector;	//this vector is for the data-store for milBuffer with no BitmapInfo. It must be valid as long as pHostBuffer is used.

		if (isMilInfo)
		{
			pHostBuffer = reinterpret_cast<void*>(MbufInquire(milId, M_HOST_ADDRESS, M_NULL));
		}
		else
		{
			switch (rBitMapInfo.GetBitCount())
			{
				case 32:
					MbufGetColor(milId, M_PACKED+M_BGR32, M_ALL_BAND, bufferVector);
					break;
				case 24:
					MbufGetColor(milId, M_PACKED + M_BGR24, M_ALL_BAND, bufferVector);
					break;
				case 8:
				default:
					MbufGet(milId, bufferVector);
					break;
			}
						
			assert(bufferVector.size() == fabs(rBitMapInfo.GetHeight()*rBitMapInfo.GetWidth()*(rBitMapInfo.GetBitCount() / 8)));
			if (0 < bufferVector.size())
			{
				pHostBuffer = bufferVector.data();
			}
		}

		if (nullptr != pHostBuffer)
		{
			size_t l_InfoSize = rBitMapInfo.GetBitmapInfoSizeInBytes();
			size_t l_ImageSize = rBitMapInfo.GetBitmapImageSizeInBytes();

			BITMAPFILEHEADER hdr;
			memset(&hdr, 0, sizeof(hdr));
			size_t toSize = 0;
			size_t bitmapPos = 0;
			if (addFileHeader)
			{
				hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
				hdr.bfSize = static_cast<DWORD>(sizeof(BITMAPFILEHEADER) + l_InfoSize + l_ImageSize);
				hdr.bfReserved1 = 0;
				hdr.bfReserved2 = 0;
				hdr.bfOffBits = static_cast<DWORD>(sizeof(BITMAPFILEHEADER) + l_InfoSize);
				toSize = hdr.bfSize;
				bitmapPos = sizeof(BITMAPFILEHEADER);
			}
			else
			{
				toSize = l_InfoSize + l_ImageSize;
			}

			rTo.resize(toSize);

			if (addFileHeader)
			{
				::memcpy(&(rTo[0]), &hdr, sizeof(BITMAPFILEHEADER));
			}

			::memcpy(&(rTo[bitmapPos]), rBitMapInfo.GetBitmapInfo(), l_InfoSize);

			BITMAPINFO* l_pBitmapInfo = reinterpret_cast<BITMAPINFO*>(&(rTo[bitmapPos]));

			//@WARNING[MZA][8.10][27.11.2018] The switch of the negative height is needed by the inspectionCommands, but not good for the live display.
			//By the live display the fileHeader is not added.
			if (l_pBitmapInfo->bmiHeader.biHeight < 0 && addFileHeader)
			{
				size_t toStride = rBitMapInfo.GetBitmapImageStrideInBytes();
				size_t fromStride = toStride;
				if (!isMilInfo)
				{
					fromStride = l_pBitmapInfo->bmiHeader.biWidth * (l_pBitmapInfo->bmiHeader.biBitCount / 8);
				}

				unsigned char* l_pFrom = reinterpret_cast<unsigned char*>(pHostBuffer);
				unsigned char* l_pTo = reinterpret_cast<unsigned char*>(&(rTo[bitmapPos + l_InfoSize]));

				l_pBitmapInfo->bmiHeader.biHeight = ::labs(l_pBitmapInfo->bmiHeader.biHeight);
				l_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
				l_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
				l_pBitmapInfo->bmiHeader.biClrImportant = 0;

				for (size_t i = 0; i < static_cast<size_t>(l_pBitmapInfo->bmiHeader.biHeight); ++i)
				{
					unsigned char* l_pToRow = l_pTo + (i * toStride);
					unsigned char* l_pFromRow = l_pFrom + ((l_pBitmapInfo->bmiHeader.biHeight - 1 - i) * fromStride);

					::memcpy(l_pToRow, l_pFromRow, fromStride);
				}
			}
			else
			{
				if (isMilInfo || rBitMapInfo.GetBitmapImageStrideInBytes() == l_pBitmapInfo->bmiHeader.biWidth)
				{
					::memcpy(&(rTo[bitmapPos + l_InfoSize]), pHostBuffer, l_ImageSize);
				}
				else
				{
					unsigned char* l_pFrom = reinterpret_cast<unsigned char*>(pHostBuffer);
					unsigned char* l_pTo = reinterpret_cast<unsigned char*>(&(rTo[bitmapPos + l_InfoSize]));
					size_t toStride = rBitMapInfo.GetBitmapImageStrideInBytes();
					size_t fromStride = l_pBitmapInfo->bmiHeader.biWidth * (l_pBitmapInfo->bmiHeader.biBitCount/8);
					for (size_t i = 0; i < abs(l_pBitmapInfo->bmiHeader.biHeight); ++i)
					{
						unsigned char* l_pToRow = l_pTo + (i * toStride);
						unsigned char* l_pFromRow = l_pFrom + (i * fromStride);

						::memcpy(l_pToRow, l_pFromRow, fromStride);
					}
				}
			}
		}
		else
		{
			hres = SVMEE_INVALID_HANDLE;
		}
	}

	return hres;
}


/**
@SVOperationName PutBuffer - const unsigned char*

@SVOperationDescription This function copies the data from a user array to a MatroxBuffer.

*/
HRESULT SVMatroxBufferInterface::PutBuffer(const SVMatroxBuffer& p_rTo,
	const unsigned char* p_pcArrayData)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rTo.empty())
		{
			MbufPut(p_rTo.GetIdentifier(),
				const_cast<unsigned char*>(p_pcArrayData));

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName PutBuffer - const long*

@SVOperationDescription This function copies the data from a user array to a MatroxBuffer.

*/
HRESULT SVMatroxBufferInterface::PutBuffer(const SVMatroxBuffer& p_rTo, const long* p_plArrayData)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rTo.empty())
		{
			MbufPut(p_rTo.GetIdentifier(),
				const_cast<long*>(p_plArrayData));

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName PutLine

@SVOperationDescription This function copies the data from a user array to a MatroxBuffer.

*/
HRESULT SVMatroxBufferInterface::PutLine(const SVMatroxBuffer& p_rTo,
	long p_lCount,
	const unsigned char* p_pArrayData)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rTo.empty())
		{
			MbufPut1d(p_rTo.GetIdentifier(),
				0,
				p_lCount,
				const_cast<unsigned char*>(p_pArrayData));

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Get - double

@SVOperationDescription This function uses MbufInquire to get information about a SVMatroxBuffer and stores it in a double.

*/
HRESULT SVMatroxBufferInterface::Get(const SVMatroxBuffer& p_rBuf, SVMatroxBufferInfoEnum p_eWhat, double& rResult)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rBuf.empty())
		{
			MIL_INT l_lMatroxType = Convert2MatroxType(p_eWhat);
			if (0 != l_lMatroxType)
			{
				// MbufInquire is expecting MIL_INT except for M_MIN or M_MAX will use double
				if (M_MIN == (l_lMatroxType & M_MIN) || M_MAX == (l_lMatroxType & M_MAX))
				{
					MbufInquire(p_rBuf.GetIdentifier(),
						l_lMatroxType,
						&rResult);
				}
				else
				{
					MIL_INT mValue = 0;
					MbufInquire(p_rBuf.GetIdentifier(),
						l_lMatroxType,
						&mValue);
					rResult = static_cast<double>(mValue);
				}
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_PARAMETER;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Get - LONGLONG

@SVOperationDescription This function uses MbufInquire to get information about a SVMatroxBuffer and stores it in a LONGLONG.

*/
HRESULT SVMatroxBufferInterface::Get(const SVMatroxBuffer& p_rBuf, SVMatroxBufferInfoEnum p_eWhat, LONGLONG& rResult)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rBuf.empty())
		{
			MIL_INT l_lMatroxType = Convert2MatroxType(p_eWhat);
			if (0 != l_lMatroxType)
			{
				MbufInquire(p_rBuf.GetIdentifier(),
					l_lMatroxType,
					&rResult);

				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_PARAMETER;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

//! This method is required for backward compatibility the LONGLONG version is required when getting the hDC
HRESULT SVMatroxBufferInterface::Get(const SVMatroxBuffer& p_rBuf, SVMatroxBufferInfoEnum p_eWhat, long& rResult)
{
	HRESULT l_Code(S_OK);

	LONGLONG Value;

	l_Code = SVMatroxBufferInterface::Get(p_rBuf, p_eWhat, Value);
	rResult = static_cast<long> (Value);

	return l_Code;
}

/**
@SVOperationName Set - double

@SVOperationDescription This function allows you to control a specified data buffer settings with a supplied double.

*/
HRESULT SVMatroxBufferInterface::Set(const SVMatroxBuffer& p_rBuf,
	SVMatroxBufferInfoEnum p_eWhat,
	const double p_rdValue)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rBuf.empty())
		{
			long l_lMatroxType = Convert2MatroxType(p_eWhat);
			if (0 != l_lMatroxType)
			{
				MbufControl(p_rBuf.GetIdentifier(),
					l_lMatroxType,
					p_rdValue);

				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_PARAMETER;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Set - long

@SVOperationDescription This function allows you to control a specified data buffer settings with a supplied long.

*/
HRESULT SVMatroxBufferInterface::Set(const SVMatroxBuffer& p_rBuf,
	SVMatroxBufferInfoEnum p_eWhat,
	const long long p_rlValue)
{
	return Set(p_rBuf, p_eWhat, static_cast<const double>(p_rlValue));
}

/**
@SVOperationName ControlNeighborhood

@SVOperationDescription This function changes the setting of an operation control type of the specified kernel buffer or structuring element buffer.

*/
HRESULT SVMatroxBufferInterface::ControlNeighborhood(const SVMatroxBuffer& p_rBuf,
	SVMatroxBufferInfoEnum p_eWhat,
	long p_lValue)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rBuf.empty())
		{
			long l_lMatroxType = Convert2MatroxType(p_eWhat);
			if (0 != l_lMatroxType)
			{
				MbufControlNeighborhood(p_rBuf.GetIdentifier(),
					l_lMatroxType,
					p_lValue);

				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_PARAMETER;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName ClearBuffer - SVMatroxBuffer

@SVOperationDescription This function clears the entire specified buffer to the specified color.

*/
HRESULT SVMatroxBufferInterface::ClearBuffer(const SVMatroxBuffer& p_rBuffer, double p_dColor)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rBuffer.empty())
		{
			MbufClear(p_rBuffer.GetIdentifier(), p_dColor);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;

}

HRESULT SVMatroxBufferInterface::GetImageSize(const std::string& rFileName, long &rWidth, long &rHeight)
{
	MIL_INT  M_Width(0), M_Height(0);
	HRESULT  Code(S_OK);
	if (M_INVALID == MbufDiskInquire(const_cast<MIL_TEXT_CHAR*>(rFileName.c_str()), M_SIZE_X, &M_Width))
	{
		Code = M_INVALID;
	}
	if (M_INVALID == MbufDiskInquire(const_cast<MIL_TEXT_CHAR*>(rFileName.c_str()), M_SIZE_Y, &M_Height))
	{
		Code = M_INVALID;
	}
	if (S_OK == Code)
	{
		rWidth = static_cast<long>(M_Width);
		rHeight = static_cast<long>(M_Height);
	}
	return (Code);
}

/**
@SVOperationName Import

@SVOperationDescription This function imports data from a file into an existing or automatically allocated MIL data buffer.

*/
HRESULT SVMatroxBufferInterface::Import(SVMatroxBuffer& rBuffer,
	const std::string& p_rFileName,
	SVMatroxFileTypeEnum p_eFileType,
	bool p_bRestore)
{
	long l_lFileFormat = 0;
	long l_lOperation = 0;

	// File Formats
	if (SVFileBitmap == (p_eFileType & SVFileBitmap))
	{
		l_lFileFormat = M_BMP;
	}
	else if (SVFileTiff == (p_eFileType & SVFileTiff))
	{
		l_lFileFormat = M_TIFF;
	}
	else //if (SVFileMIL == (p_eFileType & SVFileMIL))
	{
		l_lFileFormat = M_MIL;
	}

	// Operations

	if (p_bRestore)
	{
		l_lOperation = M_RESTORE;
	}
	else
	{
		l_lOperation = M_LOAD;
	}

	HRESULT l_Code(S_OK);

#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (p_bRestore)
		{	// Restore Operation creates a new Mil handle.
			MIL_ID l_NewID = M_NULL;
			MbufImport(const_cast<MIL_TEXT_CHAR*>(p_rFileName.c_str()),
				l_lFileFormat,
				l_lOperation,
				M_DEFAULT_HOST,
				&l_NewID);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if (S_OK == l_Code)
			{
				createImageBufferPtr(rBuffer, l_NewID, std::string(_T("SVMatroxBufferInterface::Import")));
			}
		}
		else
		{	// Load fills a previously created Mil handle.
			if (!rBuffer.empty())
			{
				MIL_ID l_NewId = rBuffer.GetIdentifier();
				MbufImport(const_cast<MIL_TEXT_CHAR*>(p_rFileName.c_str()),
					l_lFileFormat,
					l_lOperation,
					M_DEFAULT_HOST,
					&l_NewId);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Export

@SVOperationDescription This function exports a data buffer to a file, in the specified output file format.

*/
HRESULT SVMatroxBufferInterface::Export(const SVMatroxBuffer& rBuffer,
	const std::string& rFileName,
	SVMatroxFileTypeEnum p_eFileType)
{
	long l_lFileFormat = 0;

	// File Formats. Currently only Bmps
	if (SVFileBitmap == (p_eFileType & SVFileBitmap))
	{
		l_lFileFormat |= M_BMP;
	}

	HRESULT l_Code(S_OK);

#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rBuffer.empty())
		{
			MbufExport(rFileName.c_str(),
				l_lFileFormat,
				rBuffer.GetIdentifier());

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(S_OK == l_Code);
	return l_Code;
}

void SVMatroxBufferInterface::createImageBufferPtr(SVMatroxBuffer& rBuffer, __int64 MatroxID, const std::string& rCreatorName)
{
	if (!rBuffer.empty())
	{
		rBuffer.clear();
	}
	rBuffer.m_BufferPtr = SVMatroxBufferPtr {std::make_shared<SVMatroxImageBuffer>(MatroxID, rCreatorName)};
}
