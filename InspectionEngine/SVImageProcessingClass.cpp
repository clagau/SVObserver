//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageProcessingClass
//* .File Name       : $Workfile:   SVImageProcessingClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:54:02  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageProcessingClass.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVBarCodeBuffer.h"
#include "SVDataBuffer.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVUtilityLibrary/SVImageCopyUtility.h"
#include "Definitions/StringTypeDef.h"
#include "SVMatroxLibrary/SVMatroxImagingLibrary.h"  // has MIL includes
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#pragma endregion Includes

HRESULT SVImageProcessingClass::CreateImageBuffer( const SVImageInfoClass& rInfo, SvOi::SVImageBufferHandlePtr& rHandle, SvStl::MessageContainerVector* pErrorContainer)
{
	HRESULT Result( S_OK );

	SvDef::SVImageFormatEnum l_eFormat;
	int l_iBandNumber = 0;
	int l_iBandLink = 0;
	int l_iPixelDepth = 0;
	long l_lWidth = 0;
	long l_lHeight = 0;
	SVMatroxBufferAttributeEnum format;

	rHandle.reset();

	Result = GetOutputImageCreateData( rInfo, l_eFormat, l_iPixelDepth, l_iBandNumber, l_iBandLink, l_lWidth, l_lHeight );

	if (S_OK == Result)
	{
		switch( l_eFormat )
		{
		case SvDef::SVImageFormatRGB565:
			{
				format = SVBuffAttImageProcPackedOffBoardDibPagedRgb16;
				break;
			}
		case SvDef::SVImageFormatRGB888:
			{
				format = SVBuffAttImageProcPackedOffBoardDibPagedBgr24;
				break;
			}
		case SvDef::SVImageFormatRGB8888:
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

		Result = CreateImageBuffer(l_iPixelDepth, l_iBandNumber, l_lWidth, l_lHeight, format, rHandle);
	}

	if (SVMEE_MBUF_ALLOCATION_FAILED == Result)
	{
		Result = SVMSG_SVO_5067_IMAGEALLOCATIONFAILED;
		if (nullptr != pErrorContainer)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_5067_IMAGEALLOCATIONFAILED, SvStl::Tid_Default, SvStl::SourceFileParams(StdMessageParams));
			pErrorContainer->push_back(message);
		}
	}
	else
	{
		if ( S_OK != Result && nullptr != pErrorContainer)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CreateBufferFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10064);
			pErrorContainer->push_back(message);
		}
	}

	return Result;
}

HRESULT SVImageProcessingClass::CreateImageBuffer( const SvOi::SVImageBufferHandlePtr& rFromHandle, SVImageOperationTypeEnum p_lConversionType, SvOi::SVImageBufferHandlePtr& rToHandle )
{
	HRESULT Result = S_OK;

	rToHandle.reset();

	if(nullptr != rFromHandle)
	{
		SVMatroxBuffer l_Temp;
		Result = SVMatroxBufferInterface::Create( l_Temp, rFromHandle->GetBuffer() );

		if( S_OK == Result )
		{
			rToHandle = SvOi::SVImageBufferHandlePtr{ new SVImageBufferHandleImage(l_Temp) };
		}

		if( S_OK == Result )
		{
			Result = SVMatroxImageInterface::Convert( l_Temp, rFromHandle->GetBuffer(), p_lConversionType );
		}
	}
	else
	{
		Result = E_FAIL;
	}

	if( S_OK != Result )
	{
		rToHandle.reset();
	}

	return Result;
}

