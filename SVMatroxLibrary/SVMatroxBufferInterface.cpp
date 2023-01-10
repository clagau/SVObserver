// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVUtilityLibrary/SVBitmapInfo.h"
#include "SVStatusLibrary/MessageManager.h"
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


namespace
{
MIL_INT64 getMilImageFormatCode(ImageFileFormat fileFormat)
{
	switch (fileFormat)
	{
		case ImageFileFormat::invalid:
		case ImageFileFormat::any:
		default:
			return -1;
		case ImageFileFormat::mim:
			return M_MIL;
		case ImageFileFormat::tiff:
			return M_TIFF;
		case ImageFileFormat::bmp:
			return M_BMP;
		case ImageFileFormat::png:
			return M_PNG;
	}
}
}

long SVMatroxBufferInterface::Convert2MatroxType(SVMatroxBufferTypeEnum eType)
{
	long matroxType = 0;

	if (SVBufferSigned == (SVBufferSigned & eType))
	{
		matroxType |= M_SIGNED;
	}

	if (SVBufferUnsigned == (SVBufferUnsigned & eType))
	{
		matroxType |= M_UNSIGNED;
	}

	if (SVBufferFloat == (SVBufferFloat & eType))
	{
		matroxType |= M_FLOAT;
	}

	if (SV1BitUnsigned == (SV1BitUnsigned & eType))
	{
		matroxType |= Pixel1;
	}
	if (SV8BitUnsigned == (SV8BitUnsigned & eType))
	{
		matroxType |= Pixel8;
	}
	if (SV16BitUnsigned == (SV16BitUnsigned & eType))
	{
		matroxType |= Pixel16;
	}
	if (SV32BitUnsigned == (SV32BitUnsigned & eType))
	{
		matroxType |= Pixel32;
	}
	return matroxType;
}

__int64 SVMatroxBufferInterface::Convert2MatroxType(SVMatroxBufferAttributeEnum eType)
{
	__int64 matroxType = 0;

	// Specifies the buffer usage
	if (SVBufAttLut == (eType & SVBufAttLut))
	{
		matroxType |= M_LUT;
	}

	if (SVBufAttImage == (eType & SVBufAttImage))
	{
		matroxType |= M_IMAGE;
	}

	if (SVBufAttKernel == (eType & SVBufAttKernel))
	{
		matroxType |= M_KERNEL;
	}

	if (SVBufAttStructElement == (eType & SVBufAttStructElement))
	{
		matroxType |= M_STRUCT_ELEMENT;
	}

	// Buffer Type
	if (SVBufAttCompress == (eType & SVBufAttCompress))
	{
		matroxType |= M_COMPRESS;
	}

	if (SVBufAttDisp == (eType & SVBufAttDisp))
	{
		matroxType |= M_DISP;
	}

	if (SVBufAttGrab == (eType & SVBufAttGrab))
	{
		matroxType |= M_GRAB;
	}
	if (SVBufAttProc == (eType & SVBufAttProc))
	{
		matroxType |= M_PROC;
	}

	switch (eType & SVBufJpeg)
	{
		// Attributes that go with SVBufAttCompress
		case SVBufAttJpegLossless:
		{
			matroxType |= M_JPEG_LOSSLESS;
			break;
		}
		case SVBufAttJpeg2000Lossless:
		{
			matroxType |= M_JPEG2000_LOSSLESS;
			break;
		}
		case SVBufAttJpegLosslessInterlaced:
		{
			matroxType |= M_JPEG_LOSSLESS_INTERLACED;
			break;
		}
		case SVBufAttJpegLossy:
		{
			matroxType |= M_JPEG_LOSSY;
			break;
		}
		case SVBufAttJpeg2000Lossy:
		{
			matroxType |= M_JPEG2000_LOSSY;
			break;
		}
		case SVBufAttJpegLossyInterlaced:
		{
			matroxType |= M_JPEG_LOSSY_INTERLACED;
			break;
		}
	}

	if (SVBufAttDib == (eType & SVBufAttDib))
	{
		matroxType |= M_DIB;
	}

	if (SVBufAttDirectX == (eType & SVBufAttDirectX))
	{
		matroxType |= M_DIRECTX;
	}

	switch (eType & SVBufLoc)
	{
		// Location for buffer
		case SVBufAttOffBoard:
		{
			matroxType |= M_OFF_BOARD;
			break;
		}
		case SVBufAttOnBoard:
		{
			matroxType |= M_ON_BOARD;
			break;
		}
	}

	switch (eType & SVBufMemType)
	{
		// Type of memory
		case SVBufAttNonPaged:
		{
			matroxType |= M_NON_PAGED;
			break;
		}
		case SVBufAttPaged:
		{
			matroxType |= M_PAGED;
			break;
		}
	}

	if (SVBufAttPacked == (eType & SVBufAttPacked))
	{
		matroxType |= M_PACKED;
	}

	if (SVBufAttPlanar == (eType & SVBufAttPlanar))
	{
		matroxType |= M_PLANAR;
	}

	if (SVBufAttNoFlip == (eType & SVBufAttNoFlip))
	{
		matroxType |= M_NO_FLIP;
	}

	if (SVBufAttRgb16 == (eType & SVBufAttRgb16))
	{
		matroxType |= M_RGB16;
	}

	if (SVBufAttBgr24 == (eType & SVBufAttBgr24))
	{
		matroxType |= M_BGR24;
	}

	if (SVBufAttBgr32 == (eType & SVBufAttBgr32))
	{
		matroxType |= M_BGR32;
	}

	return matroxType;
}


