//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file TriggerRecord.cpp
/// All Rights Reserved
//*****************************************************************************
/// The class have access to a TriggerRecord.
//******************************************************************************
#pragma once

#pragma region Includes
#include "stdafx.h"
#include "TriggerRecord.h"
#include "ImageBufferController.h"
#include "SVProtoBuf\ConverterHelper.h"
#include "ResetLocker.h"
#include "CopyData.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateChildStruct.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary\MessageTextEnum.h"
#include "SVUtilityLibrary\StringHelper.h"
#pragma endregion Includes

namespace SvTrc
{
TriggerRecord::TriggerRecord(int inspectionPos, TriggerRecordData& rData, const SvPb::ImageList& rImageList, const SvPb::DataDefinitionList& rDataDefList, int dataListSize, long resetId)
: m_inspectionPos(inspectionPos)
, m_rData(rData)
, m_rImageList(rImageList)
, m_rDataDefList(rDataDefList)
, m_dataListSize(dataListSize)
, m_ResetId(resetId)
{
}

TriggerRecord::~TriggerRecord()
{
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		bool finishedTR = (TriggerRecordData::cWriteBlocked == m_rData.m_referenceCount);
		long value = InterlockedDecrement(&(m_rData.m_referenceCount));
		if (0 > value)
		{
			InterlockedExchange(&(m_rData.m_referenceCount), 0);
		}
		if (finishedTR)
		{
			getTriggerRecordControllerInstance().setLastFinishedTR(m_inspectionPos, m_rData.m_trId);
		}
	}
}

int TriggerRecord::getId() const 
{
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		return m_rData.m_trId;
	}
	return -1;
}

const TriggerData& TriggerRecord::getTriggerData() const 
{ 
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		return m_rData.m_triggerData;
	}
	static TriggerData emptyData;
	return emptyData;
}

IImagePtr TriggerRecord::getImage(const GUID& imageId, bool lockImage) const
{
	IImagePtr pImage;
	std::string ImageIdBytes;
	SvPb::SetGuidInProtoBytes(&ImageIdBytes, imageId);
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		int pos = findGuidPos(m_rImageList.list(), ImageIdBytes);
		if (-1 < pos)
		{
			pImage = getImage(pos, lockImage);
		}
		else //check if child image
		{
			pos = findGuidPos(m_rImageList.childlist(), ImageIdBytes);
			if (-1 < pos)
			{
				pImage = getChildImage(pos, lockImage);
			}
		}
	}
	return pImage;
}

IImagePtr TriggerRecord::getImage(int pos, bool lockImage) const
{
	IImagePtr pImage;
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		auto& rImageController = getImageBufferControllerInstance();
		const auto& rImageList = m_rImageList.list();
		int* const pImagePos = m_rData.getImagePos();
		if (0 <= pos && rImageList.size() > pos)
		{
			if (0 <= pImagePos[pos])
			{
				pImage = rImageController.getImage(pImagePos[pos], m_ResetId, lockImage);
				if (lockImage)
				{
					rImageController.increaseImageRefCounter(pImagePos[pos]);
				}
			}
			else
			{
				//This part is needed if a buffer of an image with is not set yet. For this set a new buffer and clear it.
				pImage = rImageController.createNewImageHandle(rImageList[pos].structid(), pImagePos[pos], m_ResetId, lockImage);
				if (nullptr == pImage || pImage->isEmpty())
				{
					pImagePos[pos] = -1;
					SvDef::StringVector msgList;
					msgList.push_back(SvUl::Format(_T("%d"), pos));
					SvStl::MessageMgrStd e(SvStl::MsgType::Log);
					e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NewBufferFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				}
				else
				{
					MbufClear(pImage->getHandle()->GetBuffer().GetIdentifier(), 0);
					if (lockImage)
					{
						rImageController.increaseImageRefCounter(pImagePos[pos]);
					}
				}
			}
		}
	}
	return pImage;
}

