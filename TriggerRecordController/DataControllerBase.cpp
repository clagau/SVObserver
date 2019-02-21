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
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "CopyData.h"
#include "ImageBufferController.h"
#include "SVStatusLibrary\SVRegistry.h"
#pragma endregion Includes

namespace SvTrc
{
#pragma region Constructor
DataControllerBase::DataControllerBase()
{
	constexpr int handleForReset = 1000;
	SVRegistryClass reg(_T(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\\Windows\)"));
	DWORD value = 0;
	if (reg.GetRegistryValue(_T("GDIProcessHandleQuota"), &value) || 0 < value)
	{
		if (handleForReset < value)
		{
			m_maxNumberOfRequiredBuffer = static_cast<int>(value) - handleForReset;
		}
		else
		{
			m_maxNumberOfRequiredBuffer = static_cast<int>(value) * 0.9;
		}
	}

	PSECURITY_DESCRIPTOR psd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(psd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(psd, TRUE, NULL, FALSE);
	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = psd;
	sa.bInheritHandle = FALSE;

	m_hResetEvent = ::CreateEvent(&sa, false, false, GNameResetEvent);
	if (nullptr == m_hResetEvent)
	{
		DWORD errorCode = GetLastError();
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%x"), errorCode));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_EventCreation, msgList, SvStl::SourceFileParams(StdMessageParams));
		assert(false);
	}

	m_hTridEvent = ::CreateEvent(&sa, false, false, GNameTridEvent);
	if (nullptr == m_hTridEvent)
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

void DataControllerBase::clearAll()
{
	m_bufferVector.clear();
}

TriggerRecordData& DataControllerBase::getTRData(int inspectionPos, int pos) const
{
	auto* pData = getTRControllerData(inspectionPos);
	//do not check of the input parameter, because this is are private method and the parameter will be checked before in the calling method.
	assert(nullptr != pData);

	return pData->getTRData(pos);
}

void DataControllerBase::setLastFinishedTR(int inspectionPos, int id)
{
	auto* pData = getTRControllerData(inspectionPos);
	if (nullptr != pData)
	{
		pData->setLastFinishedTRID(id);
		SetEvent(m_hTridEvent);
	}
}

int DataControllerBase::getLastTRId(int inspectionPos) const
{
	int id = -1;
	auto* pData = getTRControllerData(inspectionPos);
	if (nullptr != pData)
	{
		id = pData->getBasicData().m_lastFinishedTRID;
	}
	return id;
}

const SvPb::ImageList& DataControllerBase::getImageDefList(int inspectionPos) const
{
	auto* pData = getTRControllerData(inspectionPos);
	assert(nullptr != pData);
	if (nullptr != pData && pData->getBasicData().m_bInit)
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
}