long SVMatroxBufferInterface::Convert2MatroxType(SVMatroxBufferInfoEnum eType)
{
	long matroxType = 0;

	if (SVWindowDibHeader == (eType &  SVWindowDibHeader))
	{
		matroxType |= M_BITMAPINFO;
	}
	if (SVHostAddress == (eType & SVHostAddress))
	{
		matroxType |= M_HOST_ADDRESS;
	}

	if (SVPhysicalAddress == (eType & SVPhysicalAddress))
	{
		matroxType |= M_PHYSICAL_ADDRESS;

	}
	if (SVSizeX == (eType & SVSizeX))
	{
		matroxType |= M_SIZE_X;

	}
	if (SVSizeY == (eType & SVSizeY))
	{
		matroxType |= M_SIZE_Y;

	}
	if (SVSizeBand == (eType & SVSizeBand))
	{
		matroxType |= M_SIZE_BAND;

	}
	if (SVType == (eType & SVType))
	{
		matroxType |= M_TYPE;

	}
	if (SVWindowDC == (eType & SVWindowDC))
	{
		matroxType |= M_DC_HANDLE;

	}
	if (SVAttribute == (eType & SVAttribute))
	{
		matroxType |= M_ATTRIBUTE;

	}
	if (SVDataFormat == (eType & SVDataFormat))
	{
		matroxType |= M_DATA_FORMAT;
	}

	// Kernel Control Flags
	if (SVKernelDefault == (eType & SVKernelDefault))
	{
		matroxType |= M_DEFAULT;

	}
	if (SVAbsoluteValue == (eType & SVAbsoluteValue))
	{
		matroxType |= M_ABSOLUTE_VALUE;

	}
	if (SVNormalization == (eType & SVNormalization))
	{
		matroxType |= M_NORMALIZATION_FACTOR;

	}
	if (SVBufOverScan == (eType & SVBufOverScan))
	{
		matroxType |= M_OVERSCAN;

	}
	if (SVBufSaturation == (eType & SVBufSaturation))
	{
		matroxType |= M_SATURATION;

	}
	if (SVBufWindowDCAlloc == (eType & SVBufWindowDCAlloc))
	{
		matroxType |= M_DC_ALLOC;

	}
	if (SVBufWindowDCFree == (eType & SVBufWindowDCFree))
	{
		matroxType |= M_DC_FREE;

	}
	if (SVBufModified == (eType & SVBufModified))
	{
		matroxType |= M_MODIFIED;
	}
	if (SVDirectXSurface == (eType & SVDirectXSurface))
	{
		matroxType |= M_DDRAW_SURFACE;
	}

	// Combination Constants.
	if (SVPitch == (SVPitch & eType))
	{
		matroxType |= M_PITCH;
	}
	if (SVPitchByte == (SVPitchByte & eType))
	{
		matroxType |= M_PITCH_BYTE;
	}

	// Parent Info
	if (SVParentID == (SVParentID & eType))
	{
		matroxType |= M_PARENT_ID;
	}
	if (SVParentOffsetX == (SVParentOffsetX & eType))
	{
		matroxType |= M_PARENT_OFFSET_X;
	}
	if (SVParentOffsetY == (SVParentOffsetY & eType))
	{
		matroxType |= M_PARENT_OFFSET_Y;
	}
	return matroxType;
}


HRESULT SVMatroxBufferInterface::Create(SVMatroxBuffer& rBuffer, const SVMatroxBufferCreateLineStruct& CreateLineStruct)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long type = Convert2MatroxType(CreateLineStruct.m_eType);
		__int64 attribute = Convert2MatroxType(CreateLineStruct.m_eAttribute);

		if (0 != type && 0 != attribute)
		{
			MIL_ID newID = MbufAlloc1d(M_DEFAULT_HOST,
				CreateLineStruct.m_lSizeX,
				type,
				attribute,
				M_NULL);
			code = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK == code)
			{
				createImageBufferPtr(rBuffer, newID, std::string(_T("SVMatroxBufferInterface::Create-CreateLine")));
			}

		}
		else
		{
			code = SVMEE_INVALID_PARAMETER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}

