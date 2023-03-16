//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DataControllerWriter.cpp
/// All Rights Reserved
//*****************************************************************************
/// Control of the Data in writer mode.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <thread.h>
#include "DataControllerWriter.h"
#include "ImageBufferController.h"
#include "TriggerRecord.h"
#include "TriggerRecordController.h"
#include "TriggerRecordData.h"
#include "SVMessage/SVMessage.h"
#include "SVSharedMemoryLibrary\SVSharedMemorySettings.h"
#include "SVSharedMemoryLibrary\SMParameterStruct.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

namespace SvTrc
{
std::string getNewSMIPName()
{
	static int smNumber = 0;
	return std::format(_T("SVOIP{:3d}"), smNumber++);
}

int getNeedSMSize(SMData smData)
{
	return sizeof(TRControllerBaseDataPerIP::BasicData) + sizeof(SMData) + sizeof(int) *(TriggerRecordController::cMaxTriggerRecordsOfInterest+1) + smData.m_maxImageListSize + smData.m_maxDataDefListSize + smData.m_maxTriggerRecordBufferSize;
};

#pragma region TRControllerWriterDataPerIP
TRControllerWriterDataPerIP::~TRControllerWriterDataPerIP()
{
};

void TRControllerWriterDataPerIP::init(const std::function<void(const std::string&, int)>& smDataCBFunc)
{
	m_smDataCBFunc = smDataCBFunc;
	createSMBuffer(BasicData(), SMData());
};

void TRControllerWriterDataPerIP::setDataListSize(long dataSize)
{
	if (nullptr != m_pBasicData)
	{
		m_pBasicData->m_dataListSize = dataSize;
	}
}

void TRControllerWriterDataPerIP::setInitFlag(bool flag)
{
	if (nullptr != m_pBasicData)
	{
		m_pBasicData->m_bInit = flag;
	}
}

void TRControllerWriterDataPerIP::setLastFinishedTRID(int id)
{
	if (nullptr != m_pBasicData)
	{
		m_pBasicData->m_lastFinishedTRID = id;
	}
};

void TRControllerWriterDataPerIP::setLastSetOfInterestFlagPos(int pos)
{
	if (nullptr != m_pBasicData)
	{
		m_pBasicData->m_lastSetOfInterestFlagPos = pos;
	}
}

void TRControllerWriterDataPerIP::setDataDefList(SvPb::DataDefinitionList&& dataDefList)
{ 
	m_DataDefList.Swap(&dataDefList); 
	if (nullptr == m_pBasicData || nullptr == m_pSmData)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionSMData, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	int newPBSize = static_cast<int> (m_DataDefList.ByteSizeLong());
	if (newPBSize > m_pSmData->m_maxDataDefListSize)
	{
		SMData smData = *m_pSmData;
		smData.m_dataDefListSize = newPBSize;
		smData.m_maxDataDefListSize = ((newPBSize / m_dataDefListStepSize) + 1)*m_dataDefListStepSize;
		createSMBuffer(*m_pBasicData, smData);
		m_ImageList.SerializePartialToArray(m_pImageListInSM, m_pSmData->m_imageListSize);
	}
	else
	{
		m_pSmData->m_dataDefListSize = newPBSize;
	}
	m_DataDefList.SerializePartialToArray(m_pDataDefListInSM, m_pSmData->m_dataDefListSize);
	m_DataDefMap.clear();
	int pos = 0;
	for(const auto& rTmp: m_DataDefList.list())
	{
		m_DataDefMap[rTmp.objectid()] = pos++;
	}
}

TriggerRecordData& TRControllerWriterDataPerIP::getTRData(int pos) const
{
	Log_Assert(0 <= pos && getBasicData().m_TriggerRecordNumber > pos && nullptr != m_pTriggerRecords);
	if (0 > pos || getBasicData().m_TriggerRecordNumber <= pos || nullptr == m_pTriggerRecords)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetTRData, { std::to_string(pos), std::to_string(getBasicData().m_TriggerRecordNumber) }, SvStl::SourceFileParams(StdMessageParams));
	}
	void* tmp = static_cast<char*>(m_pTriggerRecords) + (pos*getBasicData().m_triggerRecordBufferSize);
	TriggerRecordData* tr = reinterpret_cast<TriggerRecordData*>(tmp);
	return *tr;
};