HRESULT SVImageProcessingClass::CreateImageChildBuffer( const SVImageInfoClass& rParentInfo,
                                                     SvOi::SVImageBufferHandlePtr rParentHandle,
                                                     SVImageInfoClass& rChildInfo,
                                                     SvOi::SVImageBufferHandlePtr& rChildHandle )
{
	HRESULT Result( S_FALSE );

	SvDef::SVImageFormatEnum l_eChildFormat;
	SvDef::SVImageFormatEnum l_eParentFormat;
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

	Result = l_svParentExtents.GetOutputRectangle( l_oParent );

	if ( S_OK == Result )
	{
		Result = l_svChildExtents.GetRectangle( l_oChild );
	}

	if ( S_OK == Result )
	{
		Result = GetChildImageCreateData( rChildInfo, l_eChildFormat, 
			l_iChildPixelDepth, l_iChildBandNumber, l_iChildBandLink, 
			l_lChildWidth, l_lChildHeight );
	}

	if ( S_OK == Result )
	{
		Result = GetOutputImageCreateData( rParentInfo, l_eParentFormat,
			l_iParentPixelDepth, l_iParentBandNumber, l_iParentBandLink,
			l_lParentWidth, l_lParentHeight );
	}


	if ( S_OK == Result &&
		nullptr == rChildHandle &&
		l_lChildWidth > 0 && l_lChildHeight > 0 &&
		l_iChildBandNumber <= l_iParentBandNumber &&
		nullptr != rParentHandle &&
		l_lParentWidth > 0 && l_lParentHeight > 0 &&
		l_iParentPixelDepth > 0  && l_iParentBandNumber > 0 )
	{

		// I don't know why, but rParentInfo contains incorrect info for extent.Height sometimes.
		// Just get the actual width & height here
		long lRealParentHeight;
		long lRealParentWidth;
		HRESULT l_Code;

		l_Code = SVMatroxBufferInterface::Get(rParentHandle->GetBuffer(), SVSizeX, lRealParentWidth );
		l_Code = SVMatroxBufferInterface::Get(rParentHandle->GetBuffer(), SVSizeY, lRealParentHeight );

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
				assert( false );
				l_oChild.top = 0;
			}
			else if (l_oChild.bottom > l_oParent.bottom) // may happen if the main image shrinks (camera files change)
			{
				l_oChild.top += l_oParent.bottom - l_oChild.bottom;
			}

			if (l_oChild.left < l_oParent.left)   // should never be true? child is relative to parent
			{
				assert( false );
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

			if ( S_OK != rChildInfo.SetExtentProperty( SvDef::SVExtentPropertyPositionPointX, l_oChild.left ) )
			{
				Result = S_FALSE;
			}

			if ( S_OK != rChildInfo.SetExtentProperty( SvDef::SVExtentPropertyPositionPointY, l_oChild.top ) )
			{
				Result = S_FALSE;
			}

			if ( S_OK != rChildInfo.SetExtentProperty( SvDef::SVExtentPropertyWidth, l_oChild.right - l_oChild.left ) )
			{
				Result = S_FALSE;
			}

			if ( S_OK != rChildInfo.SetExtentProperty( SvDef::SVExtentPropertyHeight, l_oChild.bottom - l_oChild.left ) )
			{
				Result = S_FALSE;
			}

			if ( S_OK == Result )
			{
				Result = GetChildImageCreateData( rChildInfo, l_eChildFormat, 
					l_iChildPixelDepth, l_iChildBandNumber, l_iChildBandLink, 
					l_lChildWidth, l_lChildHeight );

				if ( S_OK == Result )
				{
					l_svChildExtents = rChildInfo.GetExtents();
				}

				if ( S_OK == Result )
				{
					Result = l_svChildExtents.GetRectangle( l_oChild );
				}
			}

			SvStl::MessageContainer message;
			if (nullptr != rChildInfo.GetOwner())
			{
				if (nullptr != rParentInfo.GetOwner())
				{
					SvDef::StringVector msgList;
					msgList.push_back(std::string(rChildInfo.GetOwner()->GetCompleteName()));
					msgList.push_back(std::string(rParentInfo.GetOwner()->GetCompleteName()));
					message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CreateImageChildBuffer_parent, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10065);
				}
				else 
				{
					SvDef::StringVector msgList;
					msgList.push_back(std::string(rChildInfo.GetOwner()->GetCompleteName()));
					message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CreateImageChildBuffer_child, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10065);
				}
			}
			else
			{
				message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CreateImageChildBuffer, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10065);
			}

			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( message.getMessage() );
		}

		if ( S_OK == Result )
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
					SVMatroxBufferCreateChildStruct l_Create(rParentHandle->GetBuffer() );
					l_Create.m_lBand = l_iChildBandLink;
					l_Create.m_lParentBandCount = l_iParentBandNumber;
					l_Create.m_lOffX = l_oChild.left;
					l_Create.m_lOffY = l_oChild.top;
					l_Create.m_lSizeX = l_lChildWidth;
					l_Create.m_lSizeY = l_lChildHeight;
					l_Code = SVMatroxBufferInterface::Create( l_NewBuffer, l_Create );

					rChildHandle = SvOi::SVImageBufferHandlePtr{ new SVImageBufferHandleImage(l_NewBuffer) };

					Result = l_Code == S_OK ? S_OK : l_Code | SVMEE_MATROX_ERROR;
					if ( S_OK != Result )
					{

					}
				}
			}// end if( PChildInfo->BandNumber < PParentInfo->BandNumber && PChildInfo->BandNumber > 0 )
			else
			{
				SVMatroxBuffer l_NewBuffer;

				// Multi/single band child of a multi/single band parent 
				// (all bands!)
				SVMatroxBufferCreateChildStruct l_Create(rParentHandle->GetBuffer() );
				l_Create.m_lBand = SVValueAllBands;
				l_Create.m_lParentBandCount = l_iParentBandNumber;
				l_Create.m_lOffX = l_oChild.left;
				l_Create.m_lOffY = l_oChild.top;
				l_Create.m_lSizeX = l_lChildWidth;
				l_Create.m_lSizeY = l_lChildHeight;
				l_Code = SVMatroxBufferInterface::Create( l_NewBuffer, l_Create );

				rChildHandle = SvOi::SVImageBufferHandlePtr{ new SVImageBufferHandleImage(l_NewBuffer) };

				Result = (l_Code == S_OK) ? S_OK : l_Code | SVMEE_MATROX_ERROR;
				if ( S_OK == Result )
				{
					rChildInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, l_iParentBandNumber );
					rChildInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0 );
				}
			}
		}
	}

	if ( S_OK != Result )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CreateBufferFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10066 );
	}

	return Result;
}