HRESULT SVMatroxBufferInterface::Create(const SVMatroxSystem& rSystem, SVMatroxBuffer& rBuffer, const SVMatroxBufferCreateStruct& CreateStruct)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long type = Convert2MatroxType(CreateStruct.m_eType);
		__int64 attribute = Convert2MatroxType(CreateStruct.m_eAttribute);

		if (0 != type && 0 != attribute)
		{

			MIL_ID MilSystemID = rSystem.empty() ? M_DEFAULT_HOST : rSystem.m_SystemIdentifier;
			MIL_ID newID = M_NULL;
			if (MaxColorBands == CreateStruct.m_lSizeBand)
			{
				if (M_LUT == (attribute & M_LUT))	// Mil help states that LUT must be stored in planar format.
				{
					attribute |= M_PLANAR;
					if (M_DIB == (attribute & M_DIB))
					{
						// Remove M_DIB as it is not allowed with M_PLANAR
						attribute ^= M_DIB;
					}
				}

				if (M_DIB == (attribute & M_DIB))
				{
					attribute |= M_PACKED + M_BGR32;
				}

				newID = MbufAllocColor(MilSystemID,
					CreateStruct.m_lSizeBand,
					CreateStruct.m_lSizeX,
					CreateStruct.m_lSizeY,
					type,
					attribute,
					M_NULL);
			}
			else
			{
				newID = MbufAlloc2d(MilSystemID,
					CreateStruct.m_lSizeX,
					CreateStruct.m_lSizeY,
					type,
					attribute,
					M_NULL);
			}
			code = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK == code)
			{
				createImageBufferPtr(rBuffer, newID, std::string(_T("SVMatroxBufferInterface::Create-SystemID,BufferCreate")));
			}

		}
		else
		{
			code = SVMEE_INVALID_PARAMETER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}

HRESULT SVMatroxBufferInterface::Create(SVMatroxBuffer& rBuffer, const SVMatroxBufferCreateStruct& CreateStruct)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long type = Convert2MatroxType(CreateStruct.m_eType);
		__int64 attribute = Convert2MatroxType(CreateStruct.m_eAttribute);

		if (0 != type && 0 != attribute)
		{
			MIL_ID newID = M_NULL;
			if (MaxColorBands == CreateStruct.m_lSizeBand)
			{
				if (M_LUT == (attribute & M_LUT))	// Mil help states that LUT must be stored in planar format.
				{
					attribute |= M_PLANAR;
					// Planar cannot be used with DIB
					attribute &= ~M_DIB;
				}
				newID = MbufAllocColor(M_DEFAULT_HOST,
					CreateStruct.m_lSizeBand,
					CreateStruct.m_lSizeX,
					CreateStruct.m_lSizeY,
					type,
					attribute,
					M_NULL);
			}
			else
			{
				newID = MbufAlloc2d(M_DEFAULT_HOST,
					CreateStruct.m_lSizeX,
					CreateStruct.m_lSizeY,
					type,
					attribute,
					M_NULL);
			}
			code = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK == code)
			{
				createImageBufferPtr(rBuffer, newID, std::string(_T("SVMatroxBufferInterface::Create-BufferCreate")));
			}

		}
		else
		{
			code = SVMEE_INVALID_PARAMETER;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}