void TRControllerWriterDataPerIP::setImageList(SvPb::ImageList&& imageList)
{ 
	m_ImageList.Swap(&imageList); 
	if (nullptr == m_pBasicData || nullptr == m_pSmData)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionSMData, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
	int newPBSize = static_cast<int> (m_ImageList.ByteSizeLong());
	if (newPBSize > m_pSmData->m_maxImageListSize)
	{
		SMData smData = *m_pSmData;
		smData.m_imageListSize = newPBSize;
		smData.m_maxImageListSize = ((newPBSize / m_imageListStepSize) + 1)*m_imageListStepSize;
		createSMBuffer(*m_pBasicData, smData);
		m_DataDefList.SerializePartialToArray(m_pDataDefListInSM, m_pSmData->m_dataDefListSize);
	}
	else
	{
		m_pSmData->m_imageListSize = newPBSize;
	}
	m_ImageList.SerializePartialToArray(m_pImageListInSM, m_pSmData->m_imageListSize);
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

void TRControllerWriterDataPerIP::createTriggerRecordsBuffer(int trNumbers)
{
	unsigned int bufferSize = sizeof(TriggerRecordData) + sizeof(int)*(m_ImageList.list_size()+m_ImageList.hiddenlist_size()) + sizeof(int) + getBasicData().m_dataListSize;
	//Reserve memory space for the data size and the data
	createTriggerRecordsBuffer(bufferSize, trNumbers);
}

void* TRControllerWriterDataPerIP::createTriggerRecordsBuffer(long trBufferSize, int trNumbers)
{
	Log_Assert(m_pBasicData && m_pSmData);
	if (nullptr == m_pBasicData || nullptr == m_pSmData)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionSMData, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
	
	if (m_pSmData->m_maxTriggerRecordBufferSize < static_cast<int> (trBufferSize*trNumbers))
	{
		SMData smData = *m_pSmData;
		smData.m_maxTriggerRecordBufferSize = ((trBufferSize*trNumbers / m_triggerRecordBufferStepSize) + 1)*m_triggerRecordBufferStepSize;
		createSMBuffer(*m_pBasicData, smData);
		m_DataDefList.SerializePartialToArray(m_pDataDefListInSM, m_pSmData->m_dataDefListSize);
		m_ImageList.SerializePartialToArray(m_pImageListInSM, m_pSmData->m_imageListSize);
	}
	m_pBasicData->m_TriggerRecordNumber = trNumbers;
	m_pBasicData->m_triggerRecordBufferSize = trBufferSize;

	if (0 >= trBufferSize*trNumbers)
	{
		return nullptr;
	}
	setNextPosForFreeCheck(getNextPosForFreeCheck() % (getBasicData().m_TriggerRecordNumber));
	return m_pTriggerRecords;
}

void TRControllerWriterDataPerIP::resetFreeTrNumber()
{
	if (nullptr != m_pBasicData)
	{
		InterlockedExchange(&m_pBasicData->m_numberOfFreeTR, m_pBasicData->m_TriggerRecordNumber);
	}
}

void TRControllerWriterDataPerIP::increaseFreeTrNumber()
{
	if (nullptr != m_pBasicData)
	{
		InterlockedIncrement(&m_pBasicData->m_numberOfFreeTR);
	}
}

void TRControllerWriterDataPerIP::decreaseFreeTrNumber()
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

bool TRControllerWriterDataPerIP::isEnoughFreeForLock() const
{
	if (nullptr != m_pBasicData)
	{
		return getNumberOfTRKeepFreeForWrite() < m_pBasicData->m_numberOfFreeTR;
	}
	return false;
}

void TRControllerWriterDataPerIP::setTrOfInterestNumber(int number)
{
	Log_Assert(nullptr != m_pBasicData && nullptr != m_pTRofInterestArray);
	if (nullptr != m_pBasicData && nullptr != m_pTRofInterestArray)
	{
		Locker::LockerPtr locker = Locker::lockReset(m_pBasicData->m_mutexTrOfInterest);
		if (nullptr != locker)
		{
			if (number != m_pBasicData->m_TrOfInterestNumber && 0 <= number && TriggerRecordController::cMaxTriggerRecordsOfInterest + 1 >= number)
			{
				m_pBasicData->m_TrOfInterestNumber = number;
			}
			std::fill(m_pTRofInterestArray, m_pTRofInterestArray + TriggerRecordController::cMaxTriggerRecordsOfInterest + 1, -1);
		}
		else
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_setTrOfInterestNumberLockFailed, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}
	}
}