HDC SVImageProcessingClass::CreateBufferDC( const SVImageInfoClass& rInfo, SvOi::SVImageBufferHandlePtr rHandle )
{
	HDC Result( nullptr );

	SvDef::SVImageFormatEnum l_eFormat;
	int l_iBandNumber = 0;
	int l_iBandLink = 0;
	int l_iPixelDepth = 0;
	long l_lWidth = 0;
	long l_lHeight = 0;

	bool l_bOk = S_OK == GetOutputImageCreateData( rInfo, l_eFormat, l_iPixelDepth, l_iBandNumber, l_iBandLink, l_lWidth, l_lHeight );

	if( l_bOk && nullptr != rHandle &&
		l_iPixelDepth > 0 && l_iBandNumber > 0 )
	{
		//
		// Try using the 'THE' image for the HDC allocation.
		//
		long l_lValue = SVValueDefault;
		HRESULT l_Code = SVMatroxBufferInterface::Set(rHandle->GetBuffer(), SVBufWindowDCAlloc, static_cast<SVMatroxInt>(l_lValue) );

		//
		// Check for an error - most likely an 'invalid parameter' since
		// Image is not capable of 'creating' an HDC, does not have M_DIB
		// attribute
		//
		if( l_Code != S_OK)
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
				l_Code = SVMatroxBufferInterface::CopyBuffer( imageDIB_MIL, rHandle->GetBuffer() );

				//
				// Now request the HDC from the new image with M_DIB attribute.
				//
				long l_lValue = SVValueDefault;
				l_Code = SVMatroxBufferInterface::Set( imageDIB_MIL, SVBufWindowDCAlloc, static_cast<SVMatroxInt>(l_lValue) );

				LONGLONG Handle;
				l_Code = SVMatroxBufferInterface::Get(imageDIB_MIL, SVWindowDC, Handle);
				Result = reinterpret_cast<HDC>( Handle );

				//
				// Substitute the new MIL image in place of the old one.
				//
				rHandle = SvOi::SVImageBufferHandlePtr{ new SVImageBufferHandleImage(imageDIB_MIL) };

			}
			else
			{
				LONGLONG Value = 0;
				l_Code = SVMatroxBufferInterface::Get(rHandle->GetBuffer(), SVWindowDC, Value);
				Result = reinterpret_cast<HDC> (Value);
			}

			InitBuffer( rHandle );
		}
		else
		{
			LONGLONG Value = 0;
			l_Code = SVMatroxBufferInterface::Get(rHandle->GetBuffer(), SVWindowDC, Value);
			Result = reinterpret_cast<HDC> (Value);
		}
	}

	return Result;
}

