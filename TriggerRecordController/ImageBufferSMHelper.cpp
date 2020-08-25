//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ImageBufferSMHelper.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This class is the controller for the shared memory and the image buffers.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ImageBufferController.h"
#include "ImageBufferSMHelper.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/MatroxImageProps.h"
#include "SVMessage/SVMessage.h"
#include "SVProtoBuf/TriggerRecordController.h"
#include "SVSharedMemoryLibrary/SharedDataStore.h"
#include "SVSharedMemoryLibrary/SMParameterStruct.h"
#include "SVSharedMemoryLibrary/SVSharedMemorySettings.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvTrc
{
#pragma region Constructor
ImageBufferSMHelper::ImageBufferSMHelper(std::vector<SVMatroxBuffer>& rbufferVector, int maxNumberOfRequiredBuffer)
	: m_rBufferVector(rbufferVector)
	, m_maxNumberOfRequiredBuffer(maxNumberOfRequiredBuffer)
{
}

ImageBufferSMHelper::~ImageBufferSMHelper()
{
	clearAll();
}
#pragma endregion Constructor

#pragma region Public Methods
void ImageBufferSMHelper::clearAll()
{
	m_sharedMemoryMap.clear();
}

void ImageBufferSMHelper::removeMemory(std::string memoryName)
{
	std::vector<std::string> words;
	boost::algorithm::split(words, memoryName, [](char c) { return c == ';'; }, boost::token_compress_off);

	for (auto name : words)
	{
		m_sharedMemoryMap.erase(name);
	}
}

int ImageBufferSMHelper::createMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos)
{
	static int memoryCounter = 0;
	vectorPos -= rImageStruct.numberofbuffers();
	assert(0 <= vectorPos);
	//old memory must be free
	auto bufferIter = m_rBufferVector.begin() + vectorPos;
	assert(m_rBufferVector.size() >= vectorPos+ rImageStruct.numberofbuffers());
	m_rBufferVector.erase(bufferIter, bufferIter + rImageStruct.numberofbuffers());
	removeMemory(rImageStruct.memoryname());

	SVMatroxBufferCreateStruct bufferStruct;
	MatroxImageProps bufferProps;
	fillBufferData(rImageStruct, bufferStruct, bufferProps);

	std::vector<std::string> newMemoryNames;
	newMemoryNames.emplace_back(SvUl::Format("TRC.Images%d", memoryCounter++));
	SvSml::SMParameterStruct smParam(SvSml::SVSharedMemorySettings::DefaultConnectionTimout, SvSml::SVSharedMemorySettings::DefaultCreateWaitTime);
	m_sharedMemoryMap[newMemoryNames[0]] = std::make_unique<SvSml::SharedDataStore>();
	
	int requiredPerStore = requiredNumbers;
	bool isOk = false;
	do 
	{
		try
		{ 
			int divFactor = static_cast<int>(newMemoryNames.size());
			requiredPerStore = (requiredNumbers + divFactor - 1) / divFactor;
			for (auto memoryName : newMemoryNames)
			{
				m_sharedMemoryMap[memoryName]->CreateDataStore(memoryName.c_str(), static_cast<DWORD> (bufferProps.Bytesize), requiredPerStore, smParam);
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
				std::string DebugString = SvUl::Format(_T("ImageBufferSMHelper SM: %d - %s\n"), rImageStruct.structid(), memoryName.c_str());
				::OutputDebugString(DebugString.c_str());
#endif
			}
			isOk = true;
		}
		catch (const SvStl::MessageContainer& rExp)
		{ //if exception thrown because size bigger than DWORD, split it in more stores
			if (SvStl::Tid_SharedMemorySizeTooBig == rExp.getMessage().m_AdditionalTextId)
			{
				auto newName = SvUl::Format("TRC.Images%d", memoryCounter++);
				m_sharedMemoryMap[newName] = std::make_unique<SvSml::SharedDataStore>();
				newMemoryNames.emplace_back(newName);
			}
			else
			{	//if exception is not because size bigger than DWORD, throw further
				throw;
			}
		}
	} 
	while (!isOk);

	for (int i = 0; i < requiredNumbers; i++)
	{
		int pos = i / requiredPerStore;
		auto& rSMPointer = m_sharedMemoryMap[newMemoryNames[pos]];
		BYTE* ptr = (nullptr != rSMPointer)? rSMPointer->GetPtr(i-requiredPerStore*pos, 0) : nullptr;
		SVMatroxBuffer buffer;
		HRESULT errCode = SVMatroxBufferInterface::CreateBuffer(buffer, bufferProps, ptr);
		if (S_OK != errCode || buffer.empty())
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%X"), errCode));
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_CreateBuffer, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}
		else
		{	//set Buffer to zero. Also needed because the first use of the buffer may require more time and therefore should be avoided in RunMode.
			SVMatroxBufferInterface::ClearBuffer(buffer, 0);
		}
		if (m_rBufferVector.size() < m_maxNumberOfRequiredBuffer)
		{
			if (m_rBufferVector.size() <= vectorPos)
			{
				m_rBufferVector.push_back(buffer);
			}
			else
			{
				m_rBufferVector.insert(m_rBufferVector.begin() + vectorPos, buffer);
			}
			vectorPos++;
		}
		else
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), m_maxNumberOfRequiredBuffer));
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TooManyImageBuffer, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}
	}
	if (1 == newMemoryNames.size())
	{
		rImageStruct.set_memoryname(newMemoryNames[0]);
	}
	else
	{
		std::string newMemoryCompleteName;
		for (auto name : newMemoryNames)
		{
			newMemoryCompleteName += name + ";";
		}
		rImageStruct.set_memoryname(std::move(newMemoryCompleteName));
	}

	return vectorPos;
}

int ImageBufferSMHelper::contractMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos)
{
	if (rImageStruct.numberofbuffers() > requiredNumbers)
	{
		createMilBufferinMemory(requiredNumbers, rImageStruct, vectorPos + rImageStruct.numberofbuffers());
		rImageStruct.set_numberofbuffers(requiredNumbers);
	}
	return vectorPos + rImageStruct.numberofbuffers();
}

#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods

#pragma endregion Private Methods
} //namespace SvTrc
