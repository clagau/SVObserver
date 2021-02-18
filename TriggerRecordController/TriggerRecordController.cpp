//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file TriggerRecordController.cpp
/// All Rights Reserved
//*****************************************************************************
/// The TriggerRecordController: control the triggerRecords.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <WinBase.h>
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVLibrary\SVOINIClass.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateChildStruct.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\GlobalPath.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVUtilityLibrary\StringHelper.h"
#include "DataControllerLocal.h"
#include "DataControllerWriter.h"
#include "DataControllerReader.h"
#include "ImageBufferController.h"
#include "Image.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#include "ObjectInterfaces/ITriggerRecordRW.h"
#include "TriggerRecordController.h"
#include "TriggerRecordData.h"
#include "TriggerRecord.h"
#include "ResetLocker.h"
#pragma endregion Includes
//#define TRACE_TRC

namespace SvTrc
{
int getAdditionalTriggerRecordNumber()
{
	SvLib::SVOINIClass l_SvimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());
	constexpr int cTriggerRecordAddOn = 2; //number of additional slots for internal use
	constexpr int cTriggerRecordMax = 50 + cTriggerRecordAddOn;
	int retVal = l_SvimIni.GetValueInt(_T("TriggerRecordController"), _T("AdditionalTRNumber"), 5) + cTriggerRecordAddOn;
	return std::max(cTriggerRecordAddOn, std::min(cTriggerRecordMax, retVal));
}
static const int c_additionalTriggerRecordNumber = getAdditionalTriggerRecordNumber();

#pragma region Constructor
TriggerRecordController::TriggerRecordController(std::unique_ptr<DataControllerBase> pDataController) 
	: m_pDataController(std::move(pDataController))
	, m_imageBufferController(*m_pDataController)
{
	m_pDataController->setResetCallback(std::bind(&TriggerRecordController::sendResetCall, this));
	m_pDataController->setReadyCallback(std::bind(&TriggerRecordController::sendReadyCall, this));
	m_pDataController->setNewTrIdCallback(std::bind(&TriggerRecordController::sendTrIdCall, this, std::placeholders::_1));	
	m_pDataController->setNewInterestTrIdsCallback(std::bind(&TriggerRecordController::sendInterestTrIdCall, this, std::placeholders::_1));
}

TriggerRecordController::~TriggerRecordController()
{
	m_isResetLocked = false;
	TriggerRecordController::clearAll();
}
#pragma endregion Constructor

#pragma region Public Methods
void TriggerRecordController::setLastFinishedTr(SvOi::TrEventData data)
{ 
	m_pDataController->setLastFinishedTr(data); 
	sendTrIdCall(data);
}


