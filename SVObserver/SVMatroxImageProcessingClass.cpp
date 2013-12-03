//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxImageProcessingClass
//* .File Name       : $Workfile:   SVMatroxImageProcessingClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 15:24:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxImageProcessingClass.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVBarCodeBuffer.h"
#include "SVDataBuffer.h"
#include "SVFileNameClass.h"
#include "SVGlobal.h"
#include "SVUtilityLibrary/SVImageCopyUtility.h"
#include "SVMatroxLibrary/SVMatroxImagingLibrary.h"  // has MIL includes

SVMatroxImageProcessingClass& SVMatroxImageProcessingClass::Instance()
{
	static SVMatroxImageProcessingClass l_Object;

	return l_Object;
}

SVMatroxImageProcessingClass::SVMatroxImageProcessingClass()
{
}

SVMatroxImageProcessingClass::~SVMatroxImageProcessingClass()
{
}

HRESULT SVMatroxImageProcessingClass::CreateImageBuffer( const SVImageInfoClass& rInfo, SVSmartHandlePointer& rHandle )
{
	HRESULT hrOk = S_OK;

	bool bDisplayedErrorMessage = false;

	SVImageFormatEnum l_eFormat;
	int l_iBandNumber = 0;
	int l_iBandLink = 0;
	int l_iPixelDepth = 0;
	long l_lWidth = 0;
	long l_lHeight = 0;

	rHandle.clear();

	hrOk = GetOutputImageCreateData( rInfo, l_eFormat, l_iPixelDepth, l_iBandNumber, l_iBandLink, l_lWidth, l_lHeight );

	if( hrOk == S_OK && 
		0 < l_iPixelDepth && 0 < l_iBandNumber &&
		0 < l_lWidth && 0 < l_lHeight )
	{
		SVMatroxBufferInterface::SVStatusCode l_Code;
		SVMatroxBufferCreateStruct l_Create;

		l_Create.m_lSizeBand = l_iBandNumber;
		l_Create.m_lSizeX = l_lWidth;
		l_Create.m_lSizeY = l_lHeight;
		l_Create.SetImageDepth( l_iPixelDepth );

		switch( l_eFormat )
		{
		case SVImageFormatRGB565:
			{
				l_Create.m_eAttribute = SVBuffAttImageProcPackedOffBoardDibPagedRgb16;
				break;
			}
		case SVImageFormatRGB888:
			{
				l_Create.m_eAttribute = SVBuffAttImageProcPackedOffBoardDibPagedBgr24;
				break;
			}
		case SVImageFormatRGB8888:
			{
				l_Create.m_eAttribute = SVBufAttImageProcPackedOffBoardDibPagedBgr32;
				break;
			}
		default:
			{
				l_Create.m_eAttribute = SVBufAttImageProcDib;
			}
		}

		SVMatroxBuffer l_NewBuffer;

		// Allocate a workable multi or single band image buffer
		l_Code = SVMatroxBufferInterface::Create( l_NewBuffer, l_Create );

		rHandle = new SVImageBufferHandleStruct( l_NewBuffer );

		hrOk = l_Code == SVMEE_STATUS_OK ? S_OK : l_Code | SVMEE_MATROX_ERROR;
		if ( hrOk == S_OK )
		{
			InitBuffer( rHandle );
		}
		else
		{
			rHandle.clear();
		}
	}
	else
	{
		hrOk = S_FALSE;
	}

	if ( hrOk != S_OK && !bDisplayedErrorMessage )
	{
		ASSERT(FALSE);
		AfxMessageBox( "Failed to create an image buffer!", MB_ICONEXCLAMATION );
	}

	return hrOk;
}

