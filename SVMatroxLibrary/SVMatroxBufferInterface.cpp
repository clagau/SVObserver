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
#include <assert.h>
#include <boost/scoped_array.hpp>
#include "SVUtilityLibrary/SVImageCopyUtility.h"
#include "SVMatroxBufferInterface.h"
#include "SVMatroxImageBuffer.h"
#include "SVMatroxImageChildBuffer.h"
#include "SVSystemLibrary/SVImageConvertorGDI.h"
#include "SVMatroxDisplay.h"
#include "SVMatroxImagingLibrary.h"  // has MIL includes
#include "SVMatroxResourceMonitor.h"
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
	if (SVBufferDouble == (SVBufferDouble & p_eType))
	{
		l_lMatroxType |= M_DOUBLE;
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
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create(SVMatroxBuffer& p_rBuffer, const SVMatroxBufferCreateLineStruct& p_CreateLineStruct)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_lType = Convert2MatroxType(p_CreateLineStruct.m_eType);
		__int64 l_lAttrib = Convert2MatroxType(p_CreateLineStruct.m_eAttribute);

		if (0 != l_lType && 0 != l_lAttrib)
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock(l_AutoLock);

			if (SVMEE_STATUS_OK == l_Code)
			{
				MIL_ID l_NewBuf = MbufAlloc1d(M_DEFAULT_HOST,
					p_CreateLineStruct.m_lSizeX,
					l_lType,
					l_lAttrib,
					M_NULL);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				if (SVMEE_STATUS_OK == l_Code)
				{
					p_rBuffer.m_BufferPtr = new SVMatroxImageBuffer(l_NewBuf, "SVMatroxBufferInterface::Create-CreateLine");
				}
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create - LPBITMAPINFO

@SVOperationDescription This function creates a Matrox buffer using a LPBITMAPINFO, it does not copy any pixels.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create(SVMatroxBuffer& p_rBuffer, const LPBITMAPINFO p_pBitmapInfo)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		HRESULT hr = S_OK;

		if (nullptr != p_pBitmapInfo)
		{
			BITMAPINFOHEADER* pbmhInfo = &p_pBitmapInfo->bmiHeader;

			// Calculate the absolute height
			long lHeight = abs(pbmhInfo->biHeight);

			BUFATTRTYPE lAttrib = M_PROC + M_IMAGE + M_DIB;

			// we always use 8 bit per plane
			long type = Pixel8 + M_UNSIGNED;

			unsigned short l_BitCount = pbmhInfo->biBitCount;

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

			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock(l_AutoLock);

			if (SVMEE_STATUS_OK == l_Code)
			{
				// Allocate a multi or single band image buffer
				MIL_ID l_NewID = MbufAllocColor(M_DEFAULT_HOST,
								pbmhInfo->biBitCount <= Pixel8 ? SingleBand : MaxColorBands,
								pbmhInfo->biWidth, 
								pbmhInfo->biHeight, 
								type,
								lAttrib,
								M_NULL);

				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				if (SVMEE_STATUS_OK == l_Code)
				{
					p_rBuffer.m_BufferPtr = new SVMatroxImageBuffer(l_NewID, "SVMatroxBufferInterface::Create-LPBITMAPINFO");
				}
			}
		}// end if (nullptr != p_pBitmapInfo)
		else
		{
			l_Code = SVMEE_BAD_POINTER;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBufferCreateStruct

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxSystem and a SVMatroxBufferCreateStruct.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create(const SVMatroxSystem& p_rSystem, SVMatroxBuffer& p_rBuffer, const SVMatroxBufferCreateStruct& p_CreateStruct)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_lType = Convert2MatroxType( p_CreateStruct.m_eType );
		__int64 l_lAttrib = Convert2MatroxType( p_CreateStruct.m_eAttribute );

		if (0 != l_lType && 0 != l_lAttrib)
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock(l_AutoLock);

			if (SVMEE_STATUS_OK == l_Code)
			{
				MIL_ID MilSystemID = p_rSystem.empty() ? M_DEFAULT_HOST : p_rSystem.m_SystemIdentifier;
				MIL_ID l_NewBuf = M_NULL;
				if (MaxColorBands == p_CreateStruct.m_lSizeBand)
				{
					if (M_LUT == (l_lAttrib & M_LUT))	// Mil help states that LUT must be stored in planar format.
					{
						l_lAttrib |= M_PLANAR;
					}

                    // MIL 9.0 has a problem using this flag with MbufAllocColor.
					if (M_DIB == (l_lAttrib & M_DIB)) 
					{
						// Remove the existing M_DIB and replace it with M_PACKED and M_BGR32.
						l_lAttrib ^= M_DIB;
						l_lAttrib |= M_PACKED + M_BGR32;
					}

					l_NewBuf = MbufAllocColor(MilSystemID,
						p_CreateStruct.m_lSizeBand,
						p_CreateStruct.m_lSizeX,
						p_CreateStruct.m_lSizeY,
						l_lType,
						l_lAttrib,
						M_NULL);
				}
				else
				{
					l_NewBuf = MbufAlloc2d(MilSystemID,
						p_CreateStruct.m_lSizeX,
						p_CreateStruct.m_lSizeY,
						l_lType,
						l_lAttrib,
						M_NULL);
				}
				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				if (SVMEE_STATUS_OK == l_Code)
				{
					p_rBuffer.m_BufferPtr = new SVMatroxImageBuffer(l_NewBuf, "SVMatroxBufferInterface::Create-SystemID,BufferCreate");
				}
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBufferCreateStruct

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxBufferCreateStruct.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create(SVMatroxBuffer& p_rBuffer, const SVMatroxBufferCreateStruct& p_CreateStruct)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_lType = Convert2MatroxType(p_CreateStruct.m_eType);
		__int64 l_lAttrib = Convert2MatroxType(p_CreateStruct.m_eAttribute);

		if (0 != l_lType && 0 != l_lAttrib)
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock(l_AutoLock);

			if (SVMEE_STATUS_OK == l_Code)
			{
				MIL_ID l_NewBuf = M_NULL;
				if (MaxColorBands == p_CreateStruct.m_lSizeBand)
				{
					if (M_LUT == (l_lAttrib & M_LUT))	// Mil help states that LUT must be stored in planar format.
					{
						l_lAttrib |= M_PLANAR;
					}
					l_NewBuf = MbufAllocColor(M_DEFAULT_HOST,
						p_CreateStruct.m_lSizeBand,
						p_CreateStruct.m_lSizeX,
						p_CreateStruct.m_lSizeY,
						l_lType,
						l_lAttrib,
						M_NULL);
				}
				else
				{
					l_NewBuf = MbufAlloc2d(M_DEFAULT_HOST,
						p_CreateStruct.m_lSizeX,
						p_CreateStruct.m_lSizeY,
						l_lType,
						l_lAttrib,
						M_NULL);
				}
				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				if (SVMEE_STATUS_OK == l_Code)
				{
					p_rBuffer.m_BufferPtr = new SVMatroxImageBuffer(l_NewBuf, "SVMatroxBufferInterface::Create-BufferCreate");
				}
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBufferCreateChildStruct

@SVOperationDescription This function creates a Matrox child buffer using a SVMatroxBufferCreateChildStruct.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create(SVMatroxBuffer& p_rNewBuffer, const SVMatroxBufferCreateChildStruct& p_CreateChildStruct)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_CreateChildStruct.m_ParentBufId.empty())
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock(l_AutoLock);

			if (SVMEE_STATUS_OK == l_Code)
			{
				MIL_ID childBufID(M_NULL);
				MIL_ID parentBufID = p_CreateChildStruct.m_ParentBufId.GetIdentifier();

				if (p_CreateChildStruct.m_lParentBandCount > 1)
				{
					childBufID = MbufChildColor2d(parentBufID,
						p_CreateChildStruct.m_lBand,
						p_CreateChildStruct.m_lOffX,
						p_CreateChildStruct.m_lOffY,
						p_CreateChildStruct.m_lSizeX,
						p_CreateChildStruct.m_lSizeY,
						M_NULL);
				}
				else
				{
					childBufID = MbufChild2d(parentBufID,
						p_CreateChildStruct.m_lOffX,
						p_CreateChildStruct.m_lOffY,
						p_CreateChildStruct.m_lSizeX,
						p_CreateChildStruct.m_lSizeY,
						M_NULL);
				}

				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				if (SVMEE_STATUS_OK == l_Code)
				{
					p_rNewBuffer.m_BufferPtr = new SVMatroxImageChildBuffer(p_CreateChildStruct.m_ParentBufId.m_BufferPtr, childBufID, "SVMatroxBufferInterface::Create-CreateChildBuffer");
				}
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBufferCreateExtStruct

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxBufferCreateExtStruct.  This type is used for interfacing to other libraries such as Intek.

*/
// MbufCreateColor -  Caution	 : Uses external data buffer that you must manage.
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create(SVMatroxBuffer& p_rBuffer, SVMatroxBufferCreateExtStruct p_CreateColorStruct)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_lType = Convert2MatroxType(p_CreateColorStruct.m_eType);
		__int64 l_lAttrib = Convert2MatroxType(p_CreateColorStruct.m_eAttribute);

		if (0 != l_lType && 0 != l_lAttrib)
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock(l_AutoLock);

			if (SVMEE_STATUS_OK == l_Code)
			{
				MIL_ID l_NewBuf = M_NULL;
				if (p_CreateColorStruct.m_lSizeBand > 1)
				{
					l_NewBuf = MbufCreateColor(M_DEFAULT_HOST,
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
					l_lAttrib &= ~M_PACKED ;  //  Cannot have M_PACKED with a mono buffer.
					if (M_IS_FORMAT_RGB_BGR(l_lAttrib))
					{
						l_lAttrib &= ~0xff00;  // remove rgb or bgr attribute
					}
					l_NewBuf = MbufCreate2d(M_DEFAULT_HOST,
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

				if (SVMEE_STATUS_OK == l_Code)
				{
					p_rBuffer.m_BufferPtr = new SVMatroxImageBuffer(l_NewBuf, "SVMatroxBufferInterface::Create-BufferCreateExt");
				}
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBuffer

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create(SVMatroxBuffer& p_rNewBuffer, const SVMatroxBuffer& p_CreateFrom)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
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
		l_Code = SVMatroxApplicationInterface::GetFirstError();
		MbufInquire(p_CreateFrom.GetIdentifier(), M_SIZE_BAND, &l_lBandSize);
		MbufInquire(p_CreateFrom.GetIdentifier(), M_SIZE_X , &l_lSizeX);
		MbufInquire(p_CreateFrom.GetIdentifier(), M_SIZE_Y , &l_lSizeY);
		MbufInquire(p_CreateFrom.GetIdentifier(), M_TYPE , &l_lType);
		MbufInquire(p_CreateFrom.GetIdentifier(), M_EXTENDED_ATTRIBUTE , &l_lAttrib);

		l_Code = SVMatroxApplicationInterface::GetFirstError();

		if (SVMEE_STATUS_OK == l_Code)
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock(l_AutoLock);

			if (SVMEE_STATUS_OK == l_Code)
			{
				MIL_ID l_NewBuf = M_NULL;
				if (MaxColorBands == l_lBandSize)
				{
					l_NewBuf = MbufAllocColor(M_DEFAULT_HOST,
						l_lBandSize,
						l_lSizeX,
						l_lSizeY,
						l_lType,
						l_lAttrib,
						M_NULL);
				}
				else
				{
					l_lAttrib &= ~M_PACKED ;  //  Cannot have M_PACKED with a mono buffer.
					if (M_IS_FORMAT_RGB_BGR(l_lAttrib))
					{
						l_lAttrib &= ~0xff00;  // remove rgb or bgr attribute
					}
					l_NewBuf = MbufAlloc2d(M_DEFAULT_HOST,
						l_lSizeX,
						l_lSizeY,
						l_lType,
						l_lAttrib,
						M_NULL);
				}

				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				if (SVMEE_STATUS_OK == l_Code)
				{
					p_rNewBuffer.m_BufferPtr = new SVMatroxImageBuffer(l_NewBuf, "SVMatroxBufferInterface::Create-MatroxBuffer");
				}
			}
		}

	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create

@SVOperationDescription This function creates a SVMatroxBuffer from a HBitmap.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create(SVMatroxBuffer& p_rNewMilId, HBITMAP& p_rHbm)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
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
			BUFATTRTYPE lAttrib = M_PROC + M_IMAGE + M_DIB;

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

			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock(l_AutoLock);

			if (SVMEE_STATUS_OK == l_Code)
			{
				MIL_ID l_NewBuf = M_NULL;

				// Allocate a multi or single band image buffer
				MbufAllocColor(M_DEFAULT_HOST,
								l_BitCount <= Pixel8 ? SingleBand : MaxColorBands,
								l_Width, 
								l_Height, 
								type,
								lAttrib,
								&l_NewBuf);

				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				l_Code = (l_NewBuf != M_NULL) ? S_OK : S_FALSE;

				if (S_OK == l_Code)
				{
					LPVOID pHostBuffer = nullptr;
					pHostBuffer = (LPVOID) MbufInquire(l_NewBuf, M_HOST_ADDRESS, M_NULL);

					p_rNewMilId.m_BufferPtr = new SVMatroxImageBuffer(l_NewBuf, "SVMatroxBufferInterface::Create-HBITMAP");

					// Set the bits of our new bitmap
					if (dib.dsBm.bmBits)
					{
						memcpy(pHostBuffer, dib.dsBm.bmBits, l_SizeImage);
					}
					else
					{
						long cnt = ::GetBitmapBits(p_rHbm, l_SizeImage, pHostBuffer);
					}
				}
			}
		}// end if (nullptr != p_rHbm)
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Create

@SVOperationDescription This function creates a new HBITMAP from a SVMatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create(HBITMAP& p_rHbm, const SVMatroxBuffer& p_rFromId)
{	// MilHandleToNewHBitmap
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (p_rFromId.empty())
		{
			l_Code = S_FALSE;
			p_rHbm = nullptr;
		}
		else
		{
			// Get the BITMAPINFO from MIL
			BITMAPINFO* pbmInfo = reinterpret_cast<LPBITMAPINFO>(MbufInquire(p_rFromId.GetIdentifier(), M_BITMAPINFO, M_NULL));
			if (pbmInfo)
			{
				BITMAPINFOHEADER* pbmhInfo = reinterpret_cast<LPBITMAPINFOHEADER>(&pbmInfo->bmiHeader);
			
				// Source images seem to be flipped even though MIL is not supposed to flip them
				if (pbmhInfo->biHeight > 0)
				{
					pbmhInfo->biHeight =  pbmhInfo->biHeight * -1;
				}
			
				// Calculate the absolute height
				long lHeight = pbmhInfo->biHeight * ((pbmhInfo->biHeight < 0) ? -1 : 1);
			
				// Make sure image size is calculated
				if (0 == pbmhInfo->biSizeImage)
				{
					pbmhInfo->biSizeImage = ((((pbmhInfo->biWidth * pbmhInfo->biBitCount) + 31) & ~31) >> 3) * lHeight;
				}
			
				// Make sure color table size is calculated
				long lNumColor = pbmhInfo->biClrUsed;
				if (0 == lNumColor)
				{
					if (pbmhInfo->biBitCount <= Pixel8)
					{
						lNumColor = 1 << pbmhInfo->biBitCount;
					}
				}
				long lTabSize = lNumColor * sizeof(RGBQUAD);
			
				// Calculate total size buffer needed for image
				long lBufSize = sizeof(BITMAPINFOHEADER) + lTabSize + pbmhInfo->biSizeImage;
		
				//-------------------------------------------------------------------------------

				// Get buffer from MIL 
				LPVOID pHostBuffer = reinterpret_cast<LPVOID>(MbufInquire(p_rFromId.GetIdentifier(), M_HOST_ADDRESS, M_NULL));
				if (pHostBuffer)
				{
					p_rHbm = SVImageConvertorGDI::CreateDIB(pbmInfo, pHostBuffer);
				}
				else
				{
					l_Code = E_POINTER;
				}
			}
			else
			{
				// it was either a child buffer or the MIL buffer wasn't created with the M_DIB attribute
				// color bands are handled special, since the color band data needs to be extracted 
				if (IsColorBandBuffer(p_rFromId))
				{
					long width = static_cast<long>(MbufInquire(p_rFromId.GetIdentifier(), M_SIZE_X, M_NULL));
					long height = static_cast<long>(MbufInquire(p_rFromId.GetIdentifier(), M_SIZE_Y, M_NULL));
					SVBitmapInfo bmpInfo(width, -height, Pixel8, SVBitmapInfo::GetDefaultColorTable(Pixel8));
					BITMAPINFO* pbmInfo = bmpInfo.GetBitmapInfo();

					long pitch = static_cast<long>(MbufInquire(p_rFromId.GetIdentifier(), M_PITCH, M_NULL));
					boost::scoped_array<unsigned char> srcPixels(new unsigned char[pitch * abs(pbmInfo->bmiHeader.biHeight)]);
					unsigned char* pSrc = srcPixels.get();
					MbufGetColor(p_rFromId.GetIdentifier(), M_SINGLE_BAND, 0, pSrc);
					p_rHbm = SVImageConvertorGDI::CreateDIB(pbmInfo, pSrc, pitch);
					if (!p_rHbm)
					{
						l_Code = E_HANDLE;
					}
				}
				else if (IsChildBuffer(p_rFromId))
				{
					SVBitmapInfo bmpInfo;
					GetBitmapInfo(bmpInfo, p_rFromId);
					BITMAPINFO* pbmInfo = bmpInfo.GetBitmapInfo();
					long pitch = static_cast<long>(MbufInquire(p_rFromId.GetIdentifier(), M_PITCH, M_NULL));
					boost::scoped_array<unsigned char> srcPixels(new unsigned char[pitch * abs(pbmInfo->bmiHeader.biHeight)]);
					unsigned char* pSrc = srcPixels.get();
					MbufGetColor(p_rFromId.GetIdentifier(), M_SINGLE_BAND, 0, pSrc);
					p_rHbm = SVImageConvertorGDI::CreateDIB(pbmInfo, pSrc, pitch);
					if (!p_rHbm)
					{
						l_Code = E_HANDLE;
					}
				}
				else
				{
					l_Code = E_POINTER;
				}
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName GenLutRamp

@SVOperationDescription This function generates a ramp, inverse ramp, or a constant in the specified LUT buffer region (StartIndex to EndIndex). The increment between LUT entries is the difference between StartValue and EndValue, divided by the number of entries. 

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GenLutRamp(SVMatroxBuffer& p_rMilId, long StartIndex, double StartValue, long EndIndex, double EndValue)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rMilId.empty())
		{
			MgenLutRamp(p_rMilId.GetIdentifier(), StartIndex, StartValue, EndIndex, EndValue);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::IsParent(const SVMatroxBuffer& p_rParentBuffer, const SVMatroxBuffer& p_rChildBuffer)
{
	SVStatusCode l_Status(SVMEE_STATUS_OK);

	if (!(p_rParentBuffer.empty()) && !(p_rChildBuffer.empty()))
	{
		long l_ParentID = 0;

		l_Status = SVMatroxBufferInterface::Get(p_rChildBuffer, SVParentID, l_ParentID);

		if (SVMEE_STATUS_OK == l_Status)
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

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetPositionPoint(POINT& p_rPoint, const SVMatroxBuffer& p_rBuffer)
{
	SVStatusCode l_Status(SVMEE_STATUS_OK);

	SVPOINT l_Point;

	l_Status = GetPositionPoint(l_Point, p_rBuffer);

	p_rPoint = l_Point;

	return l_Status;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetPositionPoint(SVPOINT& p_rPoint, const SVMatroxBuffer& p_rBuffer)
{
	SVStatusCode l_Status(SVMEE_STATUS_OK);

	p_rPoint = SVPOINT(0, 0);

	if (!(p_rBuffer.empty()))
	{
		long l_ParentID = 0;

		l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVParentID, l_ParentID);

		if (SVMEE_STATUS_OK == l_Status)
		{
			if (0 != l_ParentID)
			{
				if (l_ParentID != p_rBuffer.GetIdentifier())
				{
					long l_OffsetX = 0;
					long l_OffsetY = 0;

					l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVParentOffsetX, l_OffsetX);

					if (SVMEE_STATUS_OK == l_Status)
					{
						l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVParentOffsetY, l_OffsetY);
					}

					if (SVMEE_STATUS_OK == l_Status)
					{
						p_rPoint = SVPOINT(static_cast< long >(l_OffsetX), static_cast< long >(l_OffsetY));
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
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetBitmapInfo(LPBITMAPINFO& p_rpBitmapInfo, const SVMatroxBuffer& p_rBuffer)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
	if (!p_rBuffer.empty())
	{
		l_Code = GetBitmapInfo(p_rpBitmapInfo, p_rBuffer.GetIdentifier());
	}
	else
	{
		l_Code = SVMEE_INVALID_HANDLE;
	}
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName GetBitmapInfo

@SVOperationDescription This function returns a pointer (LPBITMAPINFO) to the header of the DIB associated with the MIL buffer (if any) or NULL.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetBitmapInfo(LPBITMAPINFO& p_rpBitmapInfo, SVMatroxIdentifier p_Identifier)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (M_NULL != p_Identifier)
		{
			MbufInquire(p_Identifier, M_BITMAPINFO, &p_rpBitmapInfo);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetBitmapInfo(SVBitmapInfo& p_rBitmapInfo, const SVMatroxBuffer& p_rBuffer)
{
	SVStatusCode l_Status = S_OK;

	BITMAPINFO* l_pBitmapInfo = nullptr;

	p_rBitmapInfo.clear();

	l_Status = GetBitmapInfo(l_pBitmapInfo, p_rBuffer);

	if (SVMEE_STATUS_OK == l_Status && nullptr != l_pBitmapInfo)
	{
		p_rBitmapInfo.Assign(*l_pBitmapInfo);
	}
	else if (!(p_rBuffer.empty())) // no DIB available...
	{
		long l_Width = 0;
		long l_Height = 0;
		unsigned short l_BitCount = 0;

		l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVSizeX, l_Width);

		if (SVMEE_STATUS_OK == l_Status)
		{
			l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVSizeY, l_Height);
		}

		if (SVMEE_STATUS_OK == l_Status)
		{
			long l_PixelDepth = 0;
			long l_BandSize = 0;

			l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVType, l_PixelDepth);

			if (SVMEE_STATUS_OK == l_Status)
			{
				l_Status = SVMatroxBufferInterface::Get(p_rBuffer, SVSizeBand, l_BandSize);
			}

			if (SVMEE_STATUS_OK == l_Status)
			{
				long l_DataFormat = 0;

				if (SVMEE_STATUS_OK == SVMatroxBufferInterface::Get(p_rBuffer, SVDataFormat, l_DataFormat))
				{
						NB_OF_BITS_PER_PIXEL(l_DataFormat, l_BitCount);
				}

				if (l_BitCount < 1)
				{
					l_BitCount = static_cast< unsigned short >(l_PixelDepth * l_BandSize); 
				}
			}
		}

		if (SVMEE_STATUS_OK == l_Status)
		{
			p_rBitmapInfo = SVBitmapInfo(static_cast< long >(l_Width), static_cast< long >(-l_Height), l_BitCount, SVBitmapInfo::GetDefaultColorTable(l_BitCount));
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetBitmapInfo(SVBitmapInfo& p_rBitmapInfo, SVMatroxIdentifier p_Identifier)
{
	BITMAPINFO* l_pBitmapInfo = nullptr;

	p_rBitmapInfo.clear();

	SVStatusCode l_Status = GetBitmapInfo(l_pBitmapInfo, p_Identifier);

	if (SVMEE_STATUS_OK == l_Status && nullptr != l_pBitmapInfo)
	{
		p_rBitmapInfo.Assign(*l_pBitmapInfo);
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

/**
@SVOperationName GetHostAddress

@SVOperationDescription This function returns the Host address of the buffer, if the buffer is visible from the Host address space and is not a planar 3-band buffer

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetHostAddress(LPVOID p_rpHostAddress, const SVMatroxBuffer& p_rBuffer)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rBuffer.empty())
		{
			MIL_ID l_NewBuf = MbufInquire(p_rBuffer.GetIdentifier(),
				M_HOST_ADDRESS,
				p_rpHostAddress);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			assert(p_rpHostAddress);
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetDDSurfaceInterfacePtr(LPVOID& p_rpSurface, const SVMatroxBuffer& p_rBuffer)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rBuffer.empty())
		{
			MIL_ID l_NewBuf = MbufInquire(p_rBuffer.GetIdentifier(),
				M_DDRAW_SURFACE,
				&p_rpSurface);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			assert(p_rpSurface);
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}
 
bool SVMatroxBufferInterface::IsChildBuffer(const SVMatroxBuffer& p_rBuffer)
{
	bool bIsChild = false;
	MIL_ID ancestorID = MbufInquire(p_rBuffer.GetIdentifier(), M_ANCESTOR_ID, M_NULL);
	bIsChild = (M_NULL != ancestorID && ancestorID != p_rBuffer.GetIdentifier());
	return bIsChild;
}

bool SVMatroxBufferInterface::IsColorBandBuffer(const SVMatroxBuffer& p_rBuffer)
{
	bool bIsColorBand = false;
	// Determine if dealing with a single band of a color MIL buffer
	// The Band Size will be 1 and the Ancestor Band Size wil be 3
	long numBands = static_cast<long>(MbufInquire(p_rBuffer.GetIdentifier(), M_SIZE_BAND, M_NULL));
	if (SingleBand == numBands)
	{
		MIL_ID ancestorID = MbufInquire(p_rBuffer.GetIdentifier(), M_ANCESTOR_ID, M_NULL);
		if (M_NULL != ancestorID && ancestorID != p_rBuffer.GetIdentifier())
		{
			bIsColorBand = (MaxColorBands == static_cast<long>(MbufInquire(ancestorID, M_SIZE_BAND, M_NULL)));
		}
	}
	return bIsColorBand;
}

/**
@SVOperationName CopyBuffer (SVMatroxBuffer to SVMatroxBuffer)

@SVOperationDescription This function copies the data from a SVMatroxBuffer to a SVMatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer(SVMatroxBuffer& p_rTo, const SVMatroxBuffer& p_rFrom)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rTo.empty() && !p_rFrom.empty())
		{
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer(SVMatroxBuffer& p_rTo, SVMatroxIdentifier p_From)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer(SVMatroxIdentifier p_To, const SVMatroxBuffer& p_rFrom)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName CopyBuffer (SVMatroxBuffer to SVMatroxDisplayBuffer)

@SVOperationDescription This function copies the data from a SVMatroxDisplayBuffer to a SVMatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer(SVMatroxDisplayBuffer& p_rTo, const SVMatroxBuffer& p_rFrom)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rTo.empty() && !p_rFrom.empty())
		{
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName CopyBuffer (SVMatroxBuffer and offsets to SVMatroxBuffer)

@SVOperationDescription This function copies the data from a SVMatroxBuffer to a SVMatroxBuffer with x and y offsets.  If the destination is not large enough to put the source with its offsets then it is clipped.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer(SVMatroxBuffer& p_rTo, const SVMatroxBuffer& p_rFrom, long p_lXOffset, long p_lYOffset)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName CopyBuffer (SVMatroxBuffer band to SVMatroxBuffer)

@SVOperationDescription This function copies the data from a SVMatroxBuffer to a SVMatroxBuffer with x and y offsets.  If the destination is not large enough to put the source with its offsets then it is clipped.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer(SVMatroxBuffer& p_rTo, const SVMatroxBuffer& p_rFrom, long p_lBand)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName CopyBuffer (HBITMAP to SVMatroxBuffer)

@SVOperationDescription This function copies the data from a HBitmap to a SVMatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer(SVMatroxBuffer& p_rMilId, HBITMAP& p_rHbm)
{	// HBitmapToMilHandle
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		DIBSECTION dib;

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

			l_Code = SVImageConvertorGDI::CopyDIBits(&(dib.dsBmih), dib.dsBm.bmBits, &(pbmInfoMil->bmiHeader), pHostBuffer);
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName CopyBuffer - (SVMatroxBuffer to HBITMAP)

@SVOperationDescription This function copies the data from a SVMatroxBuffer to a HBITMAP.  It is assumed that the MIL handle and HBITMAP are compatible buffer formats.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer(HBITMAP& p_rBitmap, SVMatroxBuffer& p_rFromId)
{	// MilHandleToExistingDIB
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		DIBSECTION dib;

		if (p_rFromId.empty())
		{
			l_Code = S_FALSE;
		}

		if (S_OK == l_Code)
		{
			if (nullptr != p_rBitmap)
			{
				if (0 != ::GetObject(p_rBitmap, sizeof(DIBSECTION), &dib))
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
			BITMAPINFO* pbmInfo = reinterpret_cast<LPBITMAPINFO>(MbufInquire(p_rFromId.GetIdentifier(), M_BITMAPINFO, M_NULL));
			LPVOID pHostBuffer = reinterpret_cast<LPVOID>(MbufInquire(p_rFromId.GetIdentifier(), M_HOST_ADDRESS, M_NULL));

			l_Code = SVImageConvertorGDI::CopyDIBits( &( pbmInfo->bmiHeader ), pHostBuffer, &( dib.dsBmih ), dib.dsBm.bmBits );
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer(SVByteVector& p_rToDIB, const SVMatroxBuffer& p_rFromId)
{
	SVStatusCode l_Code = SVMEE_STATUS_OK;

	SVBitmapInfo l_Info;

	p_rToDIB.clear();

	l_Code = GetBitmapInfo(l_Info, p_rFromId);

	if (SVMEE_STATUS_OK == l_Code)
	{
		void* l_pHostBuffer = reinterpret_cast< void* >(MbufInquire(p_rFromId.GetIdentifier(), M_HOST_ADDRESS, M_NULL));

		if (nullptr != l_pHostBuffer)
		{
			size_t l_InfoSize = l_Info.GetBitmapInfoSizeInBytes();
			size_t l_ImageSize = l_Info.GetBitmapImageSizeInBytes();

			p_rToDIB.resize(l_InfoSize + l_ImageSize, 0);

			::memcpy(&(p_rToDIB[ 0 ]), l_Info.GetBitmapInfo(), l_InfoSize);
			::memcpy(&(p_rToDIB[ l_InfoSize ]), l_pHostBuffer, l_ImageSize);
		}
		else
		{
			SVMatroxBuffer l_ToId;

			l_Code = Create(l_ToId, p_rFromId);

			void* l_pNewHostBuffer = reinterpret_cast< void* >(MbufInquire(l_ToId.GetIdentifier(), M_HOST_ADDRESS, M_NULL));

			if (nullptr != l_pNewHostBuffer)
			{
				l_Code = CopyBuffer(l_ToId, p_rFromId);

				if (SVMEE_STATUS_OK == l_Code)
				{
					l_Code = CopyBuffer(p_rToDIB, l_ToId);
				}
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
	}

	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer(SVByteVector& p_rToDIB, const SVBitmapInfo& p_rToBitmapInfo, const SVMatroxBuffer& p_rFromId)
{
	SVStatusCode l_Code = SVMEE_STATUS_OK;

	SVMatroxBuffer l_ToId;

	l_Code = Create(l_ToId, const_cast< LPBITMAPINFO >(p_rToBitmapInfo.GetBitmapInfo()));

	void* l_pNewHostBuffer = reinterpret_cast< void* >(MbufInquire(l_ToId.GetIdentifier(), M_HOST_ADDRESS, M_NULL));

	if (nullptr != l_pNewHostBuffer)
	{
		l_Code = CopyBuffer(l_ToId, p_rFromId);

		if (SVMEE_STATUS_OK == l_Code)
		{
			l_Code = CopyBuffer(p_rToDIB, l_ToId);
		}
	}
	else
	{
		l_Code = SVMEE_INVALID_HANDLE;
	}

	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer(const SVBitmapInfo& dstInfo, unsigned char* p_rTo, const SVMatroxBuffer& p_rFrom)
{
	return CopyBuffer(dstInfo, p_rTo, p_rFrom.GetIdentifier());
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer(const SVBitmapInfo& dstInfo, unsigned char* p_rTo, const SVMatroxIdentifier& p_rFromId)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);

	SVBitmapInfo l_Info;

	l_Code = GetBitmapInfo(l_Info, p_rFromId);

	if (SVMEE_STATUS_OK == l_Code)
	{
		void* l_pHostBuffer = reinterpret_cast< void* >(MbufInquire(p_rFromId, M_HOST_ADDRESS, M_NULL));

		if (nullptr != l_pHostBuffer)
		{
			size_t l_ImageSize = l_Info.GetBitmapImageSizeInBytes();

			// Should check if source size is same as destination, else only copy the smaller of the 2...
			::memcpy(p_rTo, l_pHostBuffer, l_ImageSize);
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBufferToFileDIB(SVByteVector& p_rToDIB, const SVMatroxBuffer& p_rFromId)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);

	SVBitmapInfo l_Info;

	p_rToDIB.clear();

	l_Code = GetBitmapInfo(l_Info, p_rFromId);

	if (SVMEE_STATUS_OK == l_Code)
	{
		void* l_pHostBuffer = reinterpret_cast< void* >(MbufInquire(p_rFromId.GetIdentifier(), M_HOST_ADDRESS, M_NULL));

		if (nullptr != l_pHostBuffer)
		{
			size_t l_InfoSize = l_Info.GetBitmapInfoSizeInBytes();
			size_t l_ImageSize = l_Info.GetBitmapImageSizeInBytes();

			BITMAPFILEHEADER hdr;

			hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
			hdr.bfSize = static_cast< DWORD >(sizeof(BITMAPFILEHEADER) + l_InfoSize + l_ImageSize);
			hdr.bfReserved1 = 0; 
			hdr.bfReserved2 = 0; 
			hdr.bfOffBits = static_cast< DWORD >(sizeof(BITMAPFILEHEADER) + l_InfoSize);

			p_rToDIB.resize(hdr.bfSize);

			::memcpy(&(p_rToDIB[ 0 ]), &hdr, sizeof(BITMAPFILEHEADER));
			::memcpy(&(p_rToDIB[ sizeof(BITMAPFILEHEADER) ]), l_Info.GetBitmapInfo(), l_InfoSize);

			BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >(&(p_rToDIB[ sizeof(BITMAPFILEHEADER) ]));

			if (l_pBitmapInfo->bmiHeader.biHeight < 0)
			{
				size_t l_Stride = l_Info.GetBitmapImageStrideInBytes();

				unsigned char* l_pFrom = reinterpret_cast< unsigned char* >(l_pHostBuffer);
				unsigned char* l_pTo = reinterpret_cast< unsigned char* >(&(p_rToDIB[ sizeof(BITMAPFILEHEADER) + l_InfoSize ]));

				l_pBitmapInfo->bmiHeader.biHeight = ::labs(l_pBitmapInfo->bmiHeader.biHeight);
				l_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
				l_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
				l_pBitmapInfo->bmiHeader.biClrImportant = 0;

				for(size_t i = 0; i < static_cast< size_t >(l_pBitmapInfo->bmiHeader.biHeight); ++i)
				{
					unsigned char* l_pToRow = l_pTo + (i * l_Stride);
					unsigned char* l_pFromRow = l_pFrom + ((l_pBitmapInfo->bmiHeader.biHeight - 1 - i) * l_Stride);

					::memcpy(l_pToRow, l_pFromRow, l_Stride);
				}
			}
			else
			{
				::memcpy(&(p_rToDIB[ sizeof(BITMAPFILEHEADER) + l_InfoSize ]), l_pHostBuffer, l_ImageSize);
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}

	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyDIBBufferToMemory(SVImageCopyUtility& p_rImageCopier, const SVMatroxBuffer& p_rFromId)
{
	SVBitmapInfo l_Info;
	HRESULT l_Status = SVMatroxBufferInterface::GetBitmapInfo(l_Info, p_rFromId);
	if (l_Status == S_OK)
	{
		void* l_pHostBuffer = reinterpret_cast< void* >(MbufInquire(p_rFromId.GetIdentifier(), M_HOST_ADDRESS, M_NULL));

		if (nullptr != l_pHostBuffer)
		{
			l_Status = p_rImageCopier.Copy(l_Info, l_pHostBuffer);
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	return l_Status;
}

/**
@SVOperationName PutBuffer - const unsigned char*

@SVOperationDescription This function copies the data from a user array to a MatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::PutBuffer(SVMatroxBuffer& p_rTo, 
																		 const unsigned char* p_pcArrayData)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName PutBuffer - const long*

@SVOperationDescription This function copies the data from a user array to a MatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::PutBuffer(SVMatroxBuffer& p_rTo, const long* p_plArrayData)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}


/**
@SVOperationName PutColor

@SVOperationDescription This function copies the data from a user array to a MatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::PutColor(SVMatroxBuffer& p_rTo, 
																		const unsigned char* p_pArrayData)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rTo.empty())
		{
			MbufPutColor(p_rTo.GetIdentifier(),
				M_RGB24+M_PACKED,
				M_ALL_BAND,
				const_cast<unsigned char*>(p_pArrayData));

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName PutLine

@SVOperationDescription This function copies the data from a user array to a MatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::PutLine(SVMatroxBuffer& p_rTo, 
																	   long p_lCount, 
																	   const unsigned char* p_pArrayData)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName GetLine

@SVOperationDescription This function reads the series of pixels along a theoretical line (defined by coordinates) from an image and stores their values in a user-defined array. 

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetLine(SVMatroxBuffer& p_rBuf, 
																	   SVMatroxInt p_lXStart, 
																	   SVMatroxInt p_lYStart, 
																	   SVMatroxInt p_lXEnd, 
																	   SVMatroxInt p_lYEnd, 
																	   SVMatroxInt& p_rlNbrPixels, 
																	   void* p_pUserArray)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVStatusCode l_Code;
		if (!p_rBuf.empty())
		{
	//		long l_lLength;
	//		MbufGetLine(p_rBuf.m_BufferIdentifier,
	//			p_lXStart,
	//			p_lYStart,
	//			p_lXEnd,
	//			p_lYEnd,
	//			M_DEFAULT,
	//			&l_lLength,
	//			M_NULL);
	//		if (p_pUserArray.size() < l_lLength)
	//		{
	//			p_pUserArray.resize(l_lLength);
	//		}

			MbufGetLine(p_rBuf.GetIdentifier(),
				p_lXStart,
				p_lYStart,
				p_lXEnd,
				p_lYEnd,
				M_DEFAULT,
				&p_rlNbrPixels,
				p_pUserArray);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Get - double

@SVOperationDescription This function uses MbufInquire to get information about a SVMatroxBuffer and stores it in a double.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Get(const SVMatroxBuffer& p_rBuf, 
																   SVMatroxBufferInfoEnum p_eWhat, 
																   double& p_rdResult) 
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
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
						&p_rdResult);
				}
				else
				{
					MIL_INT mValue = 0;
					MbufInquire(p_rBuf.GetIdentifier(),
						l_lMatroxType,
						&mValue);
					p_rdResult = static_cast<double>(mValue);
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Get - long

@SVOperationDescription This function uses MbufInquire to get information about a SVMatroxBuffer and stores it in a long.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Get(const SVMatroxBuffer& p_rBuf, 
																   SVMatroxBufferInfoEnum p_eWhat, 
																   long& p_rlResult)  
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rBuf.empty())
		{
			MIL_INT l_lMatroxType = Convert2MatroxType(p_eWhat);
			if (0 != l_lMatroxType)
			{
				SVMatroxInt l_Temp = 0;

				MbufInquire(p_rBuf.GetIdentifier(),
					l_lMatroxType,
					&l_Temp);

				// Matrox uses 64 bits for all parameters, but we probably don't need to worry about more that 32 bits of actual data.
				p_rlResult = static_cast< long >(l_Temp);

				// Break if there is more data here than 32 bits.
				assert(static_cast< SVMatroxInt >(p_rlResult) == l_Temp);

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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Set - double

@SVOperationDescription This function allows you to control a specified data buffer settings with a supplied double. 

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Set(const SVMatroxBuffer& p_rBuf, 
																   SVMatroxBufferInfoEnum p_eWhat, 
																   const double p_rdValue)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Set - long

@SVOperationDescription This function allows you to control a specified data buffer settings with a supplied long. 

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Set(const SVMatroxBuffer& p_rBuf, 
																   SVMatroxBufferInfoEnum p_eWhat, 
																   const SVMatroxInt p_rlValue)
{
	return Set(p_rBuf, p_eWhat, static_cast< const double >(p_rlValue));
}

/**
@SVOperationName ControlNeighborhood

@SVOperationDescription This function changes the setting of an operation control type of the specified kernel buffer or structuring element buffer. 

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::ControlNeighborhood(SVMatroxBuffer& p_rBuf, 
																				   SVMatroxBufferInfoEnum p_eWhat, 
																				   long p_lValue)
{
	SVStatusCode l_Code(SVMEE_STATUS_OK);
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName ClearBuffer - SVMatroxDisplay

@SVOperationDescription This function clears the entire specified buffer to the specified color.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::ClearBuffer(SVMatroxDisplay& p_rBuffer, 
																		   double p_dColor)
{
	SVStatusCode l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rBuffer.empty())
		{
			MbufClear(p_rBuffer.m_DisplayIdentifier, p_dColor);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;

}

/**
@SVOperationName ClearBuffer - SVMatroxBuffer

@SVOperationDescription This function clears the entire specified buffer to the specified color.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::ClearBuffer(SVMatroxBuffer& p_rBuffer, 
																		   double p_dColor)
{
	SVStatusCode l_Code;
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;

}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetImageSize(const SVMatroxString& rFileName, long &rWidth, long &rHeight)
{
	MIL_INT  M_Width(0), M_Height(0);
	SVStatusCode  Code(SVMEE_STATUS_OK);
	if (M_INVALID == MbufDiskInquire(const_cast<MIL_TEXT_CHAR*>(rFileName.c_str()), M_SIZE_X,  &M_Width))
	{
		Code = M_INVALID; 
	}
	if (M_INVALID == MbufDiskInquire(const_cast<MIL_TEXT_CHAR*>(rFileName.c_str()), M_SIZE_Y,  &M_Height))
	{
		Code = M_INVALID; 
	}
	if (SVMEE_STATUS_OK == Code)
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
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Import(SVMatroxBuffer& p_rBuf, 
																	  const SVMatroxString& p_rFileName, 
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
	else if (SVFileMIL == (p_eFileType & SVFileMIL))
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

	SVStatusCode l_Code(SVMEE_STATUS_OK);

#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock(l_AutoLock);

		if (SVMEE_STATUS_OK == l_Code)
		{
			if (p_bRestore)
			{	// Restore Operation creates a new Mil handle.
				MIL_ID l_NewId = M_NULL;
				MbufImport(const_cast<MIL_TEXT_CHAR*>(p_rFileName.c_str()), 
					l_lFileFormat,
					l_lOperation,
					M_DEFAULT_HOST,
					&l_NewId);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if (SVMEE_STATUS_OK == l_Code)
				{
					p_rBuf.m_BufferPtr = new SVMatroxImageBuffer(l_NewId, "SVMatroxBufferInterface::Import");
				}
			}
			else
			{	// Load fills a previously created Mil handle.
				if (!p_rBuf.empty())
				{
					MIL_ID l_NewId = p_rBuf.GetIdentifier();
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
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

/**
@SVOperationName Export

@SVOperationDescription This function exports a data buffer to a file, in the specified output file format. 

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Export(const SVMatroxBuffer& p_rBuf, 
																	  const SVMatroxString& p_rFileName, 
																	  SVMatroxFileTypeEnum p_eFileType)
{
	long l_lFileFormat = 0;

	// File Formats. Currently only Bmps
	if (SVFileBitmap == (p_eFileType & SVFileBitmap))
	{
		l_lFileFormat |= M_BMP;
	}

	SVStatusCode l_Code(SVMEE_STATUS_OK);

#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!p_rBuf.empty())
		{
			MbufExport(const_cast<MIL_TEXT_CHAR*>(p_rFileName.c_str()), 
				l_lFileFormat,
				p_rBuf.GetIdentifier());

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(SVMEE_STATUS_OK == l_Code);
	return l_Code;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBufferInterface.cpp_v  $
 * 
 *    Rev 1.3   29 Jan 2014 10:23:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Use of proper matrox types for Inquire function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 Oct 2013 08:50:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified types to be compatible with 32 and 64bit librarys.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 11:08:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 15:02:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   19 Jul 2012 18:55:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added workaround for MbufAllocColor issue when M_DIB is used.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   18 Jul 2012 14:37:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix a problem with invalid destination buffer type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   29 Jun 2012 14:06:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code needed to implement MIL 9.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   19 Dec 2011 14:12:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update library to include matrox buffer copies to memory and to file bitmap formats.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   06 Oct 2011 08:56:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated matrox copy method to update the pixel data to zero on a buffer resize.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   16 Sep 2011 14:23:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Martox Library to include a new resource management system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   28 Apr 2011 08:47:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Create from HBitmap function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   18 Feb 2011 16:28:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to move data elements to appropriate objects to minimize tight object coupling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   10 Feb 2011 13:57:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   27 Jan 2011 10:31:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   08 Nov 2010 12:57:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to change the height direction for the bitmap data due to invalid returned values from Get Object API call.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   15 Oct 2010 15:00:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include new command functionality using the matrox library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   24 May 2010 14:10:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include file due to changes in the data manager include path update.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Feb 2010 11:32:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added functionality to allow for new OCR training functionality and fixed issues with unicode compiles.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Jun 2009 12:27:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the Create method to use the SVMatroxSystem argument or M_DEFAULT_HOST if the supplied SVMatroxSystem argument is empty
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Jun 2009 15:51:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Create method for Grab buffers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Jul 2008 09:18:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  630
 * SCR Title:  Fix Matrox interface bug in color system
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added ParentBandCount to createChildStruct to determine what kind of buffer to create.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 May 2008 11:25:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Convert2MatroxType.
 * Added const to Export parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Apr 2008 09:07:32   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added handling of SVBufAttRgb16 flag to Convert2MatroxType.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Apr 2008 09:07:28   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added handling of SVBufAttRgb16 flag to Convert2MatroxType.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Dec 2007 14:54:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  608
 * SCR Title:  Fix problems with the ranking filter
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added additional PutBuffer(...,const long* p_plBuffer) overload for Ranking filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:18:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