HRESULT SVMatroxBufferInterface::Create(SVMatroxBuffer& rBuffer, const SVMatroxBufferCreateChildStruct& CreateChildStruct)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!CreateChildStruct.m_rParentBufId.empty())
		{
			MIL_ID childBufID(M_NULL);
			MIL_ID parentBufID = CreateChildStruct.m_rParentBufId.GetIdentifier();

			if (CreateChildStruct.m_data.m_lParentBandCount > 1)
			{
				// From the Matrox Imaging Library Help file for MbufChildColor2d, the Band parameter specifies the index of the band to use. 
				// Valid index values are from 0 to (number of bands of the buffer - 1). 
				// Band 0 corresponds to: the red band (for RGB parent buffers), the hue band (for HSL parent buffers), and the Y band (for YUV parent buffers). 
				// Band 1 corresponds to: the green band (for RGB parent buffers), the saturation band (for HSL parent buffers), and the U band (for YUV parent buffers). 
				// Band 2 corresponds to: the blue band (for RGB parent buffers), the luminance band (for HSL parent buffers), and the V band (for YUV parent buffers). 
				childBufID = MbufChildColor2d(parentBufID,
					CreateChildStruct.m_data.m_lBand,
					CreateChildStruct.m_data.m_lOffX,
					CreateChildStruct.m_data.m_lOffY,
					CreateChildStruct.m_data.m_lSizeX,
					CreateChildStruct.m_data.m_lSizeY,
					M_NULL);
			}
			else
			{
				childBufID = MbufChild2d(parentBufID,
					CreateChildStruct.m_data.m_lOffX,
					CreateChildStruct.m_data.m_lOffY,
					CreateChildStruct.m_data.m_lSizeX,
					CreateChildStruct.m_data.m_lSizeY,
					M_NULL);
			}

			code = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK == code)
			{
				if (!rBuffer.empty())
				{
					rBuffer.clear();
				}
				rBuffer.m_BufferPtr = SVMatroxBufferPtr {std::make_shared<SVMatroxImageChildBuffer>(CreateChildStruct.m_rParentBufId.m_BufferPtr, childBufID, "SVMatroxBufferInterface::Create-CreateChildBuffer")};
			}

		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
//	Log_Assert(S_OK == code);
	return code;
}

// MbufCreateColor -  Caution	 : Uses external data buffer that you must manage.
HRESULT SVMatroxBufferInterface::Create(SVMatroxBuffer& rBuffer, SVMatroxBufferCreateExtStruct CreateColorStruct)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long type = Convert2MatroxType(CreateColorStruct.m_eType);
		__int64 attribute = Convert2MatroxType(CreateColorStruct.m_eAttribute);

		if (0 != type && 0 != attribute)
		{

			MIL_ID newID = M_NULL;
			if (CreateColorStruct.m_lSizeBand > 1)
			{
				newID = MbufCreateColor(M_DEFAULT_HOST,
					CreateColorStruct.m_lSizeBand,
					CreateColorStruct.m_lSizeX,
					CreateColorStruct.m_lSizeY,
					type,
					attribute,
					M_HOST_ADDRESS + M_PITCH,
					CreateColorStruct.m_PitchWidth,
					reinterpret_cast<void**>(CreateColorStruct.m_ppArrayOfDataPtr),
					M_NULL);
			}
			else
			{
				attribute &= ~M_PACKED;  //  Cannot have M_PACKED with a mono buffer.
				if (M_IS_FORMAT_RGB_BGR(attribute))
				{
					attribute &= ~0xff00;  // remove rgb or bgr attribute
				}
				newID = MbufCreate2d(M_DEFAULT_HOST,
					CreateColorStruct.m_lSizeX,
					CreateColorStruct.m_lSizeY,
					type,
					attribute,
					M_HOST_ADDRESS + M_PITCH,
					CreateColorStruct.m_PitchWidth,
					reinterpret_cast<void*>(*CreateColorStruct.m_ppArrayOfDataPtr),
					M_NULL);
			}

			code = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK == code)
			{
				createImageBufferPtr(rBuffer, newID, std::string(_T("SVMatroxBufferInterface::Create-BufferCreateExt")));
			}

		}
		else
		{
			code = SVMEE_INVALID_PARAMETER;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}

HRESULT SVMatroxBufferInterface::Create(SVMatroxBuffer& rBuffer, const SVMatroxBuffer& CreateFrom, bool addDibFlag)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT type;
		MIL_INT64 attribute;
		MIL_INT l_lBandSize;
		MIL_INT l_lSizeX;
		MIL_INT l_lSizeY;

		// Clear first error...
		/*code = */SVMatroxApplicationInterface::GetFirstError();
		MbufInquire(CreateFrom.GetIdentifier(), M_SIZE_BAND, &l_lBandSize);
		MbufInquire(CreateFrom.GetIdentifier(), M_SIZE_X, &l_lSizeX);
		MbufInquire(CreateFrom.GetIdentifier(), M_SIZE_Y, &l_lSizeY);
		MbufInquire(CreateFrom.GetIdentifier(), M_TYPE, &type);
		MbufInquire(CreateFrom.GetIdentifier(), M_EXTENDED_ATTRIBUTE, &attribute);
		if (addDibFlag)
		{
			attribute |= M_DIB;
		}

		code = SVMatroxApplicationInterface::GetFirstError();

		if (S_OK == code)
		{
			MIL_ID newID = M_NULL;
			if (MaxColorBands == l_lBandSize)
			{
				newID = MbufAllocColor(M_DEFAULT_HOST,
					l_lBandSize,
					l_lSizeX,
					l_lSizeY,
					type,
					attribute,
					M_NULL);
			}
			else
			{
				attribute &= ~M_PACKED;  //  Cannot have M_PACKED with a mono buffer.
				if (M_IS_FORMAT_RGB_BGR(attribute))
				{
					attribute &= ~0xff00;  // remove rgb or bgr attribute
				}
				newID = MbufAlloc2d(M_DEFAULT_HOST,
					l_lSizeX,
					l_lSizeY,
					type,
					attribute,
					M_NULL);
			}

			code = SVMatroxApplicationInterface::GetLastStatus();

			if (S_OK == code)
			{
				createImageBufferPtr(rBuffer, newID, std::string(_T("SVMatroxBufferInterface::Create-MatroxBuffer")));
			}

		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}


