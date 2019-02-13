//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file DataControllerWriter.cpp
/// All Rights Reserved
//*****************************************************************************
/// Control of the Data in local mode.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVSharedMemoryLibrary\SVSharedMemorySettings.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "CopyData.h"
#include "DataControllerWriter.h"
#include "ImageBufferController.h"
#include "TriggerRecord.h"
#include "TriggerRecordData.h"
#pragma endregion Includes

namespace SvTrc
{
constexpr int cMaxInspectionPbSize = 1000;
constexpr int cMaxImageStructPbSize = 10'000;


std::string getNewSMIPName()
{
	static int smNumber = 0;
	return SvUl::Format(_T("SVOIP%3d"), smNumber++);
}

int getNeedSMSize(TRControllerWriterDataPerIP::SMData smData)
{
	return sizeof(TRControllerBaseDataPerIP::BasicData) + sizeof(TRControllerWriterDataPerIP::SMData) + smData.m_maxImageListSize + smData.m_maxDataDefListSize + smData.m_maxTriggerRecordBufferSize;
};

#pragma region TRControllerWriterDataPerIP
TRControllerWriterDataPerIP::~TRControllerWriterDataPerIP()
{
};

void TRControllerWriterDataPerIP::init(std::function<void(const std::string&, int)> smDataCBFunc)
{
	m_smDataCBFunc = smDataCBFunc;
	createSMBuffer(BasicData(), SMData());
};

void TRControllerWriterDataPerIP::setDataListSize(int dataSize)
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

void TRControllerWriterDataPerIP::setDataDefList(SvPb::DataDefinitionList&& dataDefList)
{ 
	m_DataDefList.Swap(&dataDefList); 


	int newPBSize = m_DataDefList.ByteSize();
	if (newPBSize > m_pSmData->m_maxDataDefListSize)
	{
		SMData smData = *m_pSmData;
		smData.m_dataDefListSize = newPBSize;
		smData.m_maxDataDefListSize = ((newPBSize / m_dataDefListStepSize) + 1)*m_dataDefListStepSize;
		createSMBuffer(*m_pBasicData, smData);
		m_ImageList.SerializeToArray(m_pImageListInSM, m_pSmData->m_imageListSize);
	}
	else
	{
		m_pSmData->m_dataDefListSize = newPBSize;
	}
	m_DataDefList.SerializeToArray(m_pDataDefListInSM, m_pSmData->m_dataDefListSize);
}

TriggerRecordData& TRControllerWriterDataPerIP::getTRData(int pos) const
{
	assert(0 <= pos && getBasicData().m_TriggerRecordNumber > pos && nullptr != m_pTriggerRecords);

	void* tmp = static_cast<char*>(m_pTriggerRecords) + (pos*getBasicData().m_triggerRecordBufferSize);
	TriggerRecordData* tr = reinterpret_cast<TriggerRecordData*>(tmp);
	return *tr;
};

void TRControllerWriterDataPerIP::setImageList(SvPb::ImageList&& imageList)
{ 
	m_ImageList = imageList; 
	int newPBSize = m_ImageList.ByteSize();
	if (newPBSize > m_pSmData->m_maxImageListSize)
	{
		SMData smData = *m_pSmData;
		smData.m_imageListSize = newPBSize;
		smData.m_maxImageListSize = ((newPBSize / m_imageListStepSize) + 1)*m_imageListStepSize;
		createSMBuffer(*m_pBasicData, smData);
		m_DataDefList.SerializeToArray(m_pDataDefListInSM, m_pSmData->m_dataDefListSize);
	}
	else
	{
		m_pSmData->m_imageListSize = newPBSize;
	}
	m_ImageList.SerializeToArray(m_pImageListInSM, m_pSmData->m_imageListSize);
}

void* TRControllerWriterDataPerIP::createTriggerRecordsBuffer(int trBufferSize, int trNumbers)
{
	if (m_pSmData->m_maxTriggerRecordBufferSize < trBufferSize*trNumbers)
	{
		SMData smData = *m_pSmData;
		smData.m_maxTriggerRecordBufferSize = ((trBufferSize*trNumbers / m_triggerRecordBufferStepSize) + 1)*m_triggerRecordBufferStepSize;
		createSMBuffer(*m_pBasicData, smData);
		m_DataDefList.SerializeToArray(m_pDataDefListInSM, m_pSmData->m_dataDefListSize);
		m_ImageList.SerializeToArray(m_pImageListInSM, m_pSmData->m_imageListSize);
	}
	m_pBasicData->m_TriggerRecordNumber = trNumbers;
	m_pBasicData->m_triggerRecordBufferSize = trBufferSize;
	if (0 >= trBufferSize*trNumbers)
	{
		return nullptr;
	}
	return m_pTriggerRecords;
}

void TRControllerWriterDataPerIP::createSMBuffer(BasicData basicData, SMData smData)
{
	SvSml::SMParameterStruct smParam(SvSml::SVSharedMemorySettings::DefaultConnectionTimout, SvSml::SVSharedMemorySettings::DefaultCreateWaitTime);
	auto smHandleOld = std::move(m_SMHandle);
	int oldTriggerRecodBufferSize = (nullptr != m_pSmData)?m_pSmData->m_maxTriggerRecordBufferSize:0;
	void* pTriggerRecordOld = m_pTriggerRecords;
	m_SMHandle = std::make_unique<SvSml::SharedDataStore>();
	m_SMHandle->CreateDataStore(getNewSMIPName().c_str(), getNeedSMSize(smData), 1, smParam);
	byte* pTemp = m_SMHandle->GetPtr(0, 0);
	m_pBasicData = reinterpret_cast<BasicData*>(pTemp);
	*m_pBasicData = basicData;
	m_pSmData = reinterpret_cast<SMData*>(pTemp + sizeof(BasicData));
	*m_pSmData = smData;
	m_pImageListInSM = pTemp + sizeof(BasicData) + sizeof(SMData);
	m_pDataDefListInSM = pTemp + sizeof(BasicData) + sizeof(SMData) + smData.m_maxImageListSize;
	m_pTriggerRecords = pTemp + sizeof(BasicData) + sizeof(SMData) + smData.m_maxImageListSize + smData.m_maxDataDefListSize;
	int minTriggerRecordBufferSize = std::min(oldTriggerRecodBufferSize, m_pSmData->m_maxTriggerRecordBufferSize);
	if (0 < minTriggerRecordBufferSize && nullptr != pTriggerRecordOld && nullptr != m_pTriggerRecords)
	{
		memcpy(m_pTriggerRecords, pTriggerRecordOld, minTriggerRecordBufferSize);
	}
	smHandleOld.reset();

	m_smDataCBFunc(m_SMHandle->GetMapFileName(), m_SMHandle->GetSlotSize());
}
#pragma endregion TRControllerWriterDataPerIP


#pragma region Constructor
DataControllerWriter::DataControllerWriter()
	: DataControllerBase()
	, m_imageMemoryHelper(m_bufferVector, m_maxNumberOfRequiredBuffer)
	, m_cFullSizeOfCommonParameterSM(sizeof(CommonDataStruct) + m_maxNumberOfRequiredBuffer * sizeof(*m_imageRefCountArray) + cMaxInspectionPbSize + cMaxImageStructPbSize)
{
	SvSml::SMParameterStruct smParam(SvSml::SVSharedMemorySettings::DefaultConnectionTimout, SvSml::SVSharedMemorySettings::DefaultCreateWaitTime);
	m_commonSHHandle.CreateDataStore(cCommonParameterSM, m_cFullSizeOfCommonParameterSM, 1, smParam);
	byte* pTemp = m_commonSHHandle.GetPtr(0, 0);
	if (nullptr == pTemp)
	{
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
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
	clearAll();
}
#pragma endregion Constructor



void DataControllerWriter::clearAll()
{
	m_pCommonData->m_resetId = 0;
	while (0 < m_pCommonData->m_resetLockCounter) {};
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
	setImageStructList({});

	m_imageMemoryHelper.clearAll();

	__super::clearAll();
}

bool DataControllerWriter::setInspections(const SvPb::InspectionList& rInspectionList)
{
	for (auto& rInspection : rInspectionList.list())
	{
		if (cMaxTriggerRecords < rInspection.numberofrecords())
		{
			assert(false);
			return false;
		}
	}

	bool isReset = false;

	if (rInspectionList.list_size() != m_inspectionList.list_size())
	{
		m_pCommonData->m_resetId = 0;
		isReset = true;
		while (0 < m_pCommonData->m_resetLockCounter) {};

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
				rInspectionList.list(i).numberofrecords() != m_inspectionList.list(i).numberofrecords())
			{
				if (!isReset)
				{
					m_pCommonData->m_resetId = 0;
					while (0 < m_pCommonData->m_resetLockCounter) {};
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
			setInspectionList(rInspectionList);
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			//This is the topmost catch for MessageContainer exceptions
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
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

void DataControllerWriter::setImageStructList(SvPb::ImageStructList list)
{
	int sizeList = list.ByteSize();

	if (list.ByteSize() > cMaxImageStructPbSize)
	{
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionPBTooLong, SvStl::SourceFileParams(StdMessageParams));
		throw Exception;
	}
	m_imageStructList.Swap(&list);
	m_pCommonData->m_imageStructListPBSize = m_imageStructList.ByteSize();
	m_imageStructList.SerializeToArray(m_pImageStructListInSM, m_pCommonData->m_imageStructListPBSize);
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
		msgList.push_back(SvUl::Format(_T("%d"), m_maxNumberOfRequiredBuffer));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TooManyImageBuffer, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	memset(m_imageRefCountArray, 0, sizeof(*m_imageRefCountArray)*m_pCommonData->m_imageRefCountSize);
}

void DataControllerWriter::changeDataDef(SvPb::DataDefinitionList&& dataDefList, std::vector<_variant_t>&& valueObjectList, int inspectionPos)
{
	if (0 > inspectionPos || m_dataVector.size() < inspectionPos || nullptr == m_dataVector[inspectionPos])
	{
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	//Only obtain the size of the data
	m_dataVector[inspectionPos]->setDataListSize(copyDataList(std::move(valueObjectList)));
	m_dataVector[inspectionPos]->setDataDefList(std::move(dataDefList));
}

ITriggerRecordRPtr DataControllerWriter::createTriggerRecordObject(int inspectionPos, int trId)
{
	if (0 <= inspectionPos && m_dataVector.size() > inspectionPos && m_dataVector[inspectionPos]->getBasicData().m_bInit)
	{
		if (0 <= trId)
		{
			for (int i = 0; i < m_dataVector[inspectionPos]->getBasicData().m_TriggerRecordNumber; i++)
			{
				TriggerRecordData& rCurrentTR = getTRData(inspectionPos, i);
				if (rCurrentTR.m_trId == trId)
				{
					long refTemp = rCurrentTR.m_referenceCount;
					while (0 <= refTemp)
					{
						if (InterlockedCompareExchange(&(rCurrentTR.m_referenceCount), refTemp + 1, refTemp) == refTemp)
						{
							TRControllerWriterDataPerIP* pIPData = m_dataVector[inspectionPos].get();
							if (nullptr != pIPData)
							{
								return std::make_shared<TriggerRecord>(inspectionPos, rCurrentTR, pIPData->getImageList(), pIPData->getDataDefList(), pIPData->getBasicData().m_dataListSize, m_pCommonData->m_resetId);
							}
						}
						refTemp = rCurrentTR.m_referenceCount;
					}
				}
			}
		}
	}
	return nullptr;
}

ITriggerRecordRWPtr DataControllerWriter::createTriggerRecordObjectToWrite(int inspectionPos)
{
	assert(0 <= inspectionPos && m_dataVector.size() > inspectionPos && nullptr != m_dataVector[inspectionPos] && m_dataVector[inspectionPos]->getBasicData().m_bInit);

	if (0 <= inspectionPos && m_dataVector.size() > inspectionPos && nullptr != m_dataVector[inspectionPos] && m_dataVector[inspectionPos]->getBasicData().m_bInit)
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
					TRControllerWriterDataPerIP& rIPData = *m_dataVector[inspectionPos];
					rIPData.setLastStartedTRID(rCurrentTR.m_trId);
					rIPData.setNextPosForFreeCheck((currentPos + 1) % (rIPData.getBasicData().m_TriggerRecordNumber));
					return std::make_shared<TriggerRecord>(inspectionPos, rCurrentTR, rIPData.getImageList(), rIPData.getDataDefList(), rIPData.getBasicData().m_dataListSize, m_pCommonData->m_resetId);
				}
			}
			currentPos++;
			currentPos = currentPos % m_dataVector[inspectionPos]->getBasicData().m_TriggerRecordNumber;
		} while (currentPos != m_dataVector[inspectionPos]->getNextPosForFreeCheck());
		assert(false);
	}

	return nullptr;
};

std::vector<std::pair<int, int>> DataControllerWriter::ResetTriggerRecordStructure(int inspectionId, int triggerRecordNumber, SvPb::ImageList imageList, SvPb::ImageStructList imageStructList)
{
	m_pCommonData->m_resetId = std::max(1l, m_lastResetId + 1);
	m_lastResetId = m_pCommonData->m_resetId;
	while (0 < m_pCommonData->m_resetLockCounter) {}; //wait if any other method have left the access of this structure
	for (int i = 0; i < m_dataVector.size(); i++)
	{
		auto pIPData = m_dataVector[i];
		if (nullptr != pIPData)
		{
			if (i == inspectionId)
			{
				ResetInspectionData(*pIPData);
				int bufferSize = (sizeof(TriggerRecordData) + sizeof(int)*imageList.list_size()) + sizeof(int) + pIPData->getBasicData().m_dataListSize;
				//Reserve memory space for the data size and the data
				pIPData->setImageList(std::move(imageList));
				pIPData->createTriggerRecordsBuffer(bufferSize, triggerRecordNumber);
			}
			if (nullptr != pIPData->getTriggerRecords() && 0 < pIPData->getBasicData().m_triggerRecordBufferSize)
			{
				memset(pIPData->getTriggerRecords(), -1, pIPData->getBasicData().m_triggerRecordBufferSize*pIPData->getBasicData().m_TriggerRecordNumber);
				for (int j = 0; j < pIPData->getBasicData().m_TriggerRecordNumber; j++)
				{	//initialize buffer
					getTRData(i, j).init(pIPData->getImageList().list_size());
				}
			}
			else
			{
				pIPData->createTriggerRecordsBuffer(0, 0);
				pIPData->setInitFlag(false);
			}
		}
	}

	//update structId to fit to the position in m_imageStructList
	std::vector<std::pair<int, int>> changeVect = getImageBufferControllerInstance().reset(imageStructList);
	for (const auto& rChangePair : changeVect)
	{
		//update per Inspection
		for (int i = 0; i < m_dataVector.size(); i++)
		{
			SvPb::ImageList tmpImageList = m_dataVector[i]->getImageList();
			int modifyFlag = false;
			for (auto& rImageData : (*tmpImageList.mutable_list()))
			{
				if (rImageData.structid() == rChangePair.first)
				{
					rImageData.set_structid(rChangePair.second);
					modifyFlag = true;
				}
			}
			if (modifyFlag)
			{
				m_dataVector[i]->setImageList(std::move(tmpImageList));
			}
		}
	}

	if (0 <= inspectionId)
	{
		m_dataVector[inspectionId]->setInitFlag(true);
	}

	return changeVect;
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

void DataControllerWriter::ResetInspectionData(TRControllerWriterDataPerIP& rData)
{
	rData.setLastFinishedTRID(-1);
	rData.setLastStartedTRID(-1);
	if (rData.getBasicData().m_bInit)
	{
		getImageBufferControllerInstance().reduceRequiredBuffers(rData.getImageList(), rData.getBasicData().m_TriggerRecordNumber);
		rData.createTriggerRecordsBuffer(0, 0);
		rData.setImageList({});
	}
	rData.setInitFlag(false);
	rData.setNextPosForFreeCheck(0);
}

void DataControllerWriter::setInspectionList(const SvPb::InspectionList &rInspectionList)
{
	if (rInspectionList.ByteSize() > cMaxInspectionPbSize)
	{
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionPBTooLong, SvStl::SourceFileParams(StdMessageParams));
		throw Exception;
	}

	SvPb::InspectionList tmpList = rInspectionList;
	size_t size = std::min(tmpList.list_size(), m_inspectionList.list_size());

	for (int i = 0; i < size; i++)
	{
		tmpList.mutable_list(i)->set_nameofsm(m_inspectionList.list(i).nameofsm());
		tmpList.mutable_list(i)->set_sizeofsm(m_inspectionList.list(i).sizeofsm());
	}

	m_inspectionList = tmpList;
	m_pCommonData->m_inspectionListPBSize = m_inspectionList.ByteSize();
	m_inspectionList.SerializeToArray(m_pInspectionListInSM, m_pCommonData->m_inspectionListPBSize);

	//fit the m_dataVector to inspectionLsit
	if (m_inspectionList.list_size() > m_dataVector.size())
	{
		while (m_inspectionList.list_size() != m_dataVector.size())
		{
			size_t pos = m_dataVector.size();
			m_dataVector.emplace_back(std::make_shared<TRControllerWriterDataPerIP>());
			assert(nullptr != m_dataVector[pos]);
			m_dataVector[pos]->init(std::bind(&DataControllerWriter::setInspectionSMData, this, static_cast<int>(pos), std::placeholders::_1, std::placeholders::_2));
		}
	}
	else if (m_inspectionList.list_size() < m_dataVector.size())
	{
		while (m_inspectionList.list_size() != m_dataVector.size())
		{
			m_dataVector.pop_back();
		}
	}


	//SvPb::InspectionList inspectionList;
	//inspectionList.ParseFromArray(m_inspectionListInSM, cMaxInspectionPbSize);
}

void DataControllerWriter::setInspectionSMData(int ipPos, const std::string& rSmName, int smSize)
{
	assert(0 <= ipPos && m_inspectionList.list_size() > ipPos);
	if (0 <= ipPos && m_inspectionList.list_size() > ipPos)
	{
		m_inspectionList.mutable_list(ipPos)->set_nameofsm(rSmName);
		m_inspectionList.mutable_list(ipPos)->set_sizeofsm(smSize);
		if (m_inspectionList.ByteSize() > cMaxInspectionPbSize)
		{
			assert(false);
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InspectionPBTooLong, SvStl::SourceFileParams(StdMessageParams));
		}
		else
		{
			m_pCommonData->m_inspectionListPBSize = m_inspectionList.ByteSize();
			m_inspectionList.SerializeToArray(m_pInspectionListInSM, m_pCommonData->m_inspectionListPBSize);
		}
	}
}

}
