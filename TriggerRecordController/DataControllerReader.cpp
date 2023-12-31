//*****************************************************************************
/// \copyright COPYRIGHT (c) 2019 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file DataControllerReader.cpp
/// All Rights Reserved
//*****************************************************************************
/// Control of the Data in reader mode.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "DataControllerReader.h"
#include "ImageBufferController.h"
#include "ResetLocker.h"
#include "SharedMemoryStructs.h"
#include "TriggerRecord.h"
#include "TriggerRecordData.h"
#include "SVMessage/SVMessage.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary\MatroxImageProps.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVSharedMemoryLibrary\SVSharedMemorySettings.h"
#include "SVSharedMemoryLibrary\SMParameterStruct.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

namespace SvTrc
{
#pragma region TRControllerReaderDataPerIP
TRControllerReaderDataPerIP::~TRControllerReaderDataPerIP()
{
};

void TRControllerReaderDataPerIP::init(const std::string& smName)
{
	SvSml::SMParameterStruct smParam(SvSml::SVSharedMemorySettings::DefaultConnectionTimout, SvSml::SVSharedMemorySettings::DefaultCreateWaitTime);
	m_SMHandle = std::make_unique<SvSml::SharedDataStore>();
	
	if (nullptr != m_SMHandle && m_SMHandle->OpenDataStore(smName.c_str()))
	{
		byte* pTemp = m_SMHandle->GetPtr(0, 0);
		if (nullptr != pTemp)
		{
			m_smName = smName;
			m_pBasicData = reinterpret_cast<BasicData*>(pTemp);
			int offset = sizeof(BasicData);
			m_pSmData = reinterpret_cast<SMData*>(pTemp + offset);
			offset += sizeof(SMData);
			m_pTRofInterestArray = reinterpret_cast<int*>(pTemp + offset);
			offset += sizeof(int) *(TriggerRecordController::cMaxTriggerRecordsOfInterest + 1);
			m_pImageListInSM = pTemp + offset;
			offset += m_pSmData->m_maxImageListSize;
			m_pDataDefListInSM = pTemp + offset;
			offset += m_pSmData->m_maxDataDefListSize;
			m_pTriggerRecords = pTemp + offset;
			reloadData();
			return;
		}
	}
	
	m_SMHandle = nullptr;
	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_SMLoad, {smName}, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

void TRControllerReaderDataPerIP::reloadData()
{
	m_ImageList.ParsePartialFromArray(m_pImageListInSM, m_pSmData->m_imageListSize);
	m_ImageDefMap.clear();
	int pos = 0;
	for (const auto& rTmp : m_ImageList.list())
	{
		m_ImageDefMap[rTmp.objectid()] = pos++;
	}
	pos = SvOi::cTRCImageHiddenFlag;
	for (const auto& rTmp : m_ImageList.hiddenlist())
	{
		m_ImageDefMap[rTmp.objectid()] = pos++;
	}

	m_ChildImageDefMap.clear();
	pos = 0;
	for (const auto& rTmp : m_ImageList.childlist())
	{
		m_ChildImageDefMap[rTmp.objectid()] = pos++;
	}

	m_LinkedImageDefMap.clear();
	for (const auto& rTmp : m_ImageList.linkedlist())
	{
		if (auto iter = m_ImageDefMap.find(rTmp.destinationid()); m_ImageDefMap.end() != iter)
		{
			m_LinkedImageDefMap[rTmp.sourceid()] = {false, iter->second};
		}
		else if (auto childIter = m_ChildImageDefMap.find(rTmp.destinationid()); m_ChildImageDefMap.end() != childIter)
		{
			m_LinkedImageDefMap[rTmp.sourceid()] = {true, childIter->second};
		}
		else
		{
			Log_Assert(false);
		}
	}

	m_DataDefList.ParsePartialFromArray(m_pDataDefListInSM, m_pSmData->m_dataDefListSize);
	m_DataDefMap.clear();
	pos = 0;
	for (const auto& rTmp : m_DataDefList.list())
	{
		m_DataDefMap[rTmp.objectid()] = pos++;
	}
}

TriggerRecordData& TRControllerReaderDataPerIP::getTRData(int pos) const
{
	Log_Assert(0 <= pos && getBasicData().m_TriggerRecordNumber > pos && nullptr != m_pTriggerRecords);
	if (0 > pos || getBasicData().m_TriggerRecordNumber <= pos || nullptr == m_pTriggerRecords)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetTRData, {std::to_string(pos), std::to_string(getBasicData().m_TriggerRecordNumber)}, SvStl::SourceFileParams(StdMessageParams));
	}
	void* tmp = static_cast<char*>(m_pTriggerRecords) + (pos*getBasicData().m_triggerRecordBufferSize);
	TriggerRecordData* tr = reinterpret_cast<TriggerRecordData*>(tmp);
	return *tr;
}

