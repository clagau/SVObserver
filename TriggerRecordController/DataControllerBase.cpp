//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DataControllerBase.cpp
/// All Rights Reserved
//*****************************************************************************
/// Control of the Data in local mode.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "DataControllerBase.h"
#include "LocalConst.h"
#include "SVLibrary/SVOINIClass.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/RegistryAccess.h"
#include "ObjectInterfaces/ITriggerRecordControllerR.h"
#include "TriggerRecordData.h"


#pragma endregion Includes

namespace SvTrc
{
int getNumberOfTRKeepFreeForWrite()
{
	SvLib::SVOINIClass SvimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());
	constexpr int cTriggerRecordMax = 10;
	int retVal = SvimIni.GetValueInt(_T("TriggerRecordController"), _T("NumberOfTRKeepFreeForWriter"), 2);
	return std::max(0, std::min(cTriggerRecordMax, retVal));
}
const int g_cNumberOfTRKeepFreeForWrite = getNumberOfTRKeepFreeForWrite();
int TRControllerBaseDataPerIP::getNumberOfTRKeepFreeForWrite() const
{
	return g_cNumberOfTRKeepFreeForWrite;
}

#pragma region Constructor
constexpr int handleForRest = 1000;
DataControllerBase::DataControllerBase()
{
	RegistryAccess reg(_T(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\\Windows\)"));
	DWORD value = 0;
	if (reg.GetRegistryValue(_T("GDIProcessHandleQuota"), &value) || 0 < value)
	{
		if (handleForRest < value)
		{
			m_maxNumberOfRequiredBuffer = static_cast<int>(value) - handleForRest;
		}
		else
		{
			m_maxNumberOfRequiredBuffer = static_cast<int>(value * 0.9);
		}
	}

	ULONGLONG physicalMemory_kB=0;
	BOOL ok = GetPhysicallyInstalledSystemMemory(&physicalMemory_kB);
	volatile ULONGLONG availablePhysicalMemory_GB = ok ? (physicalMemory_kB /(SvDef::cKilobytesPerGigabyte)) : SvDef::cMainMemoryInGigabyteX2C;
	// if GetPhysicallyInstalledSystemMemory() fails (which should only happen on virtual machines) we assume an X2C.

	bool memoryIsSmall = ok ? (availablePhysicalMemory_GB < 4ull*SvDef::cMainMemoryInGigabyteX2C) : false;
	// if we do not have have a machine at least like NEO2 (with 64 GB of memory) we assess the available memory to be "small"

	SvLib::SVOINIClass SvimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());
	int64_t maxBufferSizeInMB = SvimIni.GetValueInt(_T("TriggerRecordController"), _T("MaxBufferSizeInMBytes"), 0);

	if (0 == maxBufferSizeInMB)
	{ // use default buffer size (which is dependent on the SVIM model): see SVO-3435
		maxBufferSizeInMB = memoryIsSmall ? SvDef::cSmallTrcBufferSize_MB : SvDef::cBigTrcBufferSize_MB;
	}

	m_maxBufferSizeInBytes = maxBufferSizeInMB * static_cast<int64_t>(SvDef::cBytesPerMegabyte);

	auto psd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(psd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(psd, TRUE, nullptr, FALSE);
	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = psd;
	sa.bInheritHandle = FALSE;

	m_hResetEvent = ::CreateEvent(&sa, true, false, GNameResetEvent);
	if (nullptr == m_hResetEvent)
	{
		DWORD errorCode = GetLastError();
		SvDef::StringVector msgList;
		msgList.push_back(std::format(_T("{:x}"), errorCode));
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_EventCreation, msgList, SvStl::SourceFileParams(StdMessageParams));
		assert(false);
	}

	m_hReadyEvent = ::CreateEvent(&sa, true, false, GNameReadyEvent);
	if (nullptr == m_hReadyEvent)
	{
		DWORD errorCode = GetLastError();
		SvDef::StringVector msgList;
		msgList.push_back(std::format(_T("{:x}"), errorCode));
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_EventCreation, msgList, SvStl::SourceFileParams(StdMessageParams));
		assert(false);
	}
}

DataControllerBase::~DataControllerBase()
{
}
#pragma endregion Constructor
void DataControllerBase::clearImageBuffer(bool)
{
	m_bufferVector.clear();
}

void DataControllerBase::clearAll()
{
	m_isGlobalInit = false;
	m_setInitFlagAfterResetSet.clear();
	clearImageBuffer();
}

TriggerRecordData& DataControllerBase::getTRData(int inspectionPos, int pos) const
{
	auto* pData = getTRControllerData(inspectionPos);
	//do not check of the input parameter, because this is are private method and the parameter will be checked before in the calling method.
	Log_Assert(nullptr != pData);

	return pData->getTRData(pos);
}

