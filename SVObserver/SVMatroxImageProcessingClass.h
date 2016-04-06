//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxImageProcessingClass
//* .File Name       : $Workfile:   SVMatroxImageProcessingClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:34:18  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#pragma endregion Includes

class SVBarCodeBufferInfoClass;
class SVDataBufferInfoClass;
class SVImageInfoClass;
class SVImageCopyUtility;

class SVMatroxImageProcessingClass
{
public:
	static SVMatroxImageProcessingClass& Instance();

	virtual ~SVMatroxImageProcessingClass();

	//************************************
	//! Create a image buffer
	//! \param rInfo [in] information data for the new image
	//! \param rHandle [out] handle to the new image
	//! \returns HRESULT
	//************************************
	HRESULT CreateImageBuffer( const SVImageInfoClass& rInfo, SVSmartHandlePointer& rHandle );

	HRESULT CreateImageBuffer( const SVSmartHandlePointer& rFromHandle, SVImageOperationTypeEnum p_lConversionType, SVSmartHandlePointer& rToHandle );

	HRESULT CreateImageChildBuffer( const SVImageInfoClass& rParentInfo, SVSmartHandlePointer rParentHandle,
	                                SVImageInfoClass& rChildInfo, SVSmartHandlePointer& rChildHandle );
	
	HDC CreateBufferDC( const SVImageInfoClass& rInfo, SVSmartHandlePointer rHandle );
	HRESULT DestroyBufferDC( SVSmartHandlePointer rChildHandle, HDC hDC );
	
	HRESULT CreateDataBuffer( SVDataBufferInfoClass* pDataInfo );
	HRESULT DestroyDataBuffer( SVDataBufferInfoClass* pDataInfo );
	HRESULT ResizeDataBuffer( SVDataBufferInfoClass* pDataInfo, long lNewLength );
	
	HRESULT InitBuffer( SVSmartHandlePointer rHandle, DWORD dwValue = 0 );

	HRESULT LoadImageBuffer( LPCTSTR tstrImagePathName, SVImageInfoClass& rInfo, SVSmartHandlePointer& rHandle, BOOL bBrowseIfNotExists );
	HRESULT LoadImageBuffer( void* pBuffer, SVImageInfoClass& rBufferInfo, SVSmartHandlePointer& rBufferHandle, SVImageInfoClass& rCameraInfo );
	
	HRESULT SaveImageBuffer( LPCTSTR tstrImagePathName, const SVSmartHandlePointer& rHandle );

	HRESULT CopyImageBuffer( SVSmartHandlePointer& rToHandle, const SVSmartHandlePointer& rFromHandle );
	HRESULT CopyImageBuffer( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle );
	HRESULT CopyImageBuffer( SVByteVector& p_rToDIB, const SVBitmapInfo& p_rToBitmapInfo, const SVSmartHandlePointer& rFromHandle );
	HRESULT CopyImageBuffer( SVImageCopyUtility& copier, const SVSmartHandlePointer& rFromHandle );
	HRESULT CopyImageBufferToFileDIB( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle );

	HRESULT ConvertImageBuffer( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle, SVImageOperationTypeEnum p_lConversionType );

	HRESULT CreateBarCodeBuffer(SVBarCodeBufferInfoClass* pDataInfo);

protected:
	SVMatroxImageProcessingClass();

	HRESULT GetOutputImageCreateData( const SVImageInfoClass &p_rsvInfo,
															      SVImageFormatEnum &p_reFormat,
															      int &p_riPixelDepth,
															      int &p_riBandNumber,
															      int &p_riBandLink,
															      long &p_rlWidth,
															      long &p_rlHeight ) const;
	
	HRESULT GetChildImageCreateData( const SVImageInfoClass &p_rsvInfo,
															     SVImageFormatEnum &p_reFormat,
															     int &p_riPixelDepth,
															     int &p_riBandNumber,
															     int &p_riBandLink,
															     long &p_rlWidth,
															     long &p_rlHeight ) const;
	

private:
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
	HRESULT CreateImageBuffer( int pixelDepth, int bandNumber, long width, long height, SVMatroxBufferAttributeEnum format, SVSmartHandlePointer &rHandle );

	//Do not implement this method
	SVMatroxImageProcessingClass( const SVMatroxImageProcessingClass& p_rObject );

	//Do not implement this method
	const SVMatroxImageProcessingClass& operator=( const SVMatroxImageProcessingClass& p_rObject );
};

