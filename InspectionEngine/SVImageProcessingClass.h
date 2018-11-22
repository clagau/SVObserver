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
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "Definitions/SVImageFormatEnum.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#pragma endregion Includes

class SVBarCodeBufferInfoClass;
class SVDataBufferInfoClass;
class SVImageInfoClass;

class SVImageProcessingClass
{
public:
	static HRESULT CreateImageBuffer( const SVImageInfoClass& rInfo, SvOi::SVImageBufferHandlePtr& rHandle, SvStl::MessageContainerVector* pErrorContainer = nullptr);
	static HRESULT CreateImageBuffer( const SvOi::SVImageBufferHandlePtr& rFromHandle, SVImageOperationTypeEnum p_lConversionType, SvOi::SVImageBufferHandlePtr& rToHandle );

	static HRESULT CreateImageChildBuffer( const SVImageInfoClass& rParentInfo, SvOi::SVImageBufferHandlePtr rParentHandle,
	                                SVImageInfoClass& rChildInfo, SvOi::SVImageBufferHandlePtr& rChildHandle );
	
	static HDC CreateBufferDC( const SVImageInfoClass& rInfo, SvOi::SVImageBufferHandlePtr rHandle );
	static HRESULT DestroyBufferDC( SvOi::SVImageBufferHandlePtr rHandle, HDC hDC );

	static HRESULT InitBuffer( SvOi::SVImageBufferHandlePtr rHandle, DWORD dwValue = 0 );

	static HRESULT LoadImageBuffer( LPCTSTR tstrImagePathName, SVImageInfoClass& rInfo, SvOi::SVImageBufferHandlePtr& rHandle );
	static HRESULT LoadImageBuffer( void* pBuffer, SVImageInfoClass& rBufferInfo, SvOi::SVImageBufferHandlePtr& rBufferHandle, SVImageInfoClass& rCameraInfo );

	static HRESULT SaveImageBuffer( LPCTSTR tstrImagePathName, SVMatroxFileTypeEnum efileFormat,const SvOi::SVImageBufferHandlePtr& rHandle );

	static HRESULT CopyImageBuffer( SVByteVector& p_rToDIB, const SvOi::SVImageBufferHandlePtr& rFromHandle );
	static HRESULT CopyImageBuffer( SVByteVector& p_rToDIB, const SVBitmapInfo& p_rToBitmapInfo, const SvOi::SVImageBufferHandlePtr& rFromHandle );

	static HRESULT CreateDataBuffer( SVDataBufferInfoClass* pDataInfo );
	static HRESULT DestroyDataBuffer( SVDataBufferInfoClass* pDataInfo );
	static HRESULT ResizeDataBuffer( SVDataBufferInfoClass* pDataInfo, long lNewLength );
	
	static HRESULT CreateBarCodeBuffer( SVBarCodeBufferInfoClass* pDataInfo );

	static HRESULT FillBufferStructFromInfo(const SVImageInfoClass& rInfo, SVMatroxBufferCreateStruct& bufferStruct);

	/// Fill a childBufferStruct with the information from the image information. (but not the parent-handle)
	/// \param rParentInfo [in] Parent imageInfo.
	/// \param rChildInfo [in,out] Child imageInfo. If childInfo will not fit to rParentInfo, it will be fitted. 
	/// \param rChildBufferStruct [in,out] The changed childBufferStruct. It will only be changed if return value is S_OK.
	/// \returns HRESULT
	static HRESULT FillChildBufferStructFromInfo(const SVImageInfoClass& rParentInfo, SVImageInfoClass& rChildInfo, MatroxBufferChildDataStruct& rChildBufferStruct);
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
	

	static HRESULT CreateImageBuffer(SVMatroxBufferCreateStruct& bufferStruct, SvOi::SVImageBufferHandlePtr &rHandle);
};

