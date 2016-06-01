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

#pragma region Includes
#include "stdafx.h"
#include "SVMatroxImageProcessingClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVBarCodeBuffer.h"
#include "SVDataBuffer.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVGlobal.h"
#include "SVUtilityLibrary/SVImageCopyUtility.h"
#include "SVMatroxLibrary/SVMatroxImagingLibrary.h"  // has MIL includes
#include "SVStatusLibrary/MessageManagerResource.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "TextDefinesSvO.h"
#include "SVOGui/TextDefinesSvOg.h"
#pragma endregion Includes

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
	SVMatroxBufferAttributeEnum format;

	rHandle.clear();

	hrOk = GetOutputImageCreateData( rInfo, l_eFormat, l_iPixelDepth, l_iBandNumber, l_iBandLink, l_lWidth, l_lHeight );
	
	if (S_OK == hrOk)
	{
		switch( l_eFormat )
		{
		case SVImageFormatRGB565:
			{
				format = SVBuffAttImageProcPackedOffBoardDibPagedRgb16;
				break;
			}
		case SVImageFormatRGB888:
			{
				format = SVBuffAttImageProcPackedOffBoardDibPagedBgr24;
				break;
			}
		case SVImageFormatRGB8888:
			{
				format = SVBufAttImageProcPackedOffBoardDibPagedBgr32;
				break;
			}
		default:
			{
				if (!rInfo.getDibBufferFlag())
				{
					format = SVBufAttImageProc;
				}
				else
				{
					format = SVBufAttImageProcDib;
				}
			}
		}

		hrOk = CreateImageBuffer(l_iPixelDepth, l_iBandNumber, l_lWidth, l_lHeight, format, rHandle);
	}

	if (SVMEE_MBUF_ALLOCATION_FAILED == hrOk)
	{
		hrOk = SVMSG_SVO_5067_IMAGEALLOCATIONFAILED;
	}
	else
	if ( S_OK != hrOk && !bDisplayedErrorMessage )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_CreateBufferFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10064 );
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

		if( S_OK == l_Status )
		{
			l_Status = SVMatroxBufferInterface::Create( l_Temp, l_FromMilHandle.GetBuffer() );

			if( S_OK == l_Status )
			{
				rToHandle = new SVImageBufferHandleStruct( l_Temp );
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = SVMatroxImageInterface::Convert( l_Temp, l_FromMilHandle.GetBuffer(), p_lConversionType );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if( S_OK != l_Status )
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

	if ( S_OK == hrOk )
	{
		hrOk = l_svChildExtents.GetRectangle( l_oChild );
	}

	if ( S_OK == hrOk )
	{
		hrOk = GetChildImageCreateData( rChildInfo, l_eChildFormat, 
			l_iChildPixelDepth, l_iChildBandNumber, l_iChildBandLink, 
			l_lChildWidth, l_lChildHeight );
	}

	if ( S_OK == hrOk )
	{
		hrOk = GetOutputImageCreateData( rParentInfo, l_eParentFormat,
			l_iParentPixelDepth, l_iParentBandNumber, l_iParentBandLink,
			l_lParentWidth, l_lParentHeight );
	}


	if ( S_OK == hrOk &&
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

			if ( S_OK != rChildInfo.SetExtentProperty( SVExtentPropertyPositionPointX, l_oChild.left ) )
			{
				hrOk = S_FALSE;
			}

			if ( S_OK != rChildInfo.SetExtentProperty( SVExtentPropertyPositionPointY, l_oChild.top ) )
			{
				hrOk = S_FALSE;
			}

			if ( S_OK != rChildInfo.SetExtentProperty( SVExtentPropertyWidth, l_oChild.right - l_oChild.left ) )
			{
				hrOk = S_FALSE;
			}

			if ( S_OK != rChildInfo.SetExtentProperty( SVExtentPropertyHeight, l_oChild.bottom - l_oChild.left ) )
			{
				hrOk = S_FALSE;
			}

			if ( S_OK == hrOk )
			{
				hrOk = GetChildImageCreateData( rChildInfo, l_eChildFormat, 
					l_iChildPixelDepth, l_iChildBandNumber, l_iChildBandLink, 
					l_lChildWidth, l_lChildHeight );

				if ( S_OK == hrOk )
				{
					l_svChildExtents = rChildInfo.GetExtents();
				}

				if ( S_OK == hrOk )
				{
					hrOk = l_svChildExtents.GetRectangle( l_oChild );
				}
			}

			SvStl::MessageContainer message;
			if (nullptr != rChildInfo.GetOwner())
			{
				if (nullptr != rParentInfo.GetOwner())
				{
					SVStringArray msgList;
					msgList.push_back(SVString(rChildInfo.GetOwner()->GetCompleteObjectName()));
					msgList.push_back(SVString(rParentInfo.GetOwner()->GetCompleteObjectName()));
					message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_CreateImageChildBuffer_parent, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10065);
				}
				else 
				{
					SVStringArray msgList;
					msgList.push_back(SVString(rChildInfo.GetOwner()->GetCompleteObjectName()));
					message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_CreateImageChildBuffer_child, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10065);
				}
			}
			else
			{
				message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_CreateImageChildBuffer, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10065);
			}

			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( message.getMessage() );
		}

		if ( S_OK == hrOk )
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
					if ( S_OK != hrOk )
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
				if ( S_OK == hrOk )
				{
					rChildInfo.SetImageProperty( SVImagePropertyBandNumber, l_iParentBandNumber );
					rChildInfo.SetImageProperty( SVImagePropertyBandLink, 0 );
				}
			}
		}
	}

	if ( S_OK != hrOk )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_CreateBufferFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10066 );
	}

	return hrOk;
}

