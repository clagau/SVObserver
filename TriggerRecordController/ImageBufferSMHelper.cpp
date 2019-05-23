//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ImageBufferSMHelper.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This class is the controller for the shared memory and the image buffers.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
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
	m_sharedMemoryMap.erase(memoryName);
}

int ImageBufferSMHelper::createMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos)
{
	static int memoryCounter = 0;
	vectorPos -= rImageStruct.numberofbuffers();

	//old memory must be free
	auto bufferIter = m_rBufferVector.begin() + vectorPos;
	m_rBufferVector.erase(bufferIter, bufferIter + rImageStruct.numberofbuffers());
	removeMemory(rImageStruct.memoryname());

	SVMatroxBufferCreateStruct bufferStruct;
	MatroxImageProps bufferProps;
	fillBufferData(rImageStruct, bufferStruct, bufferProps);

	std::string newMemoryName = SvUl::Format("TRC.Images%d", memoryCounter++);
	SvSml::SMParameterStruct smParam(SvSml::SVSharedMemorySettings::DefaultConnectionTimout, SvSml::SVSharedMemorySettings::DefaultCreateWaitTime);

	auto& rSMPointer = m_sharedMemoryMap[newMemoryName] = std::make_unique<SvSml::SharedDataStore>();
	rSMPointer->CreateDataStore(newMemoryName.c_str(), static_cast<DWORD> (bufferProps.Bytesize), requiredNumbers, smParam);

	for (int i = 0; i < requiredNumbers; i++)
	{
		BYTE* ptr = rSMPointer->GetPtr(i, 0);
		SVMatroxBuffer buffer;
		HRESULT errCode = SVMatroxBufferInterface::CreateBuffer(buffer, bufferProps, ptr);
		if (S_OK != errCode || buffer.empty())
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%X"), errCode));
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_CreateBuffer, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
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
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_TooManyImageBuffer, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}
	}
	rImageStruct.set_memoryname(std::move(newMemoryName));

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
void ImageBufferSMHelper::fillBufferData(SvPb::ImageStructData& rImageStruct, SVMatroxBufferCreateStruct& rBufferStruct, MatroxImageProps& rBufferProps)
{
	memcpy(&rBufferStruct, rImageStruct.type().c_str(), sizeof(SVMatroxBufferCreateStruct));
	if (0 < rImageStruct.imageprops().size())
	{
		memcpy(&rBufferProps, rImageStruct.imageprops().c_str(), sizeof(MatroxImageProps));
	}
	else
	{
		SVMatroxBuffer buffer;
		HRESULT errCode = SVMatroxBufferInterface::Create(buffer, rBufferStruct);
		if (S_OK == errCode && !buffer.empty())
		{
			SVMatroxBufferInterface::InquireBufferProperties(buffer, rBufferProps);
			std::string typeStr(reinterpret_cast<const char*>(&rBufferProps), sizeof(MatroxImageProps));
			rImageStruct.set_imageprops(typeStr);
		}
	}
}

#pragma endregion Private Methods
} //namespace SvTrc