const SvPb::ImageList& TriggerRecordController::getImageDefList(int inspectionPos)
{
	if (-1 == m_resetStarted4IP)
	{
		return m_pDataController->getImageDefList(inspectionPos);
	}

	assert(false);
	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetImageDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

const std::unordered_map<uint32_t, int>& TriggerRecordController::getImageDefMap(int inspectionPos)
{
	if (-1 == m_resetStarted4IP)
	{
		return m_pDataController->getImageMap(inspectionPos);
	}

	assert(false);
	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetImageDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

const std::unordered_map<uint32_t, int>& TriggerRecordController::getChildImageDefMap(int inspectionPos)
{
	if (-1 == m_resetStarted4IP)
	{
		return m_pDataController->getChildImageMap(inspectionPos);
	}

	assert(false);
	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetImageDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

const SvPb::DataDefinitionList& TriggerRecordController::getDataDefList(int inspectionPos)
{
	if (-1 == m_resetStarted4IP)
	{
		return m_pDataController->getDataDefList(inspectionPos);
	}

	assert(false);
	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetDataDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

const std::unordered_map<uint32_t, int>& TriggerRecordController::getDataDefMap(int inspectionPos)
{
	if (-1 == m_resetStarted4IP)
	{
		return m_pDataController->getDataDefMap(inspectionPos);
	}

	assert(false);
	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetDataDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

SvOi::ITriggerRecordRPtr TriggerRecordController::createTriggerRecordObject(int inspectionPos, int trId)
{
	assert(-1 == m_resetStarted4IP);
	if (-1 == m_resetStarted4IP && 0 <= trId)
	{
		return m_pDataController->createTriggerRecordObject(inspectionPos, [trId](TriggerRecordData& rData) -> bool { return rData.m_trId == trId; });
	}
	return nullptr;
}

SvOi::ITriggerRecordRPtr TriggerRecordController::createTriggerRecordObjectPerTriggerCount(int inspectionPos, int triggerCount)
{
	assert(-1 == m_resetStarted4IP);
	if (-1 == m_resetStarted4IP && 0 <= triggerCount)
	{
		return m_pDataController->createTriggerRecordObject(inspectionPos, [triggerCount](TriggerRecordData& rData) -> bool { return rData.m_triggerData.m_TriggerCount == triggerCount; });
	}
	return nullptr;
}

int TriggerRecordController::registerResetCallback(std::function<void()> pCallback)
{
	assert(pCallback);
	if (nullptr != pCallback)
	{
		static int handleCounter = 0;
		std::lock_guard<std::mutex> guard(m_callbackMutex);
		int handle = handleCounter++;
		m_resetCallbacks.push_back(std::pair<int,std::function<void()>>(handle,pCallback));
		return handle;
	}
	return -1;
}
void TriggerRecordController::unregisterResetCallback(int handleId)
{
	std::lock_guard<std::mutex> guard(m_callbackMutex);
	auto Iter = find_if(m_resetCallbacks.begin(), m_resetCallbacks.end(), [handleId](const auto& rEntry)->bool { return rEntry.first == handleId; });
	if (m_resetCallbacks.end() != Iter)
	{
		m_resetCallbacks.erase(Iter);
	}
}

int TriggerRecordController::registerReadyCallback(std::function<void()> pCallback)
{
	assert(pCallback);
	if (nullptr != pCallback)
	{
		static int handleCounter = 0;
		std::lock_guard<std::mutex> guard(m_callbackMutex);
		int handle = handleCounter++;
		m_readyCallbacks.push_back(std::pair<int, std::function<void()>>(handle, pCallback));
		return handle;
	}
	return -1;
}
void TriggerRecordController::unregisterReadyCallback(int handleId)
{
	std::lock_guard<std::mutex> guard(m_callbackMutex);
	auto Iter = find_if(m_readyCallbacks.begin(), m_readyCallbacks.end(), [handleId](const auto& rEntry)->bool { return rEntry.first == handleId; });
	if (m_readyCallbacks.end() != Iter)
	{
		m_readyCallbacks.erase(Iter);
	}
}

int TriggerRecordController::registerNewTrCallback(std::function<void(SvOi::TrEventData)> pCallback)
{
	assert(pCallback);
	if (nullptr != pCallback)
	{
		static int handleCounter = 0;
		std::lock_guard<std::mutex> guard(m_callbackMutex);
		int handle = handleCounter++;
		 m_newTrCallbacks.push_back(std::pair<int, std::function<void(SvOi::TrEventData)>>(handle, pCallback));
		return handle;
	}
	return -1;
}

void TriggerRecordController::unregisterNewTrCallback(int handleId)
{
	std::lock_guard<std::mutex> guard(m_callbackMutex);
	auto Iter = find_if(m_newTrCallbacks.begin(), m_newTrCallbacks.end(), [handleId](const auto& rEntry)->bool { return rEntry.first == handleId; });
	if (m_newTrCallbacks.end() != Iter)
	{
		m_newTrCallbacks.erase(Iter);
	}
}

int TriggerRecordController::registerNewInterestTrCallback(std::function<void(const std::vector<SvOi::TrInterestEventData>&)> pCallback)
{
	assert(pCallback);
	if (nullptr != pCallback)
	{
		static int handleCounter = 0;
		std::lock_guard<std::mutex> guard(m_callbackMutex);
		int handle = handleCounter++;
		m_newInterestTrCallbacks.push_back(std::pair<int, std::function<void(const std::vector<SvOi::TrInterestEventData>&)>>(handle, pCallback));
		return handle;
	}
	return -1;
}

void TriggerRecordController::unregisterNewInterestTrCallback(int handleId)
{
	std::lock_guard<std::mutex> guard(m_callbackMutex);
	auto Iter = find_if(m_newInterestTrCallbacks.begin(), m_newInterestTrCallbacks.end(), [handleId](const auto& rEntry)->bool { return rEntry.first == handleId; });
	if (m_newInterestTrCallbacks.end() != Iter)
	{
		m_newInterestTrCallbacks.erase(Iter);
	}
}

bool TriggerRecordController::setTrsOfInterest(const std::vector<SvOi::ITriggerRecordRPtr>& trVector, bool isInterest)
{
	bool retValue = false;
	auto pLock = ResetLocker::lockReset(m_pDataController->getResetId());
	if (nullptr != pLock)
	{
		std::vector<DataControllerBase::InterestStruct> trValueVec;
		std::vector<SvOi::TrInterestEventData> trEventVec;
		for (auto tr : trVector)
		{
			auto* pTr = dynamic_cast<TriggerRecord*>(tr.get());
			if (nullptr != pTr && pTr->isObjectUpToTime())
			{
				trValueVec.emplace_back(pTr->getInspectionPos(), pTr->getTrPos(), isInterest);
				trEventVec.emplace_back(pTr->getInspectionPos(), pTr->getId(), isInterest);
			}
		}
		retValue = m_pDataController->setTrOfInterest(trValueVec);
		pLock.reset();
		if (!retValue && isInterest)
		{
			//if isInterest-Flag set, but retValue is false (possible reason is that interest is paused) and set value to false.
			for (auto& rEvent : trEventVec)
			{
				rEvent.m_isInterest = false;
			}
		}
		sendInterestTrIdCall(std::move(trEventVec));
	}
	return retValue;
}

std::vector<SvOi::ITriggerRecordRPtr> TriggerRecordController::getTrsOfInterest(int inspectionPos, int n)
{
	auto pLock = ResetLocker::lockReset(m_pDataController->getResetId());
	if (nullptr != pLock)
	{
		return m_pDataController->getTRsOfInterest(inspectionPos, n);
	}
	else
	{
		return {};
	}
}

void TriggerRecordController::pauseTrsOfInterest(bool pauseFlag, int inspectionPos)
{
	m_pDataController->setPauseTrsOfInterest(pauseFlag, inspectionPos);
}

void TriggerRecordController::clearAll()
{
	m_pDataController->prepareReset();

	m_additionalBufferMap.clear();
	m_resetStarted4IP = -1;
	m_imageStructListResetTmp.Clear();
	m_imageListResetTmp.Clear();
	m_TriggerRecordNumberResetTmp = 0;
	{
		std::lock_guard<std::mutex> guard(m_callbackMutex);
		m_resetCallbacks.clear();
		m_readyCallbacks.clear();
		m_newTrCallbacks.clear();
		m_newInterestTrCallbacks.clear();
	}
	m_isResetLocked = false;

	m_pDataController->clearAll();
}

bool TriggerRecordController::setInspections(SvPb::InspectionList&& rInspectionList)
{
	if (m_isResetLocked)
	{
		return false;
	}

	bool result = m_pDataController->setInspections(std::move(rInspectionList));

	return result;
}

void TriggerRecordController::resizeIPNumberOfRecords(const std::vector<int>& inspectionPosVec, long newSizeTr, long newSizeTrOfIntereset)
{
	if (m_isResetLocked)
	{
		assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetLocked, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (0 >= newSizeTr || cMaxTriggerRecords < newSizeTr)
	{
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), newSizeTr));
		msgList.push_back(SvUl::Format(_T("%d"), cMaxTriggerRecords));
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TriggerRecordSize2Big, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (0 > newSizeTrOfIntereset || cMaxTriggerRecordsOfInterest < newSizeTrOfIntereset)
	{
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), newSizeTrOfIntereset));
		msgList.push_back(SvUl::Format(_T("%d"), cMaxTriggerRecordsOfInterest));
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TriggerRecordOfInterestSize2Big, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	bool mustRecalc = false;
	bool mustSetIPList = false;
	SvPb::InspectionList inspectionList = m_pDataController->getInspections();
	for (int inspectionPos : inspectionPosVec)
	{
		if (0 > inspectionPos || inspectionList.list_size() <= inspectionPos)
		{   //new start of reset is not allowed if reset is in progress.
			assert(false);
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), inspectionPos));
			msgList.push_back(SvUl::Format(_T("%d"), inspectionList.list_size()));
			SvStl::MessageManager Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetWrongInspectionId, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}

		auto* pIpData = inspectionList.mutable_list(inspectionPos);
		if (nullptr == pIpData)
		{
			assert(false);
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), inspectionPos));
			msgList.push_back(SvUl::Format(_T("%d"), inspectionList.list_size()));
			SvStl::MessageManager Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetWrongInspectionId, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}

		if (pIpData->numberofrecords() == newSizeTr && pIpData->numberrecordsofinterest() == newSizeTrOfIntereset)
		{
			continue;
		}

		m_pDataController->prepareReset();

		pIpData->set_numberofrecords(newSizeTr);
		pIpData->set_numberrecordsofinterest(newSizeTrOfIntereset);

		mustSetIPList = true;

		try
		{
			auto imageListTmp = m_pDataController->getImageDefList(inspectionPos, false);
			if (0 < imageListTmp.list_size())
			{
				mustRecalc = true;
				m_pDataController->setIpToInitFlagList(inspectionPos);
				m_pDataController->createTriggerRecordsBuffer(inspectionPos, needNumberOfTr(*pIpData));
			}
		}
		catch (...)
		{
			//nothing to do, only if imageDefList is valid and size > 0 do something.
		}
	}

	if (mustSetIPList)
	{
		m_pDataController->setInspectionList(std::move(inspectionList));
	}

	if (mustRecalc)
	{
		m_imageStructListResetTmp = m_pDataController->getImageStructList();
		m_mustRecalcRequiredBuffers = true;
		ResetTriggerRecordStructure();
	}
}

