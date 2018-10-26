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
#include "SVMatroxLibrary\SVMatroxBufferCreateChildStruct.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary\MessageTextEnum.h"
#include "SVUtilityLibrary\StringHelper.h"
#pragma endregion Includes

namespace SvTrc
{
TriggerRecord::TriggerRecord(int inspectionPos, TriggerRecordData& rData, const SvPb::ImageList& rImageList, const long& rResetId)
	: m_inspectionPos(inspectionPos), m_rData(rData), m_rImageList(rImageList), m_ResetId(rResetId)
{
}

TriggerRecord::~TriggerRecord()
{
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		bool finishedTR = (TriggerRecord::m_WriteBlocked == m_rData.m_referenceCount);
		long value = InterlockedDecrement(&(m_rData.m_referenceCount));
		if (0 > value)
		{
			InterlockedExchange(&(m_rData.m_referenceCount), 0);
		}
		if (finishedTR)
		{
			TriggerRecordController::getTriggerRecordControllerInstance().setLastFinishedTR(m_inspectionPos, m_rData.m_trId);
		}
	}
}

IImagePtr TriggerRecord::getImage(const GUID& imageId, bool lockImage) const
{
	IImagePtr pImage;
	std::string ImageIdBytes;
	SvPb::SetGuidInProtoBytes(&ImageIdBytes, imageId);
	int pos = findImagePos(m_rImageList.list(), ImageIdBytes);
	if (-1 < pos)
	{
		pImage = getImage(pos, lockImage);
	}
	else //check if child image
	{
		pos = findImagePos(m_rImageList.childlist(), ImageIdBytes);
		if (-1 < pos)
		{
			pImage = getChildImage(pos, lockImage);
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
		auto& rImageController = ImageBufferController::getImageBufferControllerInstance();
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		if (0 <= pos && rImageList.size() > pos)
		{
			if (0 <= pImagePos[pos])
			{
				pImage = rImageController.getImage(pImagePos[pos], m_ResetId, lockImage);
				if (lockImage)
				{
					rImageController.increaseRefCounter(pImagePos[pos]);
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
						rImageController.increaseRefCounter(pImagePos[pos]);
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
		auto& rImageController = ImageBufferController::getImageBufferControllerInstance();
		const auto& rChildDef = m_rImageList.childlist(childPos);
		int pos = findImagePos(m_rImageList.list(), rChildDef.parentimageid());

		MatroxBufferChildDataStruct bufferDataStruct;
		memcpy(&bufferDataStruct, rChildDef.type().c_str(), sizeof(MatroxBufferChildDataStruct));
		const int*const pImagePos = m_rData.getImagePos();
		if (0 <= pos && m_rImageList.list_size() > pos)
		{
			pImage = rImageController.getChildImage(pImagePos[pos], bufferDataStruct, m_ResetId, lockImage);
			if (lockImage)
			{
				rImageController.increaseRefCounter(pImagePos[pos]);
			}
		}
		else
		{
			int childPos2 = findImagePos(m_rImageList.childlist(), rChildDef.parentimageid());
			IImagePtr pChildImage = getChildImage(childPos2, lockImage);
			if (!pChildImage->isEmpty())
			{
				SVMatroxBufferCreateChildStruct bufferStruct(pChildImage->getHandle()->GetBuffer());
				bufferStruct.m_data = bufferDataStruct;
				pImage = rImageController.getChildImage(bufferStruct, m_ResetId, pChildImage->getBufferPos(), lockImage);
			}
		}
	}
	return pImage;
}

bool TriggerRecord::isObjectUpToTime() const
{
	auto pLock = ResetLocker::lockReset(m_ResetId);
	return (nullptr != pLock);
}

void TriggerRecord::initImages()
{
	auto& rImageController = ImageBufferController::getImageBufferControllerInstance();
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

void TriggerRecord::setImages(const ITriggerRecordR& rDestTR)
{
	if (rDestTR.isObjectUpToTime())
	{
		auto& rImageController = ImageBufferController::getImageBufferControllerInstance();
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		const int*const pImagePosOld = dynamic_cast<const TriggerRecord&>(rDestTR).getTRData().getImagePos();
		for (int i = 0; i < rImageList.size(); i++)
		{
			if (-1 != pImagePosOld[i])
			{
				if (pImagePos[i] != pImagePosOld[i])
				{
					rImageController.decreaseRefCounter(pImagePos[i]);
					rImageController.increaseRefCounter(pImagePosOld[i]);
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
	int pos = findImagePos(m_rImageList.list(), ImageIdBytes);
	setImage(pos, pImage->getBufferPos());
}

void TriggerRecord::setImage(int pos, const IImagePtr& pImage)
{
	setImage(pos, pImage->getBufferPos());
}

void TriggerRecord::setImage(const GUID& rImageId, int bufferPos)
{
	std::string ImageIdBytes;
	SvPb::SetGuidInProtoBytes(&ImageIdBytes, rImageId);
	int pos = findImagePos(m_rImageList.list(), ImageIdBytes);
	setImage(pos, bufferPos);
}

void TriggerRecord::setImage(int pos, int bufferPos)
{
	auto& rImageController = ImageBufferController::getImageBufferControllerInstance();
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
					rImageController.decreaseRefCounter(pImagePos[pos]);
					rImageController.increaseRefCounter(bufferPos);
					pImagePos[pos] = bufferPos;
				}
			}
			else
			{
				rImageController.decreaseRefCounter(pImagePos[pos]);
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

IImagePtr TriggerRecord::createNewImageHandle(const GUID& imageId)
{
	IImagePtr pImage;
	std::string ImageIdBytes;
	SvPb::SetGuidInProtoBytes(&ImageIdBytes, imageId);

	int pos = findImagePos(m_rImageList.list(), ImageIdBytes);
	if (-1 < pos)
	{
		pImage = createNewImageHandle(pos);
	}
	return pImage;
}

IImagePtr TriggerRecord::createNewImageHandle(int pos)
{
	IImagePtr pImage;
	auto& rImageController = ImageBufferController::getImageBufferControllerInstance();
	const auto& rImageList = m_rImageList.list();
	int*const pImagePos = m_rData.getImagePos();
	if (0 <= pos && rImageList.size() > pos)
	{
		rImageController.decreaseRefCounter(pImagePos[pos]);
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
	return pImage;
}
} //namespace SvTrc
