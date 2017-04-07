//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandInspectionCollectImageData
//* .File Name       : $Workfile:   SVCommandInspectionCollectImageData.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   07 Jan 2015 17:38:56  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVCommandInspectionCollectImageData.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVInspectionProcess.h"
#include "SVResultList.h"
#include "SVToolSet.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "SVPPQObject.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#pragma endregion Includes

#pragma region Constructor
SVCommandInspectionCollectImageData::SVCommandInspectionCollectImageData(const SVCommandInspectionCollectImageData& p_rObject)
: m_InspectionId( p_rObject.m_InspectionId )
, m_ImageIds( p_rObject.m_ImageIds )
, m_Product( p_rObject.m_Product )
{
}

SVCommandInspectionCollectImageData::SVCommandInspectionCollectImageData(const SVGUID& p_rInspectionId, const SVGuidSet& p_rImageIds)
: m_InspectionId( p_rInspectionId )
, m_ImageIds( p_rImageIds )
, m_Product()
{
}

SVCommandInspectionCollectImageData::~SVCommandInspectionCollectImageData()
{
}
#pragma endregion Constructor

HRESULT SVCommandInspectionCollectImageData::Execute()
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( GetInspectionId() );
	SVInspectionProcess* pInspection = dynamic_cast< SVInspectionProcess* >( l_pObject );

	if( nullptr != pInspection )
	{
		SVProductInfoStruct l_Product = pInspection->LastProductGet( SV_DISPLAY );

		SVInspectionInfoStruct& l_rIPInfo = l_Product.m_svInspectionInfos[ GetInspectionId() ];
		SVGuidSVCameraInfoStructMap& l_rCameraInfos = l_Product.m_svCameraInfos;
		SVDataManagerHandle l_Handle = l_Product.oPPQInfo.m_ResultDataDMIndexHandle;
		long l_ResultDataIndex = l_Handle.GetIndex();
		SVImageIndexStruct l_ResultImageIndex;

		l_ResultImageIndex.m_ResultDMIndexHandle.Assign( l_rIPInfo.m_ResultImageDMIndexHandle, SV_DISPLAY );
		l_ResultImageIndex.m_PublishedResultDMIndexHandle.Assign( l_Product.oPPQInfo.m_ResultImagePublishedDMIndexHandle, SV_DISPLAY );

		double l_TriggerDistance = 0.0f;
		double l_ToolSetTime = 0.0f;

		if( l_Product.oTriggerInfo.m_PreviousTrigger > 0 )
		{
			l_TriggerDistance = (double) ( l_Product.oTriggerInfo.m_BeginProcess - l_Product.oTriggerInfo.m_PreviousTrigger );
		}

		if( ( l_rIPInfo.m_BeginToolset > 0 ) &&	( l_rIPInfo.m_EndToolset > 0 ) )
		{
			l_ToolSetTime = (double) ( l_rIPInfo.m_EndToolset - l_rIPInfo.m_BeginToolset );
		}

		m_Product.m_TriggerCount = l_Product.ProcessCount();

		UpdateResults( pInspection, m_Product.m_ResultData );

		m_Product.m_ResultData.m_ToolSetEndTime = l_rIPInfo.m_ToolSetEndTime;
		m_Product.m_ResultData.m_ToolSetAvgTime = l_rIPInfo.m_ToolSetAvgTime;
		m_Product.m_ResultData.m_ToolSetTime = l_ToolSetTime;
		m_Product.m_ResultData.m_TriggerDistance = l_TriggerDistance;

		const SVGuidSet& rImageIds = GetImageIds();

		for( SVGuidSet::const_iterator l_Iter = rImageIds.begin(); l_Iter != rImageIds.end(); ++l_Iter )
		{
			SVGUID l_ImageId = *l_Iter;

			SVIPImageDataElementMap::iterator l_FindIter = m_Product.m_ImageData.find( l_ImageId );

			if( l_FindIter == m_Product.m_ImageData.end() )
			{
				SVIPImageDataElement l_ImageData;

				::Sleep( 0 );

				if( S_OK == UpdateBuffer( l_ImageId, l_rCameraInfos, l_ResultImageIndex, l_ResultDataIndex, l_ImageData.m_ImageDIB , l_ImageData.m_OverlayData ) )
				{
					::Sleep( 0 );

					m_Product.m_ImageData[ l_ImageId ] = l_ImageData;
				}
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVCommandInspectionCollectImageData::empty() const
{
	bool l_Status = true;

	l_Status = l_Status && ( m_InspectionId.empty() );
	l_Status = l_Status && ( m_ImageIds.empty() );
	l_Status = l_Status && ( m_Product.m_ImageData.empty() );
	l_Status = l_Status && ( m_Product.m_ResultData.m_ResultData.empty() );

	return l_Status;
}

const SVGUID& SVCommandInspectionCollectImageData::GetInspectionId() const
{
	return m_InspectionId;
}

const SVGuidSet& SVCommandInspectionCollectImageData::GetImageIds() const
{
	return m_ImageIds;
}

const SVIPProductStruct& SVCommandInspectionCollectImageData::GetProduct() const
{
	return m_Product;
}

HRESULT SVCommandInspectionCollectImageData::UpdateResults( SVInspectionProcess* pInspection, SVIPResultData& p_rResultData )
{
	HRESULT hRet = E_FAIL;
	SVResultListClass* pResultList(nullptr);

	if( nullptr != pInspection )
	{
		pResultList = pInspection->GetResultList();
	}


	if (nullptr != pResultList)
	{
		
		hRet = pResultList->GetResultData(p_rResultData);
	} 


	return hRet;
}

HRESULT SVCommandInspectionCollectImageData::UpdateBuffer(const SVGUID& p_rImageId, SVGuidSVCameraInfoStructMap& p_rsvCameraInfos,
										 SVImageIndexStruct p_svResultImageIndex, long p_ResultDataIndex,
										 SVByteVector& p_rImageDIB, SVExtentMultiLineStructVector& p_rMultiLineArray)
{
	HRESULT l_Status = S_OK;

	SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( p_rImageId.ToGUID() ) );

	if( nullptr != l_pImage )
	{
		SVDataManagerHandle	l_SourceImageDMIndexHandle;

		SVImageIndexStruct svResultImageIndex = p_svResultImageIndex;

		SVCameraImageTemplate* l_psvMainImage = dynamic_cast< SVCameraImageTemplate* >( l_pImage );
		SVRGBImageClass* l_psvRGBImage = dynamic_cast< SVRGBImageClass* >( l_pImage );

		if( nullptr == l_psvMainImage && nullptr != l_psvRGBImage )
		{
			l_psvMainImage = dynamic_cast< SVCameraImageTemplate* >( l_pImage->GetParentImage() );
		}

		if( nullptr != l_psvMainImage )
		{
			SVGuidSVCameraInfoStructMap::const_iterator l_svIter;

			l_svIter = p_rsvCameraInfos.find( l_psvMainImage->GetCamera()->GetUniqueObjectID() );

			if( l_svIter != p_rsvCameraInfos.end() )
			{
				l_SourceImageDMIndexHandle.Assign( l_svIter->second.GetSourceImageDMIndexHandle(), SV_DISPLAY );
			}
		}

		svResultImageIndex.m_CameraDMIndexHandle.Assign( l_SourceImageDMIndexHandle, SV_DISPLAY );

		SVSmartHandlePointer l_ImageBuffer;

		l_pImage->GetImageHandle( svResultImageIndex, l_ImageBuffer );

		if( !( l_ImageBuffer.empty() ) )
		{
			// Special check for Color Tool's RGBMainImage which is HSI
			if( SV_IS_KIND_OF( l_pImage, SVRGBMainImageClass ) )
			{
				l_Status = SVImageProcessingClass::ConvertImageBuffer( p_rImageDIB, l_ImageBuffer, SVImageHLSToRGB );
			}
			else
			{
				SVBitmapInfo l_Info;

				BITMAPINFOHEADER l_Header = l_pImage->GetImageInfo().GetBitmapInfoHeader();

				l_Info.Assign( l_Header.biWidth, l_Header.biHeight, l_Header.biBitCount, SVBitmapInfo::GetDefaultColorTable( l_Header.biBitCount ) );

				if( l_Info == l_ImageBuffer->GetBitmapInfo() )
				{
					l_Status = SVImageProcessingClass::CopyImageBuffer( p_rImageDIB, l_ImageBuffer );
				}
				else
				{
					l_Status = SVImageProcessingClass::CopyImageBuffer( p_rImageDIB, l_Info, l_ImageBuffer );
				}
			}
		}
		else
		{
			l_Status = E_INVALIDARG;
		}

		if( S_OK == l_Status )
		{
			if( nullptr != l_pImage->GetInspection() )
			{
				dynamic_cast<SVInspectionProcess*>(l_pImage->GetInspection())->CollectOverlays( l_pImage, p_rMultiLineArray );
			}
			else
			{
				l_Status = E_INVALIDARG;
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}