HRESULT SVMatroxBufferInterface::Create(SVMatroxBuffer& rBuffer, HBITMAP& rHbm)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (nullptr != rHbm)
		{
			long l_Width(0);
			long l_Height(0);
			long l_SizeImage(0);
			unsigned short l_BitCount(0);

			DIBSECTION dib;
			memset(&dib, 0, sizeof(dib));
			dib.dsBmih.biSize = sizeof(BITMAPINFOHEADER);

			int cnt = ::GetObject(rHbm, sizeof(dib), &dib);
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

			MIL_ID newID = M_NULL;

			// Allocate a multi or single band image buffer
			MbufAllocColor(M_DEFAULT_HOST,
				l_BitCount <= Pixel8 ? SingleBand : MaxColorBands,
				l_Width,
				l_Height,
				type,
				lAttrib,
				&newID);

			SVMatroxApplicationInterface::GetLastStatus();

			code = (newID != M_NULL) ? S_OK : S_FALSE;

			if (S_OK == code)
			{
				 LPVOID pHostBuffer = (LPVOID)MbufInquire(newID, M_HOST_ADDRESS, M_NULL);

				createImageBufferPtr(rBuffer, newID, std::string(_T("SVMatroxBufferInterface::Create-HBITMAP")));

				// Set the bits of our new bitmap
				if (dib.dsBm.bmBits)
				{
					memcpy(pHostBuffer, dib.dsBm.bmBits, l_SizeImage);
				}
				else
				{
					/*long cnt = */::GetBitmapBits(rHbm, l_SizeImage, pHostBuffer);
				}
			}

		}// end if (nullptr != rHbm)
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
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

