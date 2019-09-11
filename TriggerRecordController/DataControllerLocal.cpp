//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file DataControllerLocal.cpp
/// All Rights Reserved
//*****************************************************************************
/// Control of the Data in local mode.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "DataControllerLocal.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "CopyData.h"
#include "ImageBufferController.h"
#include "TriggerRecord.h"
#include "TriggerRecordData.h"
#pragma endregion Includes

namespace SvTrc
{
TRControllerLocalDataPerIP::~TRControllerLocalDataPerIP()
{
	if (nullptr != m_pTriggerRecords)
	{
		::free(m_pTriggerRecords);
	}
};

void TRControllerLocalDataPerIP::setTriggerRecords(void* pTR)
{ 
	if (nullptr != m_pTriggerRecords)
	{
		::free(m_pTriggerRecords);
	}
	m_pTriggerRecords = pTR; 
};

TriggerRecordData& TRControllerLocalDataPerIP::getTRData(int pos) const
{
	assert(0 <= pos && getBasicData().m_TriggerRecordNumber > pos && nullptr != m_pTriggerRecords);

	void* tmp = static_cast<char*>(m_pTriggerRecords) + (pos*getBasicData().m_triggerRecordBufferSize);
	TriggerRecordData* tr = reinterpret_cast<TriggerRecordData*>(tmp);
	return *tr;
};

void TRControllerLocalDataPerIP::setTrOfInterestNumber(int number)
{
	Locker::LockerPtr locker = Locker::lockReset(m_basicData.m_mutexTrOfInterest);
	if (number != m_basicData.m_TrOfInterestNumber)
	{
		m_basicData.m_TrOfInterestNumber = number;
		m_trOfInterestVec.resize(number);
	}
	std::fill(m_trOfInterestVec.begin(), m_trOfInterestVec.end(), -1);
}

void TRControllerLocalDataPerIP::setTrOfInterest(int inspectionPos, int pos)
{
	Locker::LockerPtr locker = Locker::lockReset(m_basicData.m_mutexTrOfInterest);
	if (locker && 0 < m_trOfInterestVec.size())
	{
		int nextPos = (m_basicData.m_TrOfInterestCurrentPos + 1) % (m_trOfInterestVec.size());
		if (0 <= m_trOfInterestVec[nextPos] && getBasicData().m_TriggerRecordNumber > m_trOfInterestVec[nextPos])
		{
			removeTrReferenceCount(inspectionPos, getTRData(m_trOfInterestVec[nextPos]).m_referenceCount);
		}
		if (0 <= pos && getBasicData().m_TriggerRecordNumber > pos)
		{
			TriggerRecordData& rCurrentTR = getTRData(pos);
			long refTemp = rCurrentTR.m_referenceCount;
			while (0 <= refTemp)
			{
				if (InterlockedCompareExchange(&(rCurrentTR.m_referenceCount), refTemp + 1, refTemp) == refTemp)
				{
					m_trOfInterestVec[nextPos] = pos;
					m_basicData.m_TrOfInterestCurrentPos = nextPos;
					return;  //successfully set
				}
				refTemp = rCurrentTR.m_referenceCount;
			}

		}
		
		m_trOfInterestVec[nextPos] = -1; //unsuccessfully set
	}
}

std::vector<int> TRControllerLocalDataPerIP::getTRofInterestPos(int n)
{
	std::vector<int> retVec;
	Locker::LockerPtr locker = Locker::lockReset(m_basicData.m_mutexTrOfInterest);
	int vecSize = static_cast<int>(m_trOfInterestVec.size());
	if (nullptr != locker && 0 < vecSize)
	{
		int number = std::min(n, vecSize - 1); //the vecSize is one more than required to avoid overwriting value during reading.
		int nextPos = std::min(static_cast<int>(m_basicData.m_TrOfInterestCurrentPos), vecSize - 1);
		for (int i = 0; i < number; i++)
		{
			if (0 > nextPos)
			{
				nextPos = vecSize - 1;
			}
			retVec.push_back(m_trOfInterestVec[nextPos]);
			nextPos--;
		}
	}
	return retVec;
}


DataControllerLocal::DataControllerLocal() 
	: DataControllerBase()
	, m_imageMemoryHelper(m_bufferVector)
{
}

DataControllerLocal::~DataControllerLocal()
{
	clearAll();
}

void DataControllerLocal::clearAll()
{
	m_inspectionList.clear_list();

	m_nextTRID = 0;

	for (auto& rData : m_dataVector)
	{
		ResetInspectionData(rData);
	}
	m_dataVector.clear();

	delete[] m_imageRefCountArray;
	m_imageRefCountArray = nullptr;
	m_imageRefCountSize = 0;
	m_imageStructList = SvPb::ImageStructList();

	m_imageMemoryHelper.clearAll();
	__super::clearAll();
}

bool DataControllerLocal::setInspections(SvPb::InspectionList&& rInspectionList)
{
	for (auto& rInspection : rInspectionList.list())
	{
		if (ITriggerRecordControllerRW::cMaxTriggerRecords < rInspection.numberofrecords() || ITriggerRecordControllerRW::cMaxTriggerRecordsOfInterest < rInspection.numberrecordsofinterest())
		{
			assert(false);
			return false;
		}
	}

	bool isReset = false;

	if (rInspectionList.list_size() != m_inspectionList.list_size())
	{
		prepareReset();
		isReset = true;
		
		for (auto& rData : m_dataVector)
		{
			ResetInspectionData(rData);
		}
		if (rInspectionList.list_size() != m_dataVector.size() && 0 < rInspectionList.list_size())
		{
			m_dataVector.clear();
			m_dataVector.resize(rInspectionList.list_size());
		}
	}
	else
	{
		for (int i = 0; i < m_dataVector.size(); i++)
		{
			if (rInspectionList.list(i).id() != m_inspectionList.list(i).id() ||
				rInspectionList.list(i).numberofrecords() != m_inspectionList.list(i).numberofrecords() ||
				rInspectionList.list(i).numberrecordsofinterest() != m_inspectionList.list(i).numberrecordsofinterest())
			{
				if (!isReset)
				{
					prepareReset();
					isReset = true;
				}
				ResetInspectionData(m_dataVector[i]);
			}
		}
	}

	if (isReset)
	{
		m_inspectionList.Swap(&rInspectionList);
	}

	return true;
}


void DataControllerLocal::setImageDefList(int inspectionPos, SvPb::ImageList&& imageList)
{
	if (0 <= inspectionPos && m_dataVector.size() > inspectionPos)
	{
		m_dataVector[inspectionPos].setImageList(std::move(imageList));
	}
}

void DataControllerLocal::resetImageRefCounter()
{
	if (m_bufferVector.size() > m_imageRefCountSize)
	{
		delete[] m_imageRefCountArray;
		m_imageRefCountSize = static_cast<int>(m_bufferVector.size());
		m_imageRefCountArray = new long[m_imageRefCountSize];
	}
	memset(m_imageRefCountArray, 0, sizeof(long)*m_imageRefCountSize);
}

void DataControllerLocal::changeDataDef(SvPb::DataDefinitionList&& dataDefList, std::vector<_variant_t>&& valueObjectList, int inspectionPos)
{
	if (0 > inspectionPos || m_dataVector.size() < inspectionPos)
	{
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
		
	//Only obtain the size of the data
	m_dataVector[inspectionPos].getMutableBasicData().m_dataListSize = copyDataList(std::move(valueObjectList));
	m_dataVector[inspectionPos].setDataDefList(std::move(dataDefList));
}

ITriggerRecordRPtr DataControllerLocal::createTriggerRecordObject(int inspectionPos, std::function<bool(TriggerRecordData&)> validFunc)
{
	if (0 <= inspectionPos && m_dataVector.size() > inspectionPos && m_dataVector[inspectionPos].getBasicData().m_bInit)
	{
		if (validFunc)
		{
			for (int i = 0; i < m_dataVector[inspectionPos].getBasicData().m_TriggerRecordNumber; i++)
			{
				TriggerRecordData& rCurrentTR = getTRData(inspectionPos, i);
				if (validFunc(rCurrentTR))
				{
					long refTemp = rCurrentTR.m_referenceCount;
					while (0 <= refTemp)
					{
						if (InterlockedCompareExchange(&(rCurrentTR.m_referenceCount), refTemp + 1, refTemp) == refTemp)
						{
							TRControllerLocalDataPerIP& rIPData = m_dataVector[inspectionPos];
							return std::make_shared<TriggerRecord>(inspectionPos, i, rCurrentTR, rIPData.getImageList(), rIPData.getDataDefList(), rIPData.getBasicData().m_dataListSize, m_resetId);
						}
						refTemp = rCurrentTR.m_referenceCount;
					}
				}
			}
		}
	}
	return nullptr;
}

ITriggerRecordRWPtr DataControllerLocal::createTriggerRecordObjectToWrite(int inspectionPos)
{
	assert(0 <= inspectionPos && m_dataVector.size() > inspectionPos && m_dataVector[inspectionPos].getBasicData().m_bInit);

	if (0 <= inspectionPos && m_dataVector.size() > inspectionPos && m_dataVector[inspectionPos].getBasicData().m_bInit)
	{
		int currentPos = m_dataVector[inspectionPos].getNextPosForFreeCheck();
		do
		{
			TriggerRecordData& rCurrentTR = getTRData(inspectionPos, currentPos);
			long count = rCurrentTR.m_referenceCount;
			while (TriggerRecordData::cInvalidData == count || 0 == count)
			{
				if (InterlockedCompareExchange(&(rCurrentTR.m_referenceCount), TriggerRecordData::cWriteBlocked, count) == count)
				{
					rCurrentTR.m_trId = m_nextTRID++;
					TRControllerLocalDataPerIP& rIPData = m_dataVector[inspectionPos];
					rIPData.setLastStartedTRID(rCurrentTR.m_trId);
					rIPData.setNextPosForFreeCheck((currentPos + 1) % (rIPData.getBasicData().m_TriggerRecordNumber));
					return std::make_shared<TriggerRecord>(inspectionPos, currentPos, rCurrentTR, rIPData.getImageList(), rIPData.getDataDefList(), rIPData.getBasicData().m_dataListSize, m_resetId);
				}
			}
			currentPos++;
			currentPos = currentPos % m_dataVector[inspectionPos].getBasicData().m_TriggerRecordNumber;
		} while (currentPos != m_dataVector[inspectionPos].getNextPosForFreeCheck());
		assert(false);
	}

	return nullptr;
}; 

std::vector<std::pair<int, int>> DataControllerLocal::ResetTriggerRecordStructure(int inspectionId, int triggerRecordNumber, SvPb::ImageList&& rImageList, SvPb::ImageStructList&& rImageStructList)
{
	prepareReset();
	assert(m_dataVector.size() == m_inspectionList.list_size());
	for (int i = 0; i < m_dataVector.size(); i++)
	{
		TRControllerBaseDataPerIP::BasicData& rBaseData = m_dataVector[i].getMutableBasicData();
		int number = m_inspectionList.list(i).numberrecordsofinterest();
		if (0 < number)
		{
			m_dataVector[i].setTrOfInterestNumber(number + 1); //add one more to reduce the possibility that the last interest will be overwritten during Reader create the interest-list.
		}
		else
		{
			m_dataVector[i].setTrOfInterestNumber(0);
		}
		if (i == inspectionId)
		{
			ResetInspectionData(m_dataVector[i]);
			rBaseData.m_TriggerRecordNumber = triggerRecordNumber;
			rBaseData.m_triggerRecordBufferSize = (sizeof(TriggerRecordData) + sizeof(int)*rImageList.list_size());
			//Reserve memory space for the data size and the data
			rBaseData.m_triggerRecordBufferSize += sizeof(int) + rBaseData.m_dataListSize;
			m_dataVector[i].setImageList(std::move(rImageList));
			m_dataVector[i].setTriggerRecords(malloc(rBaseData.m_triggerRecordBufferSize * rBaseData.m_TriggerRecordNumber));
		}
		if (nullptr != m_dataVector[i].getTriggerRecords() && 0 < rBaseData.m_triggerRecordBufferSize)
		{
			memset(m_dataVector[i].getTriggerRecords(), -1, rBaseData.m_triggerRecordBufferSize*rBaseData.m_TriggerRecordNumber);
			for (int j = 0; j < rBaseData.m_TriggerRecordNumber; j++)
			{	//initialize buffer
				getTRData(i, j).init(m_dataVector[i].getImageList().list_size());
			}
		}
		else
		{
			m_dataVector[i].setTriggerRecords(nullptr);
			rBaseData.m_bInit = false;
		}
	}

	//update structId to fit to the position in m_imageStructList
	std::vector<std::pair<int, int>> changeVect = getImageBufferControllerInstance().reset(std::move(rImageStructList));
	for (const auto& rChangePair : changeVect)
	{
		//update per Inspection
		for (int i = 0; i < m_dataVector.size(); i++)
		{
			for (auto& rImageData : (*m_dataVector[i].getMutableImageList().mutable_list()))
			{
				if (rImageData.structid() == rChangePair.first)
				{
					rImageData.set_structid(rChangePair.second);
				}
			}
		}
	}

	if (0 <= inspectionId)
	{
		m_dataVector[inspectionId].getMutableBasicData().m_bInit = true;
	}

	finishedReset();

	return changeVect;
}

void DataControllerLocal::prepareReset()
{
	::ResetEvent(m_hReadyEvent);
	m_resetId = 0;
	::SetEvent(m_hResetEvent);
	if (m_reloadCallback)
	{
		m_reloadCallback();
	}
	while (0 < m_resetLockCounter) 
	{
		std::this_thread::yield();
	}
}

void DataControllerLocal::finishedReset()
{
	m_resetId = std::max(1l, m_lastResetId + 1);
	m_lastResetId = m_resetId;
	::ResetEvent(m_hResetEvent);
	::SetEvent(m_hReadyEvent);
	if (m_readyCallback)
	{
		m_readyCallback();
	}
}

std::vector<ITriggerRecordRPtr> DataControllerLocal::getTRsOfInterest(int inspectionPos, int n)
{
	TRControllerLocalDataPerIP& rIPData = m_dataVector[inspectionPos];
	std::vector<ITriggerRecordRPtr> retVec;
	auto posVec = rIPData.getTRofInterestPos(n);

	for (int pos : posVec)
	{
		if (0 <= pos && rIPData.getBasicData().m_TriggerRecordNumber > pos)
		{
			TriggerRecordData& rCurrentTR = getTRData(inspectionPos, pos);
			long refTemp = rCurrentTR.m_referenceCount;
			while (0 <= refTemp)
			{
				if (InterlockedCompareExchange(&(rCurrentTR.m_referenceCount), refTemp + 1, refTemp) == refTemp)
				{
					retVec.emplace_back(std::make_shared<TriggerRecord>(inspectionPos, pos, rCurrentTR, rIPData.getImageList(), rIPData.getDataDefList(), rIPData.getBasicData().m_dataListSize, m_resetId));
					break;
				}
				refTemp = rCurrentTR.m_referenceCount;
			}
		}
	}
	return retVec;
}

TRControllerBaseDataPerIP* DataControllerLocal::getTRControllerData(int inspectionId)
{
	if (0 <= inspectionId && m_dataVector.size() > inspectionId)
	{
		return &m_dataVector[inspectionId];
	}
	return nullptr;
}

const TRControllerBaseDataPerIP* DataControllerLocal::getTRControllerData(int inspectionId) const
{
	if (0 <= inspectionId && m_dataVector.size() > inspectionId)
	{
		return &m_dataVector[inspectionId];
	}
	return nullptr;
}

long* DataControllerLocal::getImageRefCountPtr(int pos)
{
	if (0 <= pos &&  m_imageRefCountSize > pos)
	{
		return &m_imageRefCountArray[pos];
	}
	return nullptr;
}

void DataControllerLocal::ResetInspectionData(TRControllerLocalDataPerIP& rData)
{
	rData.getMutableBasicData().m_lastFinishedTRID = -1;
	rData.setLastStartedTRID(-1);
	if (rData.getBasicData().m_bInit)
	{
		rData.setTriggerRecords(nullptr);
		rData.getMutableBasicData().m_triggerRecordBufferSize = 0;
		getImageBufferControllerInstance().reduceRequiredBuffers(rData.getImageList(), rData.getBasicData().m_TriggerRecordNumber);
		rData.setImageList({});
	}
	rData.getMutableBasicData().m_bInit = false;
	rData.setNextPosForFreeCheck(0);
}

}