HRESULT SVImageProcessingClass::DestroyBufferDC( SvOi::SVImageBufferHandlePtr rHandle, HDC hDC )
{
	HRESULT Result( S_OK );

	HRESULT l_Code;

	if (nullptr != rHandle)
	{
		// Delete created device context. 
		long l_lValue = SVValueDefault;
		l_Code = SVMatroxBufferInterface::Set(rHandle->GetBuffer(), SVBufWindowDCFree, static_cast<SVMatroxInt>(l_lValue) );

		// Signal MIL that the buffer was modified. 
		l_Code = SVMatroxBufferInterface::Set(rHandle->GetBuffer(), SVBufModified, static_cast<SVMatroxInt>(l_lValue) );
		Result = (l_Code == S_OK) ? S_OK : l_Code | SVMEE_MATROX_ERROR;
	}
	else
	{
		Result = SVMEE_INVALID_HANDLE;
	}

	return Result;
}

HRESULT SVImageProcessingClass::InitBuffer( SvOi::SVImageBufferHandlePtr rHandle, DWORD dwValue )
{
	HRESULT Result( S_OK );

	Result = nullptr != rHandle ? S_OK : S_FALSE;
	if ( S_OK == Result )
	{
		HRESULT l_Code;
		try
		{
			l_Code = SVMatroxBufferInterface::ClearBuffer(rHandle->GetBuffer(), static_cast<double>(dwValue) );
			Result = (l_Code == S_OK) ? S_OK : l_Code | SVMEE_MATROX_ERROR;
		}
		catch( ... )
		{
			Result = S_FALSE;
		}
	}

	return Result;
}

HRESULT SVImageProcessingClass::LoadImageBuffer( LPCTSTR tstrImagePathName, SVImageInfoClass& rInfo, SvOi::SVImageBufferHandlePtr& rHandle )
{
	SVFileNameClass	svfncImageFile(tstrImagePathName);
	std::string strImagePathName = svfncImageFile.GetFullFileName();

	if( !strImagePathName.empty() )
	{
		if( ! ::SVFileExists( strImagePathName.c_str() ) )
		{
				return E_FAIL;
		}

		SVMatroxFileTypeEnum fileformat( SVMatroxImageInterface::getFileType( svfncImageFile.GetExtension().c_str() ) );

		strImagePathName = svfncImageFile.GetFullFileName();
		if( fileformat != SVFileUnknown && ::SVFileExists( strImagePathName.c_str() ) )
		{

			HRESULT l_Code;
			std::string l_strPath = strImagePathName;

			if(nullptr != rHandle)
			{
				// Load...

				l_Code = SVMatroxBufferInterface::Import(rHandle->GetBuffer(), l_strPath, fileformat );
				if( l_Code == S_OK )
				{
					return S_OK;
				}
				else
				{
					return E_FAIL;
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
					rInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatRGB888 );
				}
				else if( M_EQUIVALENT_INTERNAL_FORMAT( M_BGR32, l_DataFormat ) )
				{
					rInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatRGB8888 );
				}

				rInfo.SetExtentProperty( SvDef::SVExtentPropertyOutputPositionPoint, 0 );
				rInfo.SetExtentProperty( SvDef::SVExtentPropertyWidth, l_lSizeX );
				rInfo.SetExtentProperty( SvDef::SVExtentPropertyHeight, l_lSizeY );

				rInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, l_lBandSize );
				rInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, l_lPixelDepth );

				newBuffer.clear();

				assert( S_OK == l_Code );
				if( S_OK == CreateImageBuffer( rInfo, rHandle ) && 
					S_OK == LoadImageBuffer( strImagePathName.c_str(), rInfo, rHandle ) )
				{
					return S_OK;
				}
			}
		}

		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_UnKnownFileFormat, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10068 );
	}

	SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
	Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_FailedToLoadImage, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10069 );

	return E_FAIL;
}