void TRControllerReaderDataPerIP::setLastSetOfInterestFlagPos(int pos)
{
	if (nullptr != m_pBasicData)
	{
		m_pBasicData->m_lastSetOfInterestFlagPos = pos;
	}
}

int TRControllerReaderDataPerIP::getTrId2Send()
{
	if (nullptr != m_pBasicData && m_lastSendTrId != m_pBasicData->m_lastFinishedTRID)
	{
		m_lastSendTrId = m_pBasicData->m_lastFinishedTRID;
		return m_lastSendTrId;
	}
	return -1;
}

SvOi::TrInterestEventData TRControllerReaderDataPerIP::getInterestTrId2Send()
{
	if (nullptr != m_pBasicData && 0 <= m_pBasicData->m_lastSetOfInterestFlagPos && m_pBasicData->m_lastSetOfInterestFlagPos != m_lastSendInterestPos 
		&& m_pBasicData->m_TriggerRecordNumber > m_pBasicData->m_lastSetOfInterestFlagPos)
	{
		m_lastSendInterestPos = m_pBasicData->m_lastSetOfInterestFlagPos;
		TriggerRecordData& rCurrentTR = getTRData(m_lastSendInterestPos);
		return{ -1, rCurrentTR.m_trId, rCurrentTR.m_isInterest };
	}
	return {};
}

void TRControllerReaderDataPerIP::increaseFreeTrNumber()
{
	if (nullptr != m_pBasicData)
	{
		InterlockedIncrement(&m_pBasicData->m_numberOfFreeTR);
	}
}

void TRControllerReaderDataPerIP::decreaseFreeTrNumber()
{
	if (nullptr != m_pBasicData)
	{
		InterlockedDecrement(&m_pBasicData->m_numberOfFreeTR);
		if (0 > m_pBasicData->m_numberOfFreeTR)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_DecreaseFreeTRC, SvStl::SourceFileParams(StdMessageParams));
			InterlockedExchange(&m_pBasicData->m_numberOfFreeTR, 0l);
		}
	}
}

bool TRControllerReaderDataPerIP::isEnoughFreeForLock() const
{
	if (nullptr != m_pBasicData)
	{
		return getNumberOfTRKeepFreeForWrite() < m_pBasicData->m_numberOfFreeTR;
	}
	return false;
}

void TRControllerReaderDataPerIP::setTrOfInterest(int inspectionPos, int pos)
{
	Locker::LockerPtr locker = (nullptr != m_pBasicData) ? Locker::lockReset(m_pBasicData->m_mutexTrOfInterest) : nullptr;
	if (locker && 0 < m_pBasicData->m_TrOfInterestNumber)
	{
		int nextPos = (m_pBasicData->m_TrOfInterestCurrentPos + 1) % (m_pBasicData->m_TrOfInterestNumber);
		if (0 <= m_pTRofInterestArray[nextPos] && getBasicData().m_TriggerRecordNumber > m_pTRofInterestArray[nextPos])
		{
			removeTrReferenceCount(inspectionPos, getTRData(m_pTRofInterestArray[nextPos]).m_referenceCount);
		}
		if (0 <= pos && getBasicData().m_TriggerRecordNumber > pos)
		{
			TriggerRecordData& rCurrentTR = getTRData(pos);
			long refTemp = rCurrentTR.m_referenceCount;
			while (0 <= refTemp)
			{
				if (InterlockedCompareExchange(&(rCurrentTR.m_referenceCount), refTemp + 1, refTemp) == refTemp)
				{
					if (0 == refTemp)
					{
						decreaseFreeTrNumber();
					}
					m_pTRofInterestArray[nextPos] = pos;
					m_pBasicData->m_TrOfInterestCurrentPos = nextPos;
					rCurrentTR.m_isInterest = true;
					return;  //successfully set
				}
				refTemp = rCurrentTR.m_referenceCount;
			}

		}

		m_pTRofInterestArray[nextPos] = -1; //unsuccessfully set
	}
}

