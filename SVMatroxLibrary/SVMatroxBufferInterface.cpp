// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBufferInterface
// * .File Name       : $Workfile:   SVMatroxBufferInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   23 Oct 2013 08:50:48  $
// ******************************************************************************

#include "stdafx.h"
#include <assert.h>
#include "SVUtilityLibrary/SVImageCopyUtility.h"
#include "SVMatroxBufferInterface.h"
#include "SVMatroxImageBuffer.h"
#include "SVMatroxImageChildBuffer.h"
#include "SVSystemLibrary/SVImageConvertorGDI.h"
#include "SVMatroxDisplay.h"
#include "SVMatroxImagingLibrary.h"  // has MIL includes
#include "SVMatroxResourceMonitor.h"

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
long SVMatroxBufferInterface::Convert2MatroxType( SVMatroxBufferTypeEnum p_eType )  
{
	long l_lMatroxType = 0;
		;
	if(( SVBufferSigned & p_eType ) == SVBufferSigned )
	{
		l_lMatroxType |= M_SIGNED;
	}

	if( (SVBufferUnsigned & p_eType ) == SVBufferUnsigned)
	{
		l_lMatroxType |= M_UNSIGNED ;
	}

	if(( SVBufferFloat & p_eType ) == SVBufferFloat)
	{
		l_lMatroxType |= M_FLOAT;
	}
	if(( SVBufferDouble & p_eType ) == SVBufferDouble)
	{
		l_lMatroxType |= M_DOUBLE;
	}

	if( (SV1BitUnsigned & p_eType ) == SV1BitUnsigned)
	{
		l_lMatroxType |= 1;
	}
	if(( SV8BitUnsigned & p_eType ) == SV8BitUnsigned)
	{
		l_lMatroxType |= 8;
	}
	if(( SV16BitUnsigned & p_eType ) == SV16BitUnsigned)
	{
		l_lMatroxType |= 16;
	}
	if(( SV32BitUnsigned & p_eType ) == SV32BitUnsigned)
	{
		l_lMatroxType |= 32;
	}
	return l_lMatroxType;
}

/**
@SVOperationName Convert2MatroxType - SVMatroxBufferAttributeEnum

@SVOperationDescription This function converts the SVMatroxBufferTypeEnum to a Matrox Type.

*/
__int64 SVMatroxBufferInterface::Convert2MatroxType( SVMatroxBufferAttributeEnum p_eType )
{
	__int64 l_lMatroxType = 0;

	// Specifies the buffer usage
	if( (p_eType & SVBufAttLut) == SVBufAttLut )
	{
		l_lMatroxType |= M_LUT;
	}

	if( (p_eType & SVBufAttImage) == SVBufAttImage )
	{
		l_lMatroxType |= M_IMAGE;
	}

	if( (p_eType & SVBufAttKernel) == SVBufAttKernel )
	{
		l_lMatroxType |= M_KERNEL;
	}

	if( (p_eType & SVBufAttStructElement) == SVBufAttStructElement )
	{
		l_lMatroxType |= M_STRUCT_ELEMENT;
	}

	// Buffer Type
	if( (p_eType & SVBufAttCompress) == SVBufAttCompress )
	{
		l_lMatroxType |= M_COMPRESS;
	}

	if( (p_eType & SVBufAttDisp) == SVBufAttDisp )
	{
		l_lMatroxType |= M_DISP;
	}

	if( (p_eType & SVBufAttGrab) == SVBufAttGrab )
	{
		l_lMatroxType |= M_GRAB;
	}
	if( (p_eType & SVBufAttProc) == SVBufAttProc )
	{
		l_lMatroxType |= M_PROC;
	}

	switch( p_eType & SVBufJpeg )
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

	if( (p_eType & SVBufAttDib) == SVBufAttDib )
	{
		l_lMatroxType |= M_DIB;
	}

	if( (p_eType & SVBufAttDirectX) == SVBufAttDirectX )
	{
		l_lMatroxType |= M_DIRECTX;
	}

	switch( p_eType & SVBufLoc)
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

	switch( p_eType & SVBufMemType )
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

	if( (p_eType & SVBufAttPacked) == SVBufAttPacked )
	{
		l_lMatroxType |= M_PACKED;
	}

	if( (p_eType & SVBufAttPlanar) == SVBufAttPlanar )
	{
		l_lMatroxType |= M_PLANAR;
	}

	if( (p_eType & SVBufAttNoFlip) == SVBufAttNoFlip )
	{
		l_lMatroxType |= M_NO_FLIP;
	}

	if( ( p_eType & SVBufAttRgb16 ) == SVBufAttRgb16 )
	{
		l_lMatroxType |= M_RGB16;
	}

	if( (p_eType & SVBufAttBgr24) == SVBufAttBgr24 )
	{
		l_lMatroxType |= M_BGR24;
	}

	if( (p_eType & SVBufAttBgr32) == SVBufAttBgr32 )
	{
		l_lMatroxType |= M_BGR32;
	}

	return l_lMatroxType;
}

