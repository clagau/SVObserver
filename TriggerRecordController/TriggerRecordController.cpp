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
#include "SVMatroxLibrary\SVMatroxResourceMonitor.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVMessage\SVMessage.h"
#include "SVProtoBuf\ConverterHelper.h"
#include "SVStatusLibrary\GlobalPath.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVUtilityLibrary\StringHelper.h"
#include "DataControllerLocal.h"
#include "DataControllerWriter.h"
#include "DataControllerReader.h"
#include "ImageBufferController.h"
#include "Image.h"
#include "ITriggerRecordControllerRW.h"
#include "ITriggerRecordRW.h"
#include "TriggerRecordController.h"
#include "TriggerRecordData.h"
#include "TriggerRecord.h"
#pragma endregion Includes
//#define TRACE_TRC

namespace SvTrc
{
int getAdditionalTriggerRecordNumber()
{
	SvLib::SVOINIClass l_SvimIni(SvStl::GlobalPath::Inst().GetSVIMIniPath());
	constexpr int cTriggerRecordAddOn = 2; //number of additional slots for internal use
	return l_SvimIni.GetValueInt(_T("TriggerRecordController"), _T("AdditionalTRNumber"), 5) + cTriggerRecordAddOn;
}

#pragma region Constructor
TriggerRecordController::TriggerRecordController(std::unique_ptr<DataControllerBase> pDataController) 
	: m_pDataController(std::move(pDataController))
	, m_imageBufferController(*m_pDataController)
	, m_additionalTriggerRecordNumber(getAdditionalTriggerRecordNumber())
{
	//The next call is only to avoid a crash at the end of the application. 
	//Reason of the crash was that SVMatroxResourceMonitor was destructed before the ImageBufferController, but this need it in its destructor.
	//If the singleton of SVMatroxResourceMonitor created before of the singleton of ImageBufferController the destruction it in the right order.
	//In Release-Mode this call do nothing.
	SVMatroxResourceMonitor::SVAutoLock autoLock;
	SVMatroxResourceMonitor::GetAutoLock(autoLock);
	m_pDataController->setResetCallback(std::bind(&TriggerRecordController::sendResetCall, this));
	m_pDataController->setReadyCallback(std::bind(&TriggerRecordController::sendReadyCall, this));
	m_pDataController->setNewTrIdCallback(std::bind(&TriggerRecordController::sendTrIdCall, this, std::placeholders::_1, std::placeholders::_2));	
}

TriggerRecordController::~TriggerRecordController()
{
	m_isResetLocked = false;
	clearAll();
}
#pragma endregion Constructor

#pragma region Public Methods
void TriggerRecordController::setLastFinishedTR(int inspectionPos, int id) 
{ 
	m_pDataController->setLastFinishedTR(inspectionPos, id); 
	sendTrIdCall(inspectionPos, id);
}


const SvPb::ImageList& TriggerRecordController::getImageDefList(int inspectionPos)
{
	if (-1 == m_resetStarted4IP)
	{
		return m_pDataController->getImageDefList(inspectionPos);
	}

	assert(false);
	SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
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
	SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetDataDefList, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

ITriggerRecordRPtr TriggerRecordController::createTriggerRecordObject(int inspectionPos, int trId)
{
	assert(-1 == m_resetStarted4IP);
	if (-1 == m_resetStarted4IP && 0 <= trId)
	{
		return m_pDataController->createTriggerRecordObject(inspectionPos, [trId](SvTrc::TriggerRecordData& rData) -> bool { return rData.m_trId == trId; });
	}
	return nullptr;
}

ITriggerRecordRPtr TriggerRecordController::createTriggerRecordObjectPerTriggerCount(int inspectionPos, int triggerCount)
{
	assert(-1 == m_resetStarted4IP);
	if (-1 == m_resetStarted4IP && 0 <= triggerCount)
	{
		return m_pDataController->createTriggerRecordObject(inspectionPos, [triggerCount](SvTrc::TriggerRecordData& rData) -> bool { return rData.m_triggerData.m_TriggerCount == triggerCount; });
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

int TriggerRecordController::registerNewTrCallback(std::function<void(int, int)> pCallback)
{
	assert(pCallback);
	if (nullptr != pCallback)
	{
		static int handleCounter = 0;
		std::lock_guard<std::mutex> guard(m_callbackMutex);
		int handle = handleCounter++;
		 m_newTRCallbacks.push_back(std::pair<int, std::function<void(int,int)>>(handle, pCallback));
		return handle;
	}
	return -1;
}

void TriggerRecordController::unregisterNewTrCallback(int handleId)
{
	std::lock_guard<std::mutex> guard(m_callbackMutex);
	auto Iter = find_if(m_newTRCallbacks.begin(), m_newTRCallbacks.end(), [handleId](const auto& rEntry)->bool { return rEntry.first == handleId; });
	if (m_newTRCallbacks.end() != Iter)
	{
		m_newTRCallbacks.erase(Iter);
	}
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
		m_newTRCallbacks.clear();
	}
	m_isResetLocked = false;

	m_pDataController->clearAll();
}

bool TriggerRecordController::setInspections(const SvPb::InspectionList& rInspectionList)
{
	if (m_isResetLocked)
	{
		return false;
	}

	bool result = m_pDataController->setInspections(rInspectionList);

	return result;
}

void TriggerRecordController::resizeIPNumberOfRecords(int inspectionPos, long newSize)
{
	if (m_isResetLocked)
	{
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetLocked, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	auto inspectionList = m_pDataController->getInspections();
	if (0 > inspectionPos || inspectionList.list_size() <= inspectionPos)
	{   //new start of reset is not allowed if reset is in progress.
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), inspectionPos));
		msgList.push_back(SvUl::Format(_T("%d"), inspectionList.list_size()));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetWrongInspectionId, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
	if (0 >= newSize || cMaxTriggerRecords < newSize)
	{
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), newSize));
		msgList.push_back(SvUl::Format(_T("%d"), cMaxTriggerRecords));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TriggerRecordSize2Big, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	auto* pIpData = inspectionList.mutable_list(inspectionPos);
	if (nullptr == pIpData)
	{
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), inspectionPos));
		msgList.push_back(SvUl::Format(_T("%d"), inspectionList.list_size()));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetWrongInspectionId, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}	

	if (pIpData->numberofrecords() == newSize ) 
	{
		return;
	}

	m_pDataController->prepareReset();

	pIpData->set_numberofrecords(newSize);

	if (m_pDataController->isIPInit(inspectionPos))
	{
		auto imageListTmp = m_pDataController->getImageDefList(inspectionPos);

		startResetTriggerRecordStructure(inspectionPos);

		m_pDataController->setInspectionList(inspectionList);

		m_TriggerRecordNumberResetTmp = newSize + m_additionalTriggerRecordNumber;
		m_imageListResetTmp = imageListTmp;
		auto* pImageStructList = m_imageStructListResetTmp.mutable_list();
		for (auto imageDef : m_imageListResetTmp.list())
		{
			auto pStructIter = std::find_if(pImageStructList->begin(), pImageStructList->end(), [&imageDef](auto data)->bool
			{
				return (imageDef.structid() == data.structid());
			});
			if (pImageStructList->end() != pStructIter)
			{
				auto pStructData = &(*pStructIter);
				pStructData->set_numberofbuffersrequired(pStructData->numberofbuffersrequired() + m_TriggerRecordNumberResetTmp);
			}
		}

		ResetTriggerRecordStructure();
	}
	else
	{
		m_pDataController->setInspectionList(inspectionList);
	}
}