HRESULT SVMatroxImageProcessingClass::CreateImageBuffer( const SVSmartHandlePointer& rFromHandle, SVImageOperationTypeEnum p_lConversionType, SVSmartHandlePointer& rToHandle )
{
	HRESULT l_Status = S_OK;

	rToHandle.clear();

	if( !( rFromHandle.empty() ) )
	{
		SVMatroxBuffer l_Temp;
		SVImageBufferHandleImage l_FromMilHandle;

		l_Status = rFromHandle->GetData( l_FromMilHandle );

		if( l_Status == S_OK )
		{
			l_Status = SVMatroxBufferInterface::Create( l_Temp, l_FromMilHandle.GetBuffer() );

			if( l_Status == S_OK )
			{
				rToHandle = new SVImageBufferHandleStruct( l_Temp );
			}
		}

		if( l_Status == S_OK )
		{
			l_Status = SVMatroxImageInterface::Convert( l_Temp, l_FromMilHandle.GetBuffer(), p_lConversionType );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if( l_Status != S_OK )
	{
		rToHandle.clear();
	}

	return l_Status;
}

HRESULT SVMatroxImageProcessingClass::CreateImageChildBuffer( const SVImageInfoClass& rParentInfo, SVSmartHandlePointer rParentHandle,
															 SVImageInfoClass& rChildInfo, SVSmartHandlePointer& rChildHandle )
{
	HRESULT hrOk = S_FALSE;

	SVImageFormatEnum l_eChildFormat;
	SVImageFormatEnum l_eParentFormat;
	int l_iChildBandNumber = 0;
	int l_iParentBandNumber = 0;
	int l_iChildBandLink = 0;
	int l_iParentBandLink = 0;
	int l_iChildPixelDepth = 0;
	int l_iParentPixelDepth = 0;
	long l_lChildWidth = 0;
	long l_lParentWidth = 0;
	long l_lChildHeight = 0;
	long l_lParentHeight = 0;

	SVImageExtentClass l_svParentExtents = rParentInfo.GetExtents();
	SVImageExtentClass l_svChildExtents = rChildInfo.GetExtents();

	RECT l_oParent;
	RECT l_oChild;
	RECT l_oUnion;

	hrOk = l_svParentExtents.GetOutputRectangle( l_oParent );

	if ( hrOk == S_OK )
	{
		hrOk = l_svChildExtents.GetRectangle( l_oChild );
	}

	if ( hrOk == S_OK )
	{
		hrOk = GetChildImageCreateData( rChildInfo, l_eChildFormat, 
			l_iChildPixelDepth, l_iChildBandNumber, l_iChildBandLink, 
			l_lChildWidth, l_lChildHeight );
	}

	if ( hrOk == S_OK )
	{
		hrOk = GetOutputImageCreateData( rParentInfo, l_eParentFormat,
			l_iParentPixelDepth, l_iParentBandNumber, l_iParentBandLink,
			l_lParentWidth, l_lParentHeight );
	}


	if ( hrOk == S_OK &&
		rChildHandle.empty() &&
		l_lChildWidth > 0 && l_lChildHeight > 0 &&
		l_iChildBandNumber <= l_iParentBandNumber &&
		!rParentHandle.empty() &&
		l_lParentWidth > 0 && l_lParentHeight > 0 &&
		l_iParentPixelDepth > 0  && l_iParentBandNumber > 0 )
	{

		// I don't know why, but rParentInfo contains incorrect info for extent.Height sometimes.
		// Just get the actual width & height here
		long lRealParentHeight;
		long lRealParentWidth;
		SVMatroxBufferInterface::SVStatusCode l_Code;

		SVImageBufferHandleImage l_ParentMilHandle;
		rParentHandle->GetData( l_ParentMilHandle );

		l_Code = SVMatroxBufferInterface::Get( l_ParentMilHandle.GetBuffer(), SVSizeX, lRealParentWidth );
		l_Code = SVMatroxBufferInterface::Get( l_ParentMilHandle.GetBuffer(), SVSizeY, lRealParentHeight );

		// EB 2002 10 22
		// crop child; if camera changes and parent decreases in size, we will have problems
		// creating the child buffer if we don't crop
		// algorithm:
		//  check to see if child is completely inside parent
		//  if not,
		//      try shifting child's origin as much as possible to fit
		//      if child still doesn't fit (it's too big),
		//          crop child

		if( ! ::UnionRect( &l_oUnion, &l_oParent, &l_oChild ) ||
			! ::EqualRect( &l_oUnion, &l_oParent ) )
		{
			if (l_oChild.top < l_oParent.top) // should never be true? child is relative to parent
			{
				ASSERT(FALSE);
				l_oChild.top = 0;
			}
			else if (l_oChild.bottom > l_oParent.bottom) // may happen if the main image shrinks (camera files change)
			{
				l_oChild.top += l_oParent.bottom - l_oChild.bottom;
			}

			if (l_oChild.left < l_oParent.left)   // should never be true? child is relative to parent
			{
				ASSERT(FALSE);
				l_oChild.left = 0;
			}
			else if (l_oChild.right > l_oParent.right) // may happen if the main image shrinks (camera files change)
			{
				l_oChild.left += l_oParent.right - l_oChild.right;
			}

			if( ! ::UnionRect( &l_oUnion, &l_oParent, &l_oChild ) ||
				! ::EqualRect( &l_oUnion, &l_oParent ) )
			{
				if (l_oChild.bottom > l_oParent.bottom) // may happen if the main image shrinks (camera files change)
				{
					l_oChild.bottom = l_oParent.bottom;
				}

				else if (l_oChild.right > l_oParent.right) // may happen if the main image shrinks (camera files change)
				{
					l_oChild.right = l_oParent.right;
				}
			}

			if ( rChildInfo.SetExtentProperty( SVExtentPropertyPositionPointX, l_oChild.left ) != S_OK )
			{
				hrOk = S_FALSE;
			}

			if ( rChildInfo.SetExtentProperty( SVExtentPropertyPositionPointY, l_oChild.top ) != S_OK )
			{
				hrOk = S_FALSE;
			}

			if ( rChildInfo.SetExtentProperty( SVExtentPropertyWidth, l_oChild.right - l_oChild.left ) != S_OK )
			{
				hrOk = S_FALSE;
			}

			if ( rChildInfo.SetExtentProperty( SVExtentPropertyHeight, l_oChild.bottom - l_oChild.left ) != S_OK )
			{
				hrOk = S_FALSE;
			}

			if ( hrOk == S_OK )
			{
				hrOk = GetChildImageCreateData( rChildInfo, l_eChildFormat, 
					l_iChildPixelDepth, l_iChildBandNumber, l_iChildBandLink, 
					l_lChildWidth, l_lChildHeight );

				if ( hrOk == S_OK )
				{
					l_svChildExtents = rChildInfo.GetExtents();
				}

				if ( hrOk == S_OK )
				{
					hrOk = l_svChildExtents.GetRectangle( l_oChild );
				}
			}

			CString sMsg;
			if (rChildInfo.GetOwner() != NULL)
			{
				if (rParentInfo.GetOwner() != NULL)
				{
					sMsg.Format("\"%s\" was located partially or completely outside of "
						"the bounds of \"%s\".\nIt has been moved and/or resized "
						"to fit in the bounds.\nThis configuration should be "
						"saved to prevent this message from appearing again.", 
						rChildInfo.GetOwner()->GetCompleteObjectName(), 
						rParentInfo.GetOwner()->GetCompleteObjectName());
				}
				else 
				{
					sMsg.Format("\"%s\" was located partially or completely outside of "
						"the bounds of its parent image.\nIt has been moved "
						"and/or resized to fit in the bounds.\nThis configuration "
						"should be saved to prevent this message from appearing "
						"again.", rChildInfo.GetOwner()->GetCompleteObjectName());
				}
			}
			else
			{
				sMsg.Format("A child buffer was located partially or completely outside "
					"of the bounds of its parent image.\nIt has been moved "
					"and/or resized to fit in the bounds.\nThis configuration "
					"should be saved to prevent this message from appearing "
					"again.");
			}

			AfxMessageBox(sMsg);
		}


		if ( hrOk == S_OK )
		{
			if( l_iChildBandNumber < l_iParentBandNumber &&
				l_iChildBandNumber > 0 )
			{
				if( l_iChildBandLink < l_iParentBandNumber &&
					l_iChildBandLink >= 0 && l_iChildBandNumber == 1 )
				{
					SVMatroxBuffer l_NewBuffer;

					// Single band child of the BandLink band of a multi band parent
					// MIL Restriction: It is not possible to derive a 
					//                  multi band child of a multi band
					//					parent with different numbers of 
					//                  bands
					SVMatroxBufferCreateChildStruct l_Create( l_ParentMilHandle.GetBuffer() );
					l_Create.m_lBand = l_iChildBandLink;
					l_Create.m_lParentBandCount = l_iParentBandNumber;
					l_Create.m_lOffX = l_oChild.left;
					l_Create.m_lOffY = l_oChild.top;
					l_Create.m_lSizeX = l_lChildWidth;
					l_Create.m_lSizeY = l_lChildHeight;
					l_Code = SVMatroxBufferInterface::Create( l_NewBuffer, l_Create );

					rChildHandle = new SVImageBufferHandleStruct( l_NewBuffer );

					hrOk = l_Code == SVMEE_STATUS_OK ? S_OK : l_Code | SVMEE_MATROX_ERROR;
					if ( hrOk != S_OK )
					{

					}
				}
			}// end if( PChildInfo->BandNumber < PParentInfo->BandNumber && PChildInfo->BandNumber > 0 )
			else
			{
				SVMatroxBuffer l_NewBuffer;

				// Multi/single band child of a multi/single band parent 
				// (all bands!)
				SVMatroxBufferCreateChildStruct l_Create( l_ParentMilHandle.GetBuffer() );
				l_Create.m_lBand = SVValueAllBands;
				l_Create.m_lParentBandCount = l_iParentBandNumber;
				l_Create.m_lOffX = l_oChild.left;
				l_Create.m_lOffY = l_oChild.top;
				l_Create.m_lSizeX = l_lChildWidth;
				l_Create.m_lSizeY = l_lChildHeight;
				l_Code = SVMatroxBufferInterface::Create( l_NewBuffer, l_Create );

				rChildHandle = new SVImageBufferHandleStruct( l_NewBuffer );

				hrOk = (l_Code == SVMEE_STATUS_OK) ? S_OK : l_Code | SVMEE_MATROX_ERROR;
				if ( hrOk == S_OK )
				{
					rChildInfo.SetImageProperty( SVImagePropertyBandNumber, l_iParentBandNumber );
					rChildInfo.SetImageProperty( SVImagePropertyBandLink, 0 );
				}
				else
				{

				}
			}
		}
	}

	if ( hrOk != S_OK )
	{
		ASSERT(FALSE);
		AfxMessageBox( "Failed to create a child buffer!", MB_ICONEXCLAMATION );
	}

	return hrOk;
}

HDC SVMatroxImageProcessingClass::CreateBufferDC( const SVImageInfoClass& rInfo, SVSmartHandlePointer rHandle )
{
	HDC hDC = NULL;

	SVImageFormatEnum l_eFormat;
	int l_iBandNumber = 0;
	int l_iBandLink = 0;
	int l_iPixelDepth = 0;
	long l_lWidth = 0;
	long l_lHeight = 0;

	bool l_bOk = GetOutputImageCreateData( rInfo, l_eFormat, l_iPixelDepth, l_iBandNumber, l_iBandLink, l_lWidth, l_lHeight ) == S_OK;

	if( l_bOk && !rHandle.empty() &&
		l_iPixelDepth > 0 && l_iBandNumber > 0 )
	{
		SVImageBufferHandleImage l_MilHandle;
		rHandle->GetData( l_MilHandle );

		SVMatroxBufferInterface::SVStatusCode l_Code;

		//
		// Try using the 'THE' image for the HDC allocation.
		//
		long l_lValue = SVValueDefault;
		l_Code = SVMatroxBufferInterface::Set(l_MilHandle.GetBuffer(), SVBufWindowDCAlloc, static_cast<SVMatroxInt>(l_lValue) );

		//
		// Check for an error - most likely an 'invalid parameter' since
		// Image is not capable of 'creating' an HDC, does not have M_DIB
		// attribute
		//
		if( l_Code != SVMEE_STATUS_OK)
		{
			SVMatroxBuffer imageDIB_MIL;

			//
			// Create a new Mil image with a M_DIB attribute and copy the
			// image to new one.
			//
			SVMatroxBufferCreateStruct l_Create;
			l_Create.m_eAttribute = SVBufAttImageProcDib;
			l_Create.SetImageDepth( l_iPixelDepth );
			l_Create.m_lSizeX = l_lWidth;
			l_Create.m_lSizeY = l_lHeight;
			l_Create.m_lSizeBand = l_iBandNumber;

			l_Code = SVMatroxBufferInterface::Create(imageDIB_MIL, l_Create );


			if ( !imageDIB_MIL.empty()  )
			{
				//
				// Copy old image data to new image data buffer.
				//
				l_Code = SVMatroxBufferInterface::CopyBuffer( imageDIB_MIL, l_MilHandle.GetBuffer() );

				//
				// Now request the HDC from the new image with M_DIB attribute.
				//
				long l_lValue = SVValueDefault;
				l_Code = SVMatroxBufferInterface::Set( imageDIB_MIL, SVBufWindowDCAlloc, static_cast<SVMatroxInt>(l_lValue) );

				l_lValue = 0;
				l_Code = SVMatroxBufferInterface::Get(imageDIB_MIL, SVWindowDC, l_lValue);
				hDC = reinterpret_cast<HDC>( l_lValue );

				//
				// Substitute the new MIL image in place of the old one.
				//
				rHandle = new SVImageBufferHandleStruct( imageDIB_MIL );

			}
			else
			{
				long l_lValue = 0;
				l_Code = SVMatroxBufferInterface::Get(l_MilHandle.GetBuffer(), SVWindowDC, l_lValue);
				hDC = reinterpret_cast<HDC>( l_lValue);
			}

			InitBuffer( rHandle );
		}
		else
		{
			long l_lValue;
			l_Code = SVMatroxBufferInterface::Get(l_MilHandle.GetBuffer(), SVWindowDC, l_lValue);
			hDC = reinterpret_cast<HDC>(l_lValue);
		}
	}

	return hDC;
}

HRESULT SVMatroxImageProcessingClass::DestroyBufferDC( SVSmartHandlePointer rHandle, HDC hDC )
{
	HRESULT hrOk = S_OK;


	SVMatroxBufferInterface::SVStatusCode l_Code;

	if ( !rHandle.empty() )
	{
		SVImageBufferHandleImage l_MilHandle;
		rHandle->GetData( l_MilHandle );

		// Delete created device context. 
		long l_lValue = SVValueDefault;
		l_Code = SVMatroxBufferInterface::Set( l_MilHandle.GetBuffer(), SVBufWindowDCFree, static_cast<SVMatroxInt>(l_lValue) );		

		// Signal MIL that the buffer was modified. 
		l_Code = SVMatroxBufferInterface::Set( l_MilHandle.GetBuffer(), SVBufModified, static_cast<SVMatroxInt>(l_lValue) );		
		hrOk = (l_Code == SVMEE_STATUS_OK) ? S_OK : l_Code | SVMEE_MATROX_ERROR;
	}
	else
	{
		hrOk = SVMEE_INVALID_HANDLE;
	}

	return hrOk;
}

HRESULT SVMatroxImageProcessingClass::InitBuffer( SVSmartHandlePointer rHandle, DWORD dwValue )
{
	HRESULT hrOk = S_OK;

	hrOk = !rHandle.empty() ? S_OK : S_FALSE;
	if ( hrOk == S_OK )
	{
		SVImageBufferHandleImage l_MilHandle;
		rHandle->GetData( l_MilHandle );

		SVMatroxBufferInterface::SVStatusCode l_Code;
		try
		{
			l_Code = SVMatroxBufferInterface::ClearBuffer( l_MilHandle.GetBuffer(), static_cast<double>(dwValue) );
			hrOk = (l_Code == SVMEE_STATUS_OK) ? S_OK : l_Code | SVMEE_MATROX_ERROR;
		}
		catch( ... )
		{
			hrOk = S_FALSE;
		}
	}

	return hrOk;
}

HRESULT SVMatroxImageProcessingClass::LoadImageBuffer( LPCTSTR tstrImagePathName, SVImageInfoClass& rInfo, SVSmartHandlePointer& rHandle, BOOL bBrowseIfNotExists )
{
	HRESULT hrOk = S_OK;

	try
	{
		SVFileNameClass	svfncImageFile(tstrImagePathName);
		CString strImagePathName = svfncImageFile.GetFullFileName();

		if( tstrImagePathName != NULL )
		{
			if( ! ::SVFileExists( strImagePathName ) )
			{
				if( ! bBrowseIfNotExists )
					throw S_FALSE;

				CString strMessage;
				strMessage.Format( _T( "Unable to locate image file:\n%s\n\nBrowse..." ), strImagePathName );
				AfxMessageBox( strMessage );
				// Browse...
				//
				// Try to read the current image file path name from registry...
				//
				CString csPath = AfxGetApp()->GetProfileString( _T( "Settings" ),
					_T( "ImagesFilePath" ), 
					_T( "C:\\Images" ) );
				svfncImageFile.SetPathName(csPath);
				svfncImageFile.SetFileType(SV_IMAGE_SOURCE_FILE_TYPE);
				if( svfncImageFile.SelectFile() )
				{
					// Extract the file path...
					csPath = svfncImageFile.GetPathName();
					//
					// Write this path name back to registry...to initialize registry.
					//
					AfxGetApp()->WriteProfileString( _T( "Settings" ), _T( "ImagesFilePath" ), csPath );
				}
			}


			SVMatroxFileTypeEnum fileformat = SVFileUnknown;
			CString strExtension = svfncImageFile.GetExtension();
			if( strExtension.CompareNoCase(_T( ".mim" )) == 0 )
				fileformat = SVFileMIL;

			if( strExtension.CompareNoCase(_T( ".tif" )) == 0 )
				fileformat = SVFileTiff;

			if( strExtension.CompareNoCase(_T( ".bmp" )) == 0 )
				fileformat = SVFileBitmap;

			strImagePathName = svfncImageFile.GetFullFileName();
			if( fileformat != -1 && ::SVFileExists( strImagePathName ) )
			{

				SVMatroxBufferInterface::SVStatusCode l_Code;
				SVMatroxString l_strPath = strImagePathName;

				if( !rHandle.empty() )
				{
					SVImageBufferHandleImage l_MilHandle;
					rHandle->GetData( l_MilHandle );

					// Load...

					l_Code = SVMatroxBufferInterface::Import(l_MilHandle.GetBuffer(), l_strPath, fileformat );
					if( l_Code == SVMEE_STATUS_OK )
					{
						throw S_OK;
					}
					else
					{
						throw S_FALSE;
					}
				}

				// Restore
				SVMatroxBuffer newBuffer;
				l_Code = SVMatroxBufferInterface::Import( newBuffer, l_strPath, fileformat, true );
				if( !newBuffer.empty() )
				{
					// Get buffer data...
					long l_lSizeX = 0;
					long l_lSizeY = 0;
					long l_lBandSize = 0;
					long l_lPixelDepth = 0;
					long l_DataFormat = 0;

					l_Code = SVMatroxBufferInterface::Get( newBuffer, SVSizeX, l_lSizeX );
					l_Code = SVMatroxBufferInterface::Get( newBuffer, SVSizeY, l_lSizeY );
					l_Code = SVMatroxBufferInterface::Get( newBuffer, SVType, l_lPixelDepth );
					l_Code = SVMatroxBufferInterface::Get( newBuffer, SVSizeBand, l_lBandSize );
					l_Code = SVMatroxBufferInterface::Get( newBuffer, SVDataFormat, l_DataFormat );

					if( M_EQUIVALENT_INTERNAL_FORMAT( M_BGR24, l_DataFormat ) )
					{
						rInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatRGB888 );
					}
					else if( M_EQUIVALENT_INTERNAL_FORMAT( M_BGR32, l_DataFormat ) )
					{
						rInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatRGB8888 );
					}

					rInfo.SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0 );
					rInfo.SetExtentProperty( SVExtentPropertyWidth, l_lSizeX );
					rInfo.SetExtentProperty( SVExtentPropertyHeight, l_lSizeY );

					rInfo.SetImageProperty( SVImagePropertyBandNumber, l_lBandSize );
					rInfo.SetImageProperty( SVImagePropertyPixelDepth, l_lPixelDepth );

					newBuffer.clear();

					ASSERT( l_Code == SVMEE_STATUS_OK );
					if( S_OK == CreateImageBuffer( rInfo, rHandle ) && 
						S_OK == LoadImageBuffer( strImagePathName, rInfo, rHandle, FALSE ) )
					{
						throw S_OK;
					}
				}
			}

			AfxMessageBox( IDS_UNKNOWN_FILE_FORMAT_STRING );
		}

		AfxMessageBox( "Failed to load an image!", MB_ICONEXCLAMATION );
	}
	catch ( int hr )
	{
		hrOk = (HRESULT) hr;
	}
	catch ( HRESULT hr)
	{
		hrOk = hr;
	}

	return hrOk;
}

