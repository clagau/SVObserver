//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions/SVMatroxSimpleEnums.h"
#include "FilesystemUtilities/ImageFileFormats.h"

#pragma endregion Includes

class SVImageInfoClass;
struct SVMatroxBufferCreateStruct;
struct MatroxBufferChildDataStruct;


namespace SvStl
{
class MessageContainer;
typedef std::vector<MessageContainer> MessageContainerVector;
}

namespace SvIe
{

class SVBarCodeBufferInfoClass;
class SVDataBufferInfoClass;

class SVImageProcessingClass
{
public:
	static HRESULT CreateImageBuffer( const SVImageInfoClass& rInfo, SvOi::SVImageBufferHandlePtr& rHandle, SvStl::MessageContainerVector* pErrorContainer = nullptr);
	static HRESULT CreateImageBuffer( const SvOi::SVImageBufferHandlePtr& rFromHandle, SVImageOperationTypeEnum optype, SvOi::SVImageBufferHandlePtr& rToHandle );
	static HRESULT convertToMILBuffer(HBITMAP hBmp, SvOi::SVImageBufferHandlePtr& rToHandle);

	static HRESULT CreateImageChildBuffer( const SVImageInfoClass& rParentInfo, SvOi::SVImageBufferHandlePtr rParentHandle,
	                                SVImageInfoClass& rChildInfo, SvOi::SVImageBufferHandlePtr& rChildHandle );
	
	static HDC CreateBufferDC( const SVImageInfoClass& rInfo, SvOi::SVImageBufferHandlePtr rHandle );
	static HRESULT DestroyBufferDC( SvOi::SVImageBufferHandlePtr rHandle, HDC hDC );

	static HRESULT InitBuffer( SvOi::SVImageBufferHandlePtr rHandle, DWORD dwValue = 0 );

	static HRESULT LoadImageBuffer(const TCHAR* pImageFilepath, SVImageInfoClass& rInfo, SvOi::SVImageBufferHandlePtr& rHandle );
	static HRESULT LoadImageBuffer( void* pBuffer, SVImageInfoClass& rBufferInfo, SvOi::SVImageBufferHandlePtr& rBufferHandle, SVImageInfoClass& rCameraInfo );

	static HRESULT SaveImageBuffer(const TCHAR* pImageFilepath, ImageFileFormat efileFormat,const SvOi::SVImageBufferHandlePtr& rHandle );

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
	static HRESULT GetOutputImageCreateData( const SVImageInfoClass &rInfo,
															      SvDef::SVImageFormatEnum &rFormat,
															      int &rPixelDepth,
															      int &rBandNumber,
															      int &rBandLink,
															      long &rWidth,
															      long &rHeight );
	
	static HRESULT GetChildImageCreateData( const SVImageInfoClass &rInfo,
															     SvDef::SVImageFormatEnum &rFormat,
															     int &rPixelDepth,
															     int &rBandNumber,
															     int &rBandLink,
															     long &rWidth,
															     long &rHeight );
	

	static HRESULT CreateImageBuffer(SVMatroxBufferCreateStruct& bufferStruct, SvOi::SVImageBufferHandlePtr &rHandle);
};

} //namespace SvIe
