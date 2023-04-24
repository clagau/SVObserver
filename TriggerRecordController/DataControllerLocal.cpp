//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DataControllerLocal.cpp
/// All Rights Reserved
//*****************************************************************************
/// Control of the Data in local mode.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "DataControllerLocal.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "ImageBufferController.h"
#include "TriggerRecord.h"
#include "TriggerRecordController.h"
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

void TRControllerLocalDataPerIP::setDataDefList(SvPb::DataDefinitionList&& dataDefList)
{ 
	m_DataDefList.Swap(&dataDefList); 
	m_DataDefMap.clear();
	int pos = 0;
	for (const auto& rTmp : m_DataDefList.list())
	{
		m_DataDefMap[rTmp.objectid()] = pos++;
	}
}

void TRControllerLocalDataPerIP::setImageList(SvPb::ImageList&& imageList) 
{ 
	m_ImageList = imageList; 
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
}

void TRControllerLocalDataPerIP::createTriggerRecordsBuffer(int trNumbers)
{
	getMutableBasicData().m_TriggerRecordNumber = trNumbers;
	getMutableBasicData().m_triggerRecordBufferSize = sizeof(TriggerRecordData) + sizeof(int)*(m_ImageList.list_size()+m_ImageList.hiddenlist_size());
	//Reserve memory space for the data size and the data
	getMutableBasicData().m_triggerRecordBufferSize += sizeof(int) + getBasicData().m_dataListSize;
	setTriggerRecords(malloc(getBasicData().m_triggerRecordBufferSize * getBasicData().m_TriggerRecordNumber));
	setNextPosForFreeCheck(getNextPosForFreeCheck() % (getBasicData().m_TriggerRecordNumber));
}

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
	Log_Assert(0 <= pos && getBasicData().m_TriggerRecordNumber > pos && nullptr != m_pTriggerRecords);

	void* tmp = static_cast<char*>(m_pTriggerRecords) + (pos*getBasicData().m_triggerRecordBufferSize);
	TriggerRecordData* tr = reinterpret_cast<TriggerRecordData*>(tmp);
	return *tr;
};

void TRControllerLocalDataPerIP::setTrOfInterestNumber(int number)
{
	Locker::LockerPtr locker = Locker::lockReset(m_basicData.m_mutexTrOfInterest);
	if (locker && number != m_basicData.m_TrOfInterestNumber)
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
					rCurrentTR.m_isInterest = true;
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
	Locker::LockerPtr locker = Locker::lockReset(m_basicData.m_mutexTrOfInterest, false);
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
	DataControllerLocal::clearAll();
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
		if (SvOi::ITriggerRecordControllerRW::cMaxTriggerRecords < rInspection.numberofrecords() || SvOi::ITriggerRecordControllerRW::cMaxTriggerRecordsOfInterest < rInspection.numberrecordsofinterest())
		{
			Log_Assert(false);
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

void DataControllerLocal::changeDataDef(SvPb::DataDefinitionList&& dataDefList, long valueObjectMemSize, int inspectionPos)
{
	if (0 > inspectionPos || m_dataVector.size() < inspectionPos)
	{
		Log_Assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
		
	//Note we need to add to valueObjectMemSize an additional long which has the size of the memory block copied for the TRC data
	m_dataVector[inspectionPos].getMutableBasicData().m_dataListSize = valueObjectMemSize + sizeof(long);
	m_dataVector[inspectionPos].setDataDefList(std::move(dataDefList));
}

SvOi::ITriggerRecordRPtr DataControllerLocal::createTriggerRecordObject(int inspectionPos, std::function<bool(TriggerRecordData&)> validFunc)
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

SvOi::ITriggerRecordRWPtr DataControllerLocal::createTriggerRecordObjectToWrite(int inspectionPos)
{
	Log_Assert(0 <= inspectionPos && m_dataVector.size() > inspectionPos && m_dataVector[inspectionPos].getBasicData().m_bInit);

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
					rCurrentTR.m_isInterest = false;
					TRControllerLocalDataPerIP& rIPData = m_dataVector[inspectionPos];
					rIPData.setLastStartedTRID(rCurrentTR.m_trId);
					rIPData.setNextPosForFreeCheck((currentPos + 1) % (rIPData.getBasicData().m_TriggerRecordNumber));
					return std::make_shared<TriggerRecord>(inspectionPos, currentPos, rCurrentTR, rIPData.getImageList(), rIPData.getDataDefList(), rIPData.getBasicData().m_dataListSize, m_resetId);
				}
			}
			currentPos++;
			currentPos = currentPos % m_dataVector[inspectionPos].getBasicData().m_TriggerRecordNumber;
		} while (currentPos != m_dataVector[inspectionPos].getNextPosForFreeCheck());
		Log_Assert(false);
	}

	return nullptr;
}; 