HRESULT SVMatroxImageProcessingClass::LoadImageBuffer( void* pBuffer, SVImageInfoClass& rBufferInfo, SVSmartHandlePointer& rBufferHandle, SVImageInfoClass& rCameraInfo )
{
	HRESULT hrOk = S_OK;

	try
	{
		BOOL l_bCreateBuffer = FALSE;

		BITMAPINFOHEADER* pbmhInfo;
		BITMAPINFO* pbmInfo;
		BYTE* pBits;

		// Make sure color table size is calculated
		pbmInfo  = (BITMAPINFO*) (BYTE*)pBuffer;
		pbmhInfo = (BITMAPINFOHEADER*) &pbmInfo->bmiHeader;

		long l_lBitmapHeaderSize = sizeof( BITMAPINFOHEADER );
		long l_lDIBSize = pbmhInfo->biSizeImage;

		long l_lColorTableSize = 0;

		if( 0 < pbmhInfo->biClrUsed )
		{
			if( pbmhInfo->biBitCount < 16 )
			{
				l_lColorTableSize = pbmhInfo->biClrUsed * sizeof( RGBQUAD );
			}
			else
			{
				l_lColorTableSize = pbmhInfo->biClrUsed * sizeof( RGBQUAD );

				if( pbmhInfo->biBitCount == 16 || pbmhInfo->biBitCount == 32 )
				{
					l_lColorTableSize += 3 * sizeof( DWORD );
				}
			}
		}

		pBits = (BYTE*)( pBuffer ) + l_lBitmapHeaderSize + l_lColorTableSize;

		if( !rBufferHandle.empty() )
		{
			SVImageFormatEnum l_eFormat;
			int l_iBandNumber = 0;
			int l_iBandLink = 0;
			int l_iPixelDepth = 0;
			long l_lWidth = 0;
			long l_lHeight = 0;

			bool l_bOk = GetOutputImageCreateData( rBufferInfo, l_eFormat, l_iPixelDepth, l_iBandNumber, l_iBandLink, l_lWidth, l_lHeight ) == S_OK;

			if( ! l_bOk || 
				l_lWidth != pbmhInfo->biWidth ||
				l_lHeight != abs( pbmhInfo->biHeight )	||
				l_iPixelDepth != pbmhInfo->biBitCount )
				// Buffer is of the wrong size
				throw S_FALSE;
		}// end if
		else
		{
			// no buffer is available
			rBufferInfo = rCameraInfo;
			// Get Image type from the input image for this temperary image so the buffer data will be correct
			// and the pBits will fit.
			if( pbmInfo->bmiHeader.biBitCount == 32 )
			{
				rBufferInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatRGB8888 );
			}
			if( pbmInfo->bmiHeader.biBitCount == 24 )
			{
				rBufferInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatRGB888 );
			}

			if( S_OK != CreateImageBuffer( rBufferInfo, rBufferHandle ) )
				throw S_FALSE;

			l_bCreateBuffer = TRUE;
		}// end else

		SVImageInfoClass oTempInfo;
		SVSmartHandlePointer oTempHandle;

		oTempInfo = rBufferInfo;

		oTempInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );
		oTempInfo.SetImageProperty( SVImagePropertyBandLink, 0 );
		oTempInfo.SetImageProperty( SVImagePropertyPixelDepth, 8 );

		oTempInfo.SetExtentProperty( SVExtentPropertyHeight, abs(pbmhInfo->biHeight) );
		oTempInfo.SetExtentProperty( SVExtentPropertyWidth, pbmhInfo->biWidth );

		if ( 8 < pbmhInfo->biBitCount )
		{
			oTempInfo.SetImageProperty( SVImagePropertyBandNumber, 3 );
		}

		if( S_OK != CreateImageBuffer( oTempInfo, oTempHandle ) )
		{
			rBufferHandle.clear();

			throw S_FALSE;
		}

		int l_iBandNumber = 0;
		int l_iBandLink = 0;

		if ( rCameraInfo.GetImageProperty( SVImagePropertyBandNumber, l_iBandNumber ) != S_OK || 
			rCameraInfo.GetImageProperty( SVImagePropertyBandLink, l_iBandLink ) != S_OK )
		{
			throw S_FALSE;
		}

		// Copy the bits into the image object
		if( pBits != NULL && !oTempHandle.empty()  && !rBufferHandle.empty() )
		{
			SVImageBufferHandleImage l_TempMilHandle;
			SVImageBufferHandleImage l_MilHandle;

			oTempHandle->GetData( l_TempMilHandle );
			rBufferHandle->GetData( l_MilHandle );

			// Set buffer data...
			memcpy( oTempHandle->GetBufferAddress(), pBits, pbmhInfo->biSizeImage );


			SVMatroxBufferInterface::SVStatusCode l_Code;

			if( 8 < pbmhInfo->biBitCount && l_iBandNumber == 3 )
			{
				l_Code = SVMatroxBufferInterface::CopyBuffer( l_MilHandle.GetBuffer(), l_TempMilHandle.GetBuffer() );
			}
			else
			{
				l_Code = SVMatroxBufferInterface::CopyBuffer( l_MilHandle.GetBuffer(), l_TempMilHandle.GetBuffer(), l_iBandLink );
			}

			throw S_OK;
		}

		rBufferHandle.clear();

		AfxMessageBox( "Failed to load an image!", MB_ICONEXCLAMATION );
	}
	catch ( int hr )
	{
		hrOk = (HRESULT) hr;
	}
	catch ( HRESULT hr)
	{
		hrOk = hr;
	}

	return hrOk;
}