IImagePtr TriggerRecord::getChildImage(int childPos, bool lockImage) const
{
	IImagePtr pImage;
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock && 0 <= childPos && m_rImageList.childlist_size() > childPos)
	{
		auto& rImageController = getImageBufferControllerInstance();
		const auto& rChildDef = m_rImageList.childlist(childPos);
		int pos = findGuidPos(m_rImageList.list(), rChildDef.parentimageid());

		MatroxBufferChildDataStruct bufferDataStruct;
		memcpy(&bufferDataStruct, rChildDef.type().c_str(), sizeof(MatroxBufferChildDataStruct));
		const int*const pImagePos = m_rData.getImagePos();
		if (0 <= pos && m_rImageList.list_size() > pos)
		{
			pImage = rImageController.getChildImage(pImagePos[pos], bufferDataStruct, m_ResetId, lockImage);
			if (lockImage)
			{
				rImageController.increaseImageRefCounter(pImagePos[pos]);
			}
		}
		else
		{
			int childPos2 = findGuidPos(m_rImageList.childlist(), rChildDef.parentimageid());
			IImagePtr pChildImage = getChildImage(childPos2, lockImage);
			if (nullptr != pChildImage && !pChildImage->isEmpty())
			{
				SVMatroxBufferCreateChildStruct bufferStruct(pChildImage->getHandle()->GetBuffer());
				bufferStruct.m_data = bufferDataStruct;
				pImage = rImageController.getChildImage(bufferStruct, m_ResetId, pChildImage->getBufferPos(), lockImage);
			}
		}
	}
	return pImage;
}

_variant_t TriggerRecord::getDataValue(const GUID& dataId) const
{
	std::string guidIdBytes;
	SvPb::SetGuidInProtoBytes(&guidIdBytes, dataId);
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		int pos = findGuidPos(m_rDataDefList.list(), guidIdBytes);
		return getDataValue(pos);
	}
	return {};
}

_variant_t TriggerRecord::getDataValue(int pos) const
{
	_variant_t result;
	if (-1 < pos)
	{
		auto pLock = ResetLocker::lockReset(m_ResetId);
		if (nullptr != pLock)
		{
			std::atomic_int* pSize = reinterpret_cast<std::atomic_int*> (m_rData.getValueData());
			int DataSize = *pSize;
			if (DataSize > 0)
			{
				SvPb::DataList valueList;

				//The next position is where the value data list is streamed
				void* pSource = reinterpret_cast<void*> (pSize + 1);
				valueList.ParseFromArray(pSource, DataSize);
				if (valueList.valuelist_size() > pos)
				{
					SvPb::ConvertProtobufToVariant(valueList.valuelist()[pos], result);
				}
			}
		}
	}

	return result;
}

bool TriggerRecord::isValueDataValid() const
{
	bool retValue = false;
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		std::atomic_int* pSize = reinterpret_cast<std::atomic_int*> (m_rData.getValueData());
		//Data is valid when the data size > 0
		retValue = (0 < *pSize);
	}
	return retValue;
}

bool TriggerRecord::isObjectUpToTime() const
{
	auto pLock = ResetLocker::lockReset(m_ResetId);
	return (nullptr != pLock);
}

void TriggerRecord::setTriggerData(const TriggerData& data) 
{ 
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		m_rData.m_triggerData = data;
	}
}

void TriggerRecord::initImages()
{
	auto& rImageController = getImageBufferControllerInstance();
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		for (int i = 0; i < rImageList.size(); i++)
		{
			if (-1 == pImagePos[i])
			{
				IImagePtr pImage = rImageController.createNewImageHandle(rImageList[i].structid(), pImagePos[i], m_ResetId, false);
				if (nullptr == pImage)
				{
					SvDef::StringVector msgList;
					msgList.push_back(SvUl::Format(_T("%d"), pImagePos[i]));
					SvStl::MessageMgrStd e(SvStl::MsgType::Log);
					e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NewBufferFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				}
			}
		}
	}
}

void TriggerRecord::setImages(const ITriggerRecordR& rDestTR)
{
	auto& rImageController = getImageBufferControllerInstance();
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock && rDestTR.isObjectUpToTime())
	{
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		const int*const pImagePosOld = dynamic_cast<const TriggerRecord&>(rDestTR).getTRData().getImagePos();
		for (int i = 0; i < rImageList.size(); i++)
		{
			if (-1 != pImagePosOld[i])
			{
				if (pImagePos[i] != pImagePosOld[i])
				{
					rImageController.decreaseImageRefCounter(pImagePos[i]);
					rImageController.increaseImageRefCounter(pImagePosOld[i]);
					pImagePos[i] = pImagePosOld[i];
				}
			}
			else
			{
				if (-1 == pImagePos[i])
				{
					IImagePtr pImage = rImageController.createNewImageHandle(rImageList[i].structid(), pImagePos[i], m_ResetId, false);
					if (nullptr == pImage)
					{
						SvDef::StringVector msgList;
						msgList.push_back(SvUl::Format(_T("%d"), pImagePos[i]));
						SvStl::MessageMgrStd e(SvStl::MsgType::Log);
						e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NewBufferFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
					}
				}
			}
		}
	}
	else
	{
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_WARNING, SvStl::Tid_TRC_Error_SetImages_SourceTROutOfData, SvStl::SourceFileParams(StdMessageParams));
		initImages();
	}
}