void TRControllerWriterDataPerIP::setTrOfInterest(int inspectionPos, int pos)
{
	Log_Assert(nullptr != m_pBasicData);
	if (nullptr != m_pBasicData)
	{
		Locker::LockerPtr locker = Locker::lockReset(m_pBasicData->m_mutexTrOfInterest);
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
	
	SvStl::MessageManager Exception(SvStl::MsgType::Log);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_setTrOfInterestFailed, SvStl::SourceFileParams(StdMessageParams));
}

std::vector<int> TRControllerWriterDataPerIP::getTRofInterestPos(int n)
{
	std::vector<int> retVec;
	Log_Assert(m_pBasicData);
	if (nullptr != m_pBasicData)
	{
		Locker::LockerPtr locker = Locker::lockReset(m_pBasicData->m_mutexTrOfInterest, false);
		int vecSize = m_pBasicData->m_TrOfInterestNumber;
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
	}
	return retVec;
}

void TRControllerWriterDataPerIP::createSMBuffer(const BasicData& rBasicData, const SMData& rSmData)
{
	SvSml::SMParameterStruct smParam(SvSml::SVSharedMemorySettings::DefaultConnectionTimout, SvSml::SVSharedMemorySettings::DefaultCreateWaitTime);
	auto smHandleOld = std::move(m_SMHandle);
	int oldTriggerRecodBufferSize = (nullptr != m_pSmData)?m_pSmData->m_maxTriggerRecordBufferSize:0;
	void* pTriggerRecordOld = m_pTriggerRecords;
	m_SMHandle = std::make_unique<SvSml::SharedDataStore>();
	std::string newSMName = getNewSMIPName();
	try
	{
		m_SMHandle->CreateDataStore(newSMName.c_str(), getNeedSMSize(rSmData), 1, smParam);
		byte* pTemp = m_SMHandle->GetPtr(0, 0);
		if (nullptr == pTemp)
		{
			throw;
		}
		m_pBasicData = reinterpret_cast<BasicData*>(pTemp);
		*m_pBasicData = rBasicData;
		int offset = sizeof(BasicData);
		m_pSmData = reinterpret_cast<SMData*>(pTemp + offset);
		*m_pSmData = rSmData;
		offset += sizeof(SMData);
		m_pTRofInterestArray = reinterpret_cast<int*>(pTemp + offset);
		std::fill(m_pTRofInterestArray, m_pTRofInterestArray + TriggerRecordController::cMaxTriggerRecordsOfInterest + 1, -1);
		offset += sizeof(int) *(TriggerRecordController::cMaxTriggerRecordsOfInterest + 1);
		m_pImageListInSM = pTemp + offset;
		offset += rSmData.m_maxImageListSize;
		m_pDataDefListInSM = pTemp + offset;
		offset += rSmData.m_maxDataDefListSize;
		m_pTriggerRecords = pTemp + offset;
		int minTriggerRecordBufferSize = std::min(oldTriggerRecodBufferSize, static_cast<int>(m_pSmData->m_maxTriggerRecordBufferSize));
		if (0 < minTriggerRecordBufferSize && nullptr != pTriggerRecordOld && nullptr != m_pTriggerRecords)
		{
			memcpy(m_pTriggerRecords, pTriggerRecordOld, minTriggerRecordBufferSize);
		}
		smHandleOld.reset();

		m_smDataCBFunc(newSMName, m_SMHandle->GetSlotSize());
		Log_Assert(nullptr != m_pSmData && nullptr != m_pBasicData);
	}
	catch (...)
	{
		m_pBasicData = nullptr;
		m_pSmData = nullptr;
		m_pTRofInterestArray = nullptr;
		m_pImageListInSM = nullptr;
		m_pDataDefListInSM = nullptr;
		m_pTriggerRecords = nullptr;
		Log_Assert(false);
		throw;
	}
	
}
#pragma endregion TRControllerWriterDataPerIP


