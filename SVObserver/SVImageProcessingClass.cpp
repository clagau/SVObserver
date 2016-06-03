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

#include "stdafx.h"
#include "SVImageProcessingClass.h"
#include "SVDigitizerProcessingClass.h"
#include "SVImageObjectClass.h"
#include "SVMatroxImageProcessingClass.h"
#include "SVUtilityLibrary/SVImageCopyUtility.h"

SVImageProcessingClass& SVImageProcessingClass::Instance()
{
	static SVImageProcessingClass l_Object;

	return l_Object;
}

SVImageProcessingClass::SVImageProcessingClass()
{
}

SVImageProcessingClass::~SVImageProcessingClass()
{
	DestroyLocal();
}

void SVImageProcessingClass::DestroyLocal()
{
	SVDigitizerProcessingClass::Instance().clear();
}

HRESULT SVImageProcessingClass::CreateImageBuffer( const SVImageInfoClass& rInfo, SVSmartHandlePointer& rHandle )
{
	return SVMatroxImageProcessingClass::Instance().CreateImageBuffer( rInfo, rHandle );
}

HRESULT SVImageProcessingClass::CreateImageBuffer( const SVSmartHandlePointer& rFromHandle, SVImageOperationTypeEnum p_lConversionType, SVSmartHandlePointer& rToHandle )
{
	return SVMatroxImageProcessingClass::Instance().CreateImageBuffer( rFromHandle, p_lConversionType, rToHandle );
}


HRESULT SVImageProcessingClass::CreateImageChildBuffer( const SVImageInfoClass& rParentInfo,
                                                     SVSmartHandlePointer rParentHandle,
                                                     SVImageInfoClass& rChildInfo,
                                                     SVSmartHandlePointer& rChildHandle )
{
	return SVMatroxImageProcessingClass::Instance().CreateImageChildBuffer( rParentInfo, rParentHandle, rChildInfo, rChildHandle );
}

HDC SVImageProcessingClass::CreateBufferDC( const SVImageInfoClass& rInfo, SVSmartHandlePointer rHandle )
{
	return SVMatroxImageProcessingClass::Instance().CreateBufferDC( rInfo, rHandle );
}

HRESULT SVImageProcessingClass::DestroyBufferDC( SVSmartHandlePointer rHandle, HDC hDC )
{
	return SVMatroxImageProcessingClass::Instance().DestroyBufferDC( rHandle, hDC );
}

HRESULT SVImageProcessingClass::InitBuffer( SVSmartHandlePointer rHandle, DWORD dwValue )
{
	return SVMatroxImageProcessingClass::Instance().InitBuffer( rHandle, dwValue );
}

HRESULT SVImageProcessingClass::LoadImageBuffer( LPCTSTR tstrImagePathName, 
                                              SVImageInfoClass& rInfo,
                                              SVSmartHandlePointer& rHandle, 
                                              BOOL bBrowseIfNotExists )
{
	return SVMatroxImageProcessingClass::Instance().LoadImageBuffer( tstrImagePathName, rInfo, rHandle, bBrowseIfNotExists );
}

HRESULT SVImageProcessingClass::LoadImageBuffer( void* pBuffer, 
                                              SVImageInfoClass& rBufferInfo, 
                                              SVSmartHandlePointer& rBufferHandle,
                                              SVImageInfoClass& rCameraInfo )
{
	return SVMatroxImageProcessingClass::Instance().LoadImageBuffer( pBuffer, rBufferInfo, rBufferHandle, rCameraInfo );
}

HRESULT SVImageProcessingClass::SaveImageBuffer( LPCTSTR tstrImagePathName, SVMatroxFileTypeEnum efileFormat, const SVSmartHandlePointer& rHandle )
{
	return SVMatroxImageProcessingClass::Instance().SaveImageBuffer( tstrImagePathName,efileFormat,  rHandle );
}

HRESULT SVImageProcessingClass::ConvertImageBuffer( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle, SVImageOperationTypeEnum p_lConversionType )
{
	return SVMatroxImageProcessingClass::Instance().ConvertImageBuffer( p_rToDIB, rFromHandle, p_lConversionType );
}

HRESULT SVImageProcessingClass::CopyImageBuffer( SVSmartHandlePointer& rToHandle, const SVSmartHandlePointer& rFromHandle )
{
	return SVMatroxImageProcessingClass::Instance().CopyImageBuffer( rToHandle, rFromHandle );
}

HRESULT SVImageProcessingClass::CopyImageBuffer( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle )
{
	return SVMatroxImageProcessingClass::Instance().CopyImageBuffer( p_rToDIB, rFromHandle );
}

HRESULT SVImageProcessingClass::CopyImageBuffer( SVByteVector& p_rToDIB, const SVBitmapInfo& p_rToBitmapInfo, const SVSmartHandlePointer& rFromHandle )
{
	return SVMatroxImageProcessingClass::Instance().CopyImageBuffer( p_rToDIB, p_rToBitmapInfo, rFromHandle );
}

HRESULT SVImageProcessingClass::CopyImageBufferToFileDIB( SVByteVector& p_rToDIB, const SVSmartHandlePointer& rFromHandle )
{
	return SVMatroxImageProcessingClass::Instance().CopyImageBufferToFileDIB( p_rToDIB, rFromHandle );
}

HRESULT SVImageProcessingClass::CopyImageBuffer( SVImageCopyUtility& copier, const SVSmartHandlePointer& rFromHandle )
{
	return SVMatroxImageProcessingClass::Instance().CopyImageBuffer(copier, rFromHandle );
}

HRESULT SVImageProcessingClass::CreateDataBuffer( SVDataBufferInfoClass* pDataInfo )
{
	return SVMatroxImageProcessingClass::Instance().CreateDataBuffer( pDataInfo );
}

HRESULT SVImageProcessingClass::DestroyDataBuffer( SVDataBufferInfoClass* pDataInfo )
{
	return SVMatroxImageProcessingClass::Instance().DestroyDataBuffer( pDataInfo );
}

HRESULT SVImageProcessingClass::ResizeDataBuffer( SVDataBufferInfoClass* pDataInfo, long lNewLength )
{
	return SVMatroxImageProcessingClass::Instance().ResizeDataBuffer( pDataInfo, lNewLength );
}

HRESULT SVImageProcessingClass::CreateBarCodeBuffer( SVBarCodeBufferInfoClass* pDataInfo )
{
	return SVMatroxImageProcessingClass::Instance().CreateBarCodeBuffer( pDataInfo );
}