HRESULT SVMatroxBufferInterface::IsParent(const SVMatroxBuffer& rParentBuffer, const SVMatroxBuffer& rChildBuffer)
{
	HRESULT l_Status(S_OK);

	if (!(rParentBuffer.empty()) && !(rChildBuffer.empty()))
	{
		long l_ParentID = 0;

		l_Status = SVMatroxBufferInterface::Get(rChildBuffer, SVParentID, l_ParentID);

		if (S_OK == l_Status)
		{
			if (l_ParentID != rParentBuffer.GetIdentifier())
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

HRESULT SVMatroxBufferInterface::GetPositionPoint(POINT& rPoint, const SVMatroxBuffer& rBuffer)
{
	HRESULT l_Status(S_OK);

	SVPoint<long> point;

	l_Status = GetPositionPoint(point, rBuffer);

	rPoint = static_cast<POINT> (point);

	return l_Status;
}

HRESULT SVMatroxBufferInterface::GetPositionPoint(SVPoint<long>& rPoint, const SVMatroxBuffer& rBuffer)
{
	HRESULT l_Status(S_OK);

	rPoint = SVPoint<long> {};

	if (!(rBuffer.empty()))
	{
		long l_ParentID = 0;

		l_Status = SVMatroxBufferInterface::Get(rBuffer, SVParentID, l_ParentID);

		if (S_OK == l_Status)
		{
			if (0 != l_ParentID)
			{
				if (l_ParentID != rBuffer.GetIdentifier())
				{
					long offsetX {0L};
					long offsetY {0L};

					l_Status = SVMatroxBufferInterface::Get(rBuffer, SVParentOffsetX, offsetX);

					if (S_OK == l_Status)
					{
						l_Status = SVMatroxBufferInterface::Get(rBuffer, SVParentOffsetY, offsetY);
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

HRESULT SVMatroxBufferInterface::GetBitmapInfo(LPBITMAPINFO& rpBitmapInfo, const SVMatroxBuffer& rBuffer)
{
	HRESULT code(S_OK);
	MIL_INT	ret = 0; 
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		
		if (!rBuffer.empty())
		{
			ret = MbufInquire(rBuffer.GetIdentifier(), M_BITMAPINFO, &rpBitmapInfo);

			code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	if (ret == M_ERROR || S_OK != code)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		SvDef::StringVector msgList;
		msgList.push_back( " Error in SVMatroxBufferInterface::GetBitmapInfo:");
		Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
		if(S_OK == code)
		{
			code = E_FAIL;
		}	
	}
	return code;
}

HRESULT SVMatroxBufferInterface::GetBitmapInfo(SVBitmapInfo& rBitmapInfo, const SVMatroxBuffer& rBuffer, bool* pIsMilInfo)
{
	bool isMilInfo = false;

	BITMAPINFO* l_pBitmapInfo = nullptr;

	rBitmapInfo.clear();

	HRESULT l_Status = GetBitmapInfo(l_pBitmapInfo, rBuffer);

	if (S_OK == l_Status && nullptr != l_pBitmapInfo)
	{
		rBitmapInfo.Assign(*l_pBitmapInfo);
		isMilInfo = true;
	}
	else if (!(rBuffer.empty())) // no DIB available...
	{
		long l_Width = 0;
		long l_Height = 0;
		int l_BitCount = 0;

		l_Status = SVMatroxBufferInterface::Get(rBuffer, SVSizeX, l_Width);

		if (S_OK == l_Status)
		{
			l_Status = SVMatroxBufferInterface::Get(rBuffer, SVSizeY, l_Height);
		}

		if (S_OK == l_Status)
		{
			long l_PixelDepth = 0;
			long l_BandSize = 0;

			l_Status = SVMatroxBufferInterface::Get(rBuffer, SVType, l_PixelDepth);

			if (S_OK == l_Status)
			{
				l_Status = SVMatroxBufferInterface::Get(rBuffer, SVSizeBand, l_BandSize);
			}

			if (S_OK == l_Status)
			{
				long l_DataFormat = 0;

				__int64 parentId = M_NULL;
				MbufInquire(rBuffer.GetIdentifier(), M_PARENT_ID, &parentId);

				if (S_OK == SVMatroxBufferInterface::Get(rBuffer, SVDataFormat, l_DataFormat))
				{
					NB_OF_BITS_PER_PIXEL(l_DataFormat, l_BitCount);
				}

				if (l_BitCount < 1 || rBuffer.GetIdentifier() != parentId)
				{
					l_BitCount = static_cast<int>(l_PixelDepth * l_BandSize);
				}
			}
		}

		if (S_OK == l_Status)
		{
			rBitmapInfo = SVBitmapInfo(static_cast<long>(l_Width), static_cast<long>(-l_Height), l_BitCount, SVBitmapInfo::GetDefaultColorTable(l_BitCount));
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

HRESULT SVMatroxBufferInterface::CopyBuffer(const SVMatroxBuffer& rTo, const SVMatroxBuffer& rFrom)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rTo.empty() && !rFrom.empty())
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROX_FUNCTION)		
			TCHAR buf[1024];
			_stprintf_s(buf, 1024, _T("copy   From  %i To  %i \n"), int(rFrom.GetIdentifier()), int(rTo.GetIdentifier()));
			::OutputDebugString(buf);
#endif 			
			MbufCopy(rFrom.GetIdentifier(),
				rTo.GetIdentifier());

			code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
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
		SvStl::MessageContainer message(SVMSG_SVO_5079_CREATEBUFFERFAILED, SvStl::Tid_InvalidMemoryPointer, SvStl::SourceFileParams(StdMessageParams));
		throw message;
	}
	else if (rImageProps.Matrox_type == 0)
	{
		SvStl::MessageContainer message(SVMSG_SVO_5079_CREATEBUFFERFAILED, SvStl::Tid_InvalidMatroxType, SvStl::SourceFileParams(StdMessageParams));
		throw message;
	}
	else if (rImageProps.Attrib == 0)
	{
		SvStl::MessageContainer message(SVMSG_SVO_5079_CREATEBUFFERFAILED, SvStl::Tid_InvalidMatroxAttribute, SvStl::SourceFileParams(StdMessageParams));
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
		msg.push_back(std::format("Matrox Code: {:#08x}", Code));
		SvStl::MessageContainer message(SVMSG_SVO_5079_CREATEBUFFERFAILED, SvStl::Tid_Default, msg, SvStl::SourceFileParams(StdMessageParams));
		throw message;
	}


	Log_Assert(S_OK == Code);
	return Code;
}


HRESULT SVMatroxBufferInterface::CopyBuffer(const SVMatroxBuffer& rTo, __int64 From)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rTo.empty() && 0 != From)
		{
			MbufCopy(From, rTo.GetIdentifier());

			code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}

HRESULT SVMatroxBufferInterface::CopyBuffer(__int64 To, const SVMatroxBuffer& rFrom)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (0 != To && !rFrom.empty())
		{
			MbufCopy(rFrom.GetIdentifier(), To);

			code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}


HRESULT SVMatroxBufferInterface::CopyBuffer(const SVMatroxBuffer& rTo, const SVMatroxBuffer& rFrom, long lXOffset, long lYOffset)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rTo.empty() && !rFrom.empty())
		{
			MbufCopyClip(rFrom.GetIdentifier(),
				rTo.GetIdentifier(),
				lXOffset,
				lYOffset);

			code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}

HRESULT SVMatroxBufferInterface::CopyBuffer(const SVMatroxBuffer& rTo, const SVMatroxBuffer& rFrom, long lBand)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rTo.empty() && !rFrom.empty())
		{
			MbufCopyColor(rFrom.GetIdentifier(),
				rTo.GetIdentifier(),
				lBand);

			code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}


HRESULT SVMatroxBufferInterface::CopyBuffer(const SVMatroxBuffer& rMilId, HBITMAP& rHbm)
{	// HBitmapToMilHandle
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		DIBSECTION dib;
		memset(&dib, 0, sizeof(dib));

		if (rMilId.empty())
		{
			code = SVMEE_INVALID_HANDLE;
		}

		if (S_OK == code)
		{
			if (nullptr != rHbm)
			{
				if (0 != ::GetObject(rHbm, sizeof(DIBSECTION), &dib))
				{
					// see article Q186586; GetObject(DIBSECTION) always returns positive height
					// assume source is standard orientation (negative)
					dib.dsBmih.biHeight = -(abs(dib.dsBmih.biHeight));
				}
				else
				{
					code = S_FALSE;
				}
			}
			else
			{
				code = S_FALSE;
			}
		}

		if (S_OK == code)
		{
			// Get the BITMAPINFO from MIL
			BITMAPINFO* pbmInfoMil = reinterpret_cast<LPBITMAPINFO>(MbufInquire(rMilId.GetIdentifier(), M_BITMAPINFO, M_NULL));
			LPVOID pHostBuffer = reinterpret_cast<LPVOID>(MbufInquire(rMilId.GetIdentifier(), M_HOST_ADDRESS, M_NULL));
			if (pbmInfoMil && pHostBuffer)
			{
				code = SVImageConvertorGDI::CopyDIBits(&(dib.dsBmih), dib.dsBm.bmBits, &(pbmInfoMil->bmiHeader), pHostBuffer);
			}
			else
			{
				// No BitmapInfo, either it wasn't created with M_DIB or it is a child buffer on a color/mono image
				// Don't copy into a child buffer, as it will also update the parent ?
				code = S_FALSE;
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
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
						
			Log_Assert(bufferVector.size() == fabs(rBitMapInfo.GetHeight()*rBitMapInfo.GetWidth()*(rBitMapInfo.GetBitCount() / 8)));
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


HRESULT SVMatroxBufferInterface::PutBuffer(const SVMatroxBuffer& rTo,
	const unsigned char* pcArrayData)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rTo.empty())
		{
			MbufPut(rTo.GetIdentifier(),
				const_cast<unsigned char*>(pcArrayData));

			code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}

HRESULT SVMatroxBufferInterface::PutBuffer(const SVMatroxBuffer& rTo, const long* plArrayData)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rTo.empty())
		{
			MbufPut(rTo.GetIdentifier(),
				const_cast<long*>(plArrayData));

			code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}


HRESULT SVMatroxBufferInterface::PutLine(const SVMatroxBuffer& rTo,
	long lCount,
	const unsigned char* pArrayData)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rTo.empty())
		{
			MbufPut1d(rTo.GetIdentifier(),
				0,
				lCount,
				const_cast<unsigned char*>(pArrayData));

			code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}


HRESULT SVMatroxBufferInterface::Get(const SVMatroxBuffer& rBuf, SVMatroxBufferInfoEnum eWhat, double& rResult)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rBuf.empty())
		{
			MIL_INT matroxType = Convert2MatroxType(eWhat);
			if (0 != matroxType)
			{
				// MbufInquire is expecting MIL_INT except for M_MIN or M_MAX will use double
				if (M_MIN == (matroxType & M_MIN) || M_MAX == (matroxType & M_MAX))
				{
					MbufInquire(rBuf.GetIdentifier(),
						matroxType,
						&rResult);
				}
				else
				{
					MIL_INT mValue = 0;
					MbufInquire(rBuf.GetIdentifier(),
						matroxType,
						&mValue);
					rResult = static_cast<double>(mValue);
				}
				code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				code = SVMEE_INVALID_PARAMETER;
			}
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}


HRESULT SVMatroxBufferInterface::Get(const SVMatroxBuffer& rBuf, SVMatroxBufferInfoEnum eWhat, LONGLONG& rResult)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rBuf.empty())
		{
			MIL_INT matroxType = Convert2MatroxType(eWhat);
			if (0 != matroxType)
			{
				MbufInquire(rBuf.GetIdentifier(),
					matroxType,
					&rResult);

				code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				code = SVMEE_INVALID_PARAMETER;
			}
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}

//! This method is required for backward compatibility the LONGLONG version is required when getting the hDC
HRESULT SVMatroxBufferInterface::Get(const SVMatroxBuffer& rBuf, SVMatroxBufferInfoEnum eWhat, long& rResult)
{
	HRESULT code(S_OK);

	LONGLONG Value;

	code = SVMatroxBufferInterface::Get(rBuf, eWhat, Value);
	rResult = static_cast<long> (Value);

	return code;
}

HRESULT SVMatroxBufferInterface::Set(const SVMatroxBuffer& rBuf,
	SVMatroxBufferInfoEnum eWhat,
	const double rdValue)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rBuf.empty())
		{
			long matroxType = Convert2MatroxType(eWhat);
			if (0 != matroxType)
			{
				MbufControl(rBuf.GetIdentifier(),
					matroxType,
					rdValue);

				code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				code = SVMEE_INVALID_PARAMETER;
			}
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}

HRESULT SVMatroxBufferInterface::Set(const SVMatroxBuffer& rBuf,
	SVMatroxBufferInfoEnum eWhat,
	const long long rlValue)
{
	return Set(rBuf, eWhat, static_cast<const double>(rlValue));
}
HRESULT SVMatroxBufferInterface::ControlNeighborhood(const SVMatroxBuffer& rBuf,
	SVMatroxBufferInfoEnum eWhat,
	long lValue)
{
	HRESULT code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rBuf.empty())
		{
			long matroxType = Convert2MatroxType(eWhat);
			if (0 != matroxType)
			{
				MbufControlNeighborhood(rBuf.GetIdentifier(),
					matroxType,
					lValue);

				code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				code = SVMEE_INVALID_PARAMETER;
			}
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}


HRESULT SVMatroxBufferInterface::ClearBuffer(const SVMatroxBuffer& rBuffer, double dColor)
{
	HRESULT code;
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rBuffer.empty())
		{
			MbufClear(rBuffer.GetIdentifier(), dColor);

			code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;

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

HRESULT SVMatroxBufferInterface::Import(SVMatroxBuffer& rBuffer,
	const std::string& rFileName,
	ImageFileFormat fileFormat,
	bool restore)
{
	long operation = restore ? M_RESTORE : M_LOAD;

	// Operations

	HRESULT code(S_OK);

#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (restore)
		{	// Restore creates a new Mil handle.
			MIL_ID newID = M_NULL;
			MbufImport(const_cast<MIL_TEXT_CHAR*>(rFileName.c_str()),
				getMilImageFormatCode(fileFormat),
				operation,
				M_DEFAULT_HOST,
				&newID);
			code = SVMatroxApplicationInterface::GetLastStatus();
			if (S_OK == code)
			{
				createImageBufferPtr(rBuffer, newID, std::string(_T("SVMatroxBufferInterface::Import")));
			}
		}
		else
		{	// Load fills a previously created Mil handle.
			if (!rBuffer.empty())
			{
				MIL_ID l_NewId = rBuffer.GetIdentifier();
				MbufImport(const_cast<MIL_TEXT_CHAR*>(rFileName.c_str()),
					getMilImageFormatCode(fileFormat),
					operation,
					M_DEFAULT_HOST,
					&l_NewId);
				code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				code = SVMEE_INVALID_HANDLE;
			}
		}

	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}

HRESULT SVMatroxBufferInterface::Export(const SVMatroxBuffer& rBuffer,
	const std::string& rFileName,
	ImageFileFormat fileFormat)
{
	HRESULT code(S_OK);

#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (!rBuffer.empty())
		{
			MbufExport(rFileName.c_str(), getMilImageFormatCode(fileFormat), rBuffer.GetIdentifier());

			code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	Log_Assert(S_OK == code);
	return code;
}

void SVMatroxBufferInterface::createImageBufferPtr(SVMatroxBuffer& rBuffer, __int64 MatroxID, const std::string& rCreatorName)
{
	if (!rBuffer.empty())
	{
		rBuffer.clear();
	}
	rBuffer.m_BufferPtr = SVMatroxBufferPtr {std::make_shared<SVMatroxImageBuffer>(MatroxID, rCreatorName)};
}