std::vector<int> TRControllerReaderDataPerIP::getTRofInterestPos(int n)
{
	std::vector<int> retVec;
	int vecSize = m_pBasicData->m_TrOfInterestNumber;
	if (0 < vecSize)
	{
		int number = std::min(n, vecSize - 1); //the vecSize is one more than required to avoid overwriting value during reading.
		Locker::LockerPtr locker = (nullptr != m_pBasicData) ? Locker::lockReset(m_pBasicData->m_mutexTrOfInterest, false) : nullptr;
		if (nullptr != locker)
		{
			int nextPos = std::min(static_cast<int>(m_pBasicData->m_TrOfInterestCurrentPos), vecSize - 1);
			for (int i = 0; i < number; i++)
			{
				if (0 > nextPos)
				{
					nextPos = vecSize - 1;
				}
				retVec.push_back(m_pTRofInterestArray[nextPos]);
				nextPos--;
			}
		}
		else
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetInterestFailedLockFailed, SvStl::SourceFileParams(StdMessageParams));
		}
	}
	return retVec;
}
#pragma endregion TRControllerReaderDataPerIP

#pragma region Constructor
DataControllerReader::DataControllerReader()
	: DataControllerBase()
	, m_reloadFuture(std::async(std::launch::async, [&] { initAndreloadData(); }))
	, m_stopThreads (CreateEvent(NULL, true, false, NULL))
{
}

DataControllerReader::~DataControllerReader()
{
	m_stopThread = true;
	SetEvent(m_stopThreads);
	clearAll();

	if (m_reloadFuture.valid())
	{
		m_reloadFuture.wait();
	}
	if (m_newTrIdFuture.valid())
	{
		m_newTrIdFuture.wait();
	}
}
#pragma endregion Constructor


long DataControllerReader::getResetId() const 
{
	if (m_isInit)
	{
		return m_pCommonData->m_resetId;
	}
	return -1;
}

volatile long* DataControllerReader::getResetLockCounterRef()
{ 
	if (m_isInit)
	{
		return &m_pCommonData->m_resetLockCounter;
	}
	return nullptr;
}

const SvPb::InspectionList& DataControllerReader::getInspections() const 
{
	return m_inspectionList; 
}

const SvPb::ImageStructList& DataControllerReader::getImageStructList() const 
{
	return m_imageStructList; 
}

