//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxCommandDataImage
//* .File Name       : $Workfile:   SVMatroxCommandDataImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:02:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxCommandDataImage.h"
#include "SVCommandLibrary/SVCommandLibraryEnums.h"
#include "SVSystemLibrary/SVImageConvertorGDI.h"
#include "SVMatroxBufferInterface.h"
#include "SVMatroxImageInterface.h"

SVMatroxCommandDataImage::SVMatroxCommandDataImage()
: SVCommandDataImage::SVCommandDataInterface(), m_Buffer()
{
}

SVMatroxCommandDataImage::SVMatroxCommandDataImage( const SVMatroxBufferCreateStruct& p_rCreateData )
: SVCommandDataImage::SVCommandDataInterface(), m_Buffer()
{
	SVMatroxBufferInterface::Create( m_Buffer, p_rCreateData );
}

SVMatroxCommandDataImage::SVMatroxCommandDataImage( const SVMatroxBuffer& p_rBuffer, bool p_ReadOnly )
: SVCommandDataImage::SVCommandDataInterface( p_ReadOnly ), m_Buffer()
{
	SVMatroxBufferInterface::Create( m_Buffer, p_rBuffer );
	SVMatroxBufferInterface::CopyBuffer( m_Buffer, p_rBuffer );
}

SVMatroxCommandDataImage::SVMatroxCommandDataImage( const SVByteVector& p_rData, bool p_ReadOnly )
: SVCommandDataImage::SVCommandDataInterface( p_ReadOnly ), m_Buffer()
{
	SetData( p_rData );
}

SVMatroxCommandDataImage::~SVMatroxCommandDataImage()
{
	m_Buffer.clear();
}

HRESULT SVMatroxCommandDataImage::GetData( SVByteVector& p_rData ) const
{
	HBITMAP l_Bitmap = nullptr;
	HRESULT l_Status = Convert( m_Buffer, l_Bitmap );

	p_rData.clear();

	if( S_OK == l_Status )
	{
		l_Status = SVImageConvertorGDI::BitmapToByteVector( l_Bitmap, p_rData );
	}

	if( nullptr != l_Bitmap )
	{
		::DeleteObject( l_Bitmap );
	}

	return S_OK;
}

HRESULT SVMatroxCommandDataImage::SetData( const SVByteVector& p_rData )
{
	HRESULT l_Status = S_OK;

	if( ( m_DataType & SV_COMMAND_DATA_TYPE_READ_ONLY ) == 0 )
	{
		HBITMAP l_Bitmap = nullptr;

		l_Status = SVImageConvertorGDI::ByteVectorToBitmap( p_rData, l_Bitmap );

		if( S_OK == l_Status )
		{
			l_Status = Convert( l_Bitmap, m_Buffer );
		}

		if( nullptr != l_Bitmap )
		{
			::DeleteObject( l_Bitmap );
		}
	}

	return l_Status;
}

HRESULT SVMatroxCommandDataImage::Convert( HBITMAP hBitmap, SVMatroxBuffer& rImage )
{
	HRESULT hr = S_OK;

	SVMatroxBuffer srcBuffer;
	SVMatroxBuffer dstBuffer;

	BITMAP bmp;
	::GetObject(hBitmap, sizeof(BITMAP), &bmp);

	HRESULT status = SVMatroxBufferInterface::Create(srcBuffer, hBitmap);
	
	if (status == S_OK)
	{
		hr = CreateGrayscaleImage(bmp.bmWidth, bmp.bmHeight, dstBuffer);
	
		if (status == S_OK)
		{
			// check that the image is single banded
			long numBands;
			status = SVMatroxBufferInterface::Get(dstBuffer, SVSizeBand, numBands);
			if (status == S_OK)
			{
				if (numBands > 1)
				{
					//SVMatroxBuffer srcBuffer = rImage;
					hr = ConvertToGrayscale(srcBuffer, dstBuffer);
					if (S_OK == hr)
					{
						// flip it
						status = SVMatroxImageInterface::Flip(srcBuffer, dstBuffer, SVImageFlipVertical);
						if (status == S_OK)
						{
							//
							rImage = srcBuffer;
						}
						else
						{
							hr = status;
							// Cleanup
							srcBuffer.clear();
						}
					}
					// Cleanup
					dstBuffer.clear();
				}
				else
				{
					// flip it
					status = SVMatroxImageInterface::Flip(dstBuffer, srcBuffer, SVImageFlipVertical);
					if (status == S_OK)
					{
						//
						rImage = dstBuffer;
					}
					else
					{
						// Cleanup
						dstBuffer.clear();
						hr = status;
					}
					// Cleanup
					srcBuffer.clear();
				}
			}
			else
			{
				hr = status;
			}
		}
		else
		{
			hr = status;
		}
	}
	else
	{
		hr = status;
	}
	return hr;
}

HRESULT SVMatroxCommandDataImage::Convert( const SVMatroxBuffer& rImage, HBITMAP& hBitmap )
{
	HRESULT status = SVMatroxBufferInterface::Create( hBitmap, rImage );

	return status;
}

HRESULT SVMatroxCommandDataImage::CreateGrayscaleImage( long width, long height, SVMatroxBuffer& dstBuffer )
{
	// convert to single banded image
	SVMatroxBufferCreateStruct createStruct;
	createStruct.m_lSizeBand = 1;
	createStruct.m_lSizeX = width;
	createStruct.m_lSizeY = height;
	createStruct.m_eType = SV8BitUnsigned;
	createStruct.m_eAttribute = SVBufAttImageProcDib;

	HRESULT status = SVMatroxBufferInterface::Create(dstBuffer, createStruct);
	HRESULT hr = status;
	return hr;
}
				
HRESULT SVMatroxCommandDataImage::ConvertToGrayscale( SVMatroxBuffer& rDstImage, const SVMatroxBuffer& rSrcImage )
{
	HRESULT status = SVMatroxImageInterface::Convert(rDstImage, rSrcImage, SVImageRGBToL);
	HRESULT hr = status;
	return hr;
}

