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
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVMatroxLibrary/SVMatroxEnums.h"
#include "SVAcquisitionClass.h"
#pragma endregion Includes

class SVBarCodeBufferInfoClass;
class SVDataBufferInfoClass;
class SVImageInfoClass;
class SVDigitizerProcessingClass;
class SVImageCopyUtility;

class SVImageProcessingClass
{
public:
	static SVImageProcessingClass& Instance();

	virtual ~SVImageProcessingClass();

	HRESULT CreateImageBuffer( const SVImageInfoClass& rInfo, SVSmartHandlePointer& rHandle );
	HRESULT CreateImageBuffer( const SVSmartHandlePointer& rFromHandle, SVImageOperationTypeEnum p_lConversionType, SVSmartHandlePointer& rToHandle );

	HRESULT CreateImageChildBuffer( const SVImageInfoClass& rParentInfo, SVSmartHandlePointer rParentHandle,
	                                SVImageInfoClass& rChildInfo, SVSmartHandlePointer& rChildHandle );
	
	HDC CreateBufferDC( const SVImageInfoClass& rInfo, SVSmartHandlePointer rHandle );
	HRESULT DestroyBufferDC( SVSmartHandlePointer rHandle, HDC hDC );

	HRESULT InitBuffer( SVSmartHandlePointer rHandle, DWORD dwValue = 0 );

	HRESULT LoadImageBuffer( LPCTSTR tstrImagePathName, SVImageInfoClass& rInfo, SVSmartHandlePointer& rHandle, BOOL bBrowseIfNotExists );
	HRESULT LoadImageBuffer( void* pBuffer, SVImageInfoClass& rBufferInfo, SVSmartHandlePointer& rBufferHandle, SVImageInfoClass& rCameraInfo );

	HRESULT SaveImageBuffer( LPCTSTR tstrImagePathName, SVMatroxFileTypeEnum efileFormat,const SVSmartHandlePointer& rHandle );

	HRESULT ConvertImageBuffer( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle, SVImageOperationTypeEnum p_lConversionType );

	HRESULT CopyImageBuffer( SVSmartHandlePointer& rToHandle, const SVSmartHandlePointer& rFromHandle );
	HRESULT CopyImageBuffer( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle );
	HRESULT CopyImageBuffer( SVByteVector& p_rToDIB, const SVBitmapInfo& p_rToBitmapInfo, const SVSmartHandlePointer& rFromHandle );

	HRESULT CopyImageBufferToFileDIB( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle );
	HRESULT CopyImageBuffer( SVImageCopyUtility& copier, const SVSmartHandlePointer& rFromHandle );

	HRESULT CreateDataBuffer( SVDataBufferInfoClass* pDataInfo );
	HRESULT DestroyDataBuffer( SVDataBufferInfoClass* pDataInfo );
	HRESULT ResizeDataBuffer( SVDataBufferInfoClass* pDataInfo, long lNewLength );
	
	HRESULT CreateBarCodeBuffer( SVBarCodeBufferInfoClass* pDataInfo );

protected:
	SVImageProcessingClass();

private:
	void DestroyLocal();

	//Do not implement this method
	SVImageProcessingClass( const SVImageProcessingClass& p_rObject );

	//Do not implement this method
	const SVImageProcessingClass& operator=( const SVImageProcessingClass& p_rObject );
};

