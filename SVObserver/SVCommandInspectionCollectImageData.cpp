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
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVInspectionProcess.h"
#include "SVResultList.h"
#include "SVToolSet.h"
#include "Definitions/GlobalConst.h"
#include "SVPPQObject.h"
#include "Definitions/Color.h"
#pragma endregion Includes

#pragma region Constructor
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
		auto product = pInspection->getLastProductData();
		double l_TriggerDistance = 0.0f;
		double l_ToolSetTime = 0.0f;

		if(product.first.m_PreviousTrigger > 0 )
		{
			l_TriggerDistance = (double) (product.first.m_BeginProcess - product.first.m_PreviousTrigger );
		}

		if( (product.second.m_BeginToolset > 0 ) &&	(product.second.m_EndToolset > 0 ) )
		{
			l_ToolSetTime = (double) (product.second.m_EndToolset - product.second.m_BeginToolset );
		}

		m_Product.m_TriggerCount = product.first.lTriggerCount;

		UpdateResults( pInspection, m_Product.m_ResultData );

		m_Product.m_triggerRecord = product.second.m_triggerRecordComplete;
		m_Product.m_ResultData.m_ToolSetEndTime = product.second.m_ToolSetEndTime;
		m_Product.m_ResultData.m_ToolSetAvgTime = product.second.m_ToolSetAvgTime;
		m_Product.m_ResultData.m_ToolSetTime = l_ToolSetTime;
		m_Product.m_ResultData.m_TriggerDistance = l_TriggerDistance;

		const SVGuidSet& rImageIds = GetImageIds();

		for (SVGUID imageId : rImageIds)
		{
			SVIPImageDataElement l_ImageData;

			::Sleep( 0 );

			if( S_OK == UpdateBuffer(imageId, product.second.m_triggerRecordComplete, l_ImageData.m_ImageDIB , l_ImageData.m_OverlayData ) )
			{
				::Sleep( 0 );

				m_Product.m_ImageData[imageId] = l_ImageData;
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

HRESULT SVCommandInspectionCollectImageData::UpdateResults(SVInspectionProcess* pInspection, SvIe::SVIPResultData& rResultData )
{
	HRESULT hRet = E_FAIL;
	SVResultListClass* pResultList(nullptr);

	if( nullptr != pInspection )
	{
		pResultList = pInspection->GetResultList();
	}


	if (nullptr != pResultList)
	{		
		pResultList->GetResultData(rResultData);
		hRet = S_OK;
	} 


	return hRet;
}

HRESULT SVCommandInspectionCollectImageData::UpdateBuffer(const SVGUID& rImageId, const SvTrc::ITriggerRecordRPtr& pTriggerRecord, std::string& rImageDIB, SVExtentMultiLineStructVector& rMultiLineArray)
{
	HRESULT l_Status = S_OK;

	SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*> ( SVObjectManagerClass::Instance().GetObject( rImageId.ToGUID() ) );

	if( nullptr != pImage && nullptr != pTriggerRecord )
	{
		SvTrc::IImagePtr pImageBuffer = pImage->getImageReadOnly(pTriggerRecord.get());

		if(nullptr != pImageBuffer && !pImageBuffer->isEmpty())
		{
			SVBitmapInfo BitmapInfo;
			l_Status = SVMatroxBufferInterface::CopyBufferToFileDIB(rImageDIB, BitmapInfo, pImageBuffer->getHandle()->GetBuffer(), false);
		}
		else
		{
			l_Status = E_INVALIDARG;
		}

		if( S_OK == l_Status )
		{
			if( nullptr != pImage->GetInspection() )
			{
				dynamic_cast<SVInspectionProcess*>(pImage->GetInspection())->CollectOverlays( pImage, rMultiLineArray );
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