#pragma region Constructor
DataControllerWriter::DataControllerWriter()
	: DataControllerBase()
	, m_imageMemoryHelper(m_bufferVector, m_maxNumberOfRequiredBuffer)
	, m_cFullSizeOfCommonParameterSM(sizeof(CommonDataStruct) + m_maxNumberOfRequiredBuffer * sizeof(*m_imageRefCountArray) + cMaxInspectionPbSize + cMaxImageStructPbSize)
{
	bool isInit = false;
	byte* pTemp = nullptr;
	try
	{
		SvSml::SMParameterStruct smParam(SvSml::SVSharedMemorySettings::DefaultConnectionTimout, SvSml::SVSharedMemorySettings::DefaultCreateWaitTime);
		m_commonSHHandle.CreateDataStore(cCommonParameterSM, m_cFullSizeOfCommonParameterSM, 1, smParam);
		pTemp = m_commonSHHandle.GetPtr(0, 0);
		if (nullptr != pTemp)
		{
			isInit = true;
		}
	}
	catch (const SvStl::MessageContainer&)
	{
		//nothing to do, try to open SM
	}

	if (!isInit)
	{	//create failed, try to open SM.
		SvSml::SMParameterStruct smParam(SvSml::SVSharedMemorySettings::DefaultConnectionTimout, SvSml::SVSharedMemorySettings::DefaultCreateWaitTime);
		if (m_commonSHHandle.OpenDataStore(cCommonParameterSM))
		{
			pTemp = m_commonSHHandle.GetPtr(0, 0);
			if (nullptr != pTemp)
			{
				isInit = true;
			}
		}
	}

	if (!isInit || nullptr == pTemp)
	{
		Log_Assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_CreateSMCommonData, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
	
	m_pCommonData = reinterpret_cast<CommonDataStruct*>(pTemp);
	m_imageRefCountArray = reinterpret_cast<long*>(pTemp + sizeof(CommonDataStruct));
	m_pInspectionListInSM = pTemp + sizeof(CommonDataStruct) + m_maxNumberOfRequiredBuffer * sizeof(*m_imageRefCountArray);
	m_pImageStructListInSM = pTemp + sizeof(CommonDataStruct) + m_maxNumberOfRequiredBuffer * sizeof(*m_imageRefCountArray) + cMaxInspectionPbSize;
}

DataControllerWriter::~DataControllerWriter()
{
	DataControllerWriter::clearAll();
}
#pragma endregion Constructor

void DataControllerWriter::clearImageBuffer(bool shouldResetImageStruct)
{
	//first free MIL-buffer, before free SM-buffer
	__super::clearImageBuffer();
	m_imageMemoryHelper.clearAll();

	if (shouldResetImageStruct)
	{
		auto* pList = m_imageStructList.mutable_list();
		if (nullptr != pList)
		{
			for (int i=0; i< pList->size(); i++)
			{
				pList->Mutable(i)->set_numberofbuffers(0);
			}
		}
	}
	else
	{
		m_imageStructList = {};
	}
}

void DataControllerWriter::clearAll()
{
	setInspectionList({});

	m_nextTRID = 0;

	for (auto pData : m_dataVector)
	{
		if (nullptr != pData)
		{
			ResetInspectionData(*pData);
		}
	}
	m_dataVector.clear();

	m_pCommonData->m_imageRefCountSize = 0;
	__super::clearAll();
	//setImageStructList must be set after clearAll from base, because all flags must be reset.
	setImageStructList({});
}

bool DataControllerWriter::setInspections(SvPb::InspectionList&& rInspectionList)
{
	auto isRecordNumberInvalid = [](const auto& rEntry)
	{
		return SvOi::ITriggerRecordControllerRW::cMaxTriggerRecords < rEntry.numberofrecords() || SvOi::ITriggerRecordControllerRW::cMaxTriggerRecordsOfInterest < rEntry.numberrecordsofinterest();
	};
	if (std::any_of(rInspectionList.list().begin(), rInspectionList.list().end(), isRecordNumberInvalid))
	{
		Log_Assert(false);
		return false;
	}

	bool isReset = false;
	if (rInspectionList.list_size() != m_inspectionList.list_size())
	{
		prepareReset();
		isReset = true;
		
		for (auto pData : m_dataVector)
		{
			if (nullptr != pData)
			{
				ResetInspectionData(*pData);
			}
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
				if (nullptr != m_dataVector[i])
				{
					ResetInspectionData(*m_dataVector[i]);
				}
			}
		}
	}

	if (isReset)
	{
		try
		{
			setInspectionList(std::move(rInspectionList));
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			//This is the topmost catch for MessageContainer exceptions
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(rSvE.getMessage());
		}
	}

	return true;
}