HDC SVMatroxImageProcessingClass::CreateBufferDC( const SVImageInfoClass& rInfo, SVSmartHandlePointer rHandle )
{
	HDC hDC = nullptr;

	SVImageFormatEnum l_eFormat;
	int l_iBandNumber = 0;
	int l_iBandLink = 0;
	int l_iPixelDepth = 0;
	long l_lWidth = 0;
	long l_lHeight = 0;

	bool l_bOk = S_OK == GetOutputImageCreateData( rInfo, l_eFormat, l_iPixelDepth, l_iBandNumber, l_iBandLink, l_lWidth, l_lHeight );

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
	if ( S_OK == hrOk )
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
	SVFileNameClass	svfncImageFile(tstrImagePathName);
	CString strImagePathName = svfncImageFile.GetFullFileName();

	if( nullptr != tstrImagePathName )
	{
		if( ! ::SVFileExists( strImagePathName ) )
		{
			if( ! bBrowseIfNotExists )
			{
				return S_FALSE;
			}

			SVStringArray msgList;
			msgList.push_back(SVString(strImagePathName));
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_MatroxImage_UnableToFindFile, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10067 );
			// Browse...
			//
			// Try to read the current image file path name from registry...
			//
			///read from ini file 
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
			SVString l_strPath = strImagePathName;

			if( !rHandle.empty() )
			{
				SVImageBufferHandleImage l_MilHandle;
				rHandle->GetData( l_MilHandle );

				// Load...

				l_Code = SVMatroxBufferInterface::Import(l_MilHandle.GetBuffer(), l_strPath, fileformat );
				if( l_Code == SVMEE_STATUS_OK )
				{
					return S_OK;
				}
				else
				{
					return S_FALSE;
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
					return S_OK;
				}
			}
		}

		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_UnKnownFileFormat, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10068 );
	}

	SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
	Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_FailedToLoadImage, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10069 );

	return S_FALSE;
}