void DataControllerBase::setLastFinishedTr(const SvOi::TrEventData& data)
{
	try
	{
		auto* pData = getTRControllerData(data.m_inspectionPos);
		if (nullptr != pData)
		{
			pData->setLastFinishedTRID(data.m_trId);
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		//This is the topmost catch for MessageContainer exceptions
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(rExp.getMessage());
	}
}

int DataControllerBase::getLastTrId(int inspectionPos) const
{
	int id = -1;
	try
	{
		auto* pData = getTRControllerData(inspectionPos);
		if (nullptr != pData)
		{
			id = pData->getBasicData().m_lastFinishedTRID;
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		//This is the topmost catch for MessageContainer exceptions
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(rExp.getMessage());
	}
	return id;
}

const SvPb::ImageList& DataControllerBase::getImageDefList(int inspectionPos, bool onlyIfInit) const
{
	auto* pData = getTRControllerData(inspectionPos);
	Log_Assert(nullptr != pData);
	if (nullptr != pData && (!onlyIfInit || pData->getBasicData().m_bInit))
	{
		return pData->getImageList();
	}

	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetImageDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

const std::unordered_map<uint32_t, int>& DataControllerBase::getImageMap(int inspectionPos) const
{
	auto* pData = getTRControllerData(inspectionPos);
	Log_Assert(nullptr != pData);
	if (nullptr != pData && pData->getBasicData().m_bInit)
	{
		return pData->getImageMap();
	}

	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetImageDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

const std::unordered_map<uint32_t, int>& DataControllerBase::getChildImageMap(int inspectionPos) const
{
	auto* pData = getTRControllerData(inspectionPos);
	Log_Assert(nullptr != pData);
	if (nullptr != pData && pData->getBasicData().m_bInit)
	{
		return pData->getChildImageMap();
	}

	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetImageDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

const std::unordered_map<uint32_t, std::pair<bool, int>>& DataControllerBase::getLinkedImageMap(int inspectionPos) const
{
	auto* pData = getTRControllerData(inspectionPos);
	Log_Assert(nullptr != pData);
	if (nullptr != pData && pData->getBasicData().m_bInit)
	{
		return pData->getLinkedImageMap();
	}

	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetImageDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

int DataControllerBase::getTriggerRecordNumber(int inspectionPos) const
{
	try
	{
		auto* pData = getTRControllerData(inspectionPos);
		if (nullptr != pData)
		{
			return pData->getBasicData().m_TriggerRecordNumber;
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		//This is the topmost catch for MessageContainer exceptions
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(rExp.getMessage());
	}
	return 0;
}

const SvPb::DataDefinitionList& DataControllerBase::getDataDefList(int inspectionPos) const
{
	auto* pData = getTRControllerData(inspectionPos);
	Log_Assert(nullptr != pData);
	if (nullptr != pData && pData->getBasicData().m_bInit)
	{
		return pData->getDataList();
	}

	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetDataDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

const std::unordered_map<uint32_t, int>& DataControllerBase::getDataDefMap(int inspectionPos) const
{
	auto* pData = getTRControllerData(inspectionPos);
	Log_Assert(nullptr != pData);
	if (nullptr != pData && pData->getBasicData().m_bInit)
	{
		return pData->getDataMap();
	}

	SvStl::MessageManager Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetDataDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

void DataControllerBase::sendReadyCallbackIfReady(const std::function<void()>& readyCallback) const
{
	if (readyCallback && isReady())
	{
		readyCallback();
	}
}

bool DataControllerBase::isIPInit(int inspectionPos)
{
	try
	{
		auto* pData = getTRControllerData(inspectionPos);
		Log_Assert(nullptr != pData);
		if (nullptr != pData)
		{
			return pData->getBasicData().m_bInit;
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		//This is the topmost catch for MessageContainer exceptions
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(rExp.getMessage());
	}
	return false;
}

void DataControllerBase::createTriggerRecordsBuffer(int inspectionPos, int trNumbers)
{
	auto* pData = getTRControllerData(inspectionPos);
	Log_Assert(nullptr != pData);
	if (nullptr != pData)
	{
		pData->createTriggerRecordsBuffer(trNumbers);
	}
}

void DataControllerBase::prepareReset()
{
	if (m_reloadCallback)
	{
		m_reloadCallback();
	}
}

void DataControllerBase::increaseNumberOfFreeTr(int inspectionPos)
{
	try
	{
		auto* pTrDataIp = getTRControllerData(inspectionPos);
		if (nullptr != pTrDataIp)
		{
			pTrDataIp->increaseFreeTrNumber();
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		//This is the topmost catch for MessageContainer exceptions
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(rExp.getMessage());
	}
}

bool DataControllerBase::setTrOfInterest(const std::vector<InterestStruct>& rTrVec)
{
	bool isSet = false;
	for (const auto& rData : rTrVec)
	{
		try
		{
			auto* pTrDataIp = getTRControllerData(rData.m_ipPos);
			if (nullptr != pTrDataIp)
			{
				if (rData.m_isInterest && !getPauseTrsOfInterest(rData.m_ipPos) && 0 < pTrDataIp->getBasicData().m_TrOfInterestNumber)
				{
					pTrDataIp->setTrOfInterest(rData.m_ipPos, rData.m_trPos);
					isSet = true;
				}
				pTrDataIp->setLastSetOfInterestFlagPos(rData.m_trPos);
			}
		}
		catch (const SvStl::MessageContainer& rExp)
		{
			//This is the topmost catch for MessageContainer exceptions
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(rExp.getMessage());
		}
	}
	return isSet;
}
}