void TriggerRecord::setImage(const GUID& rImageId, const IImagePtr& pImage)
{
	std::string ImageIdBytes;
	SvPb::SetGuidInProtoBytes(&ImageIdBytes, rImageId);
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		int pos = findGuidPos(m_rImageList.list(), ImageIdBytes);
		setImage(pos, pImage->getBufferPos());
	}
}

void TriggerRecord::setImage(int pos, const IImagePtr& pImage)
{
	setImage(pos, pImage->getBufferPos());
}

void TriggerRecord::setImage(const GUID& rImageId, int bufferPos)
{
	std::string ImageIdBytes;
	SvPb::SetGuidInProtoBytes(&ImageIdBytes, rImageId);
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		int pos = findGuidPos(m_rImageList.list(), ImageIdBytes);
		setImage(pos, bufferPos);
	}
}

void TriggerRecord::setImage(int pos, int bufferPos)
{
	auto& rImageController = getImageBufferControllerInstance();
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		if (0 <= pos && rImageList.size() > pos)
		{
			int typePos = rImageList[pos].structid();
			auto& pStructList = rImageController.getImageStructList().list();
			if (0 <= typePos && pStructList.size() > typePos)
			{
				const auto& rSizeList = pStructList[typePos];
				if (bufferPos >= rSizeList.offset() && bufferPos < rSizeList.offset() + rSizeList.numberofbuffers())
				{
					if (pImagePos[pos] != bufferPos)
					{
						rImageController.decreaseImageRefCounter(pImagePos[pos]);
						rImageController.increaseImageRefCounter(bufferPos);
						pImagePos[pos] = bufferPos;
					}
				}
				else
				{
					rImageController.decreaseImageRefCounter(pImagePos[pos]);
					pImagePos[pos] = -1;
					assert(false);
					SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
					Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_SetImage_TypeNotFit, SvStl::SourceFileParams(StdMessageParams));
					Exception.Throw();
				}
			}
			else
			{
				assert(false);
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
				Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_SetImage_InvalidType, SvStl::SourceFileParams(StdMessageParams));
				Exception.Throw();
			}
		}
		else
		{
			assert(false);
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_SetImage_InvalidPos, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}
	}
}

IImagePtr TriggerRecord::createNewImageHandle(const GUID& imageId)
{
	IImagePtr pImage;
	std::string ImageIdBytes;
	SvPb::SetGuidInProtoBytes(&ImageIdBytes, imageId);

	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		int pos = findGuidPos(m_rImageList.list(), ImageIdBytes);
		if (-1 < pos)
		{
			pImage = createNewImageHandle(pos);
		}
	}
	return pImage;
}

IImagePtr TriggerRecord::createNewImageHandle(int pos)
{
	IImagePtr pImage;
	auto& rImageController = getImageBufferControllerInstance();
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		if (0 <= pos && rImageList.size() > pos)
		{
			rImageController.decreaseImageRefCounter(pImagePos[pos]);
			pImage = rImageController.createNewImageHandle(rImageList[pos].structid(), pImagePos[pos], m_ResetId);
			if (nullptr == pImage)
			{
				pImagePos[pos] = -1;
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%d"), pos));
				SvStl::MessageMgrStd e(SvStl::MsgType::Log);
				e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NewBufferFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
	return pImage;
}

void TriggerRecord::initValueData()
{
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		//It is sufficient to set the data size to 0 to invalidate the complete trigger record value data
		std::atomic_int* pSize = reinterpret_cast<std::atomic_int*> (m_rData.getValueData());
		*pSize = 0;
	}
}

void TriggerRecord::writeValueData(std::vector<_variant_t>&& valueObjectList)
{
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		void* pData = m_rData.getValueData();
		auto copyDataThread = std::async(std::launch::async, [&] { copyDataList(std::move(valueObjectList), pData, m_dataListSize, m_ResetId); });
	}
}

} //namespace SvTrc
