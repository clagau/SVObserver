//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVResultList.h"
#include "SVToolSet.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#pragma endregion Includes





CommandInspectionCollectImageData::CommandInspectionCollectImageData(uint32_t inspectionId, const std::set<uint32_t>& p_rImageIds)
	: m_InspectionId(inspectionId)
	, m_ImageIds(p_rImageIds)
{
}

CommandInspectionCollectImageData::~CommandInspectionCollectImageData()
{
}
#pragma endregion Constructor

std::shared_ptr<SVIPProductStruct>  CommandInspectionCollectImageData::operator()()
{
	std::shared_ptr<SVIPProductStruct> pProduct = std::make_shared<SVIPProductStruct>();


	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(m_InspectionId);
	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(l_pObject);

	if (nullptr != pInspection)
	{
		auto product = pInspection->getLastProductData();
		double l_TriggerDistance = 0.0f;
		double l_ToolSetTime = 0.0f;

		if (product.first.m_PreviousTrigger > 0)
		{
			l_TriggerDistance = (double)(product.first.m_triggerTimeStamp - product.first.m_PreviousTrigger);
		}

		if ((product.second.m_BeginToolset > 0) && (product.second.m_EndToolset > 0))
		{
			l_ToolSetTime = (double)(product.second.m_EndToolset - product.second.m_BeginToolset);
		}

		pProduct->m_TriggerCount = product.first.lTriggerCount;

		UpdateResults(pInspection, pProduct->m_ResultData);

		pProduct->m_triggerRecord = product.second.m_triggerRecordComplete;
		pProduct->m_ResultData.m_ToolSetEndTime = product.second.m_ToolSetEndTime;
		pProduct->m_ResultData.m_ToolSetAvgTime = product.second.m_ToolSetAvgTime;
		pProduct->m_ResultData.m_ToolSetTime = l_ToolSetTime;
		pProduct->m_ResultData.m_TriggerDistance = l_TriggerDistance;

		const std::set<uint32_t>& rImageIds = m_ImageIds;

		for (auto imageId : rImageIds)
		{
			SVIPImageDataElement l_ImageData;

			::Sleep(0);

			if (S_OK == UpdateBuffer(imageId, product.second.m_triggerRecordComplete, l_ImageData.m_ImageDIB, l_ImageData.m_OverlayData))
			{
				::Sleep(0);

				pProduct->m_ImageData[imageId] = l_ImageData;
			}
		}
	}
	else
	{
		throw(E_FAIL);
	}

	return pProduct;
}



HRESULT CommandInspectionCollectImageData::UpdateResults(SVInspectionProcess* pInspection, SvIe::SVIPResultData& rResultData)
{
	HRESULT hRet = E_FAIL;
	SVResultList* pResultList(nullptr);

	if (nullptr != pInspection)
	{
		pResultList = pInspection->GetResultList();
	}


	if (nullptr != pResultList)
	{
		pResultList->updateIfRequired();
		pResultList->GetResultData(rResultData);
		hRet = S_OK;
	}


	return hRet;
}

HRESULT CommandInspectionCollectImageData::UpdateBuffer(uint32_t imageId, const SvOi::ITriggerRecordRPtr& pTriggerRecord, std::string& rImageDIB, SVExtentMultiLineStructVector& rMultiLineArray)
{
	HRESULT l_Status = S_OK;

	SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*> (SVObjectManagerClass::Instance().GetObject(imageId));

	if (nullptr != pImage && nullptr != pTriggerRecord)
	{
		SvOi::ITRCImagePtr pImageBuffer = pImage->getImageReadOnly(pTriggerRecord.get());

		if (nullptr != pImageBuffer && !pImageBuffer->isEmpty())
		{
			SVBitmapInfo BitmapInfo;
			l_Status = SVMatroxBufferInterface::CopyBufferToFileDIB(rImageDIB, BitmapInfo, pImageBuffer->getHandle()->GetBuffer(), false);
		}
		else
		{
			l_Status = E_INVALIDARG;
		}

		if (S_OK == l_Status)
		{
			if (nullptr != pImage->GetInspection())
			{
				dynamic_cast<SVInspectionProcess*>(pImage->GetInspection())->CollectOverlays(pImage, rMultiLineArray);
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


