//*****************************************************************************
/// \copyright COPYRIGHT (c) 2019 by Seidenader Maschinenbau GmbH
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
#include "SVMessage\SVMessage.h"
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
	SvDef::StringVector msgList;
	msgList.push_back(SvUl::Format(_T("%s"), smName));
	SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_SMLoad, msgList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

void TRControllerReaderDataPerIP::reloadData()
{
	m_ImageList.ParseFromArray(m_pImageListInSM, m_pSmData->m_imageListSize);
	m_DataDefList.ParseFromArray(m_pDataDefListInSM, m_pSmData->m_dataDefListSize);
}

TriggerRecordData& TRControllerReaderDataPerIP::getTRData(int pos) const
{
	assert(0 <= pos && getBasicData().m_TriggerRecordNumber > pos && nullptr != m_pTriggerRecords);

	void* tmp = static_cast<char*>(m_pTriggerRecords) + (pos*getBasicData().m_triggerRecordBufferSize);
	TriggerRecordData* tr = reinterpret_cast<TriggerRecordData*>(tmp);
	return *tr;
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
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
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

void TRControllerReaderDataPerIP::setTRofInterest(int inspectionPos, int pos)
{
	Locker::LockerPtr locker = Locker::lockReset(m_pBasicData->m_mutexTrOfInterest);
	if (locker && 0 < m_pBasicData->m_TRofInterestNumber)
	{
		int nextPos = (m_pBasicData->m_TrOfInterestCurrentPos + 1) % (m_pBasicData->m_TRofInterestNumber);
		if (0 <= m_pTRofInterestArray[nextPos] && getBasicData().m_TriggerRecordNumber > m_pTRofInterestArray[nextPos])
		{
			removeTRReferenceCount(inspectionPos, getTRData(m_pTRofInterestArray[nextPos]).m_referenceCount);
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
	Locker::LockerPtr locker = Locker::lockReset(m_pBasicData->m_mutexTrOfInterest);
	int vecSize = m_pBasicData->m_TRofInterestNumber;
	if (nullptr != locker && 0 < vecSize)
	{
		int number = std::min(n, vecSize - 1); //the vecSize is one more than required to avoid overwriting value during reading.
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
	return retVec;
}
#pragma endregion TRControllerReaderDataPerIP

#pragma region Constructor
DataControllerReader::DataControllerReader()
	: DataControllerBase()
{
	m_reloadFuture = std::async(std::launch::async, [&] { initAndreloadData(); });
	m_stopThreads = CreateEvent(NULL, true, false, NULL);
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
	isUpToDate();
	return m_inspectionList; 
}

const SvPb::ImageStructList& DataControllerReader::getImageStructList() const 
{
	isUpToDate();
	return m_imageStructList; 
}

ITriggerRecordRPtr DataControllerReader::createTriggerRecordObject(int inspectionPos, std::function<bool(TriggerRecordData&)> validFunc)
{
	std::shared_lock<std::shared_mutex> lock(m_dataVectorMutex);
	if (isUpToDate() && 0 <= inspectionPos && m_dataVector.size() > inspectionPos && m_dataVector[inspectionPos]->getBasicData().m_bInit)
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
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NotEnoughFreeForLock, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
	return nullptr;
}

void DataControllerReader::setPauseTrsOfInterest(bool flag)
{
	m_pCommonData->m_pauseTRofInterest = flag;
}

bool DataControllerReader::getPauseTrsOfInterest() const
{
	return m_pCommonData->m_pauseTRofInterest;
}

std::vector<ITriggerRecordRPtr> DataControllerReader::getTRsOfInterest(int inspectionPos, int n)
{
	auto pIPData = m_dataVector[inspectionPos];
	std::vector<ITriggerRecordRPtr> retVec;
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
	if (isUpToDate() && 0 <= inspectionId && m_dataVector.size() > inspectionId)
	{
		return m_dataVector[inspectionId].get();
	}
	return nullptr;
}

const TRControllerBaseDataPerIP* DataControllerReader::getTRControllerData(int inspectionId) const
{
	std::shared_lock<std::shared_mutex> lock(m_dataVectorMutex);
	if (isUpToDate() && 0 <= inspectionId && m_dataVector.size() > inspectionId)
	{
		return m_dataVector[inspectionId].get();
	}
	return nullptr;
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
				assert(false);
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
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
			WaitForMultipleObjects(2, hReady, false, INFINITE);
			if (m_stopThread) { return; };

			{	//this is needed to free the lock before go to wait
				auto pLock = ResetLocker::lockReset(m_pCommonData->m_resetId);
				if (nullptr != pLock && 0 < m_pCommonData->m_resetId)
				{
					try
					{
						m_inspectionList.ParseFromArray(m_pInspectionListInSM, m_pCommonData->m_inspectionListPBSize);
						m_imageStructList.ParseFromArray(m_pImageStructListInSM, m_pCommonData->m_imageStructListPBSize);

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

						m_bufferVector.clear();
						m_sharedMemoryMap.clear();
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
						SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
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
	HANDLE hChange[2];
	hChange[0] = m_stopThreads;
	hChange[1] = m_hTridEvent;
	while (true)
	{
		WaitForMultipleObjects(2, hChange, false, INFINITE);
		if (m_stopThread)
		{
			return;
		}


		std::vector<std::pair<int, int>> newTrIdList;
		{
			auto pLock = ResetLocker::lockReset(m_pCommonData->m_resetId);
			if (nullptr != m_newTrIdCallback && nullptr != pLock && 0 < m_pCommonData->m_resetId)
			{
				std::shared_lock<std::shared_mutex> lock(m_dataVectorMutex);
				for (int i = 0; i < m_dataVector.size(); i++)
				{
					auto pIpData = m_dataVector[i];
					if (nullptr != pIpData)
					{
						int trId = pIpData->getTrId2Send();
						if (0 < trId)
						{
							newTrIdList.emplace_back(i, trId);
						}
					}
				}
			}
		}
		for (auto newTrIdPair : newTrIdList)
		{
			m_newTrIdCallback(newTrIdPair.first, newTrIdPair.second);
		}
	}
}

void DataControllerReader::addBuffer(const SvPb::ImageStructData &imageStruct)
{
	auto& rSMPointer = m_sharedMemoryMap[imageStruct.memoryname()] = std::make_unique<SvSml::SharedDataStore>();
	bool isOk = rSMPointer->OpenDataStore(imageStruct.memoryname().c_str());
	if (!isOk)
	{
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%s"), imageStruct.memoryname().c_str()));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_SMLoad, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
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
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ImageProps, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
	for (int i = 0; i < imageStruct.numberofbuffersrequired(); i++)
	{
		BYTE* ptr = rSMPointer->GetPtr(i, 0);
		SVMatroxBuffer buffer;
		HRESULT errCode = SVMatroxBufferInterface::CreateBuffer(buffer, bufferProps, ptr);
		if (S_OK != errCode || buffer.empty())
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%X"), errCode));
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
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
			msgList.push_back(SvUl::Format(_T("%d"), m_maxNumberOfRequiredBuffer));
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TooManyImageBuffer, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}
	}
}

}