ITriggerRecordRWPtr TriggerRecordController::createTriggerRecordObjectToWrite(int inspectionPos)
{
	assert(-1 == m_resetStarted4IP);

	if (-1 == m_resetStarted4IP)
	{
		ITriggerRecordRWPtr pTriggerRecord = m_pDataController->createTriggerRecordObjectToWrite(inspectionPos);
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

ITriggerRecordRPtr TriggerRecordController::closeWriteAndOpenReadTriggerRecordObject(ITriggerRecordRWPtr& pTriggerRecord)
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
			std::string DebugString = SvUl::Format(_T("\n\nsome trigger missing\n\n"));
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

void TriggerRecordController::closeWriteObjectWithoutUpdateLastTrId(ITriggerRecordRWPtr& pTriggerRecord)
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

IImagePtr TriggerRecordController::getImageBuffer(const SVMatroxBufferCreateStruct& rBufferStruct, bool createBufferExternIfNecessary) const
{
	IImagePtr retImage;
	if (-1 == m_resetStarted4IP)
	{
		retImage = m_imageBufferController.createNewImageHandle(rBufferStruct, m_pDataController->getResetId());
	}

	if (nullptr == retImage && createBufferExternIfNecessary)
	{
		SVMatroxBuffer ImageBufId;
		SVMatroxBufferInterface::Create(ImageBufId, rBufferStruct);
		SvOi::SVImageBufferHandlePtr pImagePtr = SvOi::SVImageBufferHandlePtr {new SVImageBufferHandleImage(ImageBufId)};
		retImage = std::make_shared<Image>(pImagePtr, 0, Image::cLocalTmpImagePos, false, false);
	}
	assert(nullptr != retImage);
	return retImage;
}

void TriggerRecordController::startResetTriggerRecordStructure(int inspectionPos)
{
	if (m_isResetLocked)
	{
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetLocked, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (-1 != m_resetStarted4IP)
	{   //new start of reset is not allowed if reset is in progress.
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), m_resetStarted4IP));
		msgList.push_back(SvUl::Format(_T("%d"), inspectionPos));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
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
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetWrongInspectionId, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (m_cResetStartedAddBuffer != inspectionPos)
	{
		int TriggerRecordSize = rInspectionList.list(inspectionPos).numberofrecords();
		if (0 >= TriggerRecordSize || cMaxTriggerRecords < TriggerRecordSize)
		{
			assert(false);
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), TriggerRecordSize));
			msgList.push_back(SvUl::Format(_T("%d"), cMaxTriggerRecords));
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TriggerRecordSize2Big, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}

		m_resetStarted4IP = inspectionPos;

		//clear imageList and reset ImageStructList
		int TRNumber = 0;
		try
		{
			m_imageListResetTmp = m_pDataController->getImageDefList(m_resetStarted4IP);
		}
		catch (const SvStl::MessageContainer&)
		{
			//nothing to do, because if 
			m_imageListResetTmp.Clear();
		}
		TRNumber = m_pDataController->getTriggerRecordNumber(m_resetStarted4IP);
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
					pImageStruct->set_numberofbuffersrequired(pImageStruct->numberofbuffersrequired() - TRNumber);
				}
			}
		}

		m_imageListResetTmp.Clear();

		m_TriggerRecordNumberResetTmp = TriggerRecordSize + m_additionalTriggerRecordNumber;
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
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
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
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetLocked, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (-1 != m_resetStarted4IP)
	{   //new start of reset is not allowed if reset is in progress.
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), m_resetStarted4IP));
		msgList.push_back(SvUl::Format(_T("%d"), -1));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
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
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetLocked, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (-1 != m_resetStarted4IP)
	{   //new start of reset is not allowed if reset is in progress.
		assert(false);
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), m_resetStarted4IP));
		msgList.push_back(SvUl::Format(_T("%d"), -1));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetAllReadyStarted, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (!m_isGlobalInit)
	{
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GlobalInitNotStarted, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	//ATTENTION: Order of the next line is important, because if getImageStructList is called m_isGlobalInit must be true to get the m_imageStructListGlobalInitTmp
	m_imageStructListResetTmp = m_pDataController->getImageStructList();
	m_isGlobalInit = false;
	m_pDataController->setGlobalInitFlag(false);
	ResetTriggerRecordStructure();
}