HRESULT SVImageProcessingClass::LoadImageBuffer( void* pBuffer, 
                                              SVImageInfoClass& rBufferInfo, 
                                              SvOi::SVImageBufferHandlePtr& rBufferHandle,
                                              SVImageInfoClass& rCameraInfo )
{
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

	if(nullptr != rBufferHandle)
	{
		SvDef::SVImageFormatEnum l_eFormat;
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
			rBufferInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatRGB8888 );
		}
		if( pbmInfo->bmiHeader.biBitCount == 24 )
		{
			rBufferInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, SvDef::SVImageFormatRGB888 );
		}

		if( S_OK != CreateImageBuffer( rBufferInfo, rBufferHandle ) )
		{
			return S_FALSE;
		}
	}// end else

	SVImageInfoClass oTempInfo;
	SvOi::SVImageBufferHandlePtr oTempHandle;

	oTempInfo = rBufferInfo;

	oTempInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1 );
	oTempInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0 );
	oTempInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, 8 );

	oTempInfo.SetExtentProperty( SvDef::SVExtentPropertyHeight, abs(pbmhInfo->biHeight) );
	oTempInfo.SetExtentProperty( SvDef::SVExtentPropertyWidth, pbmhInfo->biWidth );

	if ( 8 < pbmhInfo->biBitCount )
	{
		oTempInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 3 );
	}

	if( S_OK != CreateImageBuffer( oTempInfo, oTempHandle ) )
	{
		rBufferHandle.reset();

		return S_FALSE;
	}

	int l_iBandNumber = 0;
	int l_iBandLink = 0;

	if ( S_OK != rCameraInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, l_iBandNumber ) || 
		S_OK != rCameraInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, l_iBandLink ) )
	{
		return S_FALSE;
	}

	// Copy the bits into the image object
	if( nullptr != pBits && nullptr != oTempHandle  && nullptr != rBufferHandle)
	{
		// Set buffer data...
		memcpy( oTempHandle->GetBufferAddress(), pBits, pbmhInfo->biSizeImage );


		HRESULT l_Code;

		if( 8 < pbmhInfo->biBitCount && l_iBandNumber == 3 )
		{
			l_Code = SVMatroxBufferInterface::CopyBuffer(rBufferHandle->GetBuffer(), oTempHandle->GetBuffer() );
		}
		else
		{
			l_Code = SVMatroxBufferInterface::CopyBuffer(rBufferHandle->GetBuffer(), oTempHandle->GetBuffer(), l_iBandLink );
		}

		return S_OK;
	}

	rBufferHandle.reset();

	SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
	Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_FailedToLoadImage, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10070 );

	return S_FALSE;
}

