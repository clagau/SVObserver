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
	HBITMAP l_Bitmap = NULL;
	HRESULT l_Status = Convert( m_Buffer, l_Bitmap );

	p_rData.clear();

	if( l_Status == S_OK )
	{
		l_Status = SVImageConvertorGDI::BitmapToByteVector( l_Bitmap, p_rData );
	}

	if( l_Bitmap != NULL )
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
		HBITMAP l_Bitmap = NULL;

		l_Status = SVImageConvertorGDI::ByteVectorToBitmap( p_rData, l_Bitmap );

		if( l_Status == S_OK )
		{
			l_Status = Convert( l_Bitmap, m_Buffer );
		}

		if( l_Bitmap != NULL )
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

	SVMatroxBufferInterface::SVStatusCode status = SVMatroxBufferInterface::Create(srcBuffer, hBitmap);
	
	if (status == SVMEE_STATUS_OK)
	{
		hr = CreateGrayscaleImage(bmp.bmWidth, bmp.bmHeight, dstBuffer);
	
		if (status == SVMEE_STATUS_OK)
		{
			// check that the image is single banded
			long numBands;
			status = SVMatroxBufferInterface::Get(dstBuffer, SVSizeBand, numBands);
			if (status == SVMEE_STATUS_OK)
			{
				if (numBands > 1)
				{
					//SVMatroxBuffer srcBuffer = rImage;
					hr = ConvertToGrayscale(srcBuffer, dstBuffer);
					if (hr == S_OK)
					{
						// flip it
						status = SVMatroxImageInterface::Flip(srcBuffer, dstBuffer, SVImageFlipVertical);
						if (status == SVMEE_STATUS_OK)
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
					if (status == SVMEE_STATUS_OK)
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
	SVMatroxBufferInterface::SVStatusCode status = SVMatroxBufferInterface::Create( hBitmap, rImage );

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

	SVMatroxBufferInterface::SVStatusCode status = SVMatroxBufferInterface::Create(dstBuffer, createStruct);
	HRESULT hr = status;
	return hr;
}
				
HRESULT SVMatroxCommandDataImage::ConvertToGrayscale( SVMatroxBuffer& rDstImage, const SVMatroxBuffer& rSrcImage )
{
	SVMatroxImageInterface::SVStatusCode status = SVMatroxImageInterface::Convert(rDstImage, rSrcImage, SVImageRGBToL);
	HRESULT hr = status;
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxCommandDataImage.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:02:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jan 2011 10:36:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 15:00:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include new command functionality using the matrox library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
