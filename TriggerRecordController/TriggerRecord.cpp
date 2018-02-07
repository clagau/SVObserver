//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file TriggerRecord.cpp
/// All Rights Reserved
//*****************************************************************************
/// The class have access to a TriggerRecord.
//******************************************************************************
#pragma once

#pragma region Includes
#include "TriggerRecord.h"
#include "ImageBufferController.h"
#include "SVProtoBuf\BasicStructureHelper.h"
#include "ResetLocker.h"
#pragma endregion Includes

namespace SvTRC
{
	TriggerRecord::TriggerRecord(TriggerRecordData& rData, const SvPB::ImageList& rImageList, const time_t& rResetTime)
		: m_rData (rData), m_rImageList(rImageList), m_ResetTime(rResetTime)
	{
	}

	TriggerRecord::~TriggerRecord()
	{
		int refTmp = 0;
		int refNew = 0;
		{
			refTmp = m_rData.m_referenceCount;
			refNew = std::max(refTmp - 1 , 0);
		}
		while (InterlockedCompareExchange(&(m_rData.m_referenceCount), refNew, refTmp) == refTmp);
	}

	IImagePtr TriggerRecord::getImage(const GUID& imageId) const
	{
		IImagePtr pImage;
		auto lock = ResetLocker::lockReset(m_ResetTime);
		if (nullptr != lock)
		{
			auto& imageController = ImageBufferController::getImageBufferControllerInstance();
			const auto& rImageList = m_rImageList.list();
			SvPB::UUID imageUuid;
			SvPB::SetGuidInMessage(&imageUuid, imageId);
			auto imageIter = std::find_if(rImageList.begin(), rImageList.end(), [imageUuid](auto data)->bool
			{
				return (0 == data.imageid().guid().compare(imageUuid.guid()));
			});
			if (rImageList.end() != imageIter)
			{
				size_t pos_offset = std::distance(rImageList.begin(), imageIter);
				pImage = getImage(static_cast<int>(pos_offset));
			}
		}
		return pImage;
	}

	IImagePtr TriggerRecord::getImage(int pos) const
	{
		IImagePtr pImage;
		auto pLock = ResetLocker::lockReset(m_ResetTime);
		if (nullptr != pLock)
		{
			auto& rImageController = ImageBufferController::getImageBufferControllerInstance();
			const auto& rImageList = m_rImageList.list();
			const int*const imagePos = m_rData.getImagePos();
			if (0 <= pos && rImageList.size() > pos)
			{
				return rImageController.getImage(imagePos[pos]);
			}
		}
		return pImage;
	}

	void TriggerRecord::setImages(const ITriggerRecordR& rDestTR)
	{
		auto& rImageController = ImageBufferController::getImageBufferControllerInstance();
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		const int*const pImagePosOld = dynamic_cast<const TriggerRecord&>(rDestTR).getTRData().getImagePos();
		for (int i = 0; i < rImageList.size(); i++)
		{
			if (pImagePos[i] != pImagePosOld[i])
			{
				rImageController.DecreaseRefCounter(pImagePos[i]);
				rImageController.IncreaseRefCounter(pImagePosOld[i]);
				pImagePos[i] = pImagePosOld[i];
			}
		}
	}

	void TriggerRecord::setImage(int pos, int bufferPos)
	{
		auto& rImageController = ImageBufferController::getImageBufferControllerInstance();
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		if (0 <= pos && rImageList.size() > pos)
		{
			const auto& rSizeList = rImageController.getImageStructList().list()[pos];
			if (bufferPos >= rSizeList.offset() && bufferPos < rSizeList.offset() + rSizeList.numberofbuffers())
			{
				if (pImagePos[pos] != bufferPos)
				{
					rImageController.DecreaseRefCounter(pImagePos[pos]);
					rImageController.IncreaseRefCounter(bufferPos);
					pImagePos[pos] = bufferPos;
				}
			}
			else
			{
				assert(false);
			}
		}
	}

	IImagePtr TriggerRecord::createNewImageHandle(int pos)
	{
		IImagePtr pImage;
		auto& rImageController = ImageBufferController::getImageBufferControllerInstance();
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		if (0 <= pos && rImageList.size() > pos)
		{
			rImageController.DecreaseRefCounter(pImagePos[pos]);
			pImage = rImageController.createNewImageHandle(rImageList[pos].structid(), pImagePos[pos]);
			if (nullptr == pImage)
			{
				pImagePos[pos] = -1;
			}
		}
		return pImage;
	}
} //namespace SvTRC