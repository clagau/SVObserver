//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageProcessingClass
//* .File Name       : $Workfile:   SVImageProcessingClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:54:12  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "Definitions/SVImageFormatEnum.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#pragma endregion Includes

class SVBarCodeBufferInfoClass;
class SVDataBufferInfoClass;
class SVImageInfoClass;
class SVImageCopyUtility;

class SVImageProcessingClass
{
public:
	static HRESULT CreateImageBuffer( const SVImageInfoClass& rInfo, SVSmartHandlePointer& rHandle, SvStl::MessageContainerVector* pErrorContainer = nullptr);
	static HRESULT CreateImageBuffer( const SVSmartHandlePointer& rFromHandle, SVImageOperationTypeEnum p_lConversionType, SVSmartHandlePointer& rToHandle );

	static HRESULT CreateImageChildBuffer( const SVImageInfoClass& rParentInfo, SVSmartHandlePointer rParentHandle,
	                                SVImageInfoClass& rChildInfo, SVSmartHandlePointer& rChildHandle );
	
	static HDC CreateBufferDC( const SVImageInfoClass& rInfo, SVSmartHandlePointer rHandle );
	static HRESULT DestroyBufferDC( SVSmartHandlePointer rHandle, HDC hDC );

	static HRESULT InitBuffer( SVSmartHandlePointer rHandle, DWORD dwValue = 0 );

	static HRESULT LoadImageBuffer( LPCTSTR tstrImagePathName, SVImageInfoClass& rInfo, SVSmartHandlePointer& rHandle );
	static HRESULT LoadImageBuffer( void* pBuffer, SVImageInfoClass& rBufferInfo, SVSmartHandlePointer& rBufferHandle, SVImageInfoClass& rCameraInfo );

	static HRESULT SaveImageBuffer( LPCTSTR tstrImagePathName, SVMatroxFileTypeEnum efileFormat,const SVSmartHandlePointer& rHandle );

	static HRESULT ConvertImageBuffer( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle, SVImageOperationTypeEnum p_lConversionType );

	static HRESULT CopyImageBuffer( SVSmartHandlePointer& rToHandle, const SVSmartHandlePointer& rFromHandle );
	static HRESULT CopyImageBuffer( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle );
	static HRESULT CopyImageBuffer( SVByteVector& p_rToDIB, const SVBitmapInfo& p_rToBitmapInfo, const SVSmartHandlePointer& rFromHandle );

	static HRESULT CopyImageBufferToFileDIB( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle );
	static HRESULT CopyImageBuffer( SVImageCopyUtility& copier, const SVSmartHandlePointer& rFromHandle );

	static HRESULT CreateDataBuffer( SVDataBufferInfoClass* pDataInfo );
	static HRESULT DestroyDataBuffer( SVDataBufferInfoClass* pDataInfo );
	static HRESULT ResizeDataBuffer( SVDataBufferInfoClass* pDataInfo, long lNewLength );
	
	static HRESULT CreateBarCodeBuffer( SVBarCodeBufferInfoClass* pDataInfo );
private:
	static HRESULT GetOutputImageCreateData( const SVImageInfoClass &p_rsvInfo,
															      SvDef::SVImageFormatEnum &p_reFormat,
															      int &p_riPixelDepth,
															      int &p_riBandNumber,
															      int &p_riBandLink,
															      long &p_rlWidth,
															      long &p_rlHeight );
	
	static HRESULT GetChildImageCreateData( const SVImageInfoClass &p_rsvInfo,
															     SvDef::SVImageFormatEnum &p_reFormat,
															     int &p_riPixelDepth,
															     int &p_riBandNumber,
															     int &p_riBandLink,
															     long &p_rlWidth,
															     long &p_rlHeight );
	

	//************************************
	//! Create a image buffer
	//! \param pixelDepth [in] 
	//! \param bandNumber [in]
	//! \param width [in]
	//! \param height [in]
	//! \param format [in]
	//! \param rHandle [out] handle to the new image buffer
	//! \returns HRESULT
	//************************************
	static HRESULT CreateImageBuffer( int pixelDepth, int bandNumber, long width, long height, SVMatroxBufferAttributeEnum format, SVSmartHandlePointer &rHandle );
};

