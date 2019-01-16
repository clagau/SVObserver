//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file ImageBufferController.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This class is the controller of the whole image buffers.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ImageBufferController.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateChildStruct.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "IImage.h"
#include "Image.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\StringHelper.h"
#include "SVStatusLibrary\SVRegistry.h"
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
ImageBufferController& ImageBufferController::getImageBufferControllerInstance()
{
	static ImageBufferController controller;

	return controller;
}

ImageBufferController::ImageBufferController()
	: m_memoryHelper(m_bufferVector)
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
}

ImageBufferController::~ImageBufferController()
{
	clearAll();
}
#pragma endregion Constructor

#pragma region Public Methods
void ImageBufferController::clearAll()
{
	m_bufferVector.clear();
	delete[] m_imageRefCountArray;
	m_imageRefCountArray = nullptr;
	m_imageRefCountSize = 0;
	m_imageStructList = SvPb::ImageStructList();

	m_memoryHelper.clearAll();
}

std::vector<std::pair<int, int>> ImageBufferController::reset(const SvPb::ImageStructList& rImageStructList)
{
	int groupPos = 0;
	int vectorPos = 0;

	assert(rImageStructList.list_size() >= m_imageStructList.list_size());
	assert(0 == m_imageStructList.list_size() || rImageStructList.list(m_imageStructList.list_size() - 1).structid() == m_imageStructList.list(m_imageStructList.list_size() - 1).structid());

	int completeNumberOfRequiredBuffer = 0;
	for (const auto& rImageStruct : rImageStructList.list())
	{
		completeNumberOfRequiredBuffer += rImageStruct.numberofbuffersrequired();
	}
	if (m_maxNumberOfRequiredBuffer < completeNumberOfRequiredBuffer)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), completeNumberOfRequiredBuffer));
		msgList.push_back(SvUl::Format(_T("%d"), m_maxNumberOfRequiredBuffer));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetBuffer_TooMany, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	//step through new imageSize list to fit buffer to the new required structure
	for (const auto& rImageStruct : rImageStructList.list())
	{
		SvPb::ImageStructData* pStructData = nullptr;
		if (m_imageStructList.list_size() > groupPos)
		{
			pStructData = m_imageStructList.mutable_list(groupPos);
			if (0 == rImageStruct.numberofbuffersrequired())
			{
				//this size is not longer used, delete the buffers.
				auto bufferIter = m_bufferVector.begin() + vectorPos;
				m_bufferVector.erase(bufferIter, bufferIter + pStructData->numberofbuffers());
				//memoryName temporary store, because it will be with the next erase
				std::string memoryName = pStructData->memoryname();
				m_imageStructList.mutable_list()->erase(m_imageStructList.list().begin() + groupPos);

				m_memoryHelper.removeMemory(memoryName);;
				continue;
			}
			else
			{	
				if (pStructData->numberofbuffers() > rImageStruct.numberofbuffersrequired())
				{
					auto bufferIter = m_bufferVector.begin() + vectorPos + rImageStruct.numberofbuffersrequired();
					m_bufferVector.erase(bufferIter, bufferIter + (pStructData->numberofbuffers()-rImageStruct.numberofbuffersrequired()));
					pStructData->set_numberofbuffers(rImageStruct.numberofbuffersrequired());
				}
				//this size is used, move vectorPos to the end of this bufferSize in Vector.
				vectorPos += pStructData->numberofbuffers();
			}
		}
		else
		{	//new size, add it to the image size list
			pStructData = m_imageStructList.add_list();
			pStructData->set_type(rImageStruct.type());
		}

		int requiredNumbers = rImageStruct.numberofbuffersrequired();
		pStructData->set_numberofbuffersrequired(requiredNumbers);
		pStructData->set_structid(rImageStruct.structid());

		if (pStructData->numberofbuffers() < requiredNumbers)
		{ //add more buffer of this size if necessary
			try
			{
				vectorPos = m_memoryHelper.createMilBufferinMemory(requiredNumbers, *pStructData, vectorPos);
			}
			catch (const SvStl::MessageContainer& rSvE)
			{
				clearAll();
				//This is the topmost catch for MessageContainer exceptions
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
				Exception.setMessage(rSvE.getMessage());
				Exception.Throw();
			}

			pStructData->set_numberofbuffers(requiredNumbers);
		}

		//correct offset
		if (0 < groupPos)
		{
			pStructData->set_offset(m_imageStructList.list(groupPos - 1).offset() + m_imageStructList.list(groupPos - 1).numberofbuffers());
		}
		else
		{
			pStructData->set_offset(0);
		}

		groupPos++;
	}

	//resize the reference counter array if required and set the counter to 0
	if (m_bufferVector.size() > m_imageRefCountSize)
	{
		delete[] m_imageRefCountArray;
		m_imageRefCountSize = static_cast<int>(vectorPos);
		m_imageRefCountArray = new long[m_imageRefCountSize];
	}
	memset(m_imageRefCountArray, 0, sizeof(long)*m_imageRefCountSize);

	//update structId to fit to the position in m_imageStructList
	int i = 0;
	std::vector<std::pair<int, int>> moveVec;
	for (auto& rImageStructData : (*m_imageStructList.mutable_list()))
	{
		//old structId must be equal or higher (it is higher if an size-entry was deleted.)
		int oldId = rImageStructData.structid();
		assert(oldId >= i);
		if (oldId > i)
		{
			//Because oldID is 
			moveVec.push_back({oldId, i});
			rImageStructData.set_structid(i);
		}
		i++;
	}

	return moveVec;
}