/**
@SVOperationName Convert2MatroxType - SVMatroxBufferInfoEnum

@SVOperationDescription This function converts the SVMatroxBufferTypeEnum to a Matrox Type. 

*/
long SVMatroxBufferInterface::Convert2MatroxType( SVMatroxBufferInfoEnum p_eType ) 
{
	long l_lMatroxType = 0;

	if( (p_eType &  SVWindowDibHeader) == SVWindowDibHeader )
	{
		l_lMatroxType |= M_BITMAPINFO;
	}
	if( (p_eType & SVHostAddress) == SVHostAddress )
	{
		l_lMatroxType |= M_HOST_ADDRESS;
	}

	if( (p_eType & SVPhysicalAddress) == SVPhysicalAddress)
	{
		l_lMatroxType |= M_PHYSICAL_ADDRESS;

	}
	if( (p_eType & SVSizeX) == SVSizeX)
	{
		l_lMatroxType |= M_SIZE_X;

	}
	if( (p_eType & SVSizeY) == SVSizeY)
	{
		l_lMatroxType |= M_SIZE_Y;

	}
	if( (p_eType & SVSizeBand) == SVSizeBand)
	{
		l_lMatroxType |= M_SIZE_BAND;

	}
	if( (p_eType & SVType) == SVType)
	{
		l_lMatroxType |= M_TYPE;

	}
	if( (p_eType & SVWindowDC) == SVWindowDC)
	{
		l_lMatroxType |= M_DC_HANDLE;

	}
	if( (p_eType & SVAttribute) == SVAttribute)
	{
		l_lMatroxType |= M_ATTRIBUTE;

	}
	if( (p_eType & SVDataFormat) == SVDataFormat)
	{
		l_lMatroxType |= M_DATA_FORMAT;

	}

	// Kernel Control Flags
	if( (p_eType & SVKernelDefault) ==SVKernelDefault)
	{
		l_lMatroxType |= M_DEFAULT;

	}
	if( (p_eType & SVAbsoluteValue) == SVAbsoluteValue)
	{
		l_lMatroxType |= M_ABSOLUTE_VALUE;

	}
	if( (p_eType & SVNormalization) == SVNormalization)
	{
		l_lMatroxType |= M_NORMALIZATION_FACTOR;

	}
	if( (p_eType & SVBufOverScan) == SVBufOverScan)
	{
		l_lMatroxType |= M_OVERSCAN;

	}
	if( (p_eType & SVBufSaturation) == SVBufSaturation)
	{
		l_lMatroxType |= M_SATURATION;

	}
	if( (p_eType & SVBufWindowDCAlloc) ==SVBufWindowDCAlloc)
	{
		l_lMatroxType |= M_DC_ALLOC;

	}
	if( (p_eType & SVBufWindowDCFree) == SVBufWindowDCFree)
	{
		l_lMatroxType |= M_DC_FREE;

	}
	if( (p_eType & SVBufModified) == SVBufModified)
	{
		l_lMatroxType |= M_MODIFIED;
	}
	if( (p_eType & SVDirectXSurface) == SVDirectXSurface)
	{
		l_lMatroxType |= M_DDRAW_SURFACE;
	}

	// Combination Constants.
	if(( SVPitch & p_eType ) == SVPitch )
	{
		l_lMatroxType |= M_PITCH;
	}
	if(( SVPitchByte & p_eType ) == SVPitchByte )
	{
		l_lMatroxType |= M_PITCH_BYTE;
	}

	// Parent Info
	if(( SVParentID & p_eType ) == SVParentID )
	{
		l_lMatroxType |= M_PARENT_ID;
	}
	if(( SVParentOffsetX & p_eType ) == SVParentOffsetX )
	{
		l_lMatroxType |= M_PARENT_OFFSET_X;
	}
	if(( SVParentOffsetY & p_eType ) == SVParentOffsetY )
	{
		l_lMatroxType |= M_PARENT_OFFSET_Y;
	}

	return l_lMatroxType;
}



