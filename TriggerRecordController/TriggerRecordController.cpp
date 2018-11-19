//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file TriggerRecordController.cpp
/// All Rights Reserved
//*****************************************************************************
/// The TriggerRecordController: control the triggerRecords.
//******************************************************************************
#pragma once

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <WinBase.h>
#include "ITriggerRecordControllerRW.h"
#include "ITriggerRecordRW.h"
#include "Image.h"
#include "TriggerRecordController.h"
#include "ImageBufferController.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary\SVMatroxResourceMonitor.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVMessage\SVMessage.h"
#include "SVProtoBuf\ConverterHelper.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVUtilityLibrary\StringHelper.h"

#pragma endregion Includes

namespace SvTrc
{
#pragma region Constructor
TriggerRecordController& TriggerRecordController::getTriggerRecordControllerInstance()
{
	static TriggerRecordController controller;

	return controller;
}

TriggerRecordController::TriggerRecordController()
{
	::InitializeCriticalSection(&m_hCriticalSectionCallback);

	//The next call is only to avoid a crash at the end of the application. 
	//Reason of the crash was that SVMatroxResourceMonitor was destructed before the ImageBufferController, but this need it in its destructor.
	//If the singleton of SVMatroxResourceMonitor created before of the singleton of ImageBufferController the destruction it in the right order.
	//In Release-Mode this call do nothing.
	SVMatroxResourceMonitor::SVAutoLock autoLock;
	SVMatroxResourceMonitor::GetAutoLock(autoLock);
}

TriggerRecordController::~TriggerRecordController()
{
	m_isResetLocked = false;
	clearAll();
	::DeleteCriticalSection(&m_hCriticalSectionCallback);
}
#pragma endregion Constructor

#pragma region Public Methods
void TriggerRecordController::setLastFinishedTR(int inspectionPos, int id)
{
	if (0 <= inspectionPos && m_IPDataNumber > inspectionPos)
	{
		m_pData[inspectionPos].m_lastFinishedTRID = id;
	}
}

int TriggerRecordController::getLastTRId(int inspectionPos) const
{
	int id = -1;
	if (0 <= inspectionPos && m_IPDataNumber > inspectionPos)
	{
		id = m_pData[inspectionPos].m_lastFinishedTRID;
	}
	return id;
}

const SvPb::ImageList& TriggerRecordController::getImageDefList(int inspectionPos)
{
	if (-1 == m_resetStarted4IP)
	{
		if (0 <= inspectionPos && m_IPDataNumber > inspectionPos && m_pData[inspectionPos].m_bInit)
		{
			if (nullptr != m_pData[inspectionPos].m_pImageList)
			{
				return *m_pData[inspectionPos].m_pImageList;
			}
		}
	}

	assert(false);
	SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetImageDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

ITriggerRecordRPtr TriggerRecordController::createTriggerRecordObject(int inspectionPos, int trId)
{
	assert(-1 == m_resetStarted4IP);

	if (-1 == m_resetStarted4IP && 0 <= inspectionPos && m_IPDataNumber > inspectionPos
		&& m_pData[inspectionPos].m_bInit && nullptr != m_pData[inspectionPos].m_pImageList)
	{
		if (0 <= trId)
		{
			for (int i = 0; i < m_pData[inspectionPos].m_TriggerRecordNumber; i++)
			{
				TriggerRecord::TriggerRecordData& rCurrentTR = getTRData(inspectionPos, i);
				if (rCurrentTR.m_trId == trId)
				{
					long refTemp = rCurrentTR.m_referenceCount;
					while (0 <= refTemp)
					{
						if (InterlockedCompareExchange(&(rCurrentTR.m_referenceCount), refTemp + 1, refTemp) == refTemp)
						{
							return ITriggerRecordRPtr(new TriggerRecord(inspectionPos, rCurrentTR, *m_pData[inspectionPos].m_pImageList, m_resetId));
						}
						refTemp = rCurrentTR.m_referenceCount;
					}
				}
			}
		}
	}
	return nullptr;
}

void TriggerRecordController::registerCallback(void* pOwner, StartResetCallbackPtr pCallback)
{
	assert(pOwner);
	assert(pCallback);
	if (nullptr != pOwner && nullptr != pCallback)
	{
		::EnterCriticalSection(&m_hCriticalSectionCallback);
		auto Iter = find_if(m_ResetCallbacks.begin(), m_ResetCallbacks.end(), [pOwner, pCallback](const auto& data)->bool
		{
			return (data.m_pOwner == pOwner && data.m_pCallback == pCallback);
		});
		if (m_ResetCallbacks.end() == Iter)
		{
			StartCallbackStruct tmp(pOwner, pCallback);
			m_ResetCallbacks.push_back(tmp);
		}
		::LeaveCriticalSection(&m_hCriticalSectionCallback);
	}
}

void TriggerRecordController::unregisterCallback(void* pOwner, StartResetCallbackPtr pCallback)
{
	assert(pOwner);
	assert(pCallback);
	if (nullptr != pOwner && nullptr != pCallback)
	{
		::EnterCriticalSection(&m_hCriticalSectionCallback);
		auto Iter = find_if(m_ResetCallbacks.begin(), m_ResetCallbacks.end(), [pOwner, pCallback](const auto& data)->bool
		{
			return (data.m_pOwner == pOwner && data.m_pCallback == pCallback);
		});
		if (m_ResetCallbacks.end() != Iter)
		{
			m_ResetCallbacks.erase(Iter);
		}
		::LeaveCriticalSection(&m_hCriticalSectionCallback);
	}
}

void TriggerRecordController::clearAll()
{
	sendResetCall();
	m_nextTRID = 0;
	m_resetId = 0;
	while (0 < m_resetLockCounter) {};
	m_inspectionList.clear_list();
	m_additionalBufferMap.clear();
	m_resetStarted4IP = -1;
	m_imageStructListResetTmp.Clear();
	m_imageListResetTmp.Clear();
	m_TriggerRecordNumberResetTmp = 0;
	m_ResetCallbacks.clear();
	m_isResetLocked = false;

	for (int i = 0; i < m_IPDataNumber; i++)
	{
		ResetInspectionData(i);
	}
	delete[] m_pData;
	m_pData = nullptr;
	m_IPDataNumber = 0;

	ImageBufferController::getImageBufferControllerInstance().clearAll();
}

bool TriggerRecordController::setInspections(const SvPb::InspectionList& rInspectionList)
{
	if (m_isResetLocked)
	{
		return false;
	}

	for (auto& rInspection : rInspectionList.list())
	{
		if (m_maxTriggerRecords < rInspection.numberofrecords())
		{
			assert(false);
			return false;
		}
	}

	bool isReset = false;

	if (rInspectionList.list_size() != m_inspectionList.list_size())
	{
		sendResetCall();
		m_resetId = 0;
		isReset = true;
		while (0 < m_resetLockCounter) {};

		for (int i = 0; i < m_IPDataNumber; i++)
		{
			ResetInspectionData(i);
		}
		if (rInspectionList.list_size() != m_IPDataNumber && 0 < rInspectionList.list_size())
		{
			delete[] m_pData;
			m_IPDataNumber = rInspectionList.list_size();
			m_pData = new TRControllerDataPerIP[m_IPDataNumber];
		}
	}
	else
	{
		for (int i = 0; i < m_IPDataNumber; i++)
		{
			if (rInspectionList.list(i).id() != m_inspectionList.list(i).id() ||
				rInspectionList.list(i).numberofrecords() != m_inspectionList.list(i).numberofrecords())
			{
				if (!isReset)
				{
					sendResetCall();
					m_resetId = 0;
					while (0 < m_resetLockCounter) {};
					isReset = true;
				}
				ResetInspectionData(i);
			}
		}
	}

	if (isReset)
	{
		m_inspectionList = rInspectionList;
	}

	return true;
}

ITriggerRecordRWPtr TriggerRecordController::createTriggerRecordObjectToWrite(int inspectionPos)
{
	assert(-1 == m_resetStarted4IP && 0 <= inspectionPos && m_IPDataNumber > inspectionPos && m_pData[inspectionPos].m_bInit);

	if (-1 == m_resetStarted4IP && 0 <= inspectionPos && m_IPDataNumber > inspectionPos
		&& m_pData[inspectionPos].m_bInit && nullptr != m_pData[inspectionPos].m_pImageList)
	{
		int currentPos = m_pData[inspectionPos].m_nextPosForFreeCheck;
		do
		{
			TriggerRecord::TriggerRecordData& rCurrentTR = getTRData(inspectionPos, currentPos);
			long count = rCurrentTR.m_referenceCount;
			while (TriggerRecord::m_InvalidData == count || 0 == count)
			{
				if (InterlockedCompareExchange(&(rCurrentTR.m_referenceCount), TriggerRecord::m_WriteBlocked, count) == count)
				{
					rCurrentTR.m_trId = m_nextTRID++;
					m_pData[inspectionPos].m_lastStartedTRID = rCurrentTR.m_trId;
					m_pData[inspectionPos].m_nextPosForFreeCheck = (currentPos + 1) % (m_pData[inspectionPos].m_TriggerRecordNumber);
					return ITriggerRecordRWPtr(new TriggerRecord(inspectionPos, rCurrentTR, *m_pData[inspectionPos].m_pImageList, m_resetId));
				}
			}
			currentPos++;
			currentPos = currentPos%m_pData[inspectionPos].m_TriggerRecordNumber;
		} while (currentPos != m_pData[inspectionPos].m_nextPosForFreeCheck);
		assert(false);
	}

	return ITriggerRecordRWPtr();
};

ITriggerRecordRPtr TriggerRecordController::closeWriteAndOpenReadTriggerRecordObject(ITriggerRecordRWPtr& pTriggerRecord)
{
	if (nullptr != pTriggerRecord)
	{
		int id = pTriggerRecord->getId();
		int inspectionPos = pTriggerRecord->getInspectionPos();
		pTriggerRecord = nullptr;
		return createTriggerRecordObject(inspectionPos, id);
	}
	return nullptr;
}

IImagePtr TriggerRecordController::getImageBuffer(const SVMatroxBufferCreateStruct& rBufferStruct, bool createBufferExternIfNecessary) const
{
	IImagePtr retImage;
	if (-1 == m_resetStarted4IP)
	{
		retImage = ImageBufferController::getImageBufferControllerInstance().createNewImageHandle(rBufferStruct, m_resetId);
	}

	if (nullptr == retImage && createBufferExternIfNecessary)
	{
		SVMatroxBuffer ImageBufId;
		SVMatroxBufferInterface::Create(ImageBufId, rBufferStruct);
		SvOi::SVImageBufferHandlePtr pImagePtr = SvOi::SVImageBufferHandlePtr {new SVImageBufferHandleImage(ImageBufId)};
		retImage = IImagePtr {new Image(pImagePtr, 0, -1, false, false)};
	}
	assert(nullptr != retImage);
	return retImage;
}

void TriggerRecordController::startResetTriggerRecordStructure(int inspectionPos)
{
	if (m_isResetLocked)
	{
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetLocked, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (-1 != m_resetStarted4IP)
	{   //new start of reset is not allowed if reset is in progress.
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), m_resetStarted4IP));
		msgList.push_back(SvUl::Format(_T("%d"), inspectionPos));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetAllReadyStarted, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if ((0 > inspectionPos && m_cResetStartedAddBuffer != inspectionPos) || m_inspectionList.list_size() <= inspectionPos)
	{   //new start of reset is not allowed if reset is in progress.
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), inspectionPos));
		msgList.push_back(SvUl::Format(_T("%d"), m_inspectionList.list_size()));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetWrongInspectionId, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (m_cResetStartedAddBuffer != inspectionPos)
	{
		int TriggerRecordSize = m_inspectionList.list(inspectionPos).numberofrecords();
		if (0 >= TriggerRecordSize || m_maxTriggerRecords < TriggerRecordSize)
		{
			assert(false);
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), TriggerRecordSize));
			msgList.push_back(SvUl::Format(_T("%d"), m_maxTriggerRecords));
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TriggerRecordSize2Big, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}

		m_resetStarted4IP = inspectionPos;

		//clear imageList and reset ImageStructList
		m_imageListResetTmp.Clear();
		int TRNumber = 0;
		if (nullptr != m_pData[m_resetStarted4IP].m_pImageList)
		{
			m_imageListResetTmp = *m_pData[m_resetStarted4IP].m_pImageList;
			TRNumber = m_pData[m_resetStarted4IP].m_TriggerRecordNumber;
		}
		m_imageStructListResetTmp.Clear();
		m_imageStructListResetTmp = ImageBufferController::getImageBufferControllerInstance().getImageStructList();
		//remove number of buffers from required list from old reserved images of this inspection.
		for (const auto& rSizeData : m_imageListResetTmp.list())
		{
			int id = rSizeData.structid();
			if (0 <= id && m_imageStructListResetTmp.list_size() > id)
			{
				auto* pImageStruct = m_imageStructListResetTmp.mutable_list(id);
				if (nullptr != pImageStruct)
				{
					pImageStruct->set_numberofbuffersrequired(pImageStruct->numberofbuffersrequired() - TRNumber);
				}
			}
		}

		m_imageListResetTmp.Clear();

		m_TriggerRecordNumberResetTmp = TriggerRecordSize + m_TriggerRecordAddOn;
	}
	else
	{
		m_resetStarted4IP = inspectionPos;
		m_imageStructListResetTmp = ImageBufferController::getImageBufferControllerInstance().getImageStructList();
		m_imageListResetTmp.Clear();
	}
}

