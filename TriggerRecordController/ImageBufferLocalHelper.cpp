//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ImageBufferLocalHelper.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This class is the controller for the local memory and the image buffers.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ImageBufferLocalHelper.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\StringHelper.h"
#pragma warning( disable: 4244 )	//Disable warning for conversion
#include "SVProtoBuf/TriggerRecordController.h"
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
ImageBufferLocalHelper::ImageBufferLocalHelper(std::vector<SVMatroxBuffer>& rbufferVector)
	: m_rBufferVector(rbufferVector)
{
}

ImageBufferLocalHelper::~ImageBufferLocalHelper()
{
	clearAll();
}
#pragma endregion Constructor

#pragma region Public Methods
void ImageBufferLocalHelper::clearAll()
{
}

int ImageBufferLocalHelper::createMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos)
{
	SVMatroxBufferCreateStruct bufferStruct;
	memcpy(&bufferStruct, rImageStruct.type().c_str(), sizeof(SVMatroxBufferCreateStruct));
	for (int i = rImageStruct.numberofbuffers(); i < requiredNumbers; i++)
	{
		SVMatroxBuffer buffer;
		HRESULT errCode = SVMatroxBufferInterface::Create(buffer, bufferStruct);
		if (S_OK != errCode || buffer.empty())
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%X"), errCode));
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_CreateBuffer, msgList, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}
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
	return vectorPos;
}

int ImageBufferLocalHelper::contractMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos)
{
	if (rImageStruct.numberofbuffers() > requiredNumbers)
	{
		auto bufferIter = m_rBufferVector.begin() + vectorPos + requiredNumbers;
		m_rBufferVector.erase(bufferIter, bufferIter + (rImageStruct.numberofbuffers() - requiredNumbers));
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