SvOi::ITriggerRecordRWPtr TriggerRecordController::createTriggerRecordObjectToWrite(int inspectionPos)
{
	assert(-1 == m_resetStarted4IP);

	if (-1 == m_resetStarted4IP)
	{
		SvOi::ITriggerRecordRWPtr pTriggerRecord = m_pDataController->createTriggerRecordObjectToWrite(inspectionPos);
		if(nullptr != pTriggerRecord)
		{
			pTriggerRecord->initValueData();
		}
		return pTriggerRecord;
	}
	else
	{
		return nullptr;
	}
};

SvOi::ITriggerRecordRPtr TriggerRecordController::closeWriteAndOpenReadTriggerRecordObject(SvOi::ITriggerRecordRWPtr& pTriggerRecord)
{
	if (nullptr != pTriggerRecord)
	{
		int id = pTriggerRecord->getId();
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		std::string DebugString = SvUl::Format(_T("closeWriteAndOpenReadTriggerRecordObject; %d\n"), id);
		::OutputDebugString(DebugString.c_str());
		static int last_triggerId = id - 1;
		if (last_triggerId + 1 != id)
		{
			DebugString = SvUl::Format(_T("\n\nsome trigger missing\n\n"));
			::OutputDebugString(DebugString.c_str());
		}
		last_triggerId = id;
#endif
		int inspectionPos = pTriggerRecord->getInspectionPos();
		pTriggerRecord = nullptr;
		return createTriggerRecordObject(inspectionPos, id);
	}
	return nullptr;
}

void TriggerRecordController::closeWriteObjectWithoutUpdateLastTrId(SvOi::ITriggerRecordRWPtr& pTriggerRecord)
{
	TriggerRecord* pTR = dynamic_cast<TriggerRecord*>(pTriggerRecord.get());
	if (nullptr != pTR)
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		int id = pTriggerRecord->getId();
		std::string DebugString = SvUl::Format(_T("closeWriteObjectWithoutUpdateLastTrId; %d\n"), id);
		::OutputDebugString(DebugString.c_str());
#endif
		pTR->blockUpdateLastTrId();
	}
	pTriggerRecord = nullptr;
}