void TriggerRecordController::finishResetTriggerRecordStructure()
{
	if (-1 == m_resetStarted4IP)
	{   //stop of reset is not allowed if reset is not started.
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetNotStart, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	ResetTriggerRecordStructure();
}

int TriggerRecordController::addOrChangeImage(const GUID& rImageId, const SVMatroxBufferCreateStruct& rBufferStruct, int inspectionPos /*= -1*/)
{
	ResetEnum resetEnum = calcResetEnum(inspectionPos);
	if (ResetEnum::Invalid == resetEnum)
	{   //Not possible to add and change image.
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (ResetEnum::NewReset == resetEnum)
	{
		int TriggerRecordSize = m_inspectionList.list(inspectionPos).numberofrecords();
		if (0 >= TriggerRecordSize || m_maxTriggerRecords < TriggerRecordSize)
		{
			assert(false);
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TriggerRecordSize2Big, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}


		//prefer reset
		m_resetStarted4IP = inspectionPos;
		m_TriggerRecordNumberResetTmp = TriggerRecordSize + m_TriggerRecordAddOn;
		m_imageListResetTmp.Clear();
		if (nullptr != m_pData[m_resetStarted4IP].m_pImageList)
		{
			m_imageListResetTmp = *m_pData[m_resetStarted4IP].m_pImageList;
		}
		m_imageStructListResetTmp.Clear();
		m_imageStructListResetTmp = ImageBufferController::getImageBufferControllerInstance().getImageStructList();
	}

	//check if rBufferStruct already in sizeList
	std::string typeStr(reinterpret_cast<const char*>(&rBufferStruct), sizeof(rBufferStruct));
	auto pImageStructList = m_imageStructListResetTmp.mutable_list();
	auto bufferStructIter = std::find_if(pImageStructList->begin(), pImageStructList->end(), [typeStr](auto data)->bool
	{
		return (0 == data.type().compare(typeStr));
	});
	SvPb::ImageStructData* pStructData = nullptr;
	if (pImageStructList->end() != bufferStructIter)
	{
		pStructData = &(*bufferStructIter);
	}

	std::string ImageIdBytes;
	SvPb::SetGuidInProtoBytes(&ImageIdBytes, rImageId);
	auto* pList = m_imageListResetTmp.mutable_list();
	//check if image with this GUID already in list (this is not allowed.)
	auto pImageIter = std::find_if(pList->begin(), pList->end(), [&ImageIdBytes](auto data)->bool
	{
		return (0 == data.imageid().compare(ImageIdBytes));
	});
	int imagePos = -1;
	SvPb::ImageDefinition* pImageDefinition = nullptr;
	if (pList->end() != pImageIter)
	{
		imagePos = static_cast<int> (std::distance(pList->begin(), pImageIter));
		pImageDefinition = &(*pImageIter);
		if (pImageDefinition->type() == typeStr)
		{ //it this the same type, nothing to do
			if (ResetEnum::NewReset == resetEnum)
			{
				m_resetStarted4IP = -1;
			}
			return imagePos;
		}

		//Remove old struct from required list.
		auto* pImageStruct = m_imageStructListResetTmp.mutable_list(pImageIter->structid());
		if (nullptr != pImageStruct)
		{
			pImageStruct->set_numberofbuffersrequired(pImageStruct->numberofbuffersrequired() - m_TriggerRecordNumberResetTmp);
		}
	}
	else
	{
		pImageDefinition = m_imageListResetTmp.add_list();
		imagePos = pList->size() - 1;
		pImageDefinition->set_imageid(ImageIdBytes);
	}

	setStructData(pStructData, pImageDefinition, typeStr);
	if (ResetEnum::NewReset == resetEnum)
	{
		ResetTriggerRecordStructure();
	}
	return imagePos;
}

int TriggerRecordController::addOrChangeChildImage(const GUID& rImageId, const GUID& rParentId, const MatroxBufferChildDataStruct& rBufferStruct, int inspectionPos /* = -1*/)
{
	ResetEnum resetEnum = calcResetEnum(inspectionPos);
	if (ResetEnum::Invalid == resetEnum)
	{   //Not possible to add and change image.
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (ResetEnum::NewReset == resetEnum)
	{
		m_imageListResetTmp.Clear();
		if (nullptr != m_pData[inspectionPos].m_pImageList)
		{
			m_imageListResetTmp = *m_pData[inspectionPos].m_pImageList;
		}
	}

	std::string ImageIdBytes;
	SvPb::SetGuidInProtoBytes(&ImageIdBytes, rImageId);
	auto* pList = m_imageListResetTmp.mutable_childlist();
	//check if image with this GUID already in list (this is not allowed.)
	auto imageIter = std::find_if(pList->begin(), pList->end(), [&ImageIdBytes](auto data)->bool
	{
		return (0 == data.imageid().compare(ImageIdBytes));
	});

	int imagePos = -1;
	SvPb::ChildImageDefinition* pImageDefinition = nullptr;
	if (pList->end() != imageIter)
	{
		pImageDefinition = &(*imageIter);
		imagePos = static_cast<int> (std::distance(pList->begin(), imageIter));
	}
	else
	{
		pImageDefinition = m_imageListResetTmp.add_childlist();
		imagePos = pList->size() - 1;
	}
	pImageDefinition->set_imageid(ImageIdBytes);
	SvPb::SetGuidInProtoBytes(pImageDefinition->mutable_parentimageid(), rParentId);
	std::string typeStr(reinterpret_cast<const char*>(&rBufferStruct), sizeof(rBufferStruct));
	pImageDefinition->set_type(typeStr);

	if (ResetEnum::NewReset == resetEnum)
	{
		m_pData[inspectionPos].m_pImageList = new SvPb::ImageList(m_imageListResetTmp);
	}
	return imagePos;
}

bool TriggerRecordController::addImageBuffer(const GUID& ownerID, const SVMatroxBufferCreateStruct& bufferStruct, int numberOfBuffers)
{
	if (-1 == m_resetStarted4IP || m_isResetLocked)
	{   //addImageBuffer is not allowed if reset is not started.
		assert(false);
		return false;
	}
	auto& bufferMap = m_additionalBufferMap[ownerID];

	std::string typeString(reinterpret_cast<const char*>(&bufferStruct), sizeof(bufferStruct));

	auto* pList = m_imageStructListResetTmp.mutable_list();
	auto imageIter = std::find_if(pList->begin(), pList->end(), [typeString](auto data)->bool
	{
		return (0 == data.type().compare(typeString));
	});
	if (pList->end() != imageIter)
	{
		auto iter = bufferMap.find(imageIter->structid());
		if (bufferMap.end() != iter)
		{
			int number = std::max(imageIter->numberofbuffersrequired() - iter->second + numberOfBuffers, 0);
			iter->second = numberOfBuffers;
			imageIter->set_numberofbuffersrequired(number);
		}
		else
		{
			imageIter->set_numberofbuffersrequired(imageIter->numberofbuffersrequired() + numberOfBuffers);
			bufferMap[imageIter->structid()] = numberOfBuffers;
		}
	}
	else
	{
		SvPb::ImageStructData* pImageStructData = m_imageStructListResetTmp.add_list();
		std::string typeStr(reinterpret_cast<const char*>(&bufferStruct), sizeof(bufferStruct));
		pImageStructData->set_type(typeStr);
		pImageStructData->set_numberofbuffers(0);
		pImageStructData->set_numberofbuffersrequired(numberOfBuffers);
		pImageStructData->set_structid(m_imageStructListResetTmp.list_size() - 1);
		bufferMap[pImageStructData->structid()] = numberOfBuffers;
	}
	return true;
}

bool TriggerRecordController::removeImageBuffer(const GUID& ownerID, const SVMatroxBufferCreateStruct& bufferStruct)
{
	if (-1 == m_resetStarted4IP || m_isResetLocked)
	{   //removeImageBuffer is not allowed if reset is not started.
		assert(false);
		return false;
	}
	auto& bufferMapIter = m_additionalBufferMap.find(SVGUID(ownerID));
	if (m_additionalBufferMap.end() != bufferMapIter)
	{
		std::string typeString(reinterpret_cast<const char*>(&bufferStruct), sizeof(bufferStruct));

		auto* pList = m_imageStructListResetTmp.mutable_list();
		auto imageIter = std::find_if(pList->begin(), pList->end(), [typeString](auto data)->bool
		{
			return (0 == data.type().compare(typeString));
		});
		if (pList->end() != imageIter)
		{
			auto iter = bufferMapIter->second.find(imageIter->structid());
			if (bufferMapIter->second.end() != iter)
			{
				int number = std::max(imageIter->numberofbuffersrequired() - iter->second, 0);
				imageIter->set_numberofbuffersrequired(number);
				bufferMapIter->second.erase(iter);
				if (0 >= bufferMapIter->second.size())
				{
					m_additionalBufferMap.erase(bufferMapIter);
				}
				return true;
			}
		}
	}
	return false;
}

bool TriggerRecordController::removeAllImageBuffer(const GUID& ownerID)
{
	if (-1 == m_resetStarted4IP || m_isResetLocked)
	{   //removeAllImageBuffer is not allowed if reset is not started.
		assert(false);
		return false;
	}

	auto& bufferMapIter = m_additionalBufferMap.find(SVGUID(ownerID));
	if (m_additionalBufferMap.end() != bufferMapIter)
	{
		auto* pList = m_imageStructListResetTmp.mutable_list();
		for (auto& structNumberPair : bufferMapIter->second)
		{
			auto imageIter = std::find_if(pList->begin(), pList->end(), [structNumberPair](auto data)->bool
			{
				return (structNumberPair.first == data.structid());
			});
			if (pList->end() != imageIter)
			{
				int number = std::max(imageIter->numberofbuffersrequired() - structNumberPair.second, 0);
				imageIter->set_numberofbuffersrequired(number);
			}
		}
		m_additionalBufferMap.erase(bufferMapIter);
	}
	return true;
}

bool TriggerRecordController::lockReset()
{
	if (-1 == m_resetStarted4IP)
	{
		m_isResetLocked = true;
	}

	return m_isResetLocked;
}
#pragma endregion Public Methods

#pragma region Private Methods
TriggerRecord::TriggerRecordData& TriggerRecordController::getTRData(int inspectionPos, int pos) const
{
	//do not check of the input parameter, because this is are private method and the parameter will be checked before in the calling method.
	assert(0 <= pos && 0 <= inspectionPos && m_IPDataNumber > inspectionPos && m_pData[inspectionPos].m_TriggerRecordNumber > pos && nullptr != m_pData[inspectionPos].m_pTriggerRecords);

	void* tmp = static_cast<char*>(m_pData[inspectionPos].m_pTriggerRecords) + (pos*m_pData[inspectionPos].m_triggerRecordBufferSize);
	TriggerRecord::TriggerRecordData* tr = reinterpret_cast<TriggerRecord::TriggerRecordData*>(tmp);
	return *tr;
}

void TriggerRecordController::ResetInspectionData(int inspectionID)
{
	if (0 <= inspectionID && m_IPDataNumber > inspectionID)
	{
		m_pData[inspectionID].m_lastFinishedTRID = -1;
		m_pData[inspectionID].m_lastStartedTRID = -1;
		if (m_pData[inspectionID].m_bInit)
		{
			free(m_pData[inspectionID].m_pTriggerRecords);
			m_pData[inspectionID].m_pTriggerRecords = nullptr;
			m_pData[inspectionID].m_triggerRecordBufferSize = 0;
			delete m_pData[inspectionID].m_pImageList;
			m_pData[inspectionID].m_pImageList = nullptr;
		}
		m_pData[inspectionID].m_bInit = false;
		m_pData[m_resetStarted4IP].m_nextPosForFreeCheck = 0;
	}
}

void TriggerRecordController::ResetTriggerRecordStructure()
{
	m_resetId = std::max(1l, m_resetId + 1);
	while (0 < m_resetLockCounter) {}; //wait if any other method have left the access of this structure
	sendResetCall();
	for (int i = 0; i < m_IPDataNumber; i++)
	{
		if (i == m_resetStarted4IP)
		{
			ResetInspectionData(i);
			m_pData[i].m_TriggerRecordNumber = m_TriggerRecordNumberResetTmp;
			m_pData[i].m_triggerRecordBufferSize = (sizeof(TriggerRecord::TriggerRecordData) + sizeof(int)*m_imageListResetTmp.list_size());
			m_pData[i].m_pImageList = new SvPb::ImageList(m_imageListResetTmp);
			m_pData[i].m_pTriggerRecords = malloc(m_pData[i].m_triggerRecordBufferSize*m_pData[i].m_TriggerRecordNumber);
		}
		if (nullptr != m_pData[i].m_pTriggerRecords && 0 < m_pData[i].m_triggerRecordBufferSize)
		{
			memset(m_pData[i].m_pTriggerRecords, -1, m_pData[i].m_triggerRecordBufferSize*m_pData[i].m_TriggerRecordNumber);
			for (int j = 0; j < m_pData[i].m_TriggerRecordNumber; j++)
			{	//initialize buffer
				getTRData(i, j).init();
			}
		}
		else
		{
			free(m_pData[i].m_pTriggerRecords);
			m_pData[i].m_pTriggerRecords = nullptr;
			m_pData[i].m_bInit = false;
		}
	}

	//update structId to fit to the position in m_imageStructList
	try
	{
		std::vector<std::pair<int, int>> changeVect = ImageBufferController::getImageBufferControllerInstance().reset(m_imageStructListResetTmp);
		for (const auto& rChangePair : changeVect)
		{
			//update per Inspection
			for (int i = 0; i < m_IPDataNumber; i++)
			{
				auto* pImageList = m_pData[i].m_pImageList;
				if (nullptr != pImageList)
				{
					for (auto& rImageData : (*pImageList->mutable_list()))
					{
						if (rImageData.structid() == rChangePair.first)
						{
							rImageData.set_structid(rChangePair.second);
						}
					}
				}
			}

			//update additional buffers
			for (auto& rMap : m_additionalBufferMap)
			{
				auto iter = rMap.second.find(rChangePair.first);
				if (rMap.second.end() != iter)
				{
					rMap.second[rChangePair.second] = iter->second;
					rMap.second.erase(iter);
				}
			}
		}


	}
	catch (const SvStl::MessageContainer& rSvE)
	{
		m_resetStarted4IP = -1;
		//This is the topmost catch for MessageContainer exceptions
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		//SvStl::MessageData Msg(rExp.getMessage());
		//Msg.m_MessageCode = SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED;
		Exception.setMessage(rSvE.getMessage());
		Exception.Throw();
	}

	if (0 <= m_resetStarted4IP)
	{
		m_pData[m_resetStarted4IP].m_bInit = true;
	}

	m_resetStarted4IP = -1;
}

void TriggerRecordController::setStructData(SvPb::ImageStructData* pStructData, SvPb::ImageDefinition* pImageDefinition, const std::string& typeStr)
{
	if (nullptr != pStructData)
	{   //use existing structData-Entry
		pStructData->set_numberofbuffersrequired(pStructData->numberofbuffersrequired() + m_TriggerRecordNumberResetTmp);
		pImageDefinition->set_type(pStructData->type());
		pImageDefinition->set_structid(pStructData->structid());
	}
	else
	{	//add new structData-Entry
		SvPb::ImageStructData* pImageStructData = m_imageStructListResetTmp.add_list();
		pImageStructData->set_type(typeStr);
		pImageStructData->set_numberofbuffers(0);
		pImageStructData->set_numberofbuffersrequired(m_TriggerRecordNumberResetTmp);
		pImageStructData->set_structid(m_imageStructListResetTmp.list_size() - 1);
		pImageDefinition->set_type(pImageStructData->type());
		pImageDefinition->set_structid(pImageStructData->structid());
	}
}

TriggerRecordController::ResetEnum TriggerRecordController::calcResetEnum(int inspectionPos)
{
	if (!m_isResetLocked)
	{
		if ((0 <= m_resetStarted4IP) && (-1 == inspectionPos || m_resetStarted4IP == inspectionPos))
		{
			return ResetEnum::ResetRunning;
		}
		if ((-1 == m_resetStarted4IP) && (0 <= inspectionPos && m_inspectionList.list_size() > inspectionPos))
		{
			return ResetEnum::NewReset;
		}
	}

	return ResetEnum::Invalid;
}

void TriggerRecordController::sendResetCall()
{
	for (auto& callbackStruct : m_ResetCallbacks)
	{
		if (nullptr != callbackStruct.m_pCallback)
		{
			callbackStruct.m_pCallback(callbackStruct.m_pOwner);
		}
	}
}
#pragma endregion Private Methods

ITriggerRecordControllerRW& getTriggerRecordControllerRWInstance()
{
	return TriggerRecordController::getTriggerRecordControllerInstance();
}

ITriggerRecordControllerR& getTriggerRecordControllerRInstance()
{
	return TriggerRecordController::getTriggerRecordControllerInstance();
}

int getInspectionPos(const GUID& inspectionGuid)
{
	int Return = -1;
	std::string inspectionIdBytes;
	SvPb::SetGuidInProtoBytes(&inspectionIdBytes, inspectionGuid);
	auto& rInspectionList = TriggerRecordController::getTriggerRecordControllerInstance().getInspections();
	const auto& Iter = std::find_if(rInspectionList.list().begin(), rInspectionList.list().end(), [inspectionIdBytes](const auto& item)-> bool
	{
		return (0 == item.id().compare(inspectionIdBytes));
	});
	if (rInspectionList.list().end() != Iter)
	{
		Return = static_cast<int>(std::distance(rInspectionList.list().begin(), Iter));
	}

	return Return;
}
} //namespace SvTrc