/**
@SVOperationName Create - SVMatroxBufferCreateLineStruct

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxBufferCreateLineStruct.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create( SVMatroxBuffer& p_rBuffer, 
																	  const SVMatroxBufferCreateLineStruct& p_CreateLineStruct)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_lType = Convert2MatroxType( p_CreateLineStruct.m_eType );
		__int64 l_lAttrib = Convert2MatroxType( p_CreateLineStruct.m_eAttribute );

		if( l_lType != 0 && l_lAttrib != 0 )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MIL_ID l_NewBuf = MbufAlloc1d(M_DEFAULT_HOST,
					p_CreateLineStruct.m_lSizeX,
					l_lType,
					l_lAttrib,
					M_NULL);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				if( l_Code == SVMEE_STATUS_OK )
				{
					p_rBuffer.m_BufferPtr = new SVMatroxImageBuffer( l_NewBuf, "SVMatroxBufferInterface::Create-CreateLine" );
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Create - LPBITMAPINFO

@SVOperationDescription This function creates a Matrox buffer using a LPBITMAPINFO.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create( SVMatroxBuffer& p_rBuffer, 
																	  const LPBITMAPINFO p_pBitmapInfo )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		HRESULT hr = S_OK;

		if ( p_pBitmapInfo != NULL )
		{
			BITMAPINFOHEADER* pbmhInfo;
			long lNumColor;
			long lHeight;
			long lBufSize;
			long lTabSize;

			pbmhInfo = &p_pBitmapInfo->bmiHeader;

			// Calculate the absolute height
			lHeight = abs( pbmhInfo->biHeight );

			// Make sure image size is calculated
			if( 0 == pbmhInfo->biSizeImage )
				pbmhInfo->biSizeImage = ( ( ( ( pbmhInfo->biWidth * pbmhInfo->biBitCount ) + 31 ) & ~31 ) >> 3 ) * lHeight;

			// Make sure color table size is calculated
			lNumColor = pbmhInfo->biClrUsed;
			if( 0 == lNumColor )
			{
			   if( pbmhInfo->biBitCount != 24 )
				  lNumColor = 1 << pbmhInfo->biBitCount;
			}
			lTabSize = lNumColor * sizeof( RGBQUAD );

			// Calculate total size buffer needed for image
			lBufSize = sizeof( BITMAPINFOHEADER ) + lTabSize + pbmhInfo->biSizeImage;


			BUFATTRTYPE lAttrib = M_PROC + M_IMAGE + M_DIB;

			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				// Allocate a multi or single band image buffer
				MIL_ID l_NewID = MbufAllocColor( M_DEFAULT_HOST,
								pbmhInfo->biBitCount <= 8 ? 1 : 3,
								pbmhInfo->biWidth, 
								pbmhInfo->biHeight, 
								pbmhInfo->biBitCount,
								lAttrib,
								M_NULL );

				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				if( l_Code == SVMEE_STATUS_OK )
				{
					p_rBuffer.m_BufferPtr = new SVMatroxImageBuffer( l_NewID, "SVMatroxBufferInterface::Create-LPBITMAPINFO" );
				}
			}

		}// end if ( hbm != NULL )
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBufferCreateStruct

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxSystem and a SVMatroxBufferCreateStruct.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create( const SVMatroxSystem& p_rSystem, SVMatroxBuffer& p_rBuffer, 
																	  const SVMatroxBufferCreateStruct& p_CreateStruct)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_lType = Convert2MatroxType( p_CreateStruct.m_eType );
		__int64 l_lAttrib = Convert2MatroxType( p_CreateStruct.m_eAttribute );

		if( l_lType != 0 && l_lAttrib != 0 )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MIL_ID MilSystemID = p_rSystem.empty() ? M_DEFAULT_HOST : p_rSystem.m_SystemIdentifier;
				MIL_ID l_NewBuf = M_NULL;
				if( p_CreateStruct.m_lSizeBand == 3 )
				{
					if( (l_lAttrib & M_LUT) == M_LUT)	// Mil help states that LUT must be stored in planar format.
					{
						l_lAttrib |= M_PLANAR;
					}

                    // MIL 9.0 has a problem using this flag with MbufAllocColor.
					if ( ( l_lAttrib & M_DIB ) == M_DIB ) 
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

				if( l_Code == SVMEE_STATUS_OK )
				{
					p_rBuffer.m_BufferPtr = new SVMatroxImageBuffer( l_NewBuf, "SVMatroxBufferInterface::Create-SystemID,BufferCreate" );
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBufferCreateStruct

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxBufferCreateStruct.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create( SVMatroxBuffer& p_rBuffer, 
																	  const SVMatroxBufferCreateStruct& p_CreateStruct)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_lType = Convert2MatroxType( p_CreateStruct.m_eType );
		__int64 l_lAttrib = Convert2MatroxType( p_CreateStruct.m_eAttribute );

		if( l_lType != 0 && l_lAttrib != 0 )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MIL_ID l_NewBuf = M_NULL;
				if( p_CreateStruct.m_lSizeBand == 3 )
				{
					if( (l_lAttrib & M_LUT) == M_LUT)	// Mil help states that LUT must be stored in planar format.
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

				if( l_Code == SVMEE_STATUS_OK )
				{
					p_rBuffer.m_BufferPtr = new SVMatroxImageBuffer( l_NewBuf, "SVMatroxBufferInterface::Create-BufferCreate" );
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBufferCreateChildStruct

@SVOperationDescription This function creates a Matrox child buffer using a SVMatroxBufferCreateChildStruct.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create( SVMatroxBuffer& p_rNewBuffer, 
																	  const SVMatroxBufferCreateChildStruct& p_CreateChildStruct)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_CreateChildStruct.m_ParentBufId.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MIL_ID l_NewBuf;
				if( p_CreateChildStruct.m_lParentBandCount > 1 )
				{
					l_NewBuf = MbufChildColor2d(p_CreateChildStruct.m_ParentBufId.GetIdentifier(),
						p_CreateChildStruct.m_lBand,
						p_CreateChildStruct.m_lOffX,
						p_CreateChildStruct.m_lOffY,
						p_CreateChildStruct.m_lSizeX,
						p_CreateChildStruct.m_lSizeY,
						M_NULL);
				}
				else
				{
					l_NewBuf = MbufChild2d(p_CreateChildStruct.m_ParentBufId.GetIdentifier(),
						p_CreateChildStruct.m_lOffX,
						p_CreateChildStruct.m_lOffY,
						p_CreateChildStruct.m_lSizeX,
						p_CreateChildStruct.m_lSizeY,
						M_NULL);
				}

				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				if( l_Code == SVMEE_STATUS_OK )
				{
					p_rNewBuffer.m_BufferPtr = new SVMatroxImageChildBuffer( p_CreateChildStruct.m_ParentBufId.m_BufferPtr, l_NewBuf, "SVMatroxBufferInterface::Create-CreateChildBuffer" );
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBufferCreateExtStruct

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxBufferCreateExtStruct.  This type is used for interfacing to other libraries such as Intek.

*/
// MbufCreateColor -  Caution	 : Uses external data buffer that you must manage.
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create( SVMatroxBuffer& p_rBuffer, 
																	  SVMatroxBufferCreateExtStruct p_CreateColorStruct)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_lType = Convert2MatroxType( p_CreateColorStruct.m_eType );
		__int64 l_lAttrib = Convert2MatroxType( p_CreateColorStruct.m_eAttribute );

		if( l_lType != 0 && l_lAttrib != 0 )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MIL_ID l_NewBuf = M_NULL;
				if( p_CreateColorStruct.m_lSizeBand > 1 )
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
					if( M_IS_FORMAT_RGB_BGR(l_lAttrib ))
					{
						l_lAttrib &= ~0xff00;  // remove rgb or bgr attribute
					}
					l_NewBuf = MbufCreate2d( M_DEFAULT_HOST,
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

				if( l_Code == SVMEE_STATUS_OK )
				{
					p_rBuffer.m_BufferPtr = new SVMatroxImageBuffer( l_NewBuf, "SVMatroxBufferInterface::Create-BufferCreateExt" );
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBuffer

@SVOperationDescription This function creates a Matrox buffer using a SVMatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create( SVMatroxBuffer& p_rNewBuffer, const SVMatroxBuffer& p_CreateFrom )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
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
		MbufInquire( p_CreateFrom.GetIdentifier(), M_SIZE_BAND, &l_lBandSize );
		MbufInquire( p_CreateFrom.GetIdentifier(), M_SIZE_X , &l_lSizeX );
		MbufInquire( p_CreateFrom.GetIdentifier(), M_SIZE_Y , &l_lSizeY );
		MbufInquire( p_CreateFrom.GetIdentifier(), M_TYPE , &l_lType );
		MbufInquire( p_CreateFrom.GetIdentifier(), M_EXTENDED_ATTRIBUTE , &l_lAttrib );

		l_Code = SVMatroxApplicationInterface::GetFirstError();

		if( l_Code == SVMEE_STATUS_OK )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MIL_ID l_NewBuf = M_NULL;
				if( l_lBandSize == 3 )
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
					if( M_IS_FORMAT_RGB_BGR(l_lAttrib ))
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

				if( l_Code == SVMEE_STATUS_OK )
				{
					p_rNewBuffer.m_BufferPtr = new SVMatroxImageBuffer( l_NewBuf, "SVMatroxBufferInterface::Create-MatroxBuffer" );
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Create

@SVOperationDescription This function creates a SVMatroxBuffer from a HBitmap.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create( SVMatroxBuffer& p_rNewMilId, HBITMAP& p_rHbm )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{

		if ( p_rHbm != NULL )
		{
			long l_Width;
			long l_Height;
			long l_SizeImage;
			unsigned short l_BitCount;

			DIBSECTION dib;
			memset(&dib, 0, sizeof(dib));
			dib.dsBmih.biSize = sizeof(BITMAPINFOHEADER);

			int cnt = ::GetObject(p_rHbm, sizeof(dib), &dib);
			if (cnt == 0)
				return S_FALSE;

			if (cnt > sizeof(BITMAP))
			{
				BITMAPINFOHEADER* pbmhInfo = &dib.dsBmih;

				// Calculate the absolute height
				l_Height = pbmhInfo->biHeight;
				l_Width = pbmhInfo->biWidth;
				l_BitCount = pbmhInfo->biBitCount;

				// Make sure image size is calculated
				if( 0 == pbmhInfo->biSizeImage )
					pbmhInfo->biSizeImage = ( ( ( ( l_Width * l_BitCount ) + 31 ) & ~31 ) >> 3 ) * abs( l_Height );

				l_SizeImage = pbmhInfo->biSizeImage;
/*
				// Make sure color table size is calculated
				long lNumColor = pbmhInfo->biClrUsed;
				if( 0 == lNumColor )
				{
				   //if( pbmhInfo->biBitCount != 24 )
					if( pbmhInfo->biBitCount <= 8 )
					  lNumColor = 1 << pbmhInfo->biBitCount;
				}
				long lTabSize = lNumColor * sizeof( RGBQUAD );

				// Calculate total size buffer needed for image
				long lBufSize = sizeof( BITMAPINFOHEADER ) + lTabSize + pbmhInfo->biSizeImage;
*/
			}
			else // Non DIB created bitmap
			{
				// Calculate the absolute height
				l_Height = dib.dsBm.bmHeight;
				l_Width = dib.dsBm.bmWidth;
				l_BitCount = dib.dsBm.bmBitsPixel;
				l_SizeImage = dib.dsBm.bmWidthBytes * abs( l_Height );
			}
			BUFATTRTYPE lAttrib = M_PROC + M_IMAGE + M_DIB;

			// we always use 8 bit per plane
			long type = 8 + M_UNSIGNED;
			
			if( l_BitCount == 32 )
				lAttrib += (M_BGR32 + M_PACKED);
			else if( l_BitCount == 24 )
				lAttrib += (M_BGR24 + M_PACKED);
			else if( l_BitCount == 16 )
				lAttrib += (M_RGB16 + M_PACKED);

			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MIL_ID l_NewBuf = M_NULL;

				// Allocate a multi or single band image buffer
				MbufAllocColor( M_DEFAULT_HOST,
								l_BitCount <= 8 ? 1 : 3,
								l_Width, 
								l_Height, 
								type,
								lAttrib,
								&l_NewBuf );

				l_Code = SVMatroxApplicationInterface::GetLastStatus();

				l_Code = (l_NewBuf!= M_NULL ) ? S_OK : S_FALSE;

				if ( l_Code == S_OK )
				{
					LPVOID pHostBuffer = NULL;
					pHostBuffer = ( LPVOID ) MbufInquire( l_NewBuf, M_HOST_ADDRESS, M_NULL );

					p_rNewMilId.m_BufferPtr = new SVMatroxImageBuffer( l_NewBuf, "SVMatroxBufferInterface::Create-HBITMAP" );

					// Set the bits of our new bitmap
					// SEJ - fix this ?
					if (dib.dsBm.bmBits)
					{
						memcpy( pHostBuffer, dib.dsBm.bmBits, l_SizeImage );
					}
					else
					{
						long cnt = ::GetBitmapBits(p_rHbm, l_SizeImage, pHostBuffer);
					}
				}
			}
		}// end if ( p_rHbm != NULL )

	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}// end HBitmapToNewMilHandle( HBITMAP p_rHbm, MIL_ID& p_rNewMilId)


/**
@SVOperationName Create

@SVOperationDescription This function creates a new HBITMAP from a SVMatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Create( HBITMAP& p_rHbm, const SVMatroxBuffer& p_rFromId )
{	// MilHandleToNewHBitmap
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		BITMAPINFOHEADER* pbmhInfo = NULL;
		BITMAPINFO* pbmInfo = NULL;
		long lNumColor;
		long lHeight;
		long lBufSize;
		long lTabSize;

		if( p_rFromId.empty() )
		{
			l_Code = S_FALSE;
			p_rHbm = NULL;
		}
		else
		{
			// Get the BITMAPINFO from MIL
			pbmInfo	 = (LPBITMAPINFO) MbufInquire( p_rFromId.GetIdentifier(), M_BITMAPINFO, M_NULL );
			pbmhInfo = (LPBITMAPINFOHEADER) &pbmInfo->bmiHeader;
			
			// Source images seem to be flipped even though MIL is not supposed to flip them
			if( pbmhInfo->biHeight > 0 )
				pbmhInfo->biHeight =  pbmhInfo->biHeight * -1;
			
			// Calculate the absolute height
			lHeight = pbmhInfo->biHeight * ( ( pbmhInfo->biHeight < 0 ) ? -1 : 1 );
			
			// Make sure image size is calculated
			if( 0 == pbmhInfo->biSizeImage )
				pbmhInfo->biSizeImage = ( ( ( ( pbmhInfo->biWidth * pbmhInfo->biBitCount ) + 31 ) & ~31 ) >> 3 ) * lHeight;
			
			// Make sure color table size is calculated
			lNumColor = pbmhInfo->biClrUsed;
			if( 0 == lNumColor )
			{
				//if( pbmhInfo->biBitCount != 24 )
				if( pbmhInfo->biBitCount <= 8 )
					lNumColor = 1 << pbmhInfo->biBitCount;
			}
			lTabSize = lNumColor * sizeof( RGBQUAD );
			
			// Calculate total size buffer needed for image
			lBufSize = sizeof( BITMAPINFOHEADER ) + lTabSize + pbmhInfo->biSizeImage;
		
			//-------------------------------------------------------------------------------

			// Get buffer from MIL 
			LPVOID pHostBuffer = ( LPVOID ) MbufInquire( p_rFromId.GetIdentifier(), M_HOST_ADDRESS, M_NULL );

			void* pBits=NULL;
			BYTE* pBytes=NULL;
			HBITMAP hBitmap=NULL;
			HDC hDC=NULL;

			// Create DC and a DIB bitmap
			hDC = ::CreateCompatibleDC(NULL);
			pbmhInfo->biHeight = abs(pbmhInfo->biHeight);	// always make positive
			hBitmap = ::CreateDIBSection( hDC, pbmInfo, DIB_RGB_COLORS, &pBits, NULL, 0 );
			
			SVImageConvertorGDI::CopyDIBitsFlip( hDC, hBitmap, pHostBuffer, pbmInfo);

			::DeleteDC(hDC);

			p_rHbm = hBitmap;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;

}

/**
@SVOperationName GenLutRamp

@SVOperationDescription This function generates a ramp, inverse ramp, or a constant in the specified LUT buffer region (StartIndex to EndIndex). The increment between LUT entries is the difference between StartValue and EndValue, divided by the number of entries. 

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GenLutRamp(SVMatroxBuffer& p_rMilId, long StartIndex, double StartValue, long EndIndex, double EndValue )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rMilId.empty() )
		{
			MgenLutRamp(p_rMilId.GetIdentifier(), StartIndex, StartValue, EndIndex, EndValue );
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::IsParent( const SVMatroxBuffer& p_rParentBuffer, const SVMatroxBuffer& p_rChildBuffer )
{
	SVStatusCode l_Status = SVMEE_STATUS_OK;

	if( !( p_rParentBuffer.empty() ) && !( p_rChildBuffer.empty() ) )
	{
		long l_ParentID = 0;

		l_Status = SVMatroxBufferInterface::Get( p_rChildBuffer, SVParentID, l_ParentID );

		if( l_Status == SVMEE_STATUS_OK )
		{
			if( l_ParentID != p_rParentBuffer.GetIdentifier() )
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

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetPositionPoint( POINT& p_rPoint, const SVMatroxBuffer& p_rBuffer )
{
	SVStatusCode l_Status = SVMEE_STATUS_OK;

	SVPOINT l_Point;

	l_Status = GetPositionPoint( l_Point, p_rBuffer );

	p_rPoint = l_Point;

	return l_Status;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetPositionPoint( SVPOINT& p_rPoint, const SVMatroxBuffer& p_rBuffer )
{
	SVStatusCode l_Status = SVMEE_STATUS_OK;

	p_rPoint = SVPOINT( 0, 0 );

	if( !( p_rBuffer.empty() ) )
	{
		long l_ParentID = 0;

		l_Status = SVMatroxBufferInterface::Get( p_rBuffer, SVParentID, l_ParentID );

		if( l_Status == SVMEE_STATUS_OK )
		{
			if( l_ParentID != 0 )
			{
				if( l_ParentID != p_rBuffer.GetIdentifier() )
				{
					long l_OffsetX = 0;
					long l_OffsetY = 0;

					l_Status = SVMatroxBufferInterface::Get( p_rBuffer, SVParentOffsetX, l_OffsetX );

					if( l_Status == SVMEE_STATUS_OK )
					{
						l_Status = SVMatroxBufferInterface::Get( p_rBuffer, SVParentOffsetY, l_OffsetY );
					}

					if( l_Status == SVMEE_STATUS_OK )
					{
						p_rPoint = SVPOINT( static_cast< long >( l_OffsetX ), static_cast< long >( l_OffsetY ) );
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
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetBitmapInfo( LPBITMAPINFO& p_rpBitmapInfo, 
																			 const SVMatroxBuffer& p_rBuffer )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
	if( !p_rBuffer.empty() )
	{
		l_Code = GetBitmapInfo(p_rpBitmapInfo, p_rBuffer.GetIdentifier());
	}
	else
	{
		l_Code = SVMEE_INVALID_HANDLE;
	}
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName GetBitmapInfo

@SVOperationDescription This function returns a pointer (LPBITMAPINFO) to the header of the DIB associated with the MIL buffer (if any) or NULL.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetBitmapInfo( LPBITMAPINFO& p_rpBitmapInfo, SVMatroxIdentifier p_Identifier)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( p_Identifier != M_NULL )
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetBitmapInfo( SVBitmapInfo& p_rBitmapInfo, const SVMatroxBuffer& p_rBuffer )
{
	SVStatusCode l_Status = S_OK;

	BITMAPINFO* l_pBitmapInfo = NULL;

	p_rBitmapInfo.clear();

	l_Status = GetBitmapInfo( l_pBitmapInfo, p_rBuffer );

	if( l_Status == SVMEE_STATUS_OK && l_pBitmapInfo != NULL )
	{
		p_rBitmapInfo.Assign( *l_pBitmapInfo );
	}
	else if( !( p_rBuffer.empty() ) )
	{
		long l_Width = 0;
		long l_Height = 0;
		unsigned short l_BitCount = 0;

		l_Status = SVMatroxBufferInterface::Get( p_rBuffer, SVSizeX, l_Width );

		if( l_Status  == SVMEE_STATUS_OK )
		{
			l_Status = SVMatroxBufferInterface::Get( p_rBuffer, SVSizeY, l_Height );
		}

		if( l_Status  == SVMEE_STATUS_OK )
		{
			long l_PixelDepth = 0;
			long l_BandSize = 0;

			l_Status = SVMatroxBufferInterface::Get( p_rBuffer, SVType, l_PixelDepth );

			if( l_Status == SVMEE_STATUS_OK )
			{
				l_Status = SVMatroxBufferInterface::Get( p_rBuffer, SVSizeBand, l_BandSize );
			}

			if( l_Status == SVMEE_STATUS_OK )
			{
				long l_DataFormat = 0;

				if( SVMatroxBufferInterface::Get( p_rBuffer, SVDataFormat, l_DataFormat ) == SVMEE_STATUS_OK )
				{
					NB_OF_BITS_PER_PIXEL( l_DataFormat, l_BitCount );
				}

				if( l_BitCount < 1 )
				{
					l_BitCount = static_cast< unsigned short >( l_PixelDepth * l_BandSize ); 
				}
			}
		}

		if( l_Status  == SVMEE_STATUS_OK )
		{
			p_rBitmapInfo = SVBitmapInfo( static_cast< long >( l_Width ), static_cast< long >( l_Height ), l_BitCount, SVBitmapInfo::GetDefaultColorTable( l_BitCount ) );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetBitmapInfo( SVBitmapInfo& p_rBitmapInfo, SVMatroxIdentifier p_Identifier )
{
	BITMAPINFO* l_pBitmapInfo = NULL;

	p_rBitmapInfo.clear();

	SVStatusCode l_Status = GetBitmapInfo( l_pBitmapInfo, p_Identifier );

	if( l_Status == SVMEE_STATUS_OK && l_pBitmapInfo != NULL )
	{
		p_rBitmapInfo.Assign( *l_pBitmapInfo );
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
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetHostAddress( LPVOID p_rpHostAddress, 
																			  const SVMatroxBuffer& p_rBuffer )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rBuffer.empty() )
		{
			MIL_ID l_NewBuf = MbufInquire(p_rBuffer.GetIdentifier(),
				M_HOST_ADDRESS,
				p_rpHostAddress);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			assert( p_rpHostAddress );
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetDDSurfaceInterfacePtr( LPVOID& p_rpSurface, const SVMatroxBuffer& p_rBuffer )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rBuffer.empty() )
		{
			MIL_ID l_NewBuf = MbufInquire(p_rBuffer.GetIdentifier(),
				M_DDRAW_SURFACE,
				&p_rpSurface);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			assert( p_rpSurface );
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}
 
/**
@SVOperationName CopyBuffer ( SVMatroxBuffer to SVMatroxBuffer )

@SVOperationDescription This function copies the data from a SVMatroxBuffer to a SVMatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer( SVMatroxBuffer& p_rTo, 
																		  const SVMatroxBuffer& p_rFrom )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rTo.empty() && !p_rFrom.empty() )
		{
			MbufCopy( p_rFrom.GetIdentifier(),
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer( SVMatroxBuffer& p_rTo, SVMatroxIdentifier p_From )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rTo.empty() && p_From != 0 )
		{
			MbufCopy( p_From, p_rTo.GetIdentifier());

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer( SVMatroxIdentifier p_To, const SVMatroxBuffer& p_rFrom )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( p_To != 0 && !p_rFrom.empty() )
		{
			MbufCopy( p_rFrom.GetIdentifier(), p_To);

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName CopyBuffer ( SVMatroxBuffer to SVMatroxDisplayBuffer )

@SVOperationDescription This function copies the data from a SVMatroxDisplayBuffer to a SVMatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer( SVMatroxDisplayBuffer& p_rTo, 
																		  const SVMatroxBuffer& p_rFrom )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rTo.empty() && !p_rFrom.empty() )
		{
			MbufCopy( p_rFrom.GetIdentifier(),
				p_rTo.GetIdentifier() );

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName CopyBuffer ( SVMatroxBuffer and offsets to SVMatroxBuffer )

@SVOperationDescription This function copies the data from a SVMatroxBuffer to a SVMatroxBuffer with x and y offsets.  If the destination is not large enough to put the source with its offsets then it is clipped.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer( SVMatroxBuffer& p_rTo, 
																		  const SVMatroxBuffer& p_rFrom, 
																		  long p_lXOffset, 
																		  long p_lYOffset)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rTo.empty() && !p_rFrom.empty() )
		{
			MbufCopyClip( p_rFrom.GetIdentifier(),
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName CopyBuffer ( SVMatroxBuffer band to SVMatroxBuffer )

@SVOperationDescription This function copies the data from a SVMatroxBuffer to a SVMatroxBuffer with x and y offsets.  If the destination is not large enough to put the source with its offsets then it is clipped.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer( SVMatroxBuffer& p_rTo, 
																		  const SVMatroxBuffer& p_rFrom, 
																		  long p_lBand )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rTo.empty() && !p_rFrom.empty() )
		{
			MbufCopyColor( p_rFrom.GetIdentifier(),
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


/**
@SVOperationName CopyBuffer (HBITMAP to SVMatroxBuffer)

@SVOperationDescription This function copies the data from a HBitmap to a SVMatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer( SVMatroxBuffer& p_rMilId, HBITMAP& p_rHbm )
{	// HBitmapToMilHandle
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		DIBSECTION dib;

		if ( p_rMilId.empty() )
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}

		if( l_Code == S_OK )
		{
			if ( p_rHbm != NULL )
			{
				if( ::GetObject( p_rHbm, sizeof( DIBSECTION ), &dib ) != 0 )
				{
					// see article Q186586; GetObject(DIBSECTION) always returns positive height
					// assume source is standard orientation (negative)
					dib.dsBmih.biHeight = -( abs( dib.dsBmih.biHeight ) );
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

		if( l_Code == S_OK )
		{
			// Get the BITMAPINFO from MIL
			BITMAPINFO* pbmInfoMil = (LPBITMAPINFO) MbufInquire( p_rMilId.GetIdentifier(), M_BITMAPINFO, M_NULL );
			LPVOID pHostBuffer = ( LPVOID ) MbufInquire( p_rMilId.GetIdentifier(), M_HOST_ADDRESS, M_NULL );

			l_Code = SVImageConvertorGDI::CopyDIBits( &( dib.dsBmih ), dib.dsBm.bmBits, &( pbmInfoMil->bmiHeader ), pHostBuffer );
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


/**
@SVOperationName CopyBuffer - (SVMatroxBuffer to HBITMAP)

@SVOperationDescription This function copies the data from a SVMatroxBuffer to a HBITMAP.  It is assumed that the MIL handle and HBITMAP are compatible buffer formats.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer( HBITMAP& p_rBitmap, SVMatroxBuffer& p_rFromId )
{	// MilHandleToExistingDIB
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		DIBSECTION dib;

		if( p_rFromId.empty() )
		{
			l_Code = S_FALSE;
		}

		if( l_Code == S_OK )
		{
			if( p_rBitmap != NULL )
			{
				if( ::GetObject( p_rBitmap, sizeof( DIBSECTION ), &dib ) != 0 )
				{
					// see article Q186586; GetObject(DIBSECTION) always returns positive height
					// assume source is standard orientation (negative)
					dib.dsBmih.biHeight = -( abs( dib.dsBmih.biHeight ) );
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

		if( l_Code == S_OK )
		{
			BITMAPINFO* pbmInfo = (LPBITMAPINFO) MbufInquire( p_rFromId.GetIdentifier(), M_BITMAPINFO, M_NULL );
			LPVOID pHostBuffer = ( LPVOID ) MbufInquire( p_rFromId.GetIdentifier(), M_HOST_ADDRESS, M_NULL );

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;

}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer( SVByteVector& p_rToDIB, const SVMatroxBuffer& p_rFromId )
{
	SVStatusCode l_Code = SVMEE_STATUS_OK;

	SVBitmapInfo l_Info;

	p_rToDIB.clear();

	l_Code = GetBitmapInfo( l_Info, p_rFromId );

	if( l_Code == SVMEE_STATUS_OK )
	{
		void* l_pHostBuffer = reinterpret_cast< void* >( MbufInquire( p_rFromId.GetIdentifier(), M_HOST_ADDRESS, M_NULL ) );

		if( l_pHostBuffer != NULL )
		{
			size_t l_InfoSize = l_Info.GetBitmapInfoSizeInBytes();
			size_t l_ImageSize = l_Info.GetBitmapImageSizeInBytes();

			p_rToDIB.resize( l_InfoSize + l_ImageSize, 0 );

			::memcpy( &( p_rToDIB[ 0 ] ), l_Info.GetBitmapInfo(), l_InfoSize );
			::memcpy( &( p_rToDIB[ l_InfoSize ] ), l_pHostBuffer, l_ImageSize );
		}
		else
		{
			SVMatroxBuffer l_ToId;

			l_Code = Create( l_ToId, p_rFromId );

			void* l_pNewHostBuffer = reinterpret_cast< void* >( MbufInquire( l_ToId.GetIdentifier(), M_HOST_ADDRESS, M_NULL ) );

			if( l_pNewHostBuffer != NULL )
			{
				l_Code = CopyBuffer( l_ToId, p_rFromId );

				if( l_Code == SVMEE_STATUS_OK )
				{
					l_Code = CopyBuffer( p_rToDIB, l_ToId );
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

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer( SVByteVector& p_rToDIB, const SVBitmapInfo& p_rToBitmapInfo, const SVMatroxBuffer& p_rFromId )
{
	SVStatusCode l_Code = SVMEE_STATUS_OK;

	SVMatroxBuffer l_ToId;

	l_Code = Create( l_ToId, const_cast< LPBITMAPINFO >( p_rToBitmapInfo.GetBitmapInfo() ) );

	void* l_pNewHostBuffer = reinterpret_cast< void* >( MbufInquire( l_ToId.GetIdentifier(), M_HOST_ADDRESS, M_NULL ) );

	if( l_pNewHostBuffer != NULL )
	{
		l_Code = CopyBuffer( l_ToId, p_rFromId );

		if( l_Code == SVMEE_STATUS_OK )
		{
			l_Code = CopyBuffer( p_rToDIB, l_ToId );
		}
	}
	else
	{
		l_Code = SVMEE_INVALID_HANDLE;
	}

	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer( const SVBitmapInfo& dstInfo, unsigned char* p_rTo, const SVMatroxBuffer& p_rFrom )
{
	return CopyBuffer(dstInfo, p_rTo, p_rFrom.GetIdentifier());
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBuffer( const SVBitmapInfo& dstInfo, unsigned char* p_rTo, const SVMatroxIdentifier& p_rFromId )
{
	SVStatusCode l_Code = SVMEE_STATUS_OK;

	SVBitmapInfo l_Info;

	l_Code = GetBitmapInfo( l_Info, p_rFromId );

	if( l_Code == SVMEE_STATUS_OK )
	{
		void* l_pHostBuffer = reinterpret_cast< void* >( MbufInquire( p_rFromId, M_HOST_ADDRESS, M_NULL ) );

		if( l_pHostBuffer != NULL )
		{
			size_t l_ImageSize = l_Info.GetBitmapImageSizeInBytes();

			// Should check if source size is same as destination, else only copy the smaller of the 2...
			::memcpy( p_rTo, l_pHostBuffer, l_ImageSize );
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyBufferToFileDIB( SVByteVector& p_rToDIB, const SVMatroxBuffer& p_rFromId )
{
	SVStatusCode l_Code = SVMEE_STATUS_OK;

	SVBitmapInfo l_Info;

	p_rToDIB.clear();

	l_Code = GetBitmapInfo( l_Info, p_rFromId );

	if( l_Code == SVMEE_STATUS_OK )
	{
		void* l_pHostBuffer = reinterpret_cast< void* >( MbufInquire( p_rFromId.GetIdentifier(), M_HOST_ADDRESS, M_NULL ) );

		if( l_pHostBuffer != NULL )
		{
			size_t l_InfoSize = l_Info.GetBitmapInfoSizeInBytes();
			size_t l_ImageSize = l_Info.GetBitmapImageSizeInBytes();

			BITMAPFILEHEADER hdr;

			hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
			hdr.bfSize = static_cast< DWORD >( sizeof( BITMAPFILEHEADER ) + l_InfoSize + l_ImageSize );
			hdr.bfReserved1 = 0; 
			hdr.bfReserved2 = 0; 
			hdr.bfOffBits = static_cast< DWORD >( sizeof( BITMAPFILEHEADER ) + l_InfoSize );

			p_rToDIB.resize( hdr.bfSize );

			::memcpy( &( p_rToDIB[ 0 ] ), &hdr, sizeof( BITMAPFILEHEADER ) );
			::memcpy( &( p_rToDIB[ sizeof( BITMAPFILEHEADER ) ] ), l_Info.GetBitmapInfo(), l_InfoSize );

			BITMAPINFO* l_pBitmapInfo = reinterpret_cast< BITMAPINFO* >( &( p_rToDIB[ sizeof( BITMAPFILEHEADER ) ] ) );

			if( l_pBitmapInfo->bmiHeader.biHeight < 0 )
			{
				size_t l_Stride = l_Info.GetBitmapImageStrideInBytes();

				unsigned char* l_pFrom = reinterpret_cast< unsigned char* >( l_pHostBuffer );
				unsigned char* l_pTo = reinterpret_cast< unsigned char* >( &( p_rToDIB[ sizeof( BITMAPFILEHEADER ) + l_InfoSize ] ) );

				l_pBitmapInfo->bmiHeader.biHeight = ::labs( l_pBitmapInfo->bmiHeader.biHeight );
				l_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
				l_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
				l_pBitmapInfo->bmiHeader.biClrImportant = 0;

				for( size_t i = 0; i < static_cast< size_t >( l_pBitmapInfo->bmiHeader.biHeight ); ++i )
				{
					unsigned char* l_pToRow = l_pTo + ( i * l_Stride );
					unsigned char* l_pFromRow = l_pFrom + ( ( l_pBitmapInfo->bmiHeader.biHeight - 1 - i ) * l_Stride );

					::memcpy( l_pToRow, l_pFromRow, l_Stride );
				}
			}
			else
			{
				::memcpy( &( p_rToDIB[ sizeof( BITMAPFILEHEADER ) + l_InfoSize ] ), l_pHostBuffer, l_ImageSize );
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}

	return l_Code;
}

SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::CopyDIBBufferToMemory(SVImageCopyUtility& p_rImageCopier, const SVMatroxBuffer& p_rFromId )
{
	SVBitmapInfo l_Info;
	HRESULT l_Status = SVMatroxBufferInterface::GetBitmapInfo( l_Info, p_rFromId );
	if (l_Status == S_OK)
	{
		void* l_pHostBuffer = reinterpret_cast< void* >( MbufInquire( p_rFromId.GetIdentifier(), M_HOST_ADDRESS, M_NULL ) );

		if( l_pHostBuffer != NULL )
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
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::PutBuffer( SVMatroxBuffer& p_rTo, 
																		 const unsigned char* p_pcArrayData )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rTo.empty() )
		{
			MbufPut( p_rTo.GetIdentifier(),
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName PutBuffer - const long*

@SVOperationDescription This function copies the data from a user array to a MatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::PutBuffer( SVMatroxBuffer& p_rTo, const long* p_plArrayData )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rTo.empty() )
		{
			MbufPut( p_rTo.GetIdentifier(),
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


/**
@SVOperationName PutColor

@SVOperationDescription This function copies the data from a user array to a MatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::PutColor( SVMatroxBuffer& p_rTo, 
																		const unsigned char* p_pArrayData )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rTo.empty() )
		{
			MbufPutColor( p_rTo.GetIdentifier(),
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


/**
@SVOperationName PutLine

@SVOperationDescription This function copies the data from a user array to a MatroxBuffer.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::PutLine( SVMatroxBuffer& p_rTo, 
																	   long p_lCount, 
																	   const unsigned char* p_pArrayData)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rTo.empty() )
		{
			MbufPut1d( p_rTo.GetIdentifier(),
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName GetLine

@SVOperationDescription This function reads the series of pixels along a theoretical line (defined by coordinates) from an image and stores their values in a user-defined array. 

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::GetLine( SVMatroxBuffer& p_rBuf, 
																	   SVMatroxInt p_lXStart, 
																	   SVMatroxInt p_lYStart, 
																	   SVMatroxInt p_lXEnd, 
																	   SVMatroxInt p_lYEnd, 
																	   SVMatroxInt& p_rlNbrPixels, 
																	   void* p_pUserArray)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVStatusCode l_Code;
		if( !p_rBuf.empty() )
		{
	//		long l_lLength;
	//		MbufGetLine( p_rBuf.m_BufferIdentifier,
	//			p_lXStart,
	//			p_lYStart,
	//			p_lXEnd,
	//			p_lYEnd,
	//			M_DEFAULT,
	//			&l_lLength,
	//			M_NULL );
	//		if( p_pUserArray.size() < l_lLength )
	//		{
	//			p_pUserArray.resize( l_lLength );
	//		}

			MbufGetLine( p_rBuf.GetIdentifier(),
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


/**
@SVOperationName Get - double

@SVOperationDescription This function uses MbufInquire to get information about a SVMatroxBuffer and stores it in a double.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Get( const SVMatroxBuffer& p_rBuf, 
																   SVMatroxBufferInfoEnum p_eWhat, 
																   double& p_rdResult) 
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rBuf.empty() )
		{
			MIL_INT l_lMatroxType = Convert2MatroxType(p_eWhat);
			if( l_lMatroxType != 0 )
			{
				MbufInquire( p_rBuf.GetIdentifier(),
					l_lMatroxType,
					&p_rdResult);

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Get - long

@SVOperationDescription This function uses MbufInquire to get information about a SVMatroxBuffer and stores it in a long.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Get( const SVMatroxBuffer& p_rBuf, 
																   SVMatroxBufferInfoEnum p_eWhat, 
																   long& p_rlResult)  
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rBuf.empty() )
		{
			MIL_INT l_lMatroxType = Convert2MatroxType(p_eWhat);
			if( l_lMatroxType != 0 )
			{
				SVMatroxInt l_Temp = 0;

				MbufInquire( p_rBuf.GetIdentifier(),
					l_lMatroxType,
					&l_Temp );

				// Matrox uses 64 bits for all parameters, but we probably don't need to worry about more that 32 bits of actual data.
				p_rlResult = static_cast< long >( l_Temp );

				// Break if there is more data here than 32 bits.
				assert( static_cast< SVMatroxInt >( p_rlResult ) == l_Temp );

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
	assert( l_Code == SVMEE_STATUS_OK );
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
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rBuf.empty() )
		{
			long l_lMatroxType = Convert2MatroxType(p_eWhat);
			if( l_lMatroxType != 0 )
			{
				MbufControl( p_rBuf.GetIdentifier(),
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Set - long

@SVOperationDescription This function allows you to control a specified data buffer settings with a supplied long. 

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Set(const SVMatroxBuffer& p_rBuf, 
																   SVMatroxBufferInfoEnum p_eWhat, 
																   const SVMatroxInt p_rlValue )
{
	return Set( p_rBuf, p_eWhat, static_cast< const double >( p_rlValue ) );
}

/**
@SVOperationName ControlNeighborhood

@SVOperationDescription This function changes the setting of an operation control type of the specified kernel buffer or structuring element buffer. 

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::ControlNeighborhood( SVMatroxBuffer& p_rBuf, 
																				   SVMatroxBufferInfoEnum p_eWhat, 
																				   long p_lValue)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rBuf.empty() )
		{
			long l_lMatroxType = Convert2MatroxType(p_eWhat);
			if( l_lMatroxType != 0 )
			{
				MbufControlNeighborhood( p_rBuf.GetIdentifier(),
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName ClearBuffer - SVMatroxDisplay

@SVOperationDescription This function clears the entire specified buffer to the specified color.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::ClearBuffer( SVMatroxDisplay& p_rBuffer, 
																		   double p_dColor )
{
	SVStatusCode l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rBuffer.empty() )
		{
			MbufClear(p_rBuffer.m_DisplayIdentifier, p_dColor );

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;

}

/**
@SVOperationName ClearBuffer - SVMatroxBuffer

@SVOperationDescription This function clears the entire specified buffer to the specified color.

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::ClearBuffer( SVMatroxBuffer& p_rBuffer, 
																		   double p_dColor )
{
	SVStatusCode l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rBuffer.empty() )
		{
			MbufClear(p_rBuffer.GetIdentifier(), p_dColor );

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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;

}


/**
@SVOperationName Import

@SVOperationDescription This function imports data from a file into an existing or automatically allocated MIL data buffer. 

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Import(SVMatroxBuffer& p_rBuf, 
																	  const SVMatroxString& p_rFileName, 
																	  SVMatroxFileTypeEnum p_eFileType,
																	  bool p_bRestore )
{
	long l_lFileFormat = 0;
	long l_lOperation = 0;

	// File Formats
	if( (p_eFileType & SVFileBitmap) == SVFileBitmap )
	{
		l_lFileFormat = M_BMP;
	}
	else if(( p_eFileType & SVFileTiff) == SVFileTiff)
	{
		l_lFileFormat = M_TIFF;
	}
	else if(( p_eFileType & SVFileMIL) == SVFileMIL)
	{
		l_lFileFormat = M_MIL;
	}

	// Operations

	if(p_bRestore )
	{
		l_lOperation = M_RESTORE;
	}
	else
	{
		l_lOperation = M_LOAD;
	}

	SVStatusCode l_Code( SVMEE_STATUS_OK );

#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == SVMEE_STATUS_OK )
		{
			if( p_bRestore )
			{	// Restore Operation creates a new Mil handle.
				MIL_ID l_NewId = M_NULL;
				MbufImport(const_cast<MIL_TEXT_CHAR*>(p_rFileName.c_str()), 
					l_lFileFormat,
					l_lOperation,
					M_DEFAULT_HOST,
					&l_NewId);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
				{
					p_rBuf.m_BufferPtr = new SVMatroxImageBuffer( l_NewId, "SVMatroxBufferInterface::Import" );
				}
			}
			else
			{	// Load fills a previously created Mil handle.
				if( !p_rBuf.empty() )
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Export

@SVOperationDescription This function exports a data buffer to a file, in the specified output file format. 

*/
SVMatroxBufferInterface::SVStatusCode SVMatroxBufferInterface::Export(const SVMatroxBuffer& p_rBuf, 
																	  const SVMatroxString& p_rFileName, 
																	  SVMatroxFileTypeEnum p_eFileType )
{
	long l_lFileFormat = 0;

	// File Formats. Currently only Bmps
	if( (p_eFileType & SVFileBitmap) == SVFileBitmap )
	{
		l_lFileFormat |= M_BMP;
	}

	SVStatusCode l_Code( SVMEE_STATUS_OK );

#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rBuf.empty() )
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
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;

}



// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBufferInterface.cpp_v  $
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