SvOi::ITriggerRecordRPtr DataControllerReader::createTriggerRecordObject(int inspectionPos, std::function<bool(TriggerRecordData&)> validFunc)
{
	std::shared_lock<std::shared_mutex> lock(m_dataVectorMutex);
	isUpToDateException();
	if (0 <= inspectionPos && m_dataVector.size() > inspectionPos && m_dataVector[inspectionPos]->getBasicData().m_bInit)
	{
		TRControllerReaderDataPerIP* pIPData = m_dataVector[inspectionPos].get();
		if (nullptr != pIPData && validFunc)
		{
			if (pIPData->isEnoughFreeForLock())
			{
				for (int i = 0; i < m_dataVector[inspectionPos]->getBasicData().m_TriggerRecordNumber; i++)
				{
					TriggerRecordData& rCurrentTR = getTRData(inspectionPos, i);
					if (validFunc(rCurrentTR))
					{
						long refTemp = rCurrentTR.m_referenceCount;
						while (0 <= refTemp)
						{
							if (InterlockedCompareExchange(&(rCurrentTR.m_referenceCount), refTemp + 1, refTemp) == refTemp)
							{
								if (0 == refTemp)
								{
									pIPData->decreaseFreeTrNumber();
								}
								return std::make_shared<TriggerRecord>(inspectionPos, i, rCurrentTR, pIPData->getImageList(), pIPData->getDataDefList(), pIPData->getBasicData().m_dataListSize, m_pCommonData->m_resetId);
							}
							refTemp = rCurrentTR.m_referenceCount;
						}
					}
				}
			}
			else
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NotEnoughFreeForLock, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
	return nullptr;
}

void DataControllerReader::setPauseTrsOfInterest(bool flag, int inspectionPos)
{
	if (0 <= inspectionPos && 64 > inspectionPos)
	{
		if (flag)
		{
			m_pCommonData->m_pauseTRofInterest[0] |= (1ull << inspectionPos);
		}
		else
		{
			m_pCommonData->m_pauseTRofInterest[0] &= (~(1ull << inspectionPos));
		}
	}
	else if (64 <= inspectionPos && 128 > inspectionPos)
	{
		if (flag)
		{
			m_pCommonData->m_pauseTRofInterest[1] |= (1ull << (inspectionPos - 64));
		}
		else
		{
			m_pCommonData->m_pauseTRofInterest[1] &= (~(1ull << (inspectionPos - 64)));
		}
	}
	else
	{
		m_pCommonData->m_pauseTRofInterest[0] = flag ? ULLONG_MAX : 0;
		m_pCommonData->m_pauseTRofInterest[1] = flag ? ULLONG_MAX : 0;
	}
}

bool DataControllerReader::getPauseTrsOfInterest(int inspectionPos) const
{
	if (0 <= inspectionPos && 64 > inspectionPos)
	{
		return ((1ull << inspectionPos) & m_pCommonData->m_pauseTRofInterest[0]) > 0;
	}
	else if (64 <= inspectionPos && 128 > inspectionPos)
	{
		return ((1ull << (inspectionPos - 64)) & m_pCommonData->m_pauseTRofInterest[1]) > 0;
	}

	return (1 & m_pCommonData->m_pauseTRofInterest[0]) > 0;
}

std::vector<SvOi::ITriggerRecordRPtr> DataControllerReader::getTRsOfInterest(int inspectionPos, int n)
{
	std::vector<SvOi::ITriggerRecordRPtr> retVec;
	std::shared_lock<std::shared_mutex> lock(m_dataVectorMutex);
	if (m_dataVector.size() <= inspectionPos)
	{
		return retVec;
	}
	auto pIPData = m_dataVector[inspectionPos];
	if (nullptr != pIPData)
	{
		auto posVec = pIPData->getTRofInterestPos(n);

		for (int pos : posVec)
		{
			if (0 <= pos && pIPData->getBasicData().m_TriggerRecordNumber > pos)
			{
				TriggerRecordData& rCurrentTR = getTRData(inspectionPos, pos);
				long refTemp = rCurrentTR.m_referenceCount;
				while (0 <= refTemp)
				{
					if (InterlockedCompareExchange(&(rCurrentTR.m_referenceCount), refTemp + 1, refTemp) == refTemp)
					{
						retVec.emplace_back(std::make_shared<TriggerRecord>(inspectionPos, pos, rCurrentTR, pIPData->getImageList(), pIPData->getDataDefList(), pIPData->getBasicData().m_dataListSize, m_pCommonData->m_resetId));
						break;
					}
					refTemp = rCurrentTR.m_referenceCount;
				}
			}
		}
	}
	return retVec;
}

TRControllerBaseDataPerIP* DataControllerReader::getTRControllerData(int inspectionId)
{
	std::shared_lock<std::shared_mutex> lock(m_dataVectorMutex);
	isUpToDateException();
	if (0 > inspectionId || m_dataVector.size() <= inspectionId)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionSMData, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
	return m_dataVector[inspectionId].get();
}

const TRControllerBaseDataPerIP* DataControllerReader::getTRControllerData(int inspectionId) const
{
	std::shared_lock<std::shared_mutex> lock(m_dataVectorMutex);
	isUpToDateException();
	if (0 > inspectionId || m_dataVector.size() <= inspectionId)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionSMData, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
	return m_dataVector[inspectionId].get();
}

long* DataControllerReader::getImageRefCountPtr(int pos)
{
	if (isUpToDate() && 0 <= pos &&  m_pCommonData->m_imageRefCountSize > pos)
	{
		return &m_imageRefCountArray[pos];
	}
	return nullptr;
}

bool DataControllerReader::isUpToDate() const
{ 
	return m_isInit && nullptr != m_pCommonData && m_lastResetId == m_pCommonData->m_resetId && 0 < m_lastResetId;
}

void DataControllerReader::isUpToDateException() const
{
	if (false == isUpToDate())
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NotUpToDate, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
}

void DataControllerReader::initAndreloadData()
{
	while (!m_stopThread && !m_isInit)
	{
		SvSml::SMParameterStruct smParam(SvSml::SVSharedMemorySettings::DefaultConnectionTimout, SvSml::SVSharedMemorySettings::DefaultCreateWaitTime);
		if (m_commonSHHandle.OpenDataStore(cCommonParameterSM))
		{
			byte* pTemp = m_commonSHHandle.GetPtr(0, 0);
			if (nullptr != pTemp)
			{

				m_pCommonData = reinterpret_cast<CommonDataStruct*>(pTemp);
				m_imageRefCountArray = reinterpret_cast<long*>(pTemp + sizeof(CommonDataStruct));
				m_pInspectionListInSM = pTemp + sizeof(CommonDataStruct) + m_maxNumberOfRequiredBuffer * sizeof(*m_imageRefCountArray);
				m_pImageStructListInSM = pTemp + sizeof(CommonDataStruct) + m_maxNumberOfRequiredBuffer * sizeof(*m_imageRefCountArray) + cMaxInspectionPbSize;
				m_isInit = true;
				m_newTrIdFuture = std::async(std::launch::async, [&] { newTrIdThread(); });
				break;
			}
			else
			{
				Log_Assert(false);
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_CreateSMCommonData, SvStl::SourceFileParams(StdMessageParams));
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	HANDLE hReset[2], hReady[2];
	hReset[0] = hReady[0] = m_stopThreads;
	hReset[1] = m_hResetEvent;
	hReady[1] = m_hReadyEvent;
	while (!m_stopThread)
	{
		while (!isUpToDate())
		{
			m_lastResetId = -1;
			if (m_reloadCallback)
			{
				m_reloadCallback();
			}

			m_bufferVector.clear();
			m_sharedMemoryMap.clear();
			{
				std::unique_lock<std::shared_mutex> lock(m_dataVectorMutex);
				m_dataVector.clear();
			}

			WaitForMultipleObjects(2, hReady, false, INFINITE);
			if (m_stopThread) { return; };

			{	//this is needed to free the lock before go to wait
				auto pLock = ResetLocker::lockReset(m_pCommonData->m_resetId);
				if (nullptr != pLock && 0 < m_pCommonData->m_resetId)
				{
					try
					{
						m_inspectionList.ParsePartialFromArray(m_pInspectionListInSM, m_pCommonData->m_inspectionListPBSize);
						m_imageStructList.ParsePartialFromArray(m_pImageStructListInSM, m_pCommonData->m_imageStructListPBSize);

						std::unique_lock<std::shared_mutex> lock(m_dataVectorMutex);
						m_dataVector.resize(m_inspectionList.list_size());
						for (int i = 0; i < m_dataVector.size(); i++)
						{
							if (nullptr == m_dataVector[i] || m_dataVector[i]->getSMName() != m_inspectionList.list(i).nameofsm())
							{
								m_dataVector[i] = std::make_shared<TRControllerReaderDataPerIP>();
								m_dataVector[i]->init(m_inspectionList.list(i).nameofsm());
							}
							else
							{
								m_dataVector[i]->reloadData();
							}
						}

						for (auto imageStruct : m_imageStructList.list())
						{
							addBuffer(imageStruct);
						}

						m_lastResetId = m_pCommonData->m_resetId;
						if (0 < m_lastResetId && m_readyCallback)
						{
							m_readyCallback();
						}
					}
					catch (const SvStl::MessageContainer& rSvE)
					{
						m_lastResetId = -1;
						//This is the topmost catch for MessageContainer exceptions
						SvStl::MessageManager Exception(SvStl::MsgType::Log);
						Exception.setMessage(rSvE.getMessage());
					}
				}
			}
		}

		WaitForMultipleObjects(2, hReset, false, INFINITE);
	}
}

void DataControllerReader::newTrIdThread()
{
	auto sleepTime = std::chrono::system_clock::now();
	while (true)
	{
		std::this_thread::sleep_until(sleepTime);
		if (m_stopThread)
		{
			return;
		}

		sleepTime = std::chrono::system_clock::now() + std::chrono::milliseconds(1);
		sendNewTrId();
	}
}

void DataControllerReader::addBuffer(const SvPb::ImageStructData &imageStruct)
{
	std::vector<std::string> memoryNames;
	boost::algorithm::split(memoryNames, imageStruct.memoryname(), [](char c) { return c == ';'; }, boost::token_compress_off);
	//remove empty entries
	auto removeIter = std::remove_if(memoryNames.begin(), memoryNames.end(), [](std::string entry) { return entry.empty(); });
	if (memoryNames.end() != removeIter)
	{
		memoryNames.erase(removeIter);
	}

	for (auto name : memoryNames)
	{
		auto& rSMPointer = m_sharedMemoryMap[name] = std::make_unique<SvSml::SharedDataStore>();
		bool isOk = rSMPointer->OpenDataStore(name.c_str());
		if (!isOk)
		{
			Log_Assert(false);
			SvStl::MessageManager Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_SMLoad, {name}, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}
	}

	SVMatroxBufferCreateStruct bufferStruct;
	MatroxImageProps bufferProps;
	memcpy(&bufferStruct, imageStruct.type().c_str(), sizeof(SVMatroxBufferCreateStruct));
	if (0 < imageStruct.imageprops().size())
	{
		memcpy(&bufferProps, imageStruct.imageprops().c_str(), sizeof(MatroxImageProps));
	}
	else
	{
		Log_Assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ImageProps, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	int divFactor = static_cast<int>(memoryNames.size());
	int requiredPerStore = (imageStruct.numberofbuffersrequired() + divFactor - 1) / divFactor;
	for (int i = 0; i < imageStruct.numberofbuffersrequired(); i++)
	{
		int pos = i / requiredPerStore;
		auto& rSMPointer = m_sharedMemoryMap[memoryNames[pos]];
		BYTE* ptr = (nullptr != rSMPointer) ? rSMPointer->GetPtr(i - requiredPerStore*pos, 0) : nullptr;
		SVMatroxBuffer buffer;
		HRESULT errCode = SVMatroxBufferInterface::CreateBuffer(buffer, bufferProps, ptr);
		if (S_OK != errCode || buffer.empty())
		{
			SvDef::StringVector msgList;
			msgList.push_back(std::format(_T("{:X}"), errCode));
			SvStl::MessageManager Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_CreateBuffer, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}
		if (m_bufferVector.size() < m_maxNumberOfRequiredBuffer)
		{
			m_bufferVector.push_back(buffer);
		}
		else
		{
			SvDef::StringVector msgList;
			msgList.push_back(std::format(_T("{:d}"), m_maxNumberOfRequiredBuffer));
			SvStl::MessageManager Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TooManyImageBuffer, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}

		if (0 >= getResetId())
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_WARNING, SvStl::Tid_TRC_Error_StopReloadingReader, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}
	}
}

void DataControllerReader::sendNewTrId()
{
	std::vector<SvOi::TrEventData> newTrIdList;
	std::vector<SvOi::TrInterestEventData> newInterestTrIdList;
	{
		auto pLock = ResetLocker::lockReset(m_pCommonData->m_resetId);
		if (nullptr != m_newTrIdCallback && nullptr != pLock && 0 < m_pCommonData->m_resetId)
		{
			std::shared_lock<std::shared_mutex> lock(m_dataVectorMutex);
			for (int i = 0; i < m_dataVector.size(); i++)
			{
				const auto& pIpData = m_dataVector[i];
				if (nullptr != pIpData)
				{
					int trId = pIpData->getTrId2Send();
					if (0 <= trId)
					{
						newTrIdList.emplace_back(i, trId);
					}

					auto data = pIpData->getInterestTrId2Send();
					if (0 < data.m_trId)
					{
						data.m_inspectionPos = i;
						newInterestTrIdList.emplace_back(data);
					}
				}
			}
		}
	}
	if (nullptr != m_newTrIdCallback)
	{
		for (auto newTrIdPair : newTrIdList)
		{
			m_newTrIdCallback(newTrIdPair);
		}
	}

	if (nullptr != m_newInterestTrIdsCallback)
	{
		if (0 < newInterestTrIdList.size())
		{
			m_newInterestTrIdsCallback(std::move(newInterestTrIdList));
		}
	}
}
}