HRESULT SVMatroxImageProcessingClass::SaveImageBuffer( LPCTSTR tstrImagePathName, const SVSmartHandlePointer& rHandle )
{
	HRESULT l_Status = S_OK;

	if( !( rHandle.empty() ) )
	{
		SVFileNameClass	svfncImageFile( tstrImagePathName );

		SVImageBufferHandleImage l_MilHandle;
		rHandle->GetData( l_MilHandle );

		SVMatroxFileTypeEnum l_efileformat = SVFileUnknown;

		CString strExtension = svfncImageFile.GetExtension();

		if( strExtension.CompareNoCase(_T( ".mim" )) == 0 )
			l_efileformat = SVFileMIL;

		if( strExtension.CompareNoCase(_T( ".tif" )) == 0 )
			l_efileformat = SVFileTiff;

		if( strExtension.CompareNoCase(_T( ".bmp" )) == 0 )
			l_efileformat = SVFileBitmap;

		if( l_efileformat != SVFileUnknown )
		{
			SVMatroxString l_strPath = tstrImagePathName;
			l_Status = SVMatroxBufferInterface::Export( l_MilHandle.GetBuffer(), l_strPath, l_efileformat );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVMatroxImageProcessingClass::CopyImageBuffer( SVSmartHandlePointer& rToHandle, const SVSmartHandlePointer& rFromHandle )
{
	HRESULT l_Status = S_OK;

	if( !( rToHandle.empty() ) && !( rFromHandle.empty() ) )
	{
		SVImageBufferHandleImage l_ToMilHandle;
		SVImageBufferHandleImage l_FromMilHandle;

		l_Status = rToHandle->GetData( l_ToMilHandle );

		if( l_Status == S_OK )
		{
			l_Status = rFromHandle->GetData( l_FromMilHandle );
		}

		if( l_Status == S_OK )
		{
			l_Status = SVMatroxBufferInterface::CopyBuffer( l_ToMilHandle.GetBuffer(), l_FromMilHandle.GetBuffer() );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVMatroxImageProcessingClass::CopyImageBuffer( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle )
{
	HRESULT l_Status = S_OK;

	if( !( rFromHandle.empty() ) )
	{
		SVImageBufferHandleImage l_FromMilHandle;

		l_Status = rFromHandle->GetData( l_FromMilHandle );

		if( l_Status == S_OK )
		{
			l_Status = SVMatroxBufferInterface::CopyBuffer( p_rToDIB, l_FromMilHandle.GetBuffer() );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVMatroxImageProcessingClass::CopyImageBuffer( SVByteVector& p_rToDIB, const SVBitmapInfo& p_rToBitmapInfo, const SVSmartHandlePointer& rFromHandle )
{
	HRESULT l_Status = S_OK;

	if( !( rFromHandle.empty() ) )
	{
		SVImageBufferHandleImage l_FromMilHandle;

		l_Status = rFromHandle->GetData( l_FromMilHandle );

		if( l_Status == S_OK )
		{
			l_Status = SVMatroxBufferInterface::CopyBuffer( p_rToDIB, p_rToBitmapInfo, l_FromMilHandle.GetBuffer() );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVMatroxImageProcessingClass::CopyImageBufferToFileDIB( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle )
{
	HRESULT l_Status = S_OK;

	if( !( rFromHandle.empty() ) )
	{
		SVImageBufferHandleImage l_FromMilHandle;

		l_Status = rFromHandle->GetData( l_FromMilHandle );

		if( l_Status == S_OK )
		{
			l_Status = SVMatroxBufferInterface::CopyBufferToFileDIB( p_rToDIB, l_FromMilHandle.GetBuffer() );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVMatroxImageProcessingClass::CopyImageBuffer( SVImageCopyUtility& p_rImageCopier, const SVSmartHandlePointer& rFromHandle )
{
	HRESULT l_Status = S_OK;

	if( !( rFromHandle.empty() ) )
	{
		SVImageBufferHandleImage l_FromMilHandle;

		l_Status = rFromHandle->GetData( l_FromMilHandle );

		if( l_Status == S_OK )
		{
			l_Status = SVMatroxBufferInterface::CopyDIBBufferToMemory(p_rImageCopier, l_FromMilHandle.GetBuffer());
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

HRESULT SVMatroxImageProcessingClass::ConvertImageBuffer( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle, SVImageOperationTypeEnum p_lConversionType )
{
	HRESULT l_Status = S_OK;

	if( !( rFromHandle.empty() ) )
	{
		SVMatroxBuffer l_Temp;
		SVImageBufferHandleImage l_FromMilHandle;

		l_Status = rFromHandle->GetData( l_FromMilHandle );

		if( l_Status == S_OK )
		{
			l_Status = SVMatroxBufferInterface::Create( l_Temp, l_FromMilHandle.GetBuffer() );
		}

		if( l_Status == S_OK )
		{
			l_Status = SVMatroxImageInterface::Convert( l_Temp, l_FromMilHandle.GetBuffer(), p_lConversionType );
		}

		if( l_Status == S_OK )
		{
			l_Status = SVMatroxBufferInterface::CopyBuffer( p_rToDIB, l_Temp );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVMatroxImageProcessingClass::CreateDataBuffer( SVDataBufferInfoClass* pDataInfo )
{
	HRESULT hrOk = S_OK;

	if( pDataInfo != NULL && pDataInfo->Length > 0 &&
		pDataInfo->HBuffer.milResult.empty() )
	{
		SVImageOperationTypeEnum l_eType;

		switch( pDataInfo->Type )
		{
		case SVDataBufferInfoClass::SVProjectResult:
			l_eType = SVImageProjList;
			break;
			case SVDataBufferInfoClass::SVProjectResult | 
				SVDataBufferInfoClass::SVContainsFloatData:
				l_eType = SVImageProjListFloat;
				break;
		case SVDataBufferInfoClass::SVEventResult:
			l_eType = SVImageEventList;
			break;
			case SVDataBufferInfoClass::SVEventResult | 
				SVDataBufferInfoClass::SVContainsFloatData:
				l_eType = SVImageEventListFloat;
				break;
		case SVDataBufferInfoClass::SVExtremeResult:
			l_eType = SVImageExtreemList;
			break;
			case SVDataBufferInfoClass::SVExtremeResult | 
				SVDataBufferInfoClass::SVContainsFloatData:
				l_eType = SVImageExtreemListFloat;
				break;
		case SVDataBufferInfoClass::SVCountResult:
			l_eType = SVImageCountList;
			break;
		case SVDataBufferInfoClass::SVHistResult:
			l_eType = SVImageHistList;
			break;
		}

		// Allocate result buffer...

		SVMatroxImageInterface::SVStatusCode l_Code;

		l_Code = SVMatroxImageInterface::Create(pDataInfo->HBuffer.milResult, pDataInfo->Length, l_eType);


		hrOk = !pDataInfo->HBuffer.milResult.empty() ? S_OK : S_FALSE;
	}

	return hrOk;
}

HRESULT SVMatroxImageProcessingClass::DestroyDataBuffer( SVDataBufferInfoClass* pDataInfo )
{
	HRESULT hrOk = S_OK;

	if ( pDataInfo != NULL )
	{
		hrOk = S_OK;

		if( !pDataInfo->HBuffer.milResult.empty() )
		{

			SVMatroxImageInterface::Destroy( pDataInfo->HBuffer.milResult );

		}
	}

	return hrOk;
}

HRESULT SVMatroxImageProcessingClass::ResizeDataBuffer( SVDataBufferInfoClass* pDataInfo, long lNewLength )
{
	HRESULT hrOk = S_OK;

	hrOk = DestroyDataBuffer( pDataInfo );
	if ( hrOk == S_OK )
	{
		pDataInfo->Length = lNewLength;

		hrOk = CreateDataBuffer( pDataInfo );
	}

	return hrOk;
}

HRESULT SVMatroxImageProcessingClass::CreateBarCodeBuffer(SVBarCodeBufferInfoClass* pDataInfo)
{
	HRESULT hrOk = S_OK;

	if( pDataInfo != NULL && pDataInfo->HBuffer.milBarCode.empty() )
	{
		// Allocate Bar Code buffer...

		SVMatroxBarCodeInterface::Create(pDataInfo->HBuffer.milBarCode, pDataInfo->m_lCodeType );

		hrOk = (!pDataInfo->HBuffer.milBarCode.empty()) ? S_OK : S_FALSE;
	}

	return hrOk;
}

HRESULT SVMatroxImageProcessingClass::GetOutputImageCreateData( const SVImageInfoClass &p_rsvInfo,
															   SVImageFormatEnum &p_reFormat,
															   int &p_riPixelDepth,
															   int &p_riBandNumber,
															   int &p_riBandLink,
															   long &p_rlWidth,
															   long &p_rlHeight ) const
{
	HRESULT l_hrOk = p_rsvInfo.GetExtentProperty( SVExtentPropertyOutputWidth, p_rlWidth );

	if ( l_hrOk == S_OK )
	{
		l_hrOk = p_rsvInfo.GetExtentProperty( SVExtentPropertyOutputHeight, p_rlHeight );
	}

	if ( l_hrOk == S_OK )
	{
		int l_iFormat = SVImageFormatUnknown;

		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyFormat, l_iFormat );

		if ( l_hrOk == S_OK )
		{
			p_reFormat = (SVImageFormatEnum)l_iFormat;
		}
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyBandNumber, p_riBandNumber );
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyBandLink, p_riBandLink );
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyPixelDepth, p_riPixelDepth );
	}

	return l_hrOk;
}

HRESULT SVMatroxImageProcessingClass::GetChildImageCreateData( const SVImageInfoClass &p_rsvInfo,
															  SVImageFormatEnum &p_reFormat,
															  int &p_riPixelDepth,
															  int &p_riBandNumber,
															  int &p_riBandLink,
															  long &p_rlWidth,
															  long &p_rlHeight ) const
{
	HRESULT l_hrOk = p_rsvInfo.GetExtentProperty( SVExtentPropertyWidth, p_rlWidth );

	if ( l_hrOk == S_OK )
	{
		l_hrOk = p_rsvInfo.GetExtentProperty( SVExtentPropertyHeight, p_rlHeight );
	}

	if ( l_hrOk == S_OK )
	{
		int l_iFormat = SVImageFormatUnknown;

		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyFormat, l_iFormat );

		if ( l_hrOk == S_OK )
		{
			p_reFormat = (SVImageFormatEnum)l_iFormat;
		}
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyBandNumber, p_riBandNumber );
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyBandLink, p_riBandLink );
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyPixelDepth, p_riPixelDepth );
	}

	return l_hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMatroxImageProcessingClass.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 15:24:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:34:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   11 Feb 2013 14:19:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   18 Jul 2012 14:10:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   10 Jul 2012 12:13:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added addition image copy method to interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   28 Jun 2012 16:07:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removing Shared Memory for the webserver from the SVObserver application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   19 Dec 2011 16:07:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow data to be processed into shared memory and create shared memory structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Sep 2011 16:06:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated object to use the new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   13 Apr 2011 19:16:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to correctly import and save color images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   16 Mar 2011 13:43:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   10 Feb 2011 15:00:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   27 Jan 2011 11:33:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   04 Nov 2010 13:14:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   16 Dec 2009 12:00:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   30 Jul 2009 12:04:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   16 Sep 2008 14:28:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CreateImageBufferOverlay to use the aligned width (stride) instead of the possibly unaligned width. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   10 Jul 2008 08:32:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  630
 * SCR Title:  Fix Matrox interface bug in color system
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified LoadImageBuffer to use the incomming image for the temperary image type so the copy will work on both 24 bit and 32 bit images. This function is used by SetSourceImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   10 Apr 2008 08:07:10   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   * Updated CreateImageBuffer to support 16-bit RGB565 display mode.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   10 Apr 2008 08:07:04   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   * Updated CreateImageBuffer to support 16-bit RGB565 display mode.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   24 Jul 2007 14:27:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   21 Jun 2007 14:16:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25.1.0   14 Jul 2009 14:37:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated with new image methods and data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   19 Feb 2007 16:38:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   15 Feb 2007 11:50:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated flags for the mappgeterror fucntion to remove the M_MESSAGE flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   20 Jan 2006 08:20:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove compilier warnings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   13 Dec 2005 14:29:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update method to remove an invalid check that has no impact on the program execution except waste time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   08 Nov 2005 15:59:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   17 Feb 2005 14:50:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   10 Nov 2003 12:23:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  395
 * SCR Title:  Color ROI does not update unless you move ROI in training
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the RGBMainImage and ColorTool to reduce possibilities of getting out of sync and to consolidate the color image specific code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   30 Jul 2003 11:31:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  328
 * SCR Title:  Upgrade SVObserver to latest released version of MIL
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed bug with the change from BOOL to HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   11 Jul 2003 08:21:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated functions to use the internal display handle.  Added new method to get access to that the internal display handle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   08 Jul 2003 10:03:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed BOOL to HRESULT and other general fixes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   27 Jun 2003 08:55:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with display buffers not being deleted correctly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   23 May 2003 13:59:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed location of log file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   23 May 2003 13:36:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #include
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   23 May 2003 11:11:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added logging for creating and destroying displays
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   22 Apr 2003 11:17:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   25 Mar 2003 09:11:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed code to fix ::LoadImageBuffer to get the bitmap info first and never flip the image from the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   24 Mar 2003 14:15:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changed LoadImageBuffer to not use the 4 bytes for the table size
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   21 Mar 2003 12:31:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  330
 * SCR Title:  Add SetImageList to ActiveX Interface
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the way the buffers are being created for when the activeX control sends an image for the loadImageTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   19 Mar 2003 13:30:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  330
 * SCR Title:  Add SetImageList to ActiveX Interface
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code for loading an image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Mar 2003 11:45:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  327
 * SCR Title:  Load Image Tool - unless the reload is checked you can not preview the image
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   In LoadImageBuffer added error checking for the MbufImport
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Feb 2003 10:57:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem where Coreco Image processing Reset was not being called
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Feb 2003 11:45:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Inserted forgotten "lMaxDisplays = "
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Feb 2003 11:55:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed problem with loading configurations on different hardware
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Feb 2003 10:12:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Feb 2003 10:56:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed merge errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Jan 2003 09:30:38   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated all appropriate methods with SVImageBufferHandleStruct or SVImageObjectClass parameter for methods that used SVImageInfoClass as a parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   23 Jan 2003 15:29:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed several bugs with GetProductImageList & SetSourceImage & locking images & images w/o overlays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Jan 2003 09:56:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   no change
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jan 2003 15:29:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added:
 * GetLut
 * SetLut
 * CalculateLut
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jan 2003 10:37:32   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Added the method BOOL LoadImageBuffer( void *pBuffer, SVImageInfoClass* PInfo )  to load an Image from a memory buffer.
 * Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Dec 2002 19:46:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   SVMatroxDisplayImageProcessingClass::CreateAcquisitionBuffer
 * - Modified to fix problem when loading a configuration on a machine without a frame grabber card.
 * 
 * SVMatroxDisplayImageProcessingClass::CreateImageBuffer
 * SVMatroxDisplayImageProcessingClass::CreateDisplayBuffer
 * SVMatroxDisplayImageProcessingClass::CreateChildBuffer
 * SVMatroxDisplayImageProcessingClass::DestroyDisplayBuffer
 * SVMatroxDisplayImageProcessingClass::CreateDisplayResource
 * SVMatroxDisplayImageProcessingClass::DestroyDisplayResource
 * - Modified to fix a problem with running out of display buffers.
 * 
 * SVMatroxDisplayImageProcessingClass::ReportMilError
 * - Added as new function to display MIL error information.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:56:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