bool ImageBufferController::increaseRefCounter(int pos)
{
	if (0 <= pos &&  m_imageRefCountSize > pos)
	{
		InterlockedIncrement(&m_imageRefCountArray[pos]);
		return true;
	}
	return false;
}

bool ImageBufferController::decreaseRefCounter(int pos)
{
	if (0 <= pos &&  m_imageRefCountSize > pos)
	{
		long value = InterlockedDecrement(&m_imageRefCountArray[pos]);
		if (0 > value)
		{
			assert(false);
			SvStl::MessageMgrStd e(SvStl::MsgType::Log);
			e.setMessage(SVMSG_TRC_GENERAL_WARNING, SvStl::Tid_TRC_Error_ImageRefCount, SvStl::SourceFileParams(StdMessageParams));
			InterlockedExchange(&m_imageRefCountArray[pos], 0);
			return false;
		}
		return true;
	}

	return false;
}

IImagePtr ImageBufferController::getImage(int pos, long resetId, bool shouldUnlockAutomatically) const
{
	IImagePtr pImage;
	if (0 <= pos &&  m_bufferVector.size() > pos)
	{
		SvOi::SVImageBufferHandlePtr pImagePtr = SvOi::SVImageBufferHandlePtr {new SVImageBufferHandleImage(m_bufferVector[pos])};
		pImage = IImagePtr {new Image(pImagePtr, resetId, pos, true, shouldUnlockAutomatically)};
	}
	return pImage;
}

IImagePtr ImageBufferController::getChildImage(int parentPos, const MatroxBufferChildDataStruct& rBufferDataStruct, long resetId, bool shouldUnlockAutomatically) const
{
	IImagePtr pImage;
	if (0 <= parentPos &&  m_bufferVector.size() > parentPos)
	{
		SVMatroxBufferCreateChildStruct bufferStruct(m_bufferVector[parentPos]);
		bufferStruct.m_data = rBufferDataStruct;
		pImage = getChildImage(bufferStruct, resetId, parentPos, shouldUnlockAutomatically);

	}
	return pImage;
}

SvTrc::IImagePtr ImageBufferController::getChildImage(const SVMatroxBufferCreateChildStruct& rBufferStruct, long resetId, int bufferPos, bool shouldUnlockAutomatically) const
{
	IImagePtr pImage;
	SVMatroxBuffer buffer;
	SVMatroxBufferInterface::Create(buffer, rBufferStruct);
	if (!buffer.empty())
	{
		SvOi::SVImageBufferHandlePtr pImagePtr = SvOi::SVImageBufferHandlePtr {new SVImageBufferHandleImage(buffer)};
		pImage = IImagePtr {new Image(pImagePtr, resetId, bufferPos, true, shouldUnlockAutomatically)};
	}
	return pImage;
}

IImagePtr ImageBufferController::createNewImageHandle(const SVMatroxBufferCreateStruct& bufferStruct, long resetId) const
{
	auto imageSizeIter = std::find_if(m_imageStructList.list().begin(), m_imageStructList.list().end(), [bufferStruct](auto data)->bool
	{
		SVMatroxBufferCreateStruct bufferStruct2;
		memcpy(&bufferStruct2, data.type().c_str(), sizeof(SVMatroxBufferCreateStruct));
		return bufferStruct == bufferStruct2;
	});
	if (m_imageStructList.list().end() != imageSizeIter)
	{
		int imagePos = -1;
		return createNewImageHandle(imageSizeIter->structid(), imagePos, resetId, true);
	}
	return nullptr;
}

IImagePtr ImageBufferController::createNewImageHandle(int structId, int& rImagePos, long resetId, bool shouldUnlockAutomatically) const
{
	auto& rList = m_imageStructList.list();
	if (0 <= structId &&  rList.size() > structId)
	{
		int offset = rList[structId].offset();
		for (int i = 0; i < rList[structId].numberofbuffers(); i++)
		{
			int imagePosTmp = i + offset;
			long value = m_imageRefCountArray[imagePosTmp];
			if (0 == value)
			{
				if (InterlockedCompareExchange(&(m_imageRefCountArray[imagePosTmp]), value + 1, value) == value)
				{
					rImagePos = imagePosTmp;
					SvOi::SVImageBufferHandlePtr pImagePtr = SvOi::SVImageBufferHandlePtr {new SVImageBufferHandleImage(m_bufferVector[rImagePos])};
					return IImagePtr {new Image(pImagePtr, resetId, rImagePos, false, shouldUnlockAutomatically)};
				}
			}
		}

		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), structId));
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NoBufferFree, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	else
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), structId));
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_UnknownStructID, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	return nullptr;
}

void ImageBufferController::reduceRequiredBuffers(const SvPb::ImageList& imageList, int numbers)
{
	for (const auto& rSizeData : imageList.list())
	{
		int id = rSizeData.structid();
		if (0 <= id && m_imageStructList.list_size() > id)
		{
			auto* pImageStruct = m_imageStructList.mutable_list(id);
			if (nullptr != pImageStruct)
			{
				assert(0 < pImageStruct->numberofbuffersrequired() - numbers);
				pImageStruct->set_numberofbuffersrequired(pImageStruct->numberofbuffersrequired() - numbers);
			}
		}
	}
}
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods
#pragma endregion Private Methods
} //namespace SvTrc