SvOi::ITRCImagePtr TriggerRecordController::getImageBuffer(const SVMatroxBufferCreateStruct& rBufferStruct, bool createBufferExternIfNecessary) const
{
	SvOi::ITRCImagePtr retImage;
	{
		auto pLock = ResetLocker::lockReset(m_pDataController->getResetId());
		if (-1 == m_resetStarted4IP && nullptr != pLock)
		{
			retImage = m_imageBufferController.createNewImageHandle(rBufferStruct, m_pDataController->getResetId());
		}
	}

	if (nullptr == retImage && createBufferExternIfNecessary)
	{
		SVMatroxBuffer ImageBufId;
		SVMatroxBufferInterface::Create(ImageBufId, rBufferStruct);
		SvOi::SVImageBufferHandlePtr pImagePtr = SvOi::SVImageBufferHandlePtr {new SVImageBufferHandleImage(ImageBufId)};
		retImage = std::make_shared<Image>(pImagePtr, 0, Image::cLocalTmpImagePos, false, false);
	}
	return retImage;
}

void TriggerRecordController::startResetTriggerRecordStructure(int inspectionPos)
{
	if (m_isResetLocked)
	{
		assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetLocked, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (-1 != m_resetStarted4IP)
	{   //new start of reset is not allowed if reset is in progress.
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), m_resetStarted4IP));
		msgList.push_back(SvUl::Format(_T("%d"), inspectionPos));
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetAllReadyStarted, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}


	const auto& rInspectionList = m_pDataController->getInspections();
	if ((0 > inspectionPos && m_cResetStartedAddBuffer != inspectionPos) || rInspectionList.list_size() <= inspectionPos)
	{   //new start of reset is not allowed if reset is in progress.
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), inspectionPos));
		msgList.push_back(SvUl::Format(_T("%d"), rInspectionList.list_size()));
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetWrongInspectionId, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (m_cResetStartedAddBuffer != inspectionPos)
	{
		const auto ipData = rInspectionList.list(inspectionPos);
		int TriggerRecordSize = ipData.numberofrecords();
		if (0 >= TriggerRecordSize || cMaxTriggerRecords < TriggerRecordSize)
		{
			assert(false);
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), TriggerRecordSize));
			msgList.push_back(SvUl::Format(_T("%d"), cMaxTriggerRecords));
			SvStl::MessageManager Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TriggerRecordSize2Big, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}

		int TriggerRecordOfInterestSize = ipData.numberrecordsofinterest();
		if (0 > TriggerRecordOfInterestSize || cMaxTriggerRecordsOfInterest < TriggerRecordOfInterestSize)
		{
			assert(false);
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), TriggerRecordOfInterestSize));
			msgList.push_back(SvUl::Format(_T("%d"), cMaxTriggerRecordsOfInterest));
			SvStl::MessageManager Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TriggerRecordOfInterestSize2Big, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}

		m_resetStarted4IP = inspectionPos;

		//clear imageList and reset ImageStructList
		int TrNumber = 0;
		try
		{
			m_imageListResetTmp = m_pDataController->getImageDefList(m_resetStarted4IP, false);
		}
		catch (const SvStl::MessageContainer&)
		{
			//nothing to do, because if 
			m_imageListResetTmp.Clear();
		}
		TrNumber = m_pDataController->getTriggerRecordNumber(m_resetStarted4IP);
		m_imageStructListResetTmp.Clear();
		m_imageStructListResetTmp = m_pDataController->getImageStructList();
		//remove number of buffers from required list from old reserved images of this inspection.
		for (const auto& rSizeData : m_imageListResetTmp.list())
		{
			int id = rSizeData.structid();
			if (0 <= id && m_imageStructListResetTmp.list_size() > id)
			{
				auto* pImageStruct = m_imageStructListResetTmp.mutable_list(id);
				if (nullptr != pImageStruct)
				{
					pImageStruct->set_numberofbuffersrequired(pImageStruct->numberofbuffersrequired() - TrNumber);
				}
			}
		}

		m_imageListResetTmp.Clear();

		m_TriggerRecordNumberResetTmp = needNumberOfTr(ipData);
	}
	else
	{
		m_resetStarted4IP = inspectionPos;
		m_imageStructListResetTmp = m_pDataController->getImageStructList();
		m_imageListResetTmp.Clear();
	}
}

void TriggerRecordController::finishResetTriggerRecordStructure()
{
	if (-1 == m_resetStarted4IP)
	{   //stop of reset is not allowed if reset is not started.
		assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetNotStart, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	ResetTriggerRecordStructure();
}

void TriggerRecordController::setGlobalInit()
{
	if (m_isResetLocked)
	{
		assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetLocked, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (-1 != m_resetStarted4IP)
	{   //new start of reset is not allowed if reset is in progress.
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), m_resetStarted4IP));
		msgList.push_back(SvUl::Format(_T("%d"), -1));
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetAllReadyStarted, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	m_pDataController->prepareReset();
	m_pDataController->setGlobalInitFlag(true);
	m_isGlobalInit = true;
}

void TriggerRecordController::finishGlobalInit()
{
	if (m_isResetLocked)
	{
		assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetLocked, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (-1 != m_resetStarted4IP)
	{   //new start of reset is not allowed if reset is in progress.
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), m_resetStarted4IP));
		msgList.push_back(SvUl::Format(_T("%d"), -1));
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetAllReadyStarted, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (!m_isGlobalInit)
	{
		assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GlobalInitNotStarted, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	//ATTENTION: Order of the next line is important, because if getImageStructList is called m_isGlobalInit must be true to get the m_imageStructListGlobalInitTmp
	m_imageStructListResetTmp = m_pDataController->getImageStructList();
	m_isGlobalInit = false;
	m_pDataController->setGlobalInitFlag(false);
	m_mustRecalcRequiredBuffers = true;
	ResetTriggerRecordStructure();
}

