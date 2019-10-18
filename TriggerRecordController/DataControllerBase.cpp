//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file DataControllerBase.cpp
/// All Rights Reserved
//*****************************************************************************
/// Control of the Data in local mode.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "DataControllerBase.h"
#include "ImageBufferController.h"
#include "LocalConst.h"
#include "SVLibrary/SVOINIClass.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SVRegistry.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "ITriggerRecordControllerR.h"
#pragma endregion Includes

namespace SvTrc
{
int getNumberOfTRKeepFreeForWrite()
{
	SvLib::SVOINIClass l_SvimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());
	constexpr int cTriggerRecordMax = 10;
	int retVal = l_SvimIni.GetValueInt(_T("TriggerRecordController"), _T("NumberOfTRKeepFreeForWriter"), 2);
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
	SVRegistryClass reg(_T(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\\Windows\)"));
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

	SvLib::SVOINIClass l_SvimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());
	int maxBufferSizeInMB = l_SvimIni.GetValueInt(_T("TriggerRecordController"), _T("MaxBufferSizeInMBytes"), 8000);
	m_maxBufferSizeInBytes = maxBufferSizeInMB *c_MBInBytes;

	PSECURITY_DESCRIPTOR psd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(psd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(psd, TRUE, NULL, FALSE);
	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = psd;
	sa.bInheritHandle = FALSE;

	m_hResetEvent = ::CreateEvent(&sa, true, false, GNameResetEvent);
	if (nullptr == m_hResetEvent)
	{
		DWORD errorCode = GetLastError();
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%x"), errorCode));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_EventCreation, msgList, SvStl::SourceFileParams(StdMessageParams));
		assert(false);
	}

	m_hReadyEvent = ::CreateEvent(&sa, true, false, GNameReadyEvent);
	if (nullptr == m_hReadyEvent)
	{
		DWORD errorCode = GetLastError();
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%x"), errorCode));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_EventCreation, msgList, SvStl::SourceFileParams(StdMessageParams));
		assert(false);
	}

	m_hInterestTridEvent = ::CreateEvent(&sa, false, false, GNameInterestTridEvent);
	if (nullptr == m_hInterestTridEvent)
	{
		DWORD errorCode = GetLastError();
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%x"), errorCode));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_EventCreation, msgList, SvStl::SourceFileParams(StdMessageParams));
		assert(false);
	}
}

DataControllerBase::~DataControllerBase()
{
}
#pragma endregion Constructor
void DataControllerBase::clearImageBuffer(bool shouldResetImageStruct)
{
	m_bufferVector.clear();
}

void DataControllerBase::clearAll()
{
	clearImageBuffer();
}

TriggerRecordData& DataControllerBase::getTRData(int inspectionPos, int pos) const
{
	auto* pData = getTRControllerData(inspectionPos);
	//do not check of the input parameter, because this is are private method and the parameter will be checked before in the calling method.
	assert(nullptr != pData);

	return pData->getTRData(pos);
}

void DataControllerBase::setLastFinishedTr(const TrEventData& data)
{
	auto* pData = getTRControllerData(data.m_inspectionPos);
	if (nullptr != pData)
	{
		pData->setLastFinishedTRID(data.m_trId);
	}
}

int DataControllerBase::getLastTrId(int inspectionPos) const
{
	int id = -1;
	auto* pData = getTRControllerData(inspectionPos);
	if (nullptr != pData)
	{
		id = pData->getBasicData().m_lastFinishedTRID;
	}
	return id;
}

const SvPb::ImageList& DataControllerBase::getImageDefList(int inspectionPos, bool onlyIfInit) const
{
	auto* pData = getTRControllerData(inspectionPos);
	assert(nullptr != pData);
	if (nullptr != pData && (!onlyIfInit || pData->getBasicData().m_bInit))
	{
		return pData->getImageList();
	}

	SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetImageDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

int DataControllerBase::getTriggerRecordNumber(int inspectionPos) const
{
	auto* pData = getTRControllerData(inspectionPos);
	if (nullptr != pData)
	{
		return pData->getBasicData().m_TriggerRecordNumber;
	}
	return 0;
}

const SvPb::DataDefinitionList& DataControllerBase::getDataDefList(int inspectionPos) const
{
	auto* pData = getTRControllerData(inspectionPos);
	assert(nullptr != pData);
	if (nullptr != pData && pData->getBasicData().m_bInit)
	{
		return pData->getDataList();
	}

	SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetDataDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

bool DataControllerBase::isIPInit(int inspectionPos)
{
	auto* pData = getTRControllerData(inspectionPos);
	assert(nullptr != pData);
	if (nullptr != pData)
	{
		return pData->getBasicData().m_bInit;
	}
	return false;
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
	auto* pTrDataIp = getTRControllerData(inspectionPos);
	if (nullptr != pTrDataIp)
	{
		pTrDataIp->increaseFreeTrNumber();
	}
}

bool DataControllerBase::setTrOfInterest(const std::vector<std::pair<int, int>>& rTrVec)
{
	bool isSet = false;
	for (const auto& rPairVal : rTrVec)
	{
		if (!getPauseTrsOfInterest(rPairVal.first))
		{
			auto* pTrDataIp = getTRControllerData(rPairVal.first);
			if (nullptr != pTrDataIp && 0 < pTrDataIp->getBasicData().m_TrOfInterestNumber)
			{
				pTrDataIp->setTrOfInterest(rPairVal.first, rPairVal.second);
				isSet = true;
			}
		}
	}
	if (isSet)
	{
		SetEvent(m_hInterestTridEvent);
	}
	return isSet;
}
}