HRESULT SVMatroxImageProcessingClass::LoadImageBuffer( void* pBuffer, SVImageInfoClass& rBufferInfo, SVSmartHandlePointer& rBufferHandle, SVImageInfoClass& rCameraInfo )
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

		bool l_bOk = S_OK == GetOutputImageCreateData( rBufferInfo, l_eFormat, l_iPixelDepth, l_iBandNumber, l_iBandLink, l_lWidth, l_lHeight );

		if( ! l_bOk || 
			l_lWidth != pbmhInfo->biWidth ||
			l_lHeight != abs( pbmhInfo->biHeight )	||
			l_iPixelDepth != pbmhInfo->biBitCount )
		{
			// Buffer is of the wrong size
			return S_FALSE;
		}
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
		{
			return S_FALSE;
		}

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

		return S_FALSE;
	}

	int l_iBandNumber = 0;
	int l_iBandLink = 0;

	if ( S_OK != rCameraInfo.GetImageProperty( SVImagePropertyBandNumber, l_iBandNumber ) || 
		S_OK != rCameraInfo.GetImageProperty( SVImagePropertyBandLink, l_iBandLink ) )
	{
		return S_FALSE;
	}

	// Copy the bits into the image object
	if( nullptr != pBits && !oTempHandle.empty()  && !rBufferHandle.empty() )
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

		return S_OK;
	}

	rBufferHandle.clear();

	SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
	Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_FailedToLoadImage, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10070 );

	return S_FALSE;
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
			SVString l_strPath = tstrImagePathName;
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

		if( S_OK == l_Status )
		{
			l_Status = rFromHandle->GetData( l_FromMilHandle );
		}

		if( S_OK == l_Status )
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

		if( S_OK == l_Status )
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

		if( S_OK == l_Status )
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

		if( S_OK == l_Status )
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

		if( S_OK == l_Status )
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

		if( S_OK == l_Status )
		{
			l_Status = SVMatroxBufferInterface::Create( l_Temp, l_FromMilHandle.GetBuffer() );
		}

		if( S_OK == l_Status )
		{
			l_Status = SVMatroxImageInterface::Convert( l_Temp, l_FromMilHandle.GetBuffer(), p_lConversionType );
		}

		if( S_OK == l_Status )
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

	if( nullptr != pDataInfo && pDataInfo->Length > 0 &&
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

	if ( nullptr != pDataInfo )
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
	if ( S_OK == hrOk )
	{
		pDataInfo->Length = lNewLength;

		hrOk = CreateDataBuffer( pDataInfo );
	}

	return hrOk;
}

HRESULT SVMatroxImageProcessingClass::CreateBarCodeBuffer(SVBarCodeBufferInfoClass* pDataInfo)
{
	HRESULT hrOk = S_OK;

	if( nullptr != pDataInfo && pDataInfo->HBuffer.milBarCode.empty() )
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

	if ( S_OK == l_hrOk )
	{
		l_hrOk = p_rsvInfo.GetExtentProperty( SVExtentPropertyOutputHeight, p_rlHeight );
	}

	if ( S_OK == l_hrOk )
	{
		int l_iFormat = SVImageFormatUnknown;

		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyFormat, l_iFormat );

		if ( S_OK == l_hrOk )
		{
			p_reFormat = (SVImageFormatEnum)l_iFormat;
		}
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyBandNumber, p_riBandNumber );
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyBandLink, p_riBandLink );
	}

	if ( S_OK == l_hrOk )
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

	if ( S_OK == l_hrOk )
	{
		l_hrOk = p_rsvInfo.GetExtentProperty( SVExtentPropertyHeight, p_rlHeight );
	}

	if ( S_OK == l_hrOk )
	{
		int l_iFormat = SVImageFormatUnknown;

		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyFormat, l_iFormat );

		if ( S_OK == l_hrOk )
		{
			p_reFormat = (SVImageFormatEnum)l_iFormat;
		}
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyBandNumber, p_riBandNumber );
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyBandLink, p_riBandLink );
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = p_rsvInfo.GetImageProperty( SVImagePropertyPixelDepth, p_riPixelDepth );
	}

	return l_hrOk;
}

HRESULT SVMatroxImageProcessingClass::CreateImageBuffer( int pixelDepth, int bandNumber, long width, long height, SVMatroxBufferAttributeEnum format, SVSmartHandlePointer &rHandle )
{
	HRESULT hrOk = S_OK;
	if( 0 < pixelDepth && 0 < bandNumber &&
		0 < width && 0 < height )
	{
		SVMatroxBufferInterface::SVStatusCode code;
		SVMatroxBufferCreateStruct createStruct;

		createStruct.m_lSizeBand = bandNumber;
		createStruct.m_lSizeX = width;
		createStruct.m_lSizeY = height;
		createStruct.SetImageDepth( pixelDepth );
		createStruct.m_eAttribute = format;

		SVMatroxBuffer newBuffer;
		// Allocate a workable multi or single band image buffer
		code = SVMatroxBufferInterface::Create( newBuffer, createStruct );

		rHandle = new SVImageBufferHandleStruct( newBuffer );

		hrOk = code == SVMEE_STATUS_OK ? S_OK : code | SVMEE_MATROX_ERROR;
		if ( S_OK == hrOk )
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
	return hrOk;
}