int TriggerRecordController::addOrChangeImage(uint32_t imageId, const SVMatroxBufferCreateStruct& rBufferStruct, int inspectionPos /*= -1*/)
{
	ResetEnum resetEnum = calcResetEnum(inspectionPos);
	if (ResetEnum::Invalid == resetEnum)
	{   //Not possible to add and change image.
		assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (ResetEnum::NewReset == resetEnum)
	{
		const auto ipData = m_pDataController->getInspections().list(inspectionPos);
		int TriggerRecordSize = ipData.numberofrecords();
		if (0 >= TriggerRecordSize || cMaxTriggerRecords < TriggerRecordSize)
		{
			assert(false);
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), TriggerRecordSize));
			msgList.push_back(SvUl::Format(_T("%d"), cMaxTriggerRecords));
			SvStl::MessageManager Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TriggerRecordSize2Big, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}


		//prefer reset
		m_resetStarted4IP = inspectionPos;
		m_TriggerRecordNumberResetTmp = needNumberOfTr(ipData);
		try
		{
			m_imageListResetTmp = m_pDataController->getImageDefList(m_resetStarted4IP, false);
		}
		catch (const SvStl::MessageContainer&)
		{
			//nothing to do, because if 
			m_imageListResetTmp.Clear();
		}
		m_imageStructListResetTmp.Clear();
		m_imageStructListResetTmp = m_pDataController->getImageStructList();
	}

	//check if rBufferStruct already in sizeList
	std::string typeStr(reinterpret_cast<const char*>(&rBufferStruct), sizeof(rBufferStruct));
	auto pImageStructList = m_imageStructListResetTmp.mutable_list();
	auto bufferStructIter = std::find_if(pImageStructList->begin(), pImageStructList->end(), [typeStr](const auto& rData)->bool
	{
		return (0 == rData.type().compare(typeStr));
	});
	SvPb::ImageStructData* pStructData = nullptr;
	if (pImageStructList->end() != bufferStructIter)
	{
		pStructData = &(*bufferStructIter);
	}

	auto* pList = m_imageListResetTmp.mutable_list();
	//check if image with this ID already in list
	auto pImageIter = std::find_if(pList->begin(), pList->end(), [&imageId](const auto& rData)->bool
	{
		return (rData.objectid() == imageId);
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
		pImageDefinition->set_objectid(imageId);
	}

	setStructData(pStructData, pImageDefinition, typeStr);
	if (ResetEnum::NewReset == resetEnum)
	{
		ResetTriggerRecordStructure();
	}
	return imagePos;
}