std::vector<std::pair<int, int>> DataControllerLocal::ResetTriggerRecordStructure(int inspectionId, int triggerRecordNumber, SvPb::ImageList&& rImageList, SvPb::ImageStructList&& rImageStructList)
{
	prepareReset();
	Log_Assert(m_dataVector.size() == m_inspectionList.list_size());
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
			m_dataVector[i].setImageList(std::move(rImageList));
			m_dataVector[i].createTriggerRecordsBuffer(triggerRecordNumber);
		}
		if (nullptr != m_dataVector[i].getTriggerRecords() && 0 < rBaseData.m_triggerRecordBufferSize)
		{
			memset(m_dataVector[i].getTriggerRecords(), -1, rBaseData.m_triggerRecordBufferSize*rBaseData.m_TriggerRecordNumber);
			for (int j = 0; j < rBaseData.m_TriggerRecordNumber; j++)
			{	//initialize buffer
				getTRData(i, j).init(m_dataVector[i].getImageList().list_size(), m_dataVector[i].getImageList().hiddenlist_size());
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
			for (auto* pList : {m_dataVector[i].getMutableImageList().mutable_list(), m_dataVector[i].getMutableImageList().mutable_hiddenlist()})
			{
				for (auto& rImageData : *pList)
				{
					if (rImageData.structid() == rChangePair.first)
					{
						rImageData.set_structid(rChangePair.second);
					}
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

void DataControllerLocal::setPauseTrsOfInterest(bool flag, int inspectionPos)
{
	if (0 <= inspectionPos && 64 > inspectionPos)
	{
		if (flag)
		{
			m_pauseTrOfInterestFlag[0] |= (1ull << inspectionPos);
		}
		else
		{
			m_pauseTrOfInterestFlag[0] &= (~(1ull << inspectionPos));
		}
	}
	else if (64 <= inspectionPos && 128 > inspectionPos)
	{
		if (flag)
		{
			m_pauseTrOfInterestFlag[1] |= (1ull << (inspectionPos-64));
		}
		else
		{
			m_pauseTrOfInterestFlag[1] &= (~(1ull << (inspectionPos-64)));
		}
	}
	else
	{
		m_pauseTrOfInterestFlag[0] = flag ? ULLONG_MAX : 0;
		m_pauseTrOfInterestFlag[1] = flag ? ULLONG_MAX : 0;
	}	
}

bool DataControllerLocal::getPauseTrsOfInterest(int inspectionPos) const 
{ 
	if (0 <= inspectionPos && 64 > inspectionPos)
	{
		return ((1ull << inspectionPos) & m_pauseTrOfInterestFlag[0]) > 0;
	}
	else if (64 <= inspectionPos && 128 > inspectionPos)
	{
		return ((1ull << (inspectionPos-64)) & m_pauseTrOfInterestFlag[1]) > 0;
	}

	return (1 & m_pauseTrOfInterestFlag[0]) > 0;
}

std::vector<SvOi::ITriggerRecordRPtr> DataControllerLocal::getTRsOfInterest(int inspectionPos, int n)
{
	TRControllerLocalDataPerIP& rIPData = m_dataVector[inspectionPos];
	std::vector<SvOi::ITriggerRecordRPtr> retVec;
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
	if (0 > inspectionId || m_dataVector.size() <= inspectionId)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionSMData, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
	return &m_dataVector[inspectionId];
}

const TRControllerBaseDataPerIP* DataControllerLocal::getTRControllerData(int inspectionId) const
{
	if (0 > inspectionId || m_dataVector.size() <= inspectionId)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionSMData, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
	return &m_dataVector[inspectionId];
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