HRESULT SVImageProcessingClass::SaveImageBuffer( LPCTSTR tstrImagePathName, SVMatroxFileTypeEnum efileFormat, const SvOi::SVImageBufferHandlePtr& rHandle )
{
	HRESULT Result( S_OK );

	if(nullptr != rHandle )
	{
		if( efileFormat != SVFileUnknown )
		{
			std::string l_strPath = tstrImagePathName;
			Result = SVMatroxBufferInterface::Export(rHandle->GetBuffer(), l_strPath, efileFormat );
		}
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVImageProcessingClass::ConvertImageBuffer( SVByteVector& rToDIB, const SvOi::SVImageBufferHandlePtr& rFromHandle, SVImageOperationTypeEnum ConversionType )
{
	HRESULT Result( S_OK );

	if(nullptr != rFromHandle )
	{
		SVMatroxBuffer l_Temp;
		Result = SVMatroxBufferInterface::Create( l_Temp, rFromHandle->GetBuffer() );

		if( S_OK == Result )
		{
			Result = SVMatroxImageInterface::Convert( l_Temp, rFromHandle->GetBuffer(), ConversionType );
		}

		if( S_OK == Result )
		{
			Result = SVMatroxBufferInterface::CopyBuffer( rToDIB, l_Temp );
		}
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVImageProcessingClass::CopyImageBuffer( SvOi::SVImageBufferHandlePtr& rToHandle, const SvOi::SVImageBufferHandlePtr& rFromHandle )
{
	HRESULT Result( S_OK );

	if (nullptr != rToHandle && nullptr != rFromHandle)
	{
		Result = SVMatroxBufferInterface::CopyBuffer(rToHandle->GetBuffer(), rFromHandle->GetBuffer() );
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVImageProcessingClass::CopyImageBuffer( SVByteVector& rToDIB, const SvOi::SVImageBufferHandlePtr& rFromHandle )
{
	HRESULT Result( S_OK );

	if(nullptr != rFromHandle)
	{
		Result = SVMatroxBufferInterface::CopyBuffer( rToDIB, rFromHandle->GetBuffer() );
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVImageProcessingClass::CopyImageBuffer( SVByteVector& rToDIB, const SVBitmapInfo& rToBitmapInfo, const SvOi::SVImageBufferHandlePtr& rFromHandle )
{
	HRESULT Result( S_OK );

	if(nullptr != rFromHandle)
	{
		Result = SVMatroxBufferInterface::CopyBuffer( rToDIB, rToBitmapInfo, rFromHandle->GetBuffer() );
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVImageProcessingClass::CopyImageBufferToFileDIB( SVByteVector& rToDIB, const SvOi::SVImageBufferHandlePtr& rFromHandle )
{
	HRESULT Result( S_OK );

	if(nullptr != rFromHandle)
	{
		Result = SVMatroxBufferInterface::CopyBufferToFileDIB( rToDIB, rFromHandle->GetBuffer() );
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVImageProcessingClass::CopyImageBuffer( SVImageCopyUtility& rImageCopier, const SvOi::SVImageBufferHandlePtr& rFromHandle )
{
	HRESULT Result( S_OK );

	if(nullptr != rFromHandle)
	{
		Result = SVMatroxBufferInterface::CopyDIBBufferToMemory(rImageCopier, rFromHandle->GetBuffer());
	}
	else
	{
		Result = E_FAIL;
	}
	return Result;
}

HRESULT SVImageProcessingClass::CreateDataBuffer( SVDataBufferInfoClass* pDataInfo )
{
	HRESULT Result = S_OK;

	if( nullptr != pDataInfo && pDataInfo->Length > 0 &&
		M_NULL == pDataInfo->HBuffer.milResult )
	{
		SVImageOperationTypeEnum l_eType;

		switch( pDataInfo->Type )
		{
		case SVDataBufferInfoClass::SVProjectResult:
			l_eType = SVImageProjList;
			break;
		case SVDataBufferInfoClass::SVProjectResult | SVDataBufferInfoClass::SVContainsFloatData:
			l_eType = SVImageProjListFloat;
			break;
		case SVDataBufferInfoClass::SVEventResult:
			l_eType = SVImageEventList;
			break;
		case SVDataBufferInfoClass::SVEventResult | SVDataBufferInfoClass::SVContainsFloatData:
			l_eType = SVImageEventListFloat;
			break;
		case SVDataBufferInfoClass::SVExtremeResult:
			l_eType = SVImageExtreemList;
			break;
		case SVDataBufferInfoClass::SVExtremeResult | SVDataBufferInfoClass::SVContainsFloatData:
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

		HRESULT l_Code;

		l_Code = SVMatroxImageInterface::Create(pDataInfo->HBuffer.milResult, pDataInfo->Length, l_eType);


		Result = (M_NULL != pDataInfo->HBuffer.milResult) ? S_OK : S_FALSE;
	}

	return Result;
}

HRESULT SVImageProcessingClass::DestroyDataBuffer( SVDataBufferInfoClass* pDataInfo )
{
	HRESULT Result( S_OK );

	if ( nullptr != pDataInfo )
	{
		SVMatroxImageInterface::Destroy( pDataInfo->HBuffer.milResult );
	}

	return Result;
}

HRESULT SVImageProcessingClass::ResizeDataBuffer( SVDataBufferInfoClass* pDataInfo, long lNewLength )
{
	HRESULT Result( S_OK );

	Result = DestroyDataBuffer( pDataInfo );
	if ( S_OK == Result )
	{
		pDataInfo->Length = lNewLength;

		Result = CreateDataBuffer( pDataInfo );
	}

	return Result;
}

HRESULT SVImageProcessingClass::CreateBarCodeBuffer( SVBarCodeBufferInfoClass* pDataInfo )
{
	HRESULT Result = S_OK;

	if( nullptr != pDataInfo && M_NULL == pDataInfo->HBuffer.milBarCode )
	{
		// Allocate Bar Code buffer...
		SVMatroxBarCodeInterface::Create(pDataInfo->HBuffer.milBarCode, pDataInfo->m_lCodeType );
		Result = (M_NULL != pDataInfo->HBuffer.milBarCode) ? S_OK : S_FALSE;
	}
	return Result;
}

HRESULT SVImageProcessingClass::GetOutputImageCreateData( const SVImageInfoClass &p_rsvInfo,
	SvDef::SVImageFormatEnum &p_reFormat,
	int &p_riPixelDepth,
	int &p_riBandNumber,
	int &p_riBandLink,
	long &p_rlWidth,
	long &p_rlHeight )
{
	HRESULT Result = p_rsvInfo.GetExtentProperty( SvDef::SVExtentPropertyOutputWidth, p_rlWidth );

	if ( S_OK == Result )
	{
		Result = p_rsvInfo.GetExtentProperty( SvDef::SVExtentPropertyOutputHeight, p_rlHeight );
	}

	if ( S_OK == Result )
	{
		int l_iFormat = SvDef::SVImageFormatUnknown;

		Result = p_rsvInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, l_iFormat );

		if ( S_OK == Result )
		{
			p_reFormat = (SvDef::SVImageFormatEnum)l_iFormat;
		}
	}

	if ( S_OK == Result )
	{
		Result = p_rsvInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, p_riBandNumber );
	}

	if ( S_OK == Result )
	{
		Result = p_rsvInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, p_riBandLink );
	}

	if ( S_OK == Result )
	{
		Result = p_rsvInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, p_riPixelDepth );
	}

	return Result;
}

HRESULT SVImageProcessingClass::GetChildImageCreateData( const SVImageInfoClass &p_rsvInfo,
	SvDef::SVImageFormatEnum &p_reFormat,
	int &p_riPixelDepth,
	int &p_riBandNumber,
	int &p_riBandLink,
	long &p_rlWidth,
	long &p_rlHeight )
{
	HRESULT Result = p_rsvInfo.GetExtentProperty( SvDef::SVExtentPropertyWidth, p_rlWidth );

	if ( S_OK == Result )
	{
		Result = p_rsvInfo.GetExtentProperty( SvDef::SVExtentPropertyHeight, p_rlHeight );
	}

	if ( S_OK == Result )
	{
		int l_iFormat = SvDef::SVImageFormatUnknown;

		Result = p_rsvInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, l_iFormat );

		if ( S_OK == Result )
		{
			p_reFormat = (SvDef::SVImageFormatEnum)l_iFormat;
		}
	}

	if ( S_OK == Result )
	{
		Result = p_rsvInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, p_riBandNumber );
	}

	if ( S_OK == Result )
	{
		Result = p_rsvInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, p_riBandLink );
	}

	if ( S_OK == Result )
	{
		Result = p_rsvInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, p_riPixelDepth );
	}

	return Result;
}

HRESULT SVImageProcessingClass::CreateImageBuffer( int pixelDepth, int bandNumber, long width, long height, SVMatroxBufferAttributeEnum format, SvOi::SVImageBufferHandlePtr &rHandle )
{
	HRESULT Result( S_OK );
	if( 0 < pixelDepth && 0 < bandNumber &&
		0 < width && 0 < height )
	{
		HRESULT code;
		SVMatroxBufferCreateStruct createStruct;

		createStruct.m_lSizeBand = bandNumber;
		createStruct.m_lSizeX = width;
		createStruct.m_lSizeY = height;
		createStruct.SetImageDepth( pixelDepth );
		createStruct.m_eAttribute = format;

		SVMatroxBuffer newBuffer;
		// Allocate a workable multi or single band image buffer
		code = SVMatroxBufferInterface::Create( newBuffer, createStruct );

		rHandle = SvOi::SVImageBufferHandlePtr{ new SVImageBufferHandleImage(newBuffer) };

		Result = code == S_OK ? S_OK : code | SVMEE_MATROX_ERROR;
		if ( S_OK == Result )
		{
			InitBuffer( rHandle );
		}
		else
		{
			rHandle.reset();
		}
	}
	else
	{
		Result = S_FALSE;
	}	
	return Result;
}