void DataControllerWriter::setImageDefList(int inspectionPos, SvPb::ImageList&& imageList)
{
	if (0 <= inspectionPos && m_dataVector.size() > inspectionPos)
	{
		m_dataVector[inspectionPos]->setImageList(std::move(imageList));
	}
}

const SvPb::ImageStructList& DataControllerWriter::getImageStructList() const
{ 
	if (!m_isGlobalInit)
	{
		return m_imageStructList;
	}
	else
	{
		return m_imageStructListGlobalInitTmp;
	}
};

void DataControllerWriter::setImageStructList(SvPb::ImageStructList&& list)
{
	if (!m_isGlobalInit)
	{
		if (list.ByteSizeLong() > cMaxImageStructPbSize)
		{
			Log_Assert(false);
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionPBTooLong, SvStl::SourceFileParams(StdMessageParams));
			throw Exception;
		}
		m_imageStructList.Swap(&list);
		m_pCommonData->m_imageStructListPBSize = static_cast<int> (m_imageStructList.ByteSizeLong());
		m_imageStructList.SerializePartialToArray(m_pImageStructListInSM, m_pCommonData->m_imageStructListPBSize);
	}
	else
	{
		m_imageStructListGlobalInitTmp.Swap(&list);
	}
}

void DataControllerWriter::resetImageRefCounter()
{
	if (m_bufferVector.size() <= m_maxNumberOfRequiredBuffer)
	{
		m_pCommonData->m_imageRefCountSize = static_cast<int>(m_bufferVector.size());
	}
	else
	{
		SvDef::StringVector msgList;
		msgList.push_back(std::format(_T("{:d}"), m_maxNumberOfRequiredBuffer));
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TooManyImageBuffer, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	memset(m_imageRefCountArray, 0, sizeof(*m_imageRefCountArray)*m_pCommonData->m_imageRefCountSize);
}

void DataControllerWriter::changeDataDef(SvPb::DataDefinitionList&& dataDefList, long valueObjectMemSize, int inspectionPos)
{
	if (0 > inspectionPos || m_dataVector.size() < inspectionPos || nullptr == m_dataVector[inspectionPos])
	{
		Log_Assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	//Note we need to add to valueObjectMemSize an additional long which has the size of the memory block copied for the TRC data
	m_dataVector[inspectionPos]->setDataListSize(valueObjectMemSize + sizeof(long));
	m_dataVector[inspectionPos]->setDataDefList(std::move(dataDefList));
}

SvOi::ITriggerRecordRPtr DataControllerWriter::createTriggerRecordObject(int inspectionPos, std::function<bool(TriggerRecordData&)> validFunc)
{
	if (0 <= inspectionPos && m_dataVector.size() > inspectionPos && m_dataVector[inspectionPos]->getBasicData().m_bInit)
	{
		TRControllerWriterDataPerIP* pIPData = m_dataVector[inspectionPos].get();
		if (nullptr != pIPData && validFunc)
		{
			if (pIPData->isEnoughFreeForLock())
			{
				for (int i = 0; i < pIPData->getBasicData().m_TriggerRecordNumber; i++)
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

SvOi::ITriggerRecordRWPtr DataControllerWriter::createTriggerRecordObjectToWrite(int inspectionPos)
{
	if (0 <= inspectionPos && m_dataVector.size() > inspectionPos && nullptr != m_dataVector[inspectionPos] && m_dataVector[inspectionPos]->getBasicData().m_bInit)
	{
		TRControllerWriterDataPerIP* pIPData = m_dataVector[inspectionPos].get();
		if (nullptr != pIPData)
		{
			int currentPos = m_dataVector[inspectionPos]->getNextPosForFreeCheck();
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
						pIPData->decreaseFreeTrNumber();
						pIPData->setLastStartedTRID(rCurrentTR.m_trId);
						pIPData->setNextPosForFreeCheck((currentPos + 1) % (pIPData->getBasicData().m_TriggerRecordNumber));
						return std::make_shared<TriggerRecord>(inspectionPos, currentPos, rCurrentTR, pIPData->getImageList(), pIPData->getDataDefList(), pIPData->getBasicData().m_dataListSize, m_pCommonData->m_resetId);
					}
				}
				currentPos++;
				currentPos = currentPos % m_dataVector[inspectionPos]->getBasicData().m_TriggerRecordNumber;
			} while (currentPos != m_dataVector[inspectionPos]->getNextPosForFreeCheck());
		}
		Log_Assert(false);
	}

	return nullptr;
};

std::vector<std::pair<int, int>> DataControllerWriter::ResetTriggerRecordStructure(int inspectionId, int triggerRecordNumber, SvPb::ImageList&& rImageList, SvPb::ImageStructList&& rImageStructList)
{
	prepareReset();
	for (int i = 0; i < m_dataVector.size(); i++)
	{
		auto pIPData = m_dataVector[i];
		if (nullptr != pIPData)
		{
			int number = m_inspectionList.list(i).numberrecordsofinterest();
			if (0 < number)
			{
				pIPData->setTrOfInterestNumber(number + 1); //add one more to reduce the possibility that the last interest will be overwritten during Reader create the interest-list.
			}
			else
			{
				pIPData->setTrOfInterestNumber(0);
			}
			if (i == inspectionId)
			{
				ResetInspectionData(*pIPData, false);
				pIPData->setImageList(std::move(rImageList));
				pIPData->createTriggerRecordsBuffer(triggerRecordNumber);
			}
			if (nullptr != pIPData->getTriggerRecords() && 0 < pIPData->getBasicData().m_triggerRecordBufferSize)
			{
				memset(pIPData->getTriggerRecords(), -1, pIPData->getBasicData().m_triggerRecordBufferSize*pIPData->getBasicData().m_TriggerRecordNumber);
				for (int j = 0; j < pIPData->getBasicData().m_TriggerRecordNumber; j++)
				{	//initialize buffer
					getTRData(i, j).init(pIPData->getImageList().list_size(), pIPData->getImageList().hiddenlist_size());
				}
				pIPData->resetFreeTrNumber();
			}
			else
			{
				pIPData->createTriggerRecordsBuffer(0, 0);
				pIPData->setInitFlag(false);
			}
		}
	}

	//update structId to fit to the position in m_imageStructList
	std::vector<std::pair<int, int>> changeVect = getImageBufferControllerInstance().reset(std::move(rImageStructList), m_isGlobalInit);
	for (const auto& rChangePair : changeVect)
	{
		//update per Inspection
		for (int i = 0; i < m_dataVector.size(); i++)
		{
			SvPb::ImageList tmpImageList = m_dataVector[i]->getImageList();
			int modifyFlag = false;
			for (auto* pList : {tmpImageList.mutable_list(), tmpImageList.mutable_hiddenlist()})
			{
				for (auto& rImageData : *pList)
				{
					if (rImageData.structid() == rChangePair.first)
					{
						rImageData.set_structid(rChangePair.second);
						modifyFlag = true;
					}
				}
			}
			if (modifyFlag)
			{
				m_dataVector[i]->setImageList(std::move(tmpImageList));
			}
		}
	}

	if (!m_isGlobalInit)
	{
		if (0 <= inspectionId)
		{
			m_dataVector[inspectionId]->setInitFlag(true);
		}
		else
		{  //set init flag for inspection which are changed during globalInit.
			for (auto id : m_setInitFlagAfterResetSet)
			{
				if (0 <= id && m_dataVector.size() > id)
				{
					m_dataVector[id]->setInitFlag(true);
				}
			}
			m_setInitFlagAfterResetSet.clear();
		}

		finishedReset();
	}
	else
	{
		if (0 <= inspectionId)
		{	//remember that the inspection was changed during globalInit.
			m_setInitFlagAfterResetSet.insert(inspectionId);
		}
	}
	return changeVect;
}

void DataControllerWriter::setInspectionList(SvPb::InspectionList&& rInspectionList)
{
	if (rInspectionList.ByteSizeLong() > cMaxInspectionPbSize)
	{
		Log_Assert(false);
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionPBTooLong, SvStl::SourceFileParams(StdMessageParams));
		throw Exception;
	}

	size_t size = std::min(rInspectionList.list_size(), m_inspectionList.list_size());

	for (int i = 0; i < size; i++)
	{
		rInspectionList.mutable_list(i)->set_nameofsm(m_inspectionList.list(i).nameofsm());
		rInspectionList.mutable_list(i)->set_sizeofsm(m_inspectionList.list(i).sizeofsm());
	}

	m_inspectionList.Swap(std::move(&rInspectionList));
	m_pCommonData->m_inspectionListPBSize = static_cast<int> (m_inspectionList.ByteSizeLong());
	m_inspectionList.SerializePartialToArray(m_pInspectionListInSM, m_pCommonData->m_inspectionListPBSize);

	//fit the m_dataVector to inspectionLsit
	if (m_inspectionList.list_size() > m_dataVector.size())
	{
		while (m_inspectionList.list_size() != m_dataVector.size())
		{
			size_t pos = m_dataVector.size();
			m_dataVector.emplace_back(std::make_shared<TRControllerWriterDataPerIP>());
			Log_Assert(nullptr != m_dataVector[pos]);
			auto setInspectionSMDataFunctor = [this, pos](const std::string& rSmName, int smSize) {return setInspectionSMData(static_cast<int>(pos), rSmName, smSize); };
			m_dataVector[pos]->init(setInspectionSMDataFunctor);
		}
	}
	else if (m_inspectionList.list_size() < m_dataVector.size())
	{
		while (m_inspectionList.list_size() != m_dataVector.size())
		{
			m_dataVector.pop_back();
		}
	}
}

void DataControllerWriter::setGlobalInitFlag(bool flag)
{
	if (flag && !m_isGlobalInit)
	{	//if set globalInitFlag the first time, copy image struct list to the temporary list of the globalInit.
		m_imageStructListGlobalInitTmp = m_imageStructList;
	}
	__super::setGlobalInitFlag(flag);
}

void DataControllerWriter::prepareReset()
{
	::ResetEvent(m_hReadyEvent);
	InterlockedExchange(&m_pCommonData->m_resetId, 0);
	::SetEvent(m_hResetEvent);
	if (m_reloadCallback)
	{
		m_reloadCallback();
	}

	auto start = std::chrono::system_clock::now();
	while (0 < m_pCommonData->m_resetLockCounter) //wait if any other method have left the access of this structure
	{
		std::chrono::duration<double> timeInS = std::chrono::system_clock::now() - start;
		//if a client die before decrease the resetLockCounter, a deadlock is possible
		//to solve this counter will be reseted if after a time of wait this will not happened.
		constexpr int maxWaitTime = 2;
		if (timeInS.count() > maxWaitTime)
		{
			SvDef::StringVector msgList;
			msgList.push_back(std::format(_T("{:d}"), maxWaitTime));
			msgList.push_back(std::format(_T("{:d}"), static_cast<long>(m_pCommonData->m_resetLockCounter)));
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_CounterTimeOut, SvStl::SourceFileParams(StdMessageParams));

			InterlockedExchange(&m_pCommonData->m_resetLockCounter, 0);
		}
		std::this_thread::sleep_for(std::chrono::duration<int, std::nano> (1));
	}; 
}

void DataControllerWriter::finishedReset()
{
	for (int i = 0; i < m_dataVector.size(); i++)
	{
		if (!isIPInit(i))
		{
			//not all init, do not set resetId to valid value
			return;
		}
	}
		
	InterlockedExchange(&m_pCommonData->m_resetId, std::max(1l, m_lastResetId + 1));
	m_lastResetId = m_pCommonData->m_resetId;
	if (m_readyCallback)
	{
		m_readyCallback();
	}

	::ResetEvent(m_hResetEvent);
	::SetEvent(m_hReadyEvent);
}

void DataControllerWriter::setPauseTrsOfInterest(bool flag, int inspectionPos)
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

bool DataControllerWriter::getPauseTrsOfInterest(int inspectionPos) const
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

std::vector<SvOi::ITriggerRecordRPtr> DataControllerWriter::getTRsOfInterest(int inspectionPos, int n)
{
	auto pIPData = m_dataVector[inspectionPos];
	std::vector<SvOi::ITriggerRecordRPtr> retVec;
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

TRControllerBaseDataPerIP* DataControllerWriter::getTRControllerData(int inspectionId)
{
	if (0 <= inspectionId && m_dataVector.size() > inspectionId)
	{
		return m_dataVector[inspectionId].get();
	}
	return nullptr;
}

const TRControllerBaseDataPerIP* DataControllerWriter::getTRControllerData(int inspectionId) const
{
	if (0 <= inspectionId && m_dataVector.size() > inspectionId)
	{
		return m_dataVector[inspectionId].get();
	}
	return nullptr;
}

long* DataControllerWriter::getImageRefCountPtr(int pos)
{
	if (0 <= pos &&  m_pCommonData->m_imageRefCountSize > pos)
	{
		return &m_imageRefCountArray[pos];
	}
	return nullptr;
}

void DataControllerWriter::ResetInspectionData(TRControllerWriterDataPerIP& rData, bool shouldReduceRequiredBuffer)
{
	rData.setLastFinishedTRID(-1);
	rData.setLastStartedTRID(-1);
	rData.setLastSetOfInterestFlagPos(-1);
	if (rData.getBasicData().m_bInit)
	{
		if (shouldReduceRequiredBuffer)
		{
			getImageBufferControllerInstance().reduceRequiredBuffers(rData.getImageList(), rData.getBasicData().m_TriggerRecordNumber);
		}
		rData.createTriggerRecordsBuffer(0, 0);
		rData.setImageList({});
	}
	rData.resetFreeTrNumber();
	rData.setInitFlag(false);
	rData.setNextPosForFreeCheck(0);
}

void DataControllerWriter::setInspectionSMData(int ipPos, const std::string& rSmName, int smSize)
{
	Log_Assert(0 <= ipPos && m_inspectionList.list_size() > ipPos);
	if (0 <= ipPos && m_inspectionList.list_size() > ipPos)
	{
		m_inspectionList.mutable_list(ipPos)->set_nameofsm(rSmName);
		m_inspectionList.mutable_list(ipPos)->set_sizeofsm(smSize);
		if (m_inspectionList.ByteSizeLong() > cMaxInspectionPbSize)
		{
			Log_Assert(false);
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionPBTooLong, SvStl::SourceFileParams(StdMessageParams));
		}
		else
		{
			m_pCommonData->m_inspectionListPBSize = static_cast<int> (m_inspectionList.ByteSizeLong());
			m_inspectionList.SerializePartialToArray(m_pInspectionListInSM, m_pCommonData->m_inspectionListPBSize);
		}
	}
}

}