int TriggerRecordController::addOrChangeChildImage(uint32_t imageId, uint32_t parentId, const MatroxBufferChildDataStruct& rBufferStruct, int inspectionPos /* = -1*/)
{
	ResetEnum resetEnum = calcResetEnum(inspectionPos);
	if (ResetEnum::Invalid == resetEnum)
	{   //Not possible to add and change image.
		assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (ResetEnum::NewReset == resetEnum)
	{
		try
		{
			m_imageListResetTmp = m_pDataController->getImageDefList(inspectionPos, false);
		}
		catch (const SvStl::MessageContainer&)
		{
			//nothing to do, because if 
			m_imageListResetTmp.Clear();
		}
	}

	auto* pList = m_imageListResetTmp.mutable_childlist();
	//check if image with this ID already in list (this is not allowed.)
	auto imageIter = std::find_if(pList->begin(), pList->end(), [&imageId](const auto& rData)->bool
	{
		return (rData.objectid() == imageId);
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
	pImageDefinition->set_objectid(imageId);
	pImageDefinition->set_parentimageid(parentId);
	std::string typeStr(reinterpret_cast<const char*>(&rBufferStruct), sizeof(rBufferStruct));
	pImageDefinition->set_type(typeStr);

	if (ResetEnum::NewReset == resetEnum)
	{
		m_pDataController->setImageDefList(inspectionPos, std::move(m_imageListResetTmp));
	}
	return imagePos;
}

void TriggerRecordController::addImageBuffer(uint32_t ownerID, const SVMatroxBufferCreateStruct& bufferStruct, int numberOfBuffers, bool clearBuffer)
{
	bool isNewReset = -1 == m_resetStarted4IP;

	auto& bufferMap = m_additionalBufferMap[ownerID];
	std::string typeString(reinterpret_cast<const char*>(&bufferStruct), sizeof(bufferStruct));
	if (isNewReset)
	{ //check if value have to be changed
		if (1 == bufferMap.size() || !clearBuffer)
		{
			auto& rImageStruct = m_pDataController->getImageStructList();
			auto imageIter = std::find_if(rImageStruct.list().begin(), rImageStruct.list().end(), [typeString](const auto& rData)->bool
			{
				return (0 == rData.type().compare(typeString));
			});
			if (rImageStruct.list().end() != imageIter)
			{
				auto iter = bufferMap.find(imageIter->structid());
				if (bufferMap.end() != iter && iter->second == numberOfBuffers)
				{
					return; //nothing have to do, because value are the same.
				}
			}
		}
	}

	if (m_isResetLocked)
	{   //addImageBuffer is not allowed if reset is not started.
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (isNewReset)
	{
		startResetTriggerRecordStructure();
	}
	
	auto* pList = m_imageStructListResetTmp.mutable_list();
	auto imageIter = std::find_if(pList->begin(), pList->end(), [typeString](const auto& rData)->bool
	{
		return (0 == rData.type().compare(typeString));
	});
	if (pList->end() != imageIter)
	{
		auto iter = bufferMap.find(imageIter->structid());
		if (bufferMap.end() != iter)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
			std::string DebugString = SvUl::Format(_T("addImageBuffer(%s): change from %d + (-%d + %d) for ImageType %d\n"), std::to_string(ownerID).c_str(), imageIter->numberofbuffersrequired(), iter->second, numberOfBuffers, imageIter->structid());
			::OutputDebugString(DebugString.c_str());
#endif
			int number = std::max(imageIter->numberofbuffersrequired() - iter->second + numberOfBuffers, numberOfBuffers);
			iter->second = numberOfBuffers;
			imageIter->set_numberofbuffersrequired(number);
		}
		else
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
			std::string DebugString = SvUl::Format(_T("addImageBuffer(%s): %d + %d for ImageType %d\n"), std::to_string(ownerID).c_str(), imageIter->numberofbuffersrequired(), numberOfBuffers, imageIter->structid());
			::OutputDebugString(DebugString.c_str());
#endif
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		std::string DebugString = SvUl::Format(_T("addImageBuffer(%s): new %d for ImageType %d\n"), std::to_string(ownerID).c_str(), numberOfBuffers, pImageStructData->structid());
		::OutputDebugString(DebugString.c_str());
#endif
	}

	if (isNewReset)
	{
		ResetTriggerRecordStructure();
	}

	return;
}

// cppcheck-suppress unusedFunction
bool TriggerRecordController::removeImageBuffer(uint32_t ownerID, const SVMatroxBufferCreateStruct& bufferStruct)
{
	if (-1 == m_resetStarted4IP || m_isResetLocked)
	{   //removeImageBuffer is not allowed if reset is not started.
		assert(false);
		return false;
	}
	auto bufferMapIter = m_additionalBufferMap.find(ownerID);
	if (m_additionalBufferMap.end() != bufferMapIter)
	{
		std::string typeString(reinterpret_cast<const char*>(&bufferStruct), sizeof(bufferStruct));

		auto* pList = m_imageStructListResetTmp.mutable_list();
		auto imageIter = std::find_if(pList->begin(), pList->end(), [typeString](const auto& rData)->bool
		{
			return (0 == rData.type().compare(typeString));
		});
		if (pList->end() != imageIter)
		{
			auto iter = bufferMapIter->second.find(imageIter->structid());
			if (bufferMapIter->second.end() != iter)
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
				std::string DebugString = SvUl::Format(_T("removeImageBuffer(%s) numbers %d -  %d for ImageType %d\n"), std::to_string(ownerID).c_str(), imageIter->numberofbuffersrequired(), iter->second, imageIter->structid());
				::OutputDebugString(DebugString.c_str());
#endif
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

bool TriggerRecordController::removeAllImageBuffer(uint32_t ownerID)
{
	if (-1 == m_resetStarted4IP || m_isResetLocked)
	{   //removeAllImageBuffer is not allowed if reset is not started.
		assert(false);
		return false;
	}

#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
	std::string DebugString = SvUl::Format(_T("removeAllImageBuffer(%s)\n"), std::to_string(ownerID).c_str());
	::OutputDebugString(DebugString.c_str());
#endif

	auto bufferMapIter = m_additionalBufferMap.find(ownerID);
	if (m_additionalBufferMap.end() != bufferMapIter)
	{
		reduceRequiredImageBuffer(bufferMapIter->second);
		m_additionalBufferMap.erase(bufferMapIter);
	}

	return true;
}

bool TriggerRecordController::removeAllImageBuffer()
{
	if (-1 == m_resetStarted4IP || m_isResetLocked)
	{   //removeAllImageBuffer is not allowed if reset is not started.
		assert(false);
		return false;
	}

	for (const auto& rBufferMap : m_additionalBufferMap)
	{
		reduceRequiredImageBuffer(rBufferMap.second);
	}
	m_additionalBufferMap.clear();

	return true;
}
void TriggerRecordController::changeDataDef(SvPb::DataDefinitionList&& rDataDefList, long valueObjectMemSize, int inspectionPos)
{
	ResetEnum resetEnum = calcResetEnum(inspectionPos);
	if (ResetEnum::Invalid == resetEnum)
	{   //Not possible to add and change image.
		assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (ResetEnum::NewReset == resetEnum)
	{
		//prefer reset
		m_resetStarted4IP = inspectionPos;
		m_TriggerRecordNumberResetTmp = m_pDataController->getTriggerRecordNumber(m_resetStarted4IP);
		m_imageListResetTmp = m_pDataController->getImageDefList(m_resetStarted4IP, false);
		m_imageStructListResetTmp.Clear();
		m_imageStructListResetTmp = m_pDataController->getImageStructList();
	}

	//Only obtain the size of the data
	m_pDataController->changeDataDef(std::move(rDataDefList), valueObjectMemSize, m_resetStarted4IP);
	
	if (ResetEnum::NewReset == resetEnum)
	{
		ResetTriggerRecordStructure();
	}
}


bool TriggerRecordController::lockReset()
{
	if (-1 == m_resetStarted4IP)
	{
		m_isResetLocked = true;
	}

	return m_isResetLocked;
}

int TriggerRecordController::needNumberOfTr(SvPb::Inspection ipData)
{
	return ipData.numberofrecords() + ipData.numberrecordsofinterest() + c_additionalTriggerRecordNumber;
}
#pragma endregion Public Methods

#pragma region Private Methods
void TriggerRecordController::recalcRequiredBuffer()
{
	//reset all to zero
	for (int i = 0; i < m_imageStructListResetTmp.list_size(); i++)
	{
		auto pStruct = m_imageStructListResetTmp.mutable_list(i);
		pStruct->set_numberofbuffersrequired(0);
	}

	//add required count for additionalBuffer
	for (const auto& rMap : m_additionalBufferMap)
	{
		for (const auto& rPair : rMap.second)
		{
			if (rPair.first < m_imageStructListResetTmp.list_size() && 0 <= rPair.first)
			{
				auto pBufferStruct = m_imageStructListResetTmp.mutable_list(rPair.first);
				pBufferStruct->set_numberofbuffersrequired(pBufferStruct->numberofbuffersrequired() + rPair.second);
			}
		}
	}

	//add required count for ips
	int ipSize = m_pDataController->getInspections().list_size();
	for (int i = 0; i < ipSize; i++)
	{
		try
		{
			const SvPb::ImageList& rImageDef = (m_resetStarted4IP != i) ? m_pDataController->getImageDefList(i, false) : m_imageListResetTmp;
			int bufferCount = (m_resetStarted4IP != i) ? needNumberOfTr(m_pDataController->getInspections().list(i)) : m_TriggerRecordNumberResetTmp;
			
			for (const auto& rImageData : rImageDef.list())
			{
				if (rImageData.structid() < m_imageStructListResetTmp.list_size() && 0 <= rImageData.structid())
				{
					auto pBufferStruct = m_imageStructListResetTmp.mutable_list(rImageData.structid());
					pBufferStruct->set_numberofbuffersrequired(pBufferStruct->numberofbuffersrequired() + bufferCount);
				}
			}
		}
		catch (...)
		{
			//nothing to do
			assert(false);
		}
	}
}

void TriggerRecordController::ResetTriggerRecordStructure()
{
	//update structId to fit to the position in m_imageStructList
	try
	{
		if (m_mustRecalcRequiredBuffers)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
			::OutputDebugString("\n\nrecalcRequiredBuffer\n");
#endif
			recalcRequiredBuffer();
			m_mustRecalcRequiredBuffers = false;
		}	

#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		::OutputDebugString("\n\nResetTriggerRecordStructure:\nAdditional Buffer:\n");
		for (auto& rMap : m_additionalBufferMap)
		{
			std::string DebugString = std::to_string(rMap.first) + "\n";
			::OutputDebugString(DebugString.c_str());
			for (auto& rPair : rMap.second)
			{
				DebugString = SvUl::Format(_T("%d/%d\n"), rPair.first, rPair.second);
				::OutputDebugString(DebugString.c_str());
			}
		}
		int ipSize = m_pDataController->getInspections().list_size();
		for (int i = 0; i < ipSize; i++)
		{
			SvPb::ImageList rImageDef = m_imageListResetTmp;
			int bufferCount = m_TriggerRecordNumberResetTmp;
			if (m_resetStarted4IP != i)
			{
				rImageDef = m_pDataController->getTRControllerData(i)->getImageList();
				bufferCount = needNumberOfTr(m_pDataController->getInspections().list(i));
			}
			std::string DebugString = SvUl::Format(_T("Inspection: %d, count: %d\n"), i, bufferCount);
			::OutputDebugString(DebugString.c_str());
			std::map<int, int> imageMap;
			for (auto imageData : rImageDef.list())
			{
				imageMap[imageData.structid()]++;
				DebugString = SvUl::Format(_T("structId: %d, objectId: %u\n"), imageData.structid(), imageData.objectid());
				::OutputDebugString(DebugString.c_str());
			}
			::OutputDebugString("Collected:\n");
			int collectNumber = 0;
			for (auto mapPair : imageMap)
			{
				DebugString = SvUl::Format(_T("structId: %d, count: %d\n"), mapPair.first, mapPair.second);
				::OutputDebugString(DebugString.c_str());
				collectNumber += mapPair.second;
			}
			DebugString = SvUl::Format(_T("Complette-count: %d\n"), collectNumber);
			::OutputDebugString(DebugString.c_str());
		}
#endif


		std::vector<std::pair<int, int>> changeVect = m_pDataController->ResetTriggerRecordStructure(m_resetStarted4IP, m_TriggerRecordNumberResetTmp, std::move(m_imageListResetTmp), std::move(m_imageStructListResetTmp));
		for (const auto& rChangePair : changeVect)
		{
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
		m_mustRecalcRequiredBuffers = true;
		//This is the topmost catch for MessageContainer exceptions
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(rSvE.getMessage());
		Exception.Throw();
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
		if ((-1 == m_resetStarted4IP) && (0 <= inspectionPos && m_pDataController->getInspections().list_size() > inspectionPos))
		{
			return ResetEnum::NewReset;
		}
	}

	return ResetEnum::Invalid;
}

void TriggerRecordController::sendResetCall()
{
	std::lock_guard<std::mutex> guard(m_callbackMutex);
	for (auto& resetCallback : m_resetCallbacks)
	{
		if (nullptr != resetCallback.second)
		{
			resetCallback.second();
		}
	}
}

void TriggerRecordController::sendReadyCall()
{
	std::lock_guard<std::mutex> guard(m_callbackMutex);
	for (auto& resetCallback : m_readyCallbacks)
	{
		if (nullptr != resetCallback.second)
		{
			resetCallback.second();
		}
	}
}

void TriggerRecordController::sendTrIdCall(SvOi::TrEventData data)
{
	std::lock_guard<std::mutex> guard(m_callbackMutex);
	for (auto& newTrIdCallback : m_newTrCallbacks)
	{
		if (nullptr != newTrIdCallback.second)
		{
			newTrIdCallback.second(data);
		}
	}
}

void TriggerRecordController::sendInterestTrIdCall(std::vector<SvOi::TrInterestEventData>&& data)
{
	std::lock_guard<std::mutex> guard(m_callbackMutex);
	for (auto& newTrIdCallback : m_newInterestTrCallbacks)
	{
		if (nullptr != newTrIdCallback.second)
		{
			newTrIdCallback.second(data);
		}
	}
}

void TriggerRecordController::reduceRequiredImageBuffer(const std::map<int, int>& bufferMap)
{
	auto* pList = m_imageStructListResetTmp.mutable_list();
	for (const auto& structNumberPair : bufferMap)
	{
		auto imageIter = std::find_if(pList->begin(), pList->end(), [structNumberPair](const auto& rData)->bool
		{
			return (structNumberPair.first == rData.structid());
		});
		if (pList->end() != imageIter)
		{
			int number = std::max(imageIter->numberofbuffersrequired() - structNumberPair.second, 0);
			imageIter->set_numberofbuffersrequired(number);
		}
	}
}

#pragma endregion Private Methods

std::shared_ptr<TriggerRecordController> g_pTriggerRecordController = nullptr;
TriggerRecordController* getTriggerRecordControllerInstance(bool throwException /*= false*/)
{
	if (nullptr != g_pTriggerRecordController)
	{
		return g_pTriggerRecordController.get();
	}
	else
	{
		assert(false);
		SvStl::MessageManager Exception(throwException ? SvStl::MsgType::Data : SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetTRCFailed, SvStl::SourceFileParams(StdMessageParams));
		if (throwException)
		{
			Exception.Throw();
		}
		return nullptr;
	}
}

ImageBufferController& getImageBufferControllerInstance()
{
	///getTriggerRecordControllerInstance is not checked here as there is no possibility to return an empty ImageBufferController!
	return getTriggerRecordControllerInstance()->getImageBufferControllerInstance();
}
} //namespace SvTrc

namespace SvOi
{
int getInspectionPos(uint32_t inspectionId)
{
	int result{ -1 };
	auto* pTrc = SvTrc::getTriggerRecordControllerInstance();
	if (nullptr != pTrc)
	{
		const auto& rInspectionList = pTrc->getInspections();
		const auto& Iter = std::find_if(rInspectionList.list().begin(), rInspectionList.list().end(), [inspectionId](const auto& rItem)-> bool
			{
				return rItem.id() == inspectionId;
			});
		if (rInspectionList.list().end() != Iter)
		{
			result = static_cast<int>(std::distance(rInspectionList.list().begin(), Iter));
		}
	}

	return result;
}

ITriggerRecordControllerRW* getTriggerRecordControllerRWInstance() noexcept
{
	auto* pTRC = SvTrc::getTriggerRecordControllerInstance();
	if (nullptr != pTRC && pTRC->isWritable())
	{
		//Use reinterpret cast for speed as we know it fits!
		return reinterpret_cast<ITriggerRecordControllerRW*> (pTRC);
	}
	assert(false);
	SvStl::MessageManager Exception(SvStl::MsgType::Log);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NotWriteVersion, SvStl::SourceFileParams(StdMessageParams));
	return nullptr;
}

ITriggerRecordControllerRW& getTriggerRecordControllerRWInstanceThrow() noexcept(false)
{
	auto* pTRC = SvTrc::getTriggerRecordControllerInstance(true);
	if (nullptr != pTRC && pTRC->isWritable())
	{
		//Use reinterpret cast for speed as we know it fits!
		return reinterpret_cast<ITriggerRecordControllerRW&> (*pTRC);
	}
	assert(false);
	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NotWriteVersion, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

ITriggerRecordControllerR* getTriggerRecordControllerRInstance()
{
	//Use reinterpret cast for speed as we know it fits!
	return reinterpret_cast<ITriggerRecordControllerR*> (SvTrc::getTriggerRecordControllerInstance());
}

void createTriggerRecordControllerInstance(TRC_DataType dataType)
{
	if (nullptr == SvTrc::g_pTriggerRecordController)
	{
		switch (dataType)
		{
		case ::SvOi::TRC_DataType::Local:
			SvTrc::g_pTriggerRecordController = std::make_shared<SvTrc::TriggerRecordController>(std::make_unique<SvTrc::DataControllerLocal>());
			break;
		case ::SvOi::TRC_DataType::Writer:
			SvTrc::g_pTriggerRecordController = std::make_shared<SvTrc::TriggerRecordController>(std::make_unique<SvTrc::DataControllerWriter>());
			break;
		case ::SvOi::TRC_DataType::Reader:
			SvTrc::g_pTriggerRecordController = std::make_shared<SvTrc::TriggerRecordController>(std::make_unique<SvTrc::DataControllerReader>());
			break;
		default:
			assert(false);
			break;
		}
	}
	else
	{
		assert(false);
	}
}

void destroyTriggerRecordController()
{
	if (nullptr != SvTrc::g_pTriggerRecordController)
	{
		SvTrc::g_pTriggerRecordController->clearAll();
		SvTrc::g_pTriggerRecordController = nullptr;
	}
}
}//namespace SvOi