int TriggerRecordController::addOrChangeImage(const GUID& rImageId, const SVMatroxBufferCreateStruct& rBufferStruct, int inspectionPos /*= -1*/)
{
	ResetEnum resetEnum = calcResetEnum(inspectionPos);
	if (ResetEnum::Invalid == resetEnum)
	{   //Not possible to add and change image.
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (ResetEnum::NewReset == resetEnum)
	{
		int TriggerRecordSize = m_pDataController->getInspections().list(inspectionPos).numberofrecords();
		if (0 >= TriggerRecordSize || cMaxTriggerRecords < TriggerRecordSize)
		{
			assert(false);
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TriggerRecordSize2Big, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}


		//prefer reset
		m_resetStarted4IP = inspectionPos;
		m_TriggerRecordNumberResetTmp = TriggerRecordSize + m_additionalTriggerRecordNumber;
		try
		{
			m_imageListResetTmp = m_pDataController->getImageDefList(m_resetStarted4IP);
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
	auto bufferStructIter = std::find_if(pImageStructList->begin(), pImageStructList->end(), [typeStr](auto data)->bool
	{
		return (0 == data.type().compare(typeStr));
	});
	SvPb::ImageStructData* pStructData = nullptr;
	if (pImageStructList->end() != bufferStructIter)
	{
		pStructData = &(*bufferStructIter);
	}

	std::string ImageIdBytes;
	SvPb::SetGuidInProtoBytes(&ImageIdBytes, rImageId);
	auto* pList = m_imageListResetTmp.mutable_list();
	//check if image with this GUID already in list (this is not allowed.)
	auto pImageIter = std::find_if(pList->begin(), pList->end(), [&ImageIdBytes](auto data)->bool
	{
		return (0 == data.guidid().compare(ImageIdBytes));
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
		pImageDefinition->set_guidid(ImageIdBytes);
	}

	setStructData(pStructData, pImageDefinition, typeStr);
	if (ResetEnum::NewReset == resetEnum)
	{
		ResetTriggerRecordStructure();
	}
	return imagePos;
}

int TriggerRecordController::addOrChangeChildImage(const GUID& rImageId, const GUID& rParentId, const MatroxBufferChildDataStruct& rBufferStruct, int inspectionPos /* = -1*/)
{
	ResetEnum resetEnum = calcResetEnum(inspectionPos);
	if (ResetEnum::Invalid == resetEnum)
	{   //Not possible to add and change image.
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (ResetEnum::NewReset == resetEnum)
	{
		try
		{
			m_imageListResetTmp = m_pDataController->getImageDefList(inspectionPos);
		}
		catch (const SvStl::MessageContainer&)
		{
			//nothing to do, because if 
			m_imageListResetTmp.Clear();
		}
	}

	std::string ImageIdBytes;
	SvPb::SetGuidInProtoBytes(&ImageIdBytes, rImageId);
	auto* pList = m_imageListResetTmp.mutable_childlist();
	//check if image with this GUID already in list (this is not allowed.)
	auto imageIter = std::find_if(pList->begin(), pList->end(), [&ImageIdBytes](auto data)->bool
	{
		return (0 == data.guidid().compare(ImageIdBytes));
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
	pImageDefinition->set_guidid(ImageIdBytes);
	SvPb::SetGuidInProtoBytes(pImageDefinition->mutable_parentimageid(), rParentId);
	std::string typeStr(reinterpret_cast<const char*>(&rBufferStruct), sizeof(rBufferStruct));
	pImageDefinition->set_type(typeStr);

	if (ResetEnum::NewReset == resetEnum)
	{
		m_pDataController->setImageDefList(inspectionPos, std::move(m_imageListResetTmp));
	}
	return imagePos;
}

void TriggerRecordController::addImageBuffer(const GUID& ownerID, const SVMatroxBufferCreateStruct& bufferStruct, int numberOfBuffers, bool clearBuffer)
{
	if (m_isResetLocked)
	{   //addImageBuffer is not allowed if reset is not started.
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
	bool isNewReset = -1 == m_resetStarted4IP;

	auto& bufferMap = m_additionalBufferMap[ownerID];
	std::string typeString(reinterpret_cast<const char*>(&bufferStruct), sizeof(bufferStruct));
	if (isNewReset)
	{ //check if value have to be changed
		if (1 == bufferMap.size() || !clearBuffer)
		{
			auto& rImageStruct = m_pDataController->getImageStructList();
			auto imageIter = std::find_if(rImageStruct.list().begin(), rImageStruct.list().end(), [typeString](auto data)->bool
			{
				return (0 == data.type().compare(typeString));
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

		startResetTriggerRecordStructure();
	}
	
	auto* pList = m_imageStructListResetTmp.mutable_list();
	auto imageIter = std::find_if(pList->begin(), pList->end(), [typeString](auto data)->bool
	{
		return (0 == data.type().compare(typeString));
	});
	if (pList->end() != imageIter)
	{
		auto iter = bufferMap.find(imageIter->structid());
		if (bufferMap.end() != iter)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
			std::string DebugString = SvUl::Format(_T("addImageBuffer(%s): change from %d + (-%d + %d) for ImageType %d\n"), SVGUID(ownerID).ToString().c_str(), imageIter->numberofbuffersrequired(), iter->second, numberOfBuffers, imageIter->structid());
			::OutputDebugString(DebugString.c_str());
#endif
			int number = std::max(imageIter->numberofbuffersrequired() - iter->second + numberOfBuffers, numberOfBuffers);
			iter->second = numberOfBuffers;
			imageIter->set_numberofbuffersrequired(number);
		}
		else
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
			std::string DebugString = SvUl::Format(_T("addImageBuffer(%s): %d + %d for ImageType %d\n"), SVGUID(ownerID).ToString().c_str(), imageIter->numberofbuffersrequired(), numberOfBuffers, imageIter->structid());
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
		std::string DebugString = SvUl::Format(_T("addImageBuffer(%s): new %d for ImageType %d\n"), SVGUID(ownerID).ToString().c_str(), numberOfBuffers, pImageStructData->structid());
		::OutputDebugString(DebugString.c_str());
#endif
	}

	if (isNewReset)
	{
		ResetTriggerRecordStructure();
	}

	return;
}

bool TriggerRecordController::removeImageBuffer(const GUID& ownerID, const SVMatroxBufferCreateStruct& bufferStruct)
{
	if (-1 == m_resetStarted4IP || m_isResetLocked)
	{   //removeImageBuffer is not allowed if reset is not started.
		assert(false);
		return false;
	}
	auto& bufferMapIter = m_additionalBufferMap.find(SVGUID(ownerID));
	if (m_additionalBufferMap.end() != bufferMapIter)
	{
		std::string typeString(reinterpret_cast<const char*>(&bufferStruct), sizeof(bufferStruct));

		auto* pList = m_imageStructListResetTmp.mutable_list();
		auto imageIter = std::find_if(pList->begin(), pList->end(), [typeString](auto data)->bool
		{
			return (0 == data.type().compare(typeString));
		});
		if (pList->end() != imageIter)
		{
			auto iter = bufferMapIter->second.find(imageIter->structid());
			if (bufferMapIter->second.end() != iter)
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
				std::string DebugString = SvUl::Format(_T("removeImageBuffer(%s) numbers %d -  %d for ImageType %d\n"), SVGUID(ownerID).ToString().c_str(), imageIter->numberofbuffersrequired(), iter->second, imageIter->structid());
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

bool TriggerRecordController::removeAllImageBuffer(const GUID& ownerID)
{
	if (-1 == m_resetStarted4IP || m_isResetLocked)
	{   //removeAllImageBuffer is not allowed if reset is not started.
		assert(false);
		return false;
	}

#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
	std::string DebugString = SvUl::Format(_T("removeAllImageBuffer(%s)\n"), SVGUID(ownerID).ToString().c_str());
	::OutputDebugString(DebugString.c_str());
#endif

	auto& bufferMapIter = m_additionalBufferMap.find(SVGUID(ownerID));
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
void TriggerRecordController::changeDataDef(SvPb::DataDefinitionList&& rDataDefList, std::vector<_variant_t>&& rValueObjectList, int inspectionPos)
{
	ResetEnum resetEnum = calcResetEnum(inspectionPos);
	if (ResetEnum::Invalid == resetEnum)
	{   //Not possible to add and change image.
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	if (ResetEnum::NewReset == resetEnum)
	{
		//prefer reset
		m_resetStarted4IP = inspectionPos;
		m_TriggerRecordNumberResetTmp = m_pDataController->getTriggerRecordNumber(m_resetStarted4IP);
		m_imageListResetTmp = m_pDataController->getImageDefList(m_resetStarted4IP);
		m_imageStructListResetTmp.Clear();
		m_imageStructListResetTmp = m_pDataController->getImageStructList();
	}

	//Only obtain the size of the data
	m_pDataController->changeDataDef(std::move(rDataDefList), std::move(rValueObjectList), m_resetStarted4IP);
	
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
	for (auto& rMap : m_additionalBufferMap)
	{
		for (auto& rPair : rMap.second)
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
			SvPb::ImageList rImageDef = m_imageListResetTmp;
			int bufferCount = m_TriggerRecordNumberResetTmp;
			if (m_resetStarted4IP != i)
			{
				rImageDef = m_pDataController->getImageDefList(i);
				bufferCount = m_pDataController->getInspections().list(i).numberofrecords() + m_additionalTriggerRecordNumber;
			}
			
			for (auto imageData : rImageDef.list())
			{
				if (imageData.structid() < m_imageStructListResetTmp.list_size() && 0 <= imageData.structid())
				{
					auto pBufferStruct = m_imageStructListResetTmp.mutable_list(imageData.structid());
					pBufferStruct->set_numberofbuffersrequired(pBufferStruct->numberofbuffersrequired() + bufferCount);
				}
			}
		}
		catch (...)
		{
			//nothing to do
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
			recalcRequiredBuffer();
		}
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
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
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

void TriggerRecordController::sendTrIdCall(int inspectionPos, int trId)
{
	std::lock_guard<std::mutex> guard(m_callbackMutex);
	for (auto& newTrIdCallback : m_newTRCallbacks)
	{
		if (nullptr != newTrIdCallback.second)
		{
			newTrIdCallback.second(inspectionPos, trId);
		}
	}
}

void TriggerRecordController::reduceRequiredImageBuffer(const std::map<int, int>& bufferMap)
{
	auto* pList = m_imageStructListResetTmp.mutable_list();
	for (auto& structNumberPair : bufferMap)
	{
		auto imageIter = std::find_if(pList->begin(), pList->end(), [structNumberPair](auto data)->bool
		{
			return (structNumberPair.first == data.structid());
		});
		if (pList->end() != imageIter)
		{
			int number = std::max(imageIter->numberofbuffersrequired() - structNumberPair.second, 0);
			imageIter->set_numberofbuffersrequired(number);
		}
	}
}

#pragma endregion Private Methods

ITriggerRecordControllerRW& getTriggerRecordControllerRWInstance()
{
	auto& rTRC = getTriggerRecordControllerInstance();
	if (rTRC.isWritable())
	{
		return getTriggerRecordControllerInstance();
	}
	assert(false);
	SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
	Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NotWriteVersion, SvStl::SourceFileParams(StdMessageParams));
	Exception.Throw();
}

ITriggerRecordControllerR& getTriggerRecordControllerRInstance()
{
	return getTriggerRecordControllerInstance();
}

int getInspectionPos(const GUID& inspectionGuid)
{
	int Return = -1;
	std::string inspectionIdBytes;
	SvPb::SetGuidInProtoBytes(&inspectionIdBytes, inspectionGuid);
	auto& rInspectionList = getTriggerRecordControllerInstance().getInspections();
	const auto& Iter = std::find_if(rInspectionList.list().begin(), rInspectionList.list().end(), [inspectionIdBytes](const auto& item)-> bool
	{
		return (0 == item.id().compare(inspectionIdBytes));
	});
	if (rInspectionList.list().end() != Iter)
	{
		Return = static_cast<int>(std::distance(rInspectionList.list().begin(), Iter));
	}

	return Return;
}


std::shared_ptr<TriggerRecordController> g_pTriggerRecordController = nullptr;
void createTriggerRecordControllerInstance(TRC_DataType dataType)
{
	if (nullptr == g_pTriggerRecordController)
	{
		switch (dataType)
		{
			case SvTrc::TRC_DataType::Local:
				g_pTriggerRecordController = std::make_shared<TriggerRecordController>(std::make_unique<DataControllerLocal>());
				break;
			case SvTrc::TRC_DataType::Writer:
				g_pTriggerRecordController = std::make_shared<TriggerRecordController>(std::make_unique<DataControllerWriter>());
				break;
			case SvTrc::TRC_DataType::Reader:
				g_pTriggerRecordController = std::make_shared<TriggerRecordController>(std::make_unique<DataControllerReader>());
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
	if (nullptr != g_pTriggerRecordController)
	{
		g_pTriggerRecordController->clearAll();
		g_pTriggerRecordController = nullptr;
	}
}

TriggerRecordController& getTriggerRecordControllerInstance()
{
	if (nullptr != g_pTriggerRecordController)
	{
		return *g_pTriggerRecordController.get();
	}
	else
	{
		assert(false);
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetTRCFailed, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}
}

ImageBufferController& getImageBufferControllerInstance()
{
	return getTriggerRecordControllerInstance().getImageBufferControllerInstance();
}
} //namespace SvTrc
