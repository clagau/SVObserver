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
#pragma endregion Includes

namespace SvTRC
{
	TriggerRecord::TriggerRecord(TriggerRecordData& data)
		: m_data (data)
	{
	}

	TriggerRecord::~TriggerRecord()
	{
		//@TODO[MZA][8.00][20.12.2017] check if here is thread-safe-function to add
		if (0 < m_data.m_referenceCount)
		{
			m_data.m_referenceCount--;
		}
		else
		{
			m_data.m_referenceCount = 0;
		}
	}

	IImagePtr TriggerRecord::getImage(const GUID& imageId) const
	{
		IImagePtr pImage;
		auto& imageController = ImageBufferController::getImageBufferControllerInstance();
		const auto& rImageList = imageController.getImageList().list();
		SvPB::UUID imageUuid = SvPB::setGuidToMessage(imageId);
		auto imageIter = std::find_if(rImageList.begin(), rImageList.end(), [imageUuid](auto data)->bool
		{
			return data.imageid().part1() == imageUuid.part1() && data.imageid().part2() == imageUuid.part2();
		});
		if (rImageList.end() != imageIter)
		{
			size_t pos_offset = std::distance(rImageList.begin(), imageIter);
			pImage = getImage(static_cast<int>(pos_offset));
		}
		return pImage;
	}

	IImagePtr TriggerRecord::getImage(int pos) const
	{
		IImagePtr pImage;
		auto& rImageController = ImageBufferController::getImageBufferControllerInstance();
		const auto& rImageList = rImageController.getImageList().list();
		const int*const imagePos = m_data.getImagePos();
		if (0 <= pos && rImageList.size() > pos)
		{
			return rImageController.getImage(imagePos[pos]);
		}
		return pImage;
	}

	void TriggerRecord::setImages(const ITriggerRecordR& rDestTR)
	{
		auto& rImageController = ImageBufferController::getImageBufferControllerInstance();
		const auto& rImageList = rImageController.getImageList().list();
		int*const pImagePos = m_data.getImagePos();
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
		const auto& rImageList = rImageController.getImageList().list();
		int*const pImagePos = m_data.getImagePos();
		if (0 <= pos && rImageList.size() > pos)
		{
			int sizeId = rImageList[pos].sizeid();
			const auto& rSizeList = rImageController.getImageSizeList().list()[pos];
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
		const auto& rImageList = rImageController.getImageList().list();
		int*const pImagePos = m_data.getImagePos();
		if (0 <= pos && rImageList.size() > pos)
		{
			rImageController.DecreaseRefCounter(pImagePos[pos]);
			pImage = rImageController.createNewImageHandle(rImageList[pos].sizeid(), pImagePos[pos]);
			if (nullptr == pImage)
			{
				pImagePos[pos] = -1;
			}
		}
		return pImage;
	}
} //namespace SvTRC