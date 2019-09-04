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
#include "IImage.h"
#include "Image.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateChildStruct.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVUtilityLibrary\StringHelper.h"
#include "SVMatroxLibrary\MatroxImageProps.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvTrc
{
void fillBufferData(SvPb::ImageStructData& rImageStruct, SVMatroxBufferCreateStruct& rBufferStruct, MatroxImageProps& rBufferProps)
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

#pragma region Constructor
ImageBufferController::ImageBufferController(DataControllerBase& rDataController)
	: m_rDataController(rDataController)
{
}

ImageBufferController::~ImageBufferController()
{
	m_rDataController.clearImageBuffer();
}
#pragma endregion Constructor

#pragma region Public Methods
std::vector<std::pair<int, int>> ImageBufferController::reset(const SvPb::ImageStructList& rImageStructList, bool isGlobalInit)
{
	int completeNumberOfRequiredBuffer = 0;
	for (const auto& rImageStruct : rImageStructList.list())
	{
		completeNumberOfRequiredBuffer += rImageStruct.numberofbuffersrequired();
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		std::string DebugString = SvUl::Format(_T("Required image %d for ImageType %d"), rImageStruct.numberofbuffersrequired(), rImageStruct.structid());
		if (0 < rImageStruct.imageprops().size())
		{
			MatroxImageProps debugBufferProps;
			memcpy(&debugBufferProps, rImageStruct.imageprops().c_str(), sizeof(MatroxImageProps));
			DebugString += SvUl::Format(_T(" (%d/%d = %lld)"), debugBufferProps.sizeX, debugBufferProps.sizeY, debugBufferProps.Bytesize);
		}
		else
		{
			auto imageStructTmp = rImageStruct;
			SVMatroxBufferCreateStruct bufferStruct;
			MatroxImageProps bufferProps;
			fillBufferData(imageStructTmp, bufferStruct, bufferProps);
			DebugString += SvUl::Format(_T(" (%d/%d = %lld)"), bufferProps.sizeX, bufferProps.sizeY, bufferProps.Bytesize);
		}
		
		::OutputDebugString((DebugString+_T("\n")).c_str());
#endif
	}
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
	std::string DebugString = SvUl::Format(_T("Complete required image %d\n\n"), completeNumberOfRequiredBuffer);
	::OutputDebugString(DebugString.c_str());
#endif
	if (m_rDataController.getMaxNumberOfRequiredBuffer() < completeNumberOfRequiredBuffer)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), completeNumberOfRequiredBuffer));
		msgList.push_back(SvUl::Format(_T("%d"), m_rDataController.getMaxNumberOfRequiredBuffer()));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetBuffer_TooMany, msgList, SvStl::SourceFileParams(StdMessageParams));
		Exception.Throw();
	}

	std::vector<std::pair<int, int>> moveVec;
	if (!isGlobalInit)
	{
		SvPb::ImageStructList newImageStructList = m_rDataController.getImageStructList();
		assert(rImageStructList.list_size() >= newImageStructList.list_size());
		assert(0 == newImageStructList.list_size() || rImageStructList.list(newImageStructList.list_size() - 1).structid() == newImageStructList.list(newImageStructList.list_size() - 1).structid());
		auto& rBufVec = m_rDataController.getBufferVectorRef();

		int groupPos = 0;
		int vectorPos = 0;
		long long neededBufferSpace = 0;
		//step through new imageSize list to fit buffer to the new required structure
		for (const auto& rImageStruct : rImageStructList.list())
		{
			SvPb::ImageStructData* pStructData = nullptr;
			if (newImageStructList.list_size() > groupPos)
			{
				pStructData = newImageStructList.mutable_list(groupPos);
				if (0 == rImageStruct.numberofbuffersrequired())
				{
					//this size is not longer used, delete the buffers.
					auto bufferIter = rBufVec.begin() + vectorPos;
					rBufVec.erase(bufferIter, bufferIter + pStructData->numberofbuffers());
					//memoryName temporary store, because it will be with the next erase
					std::string memoryName = pStructData->memoryname();
					newImageStructList.mutable_list()->erase(newImageStructList.list().begin() + groupPos);

					m_rDataController.removeImageMemory(memoryName);
					continue;
				}
				else
				{
					vectorPos = m_rDataController.contractMilBufferinMemory(rImageStruct.numberofbuffersrequired(), *pStructData, vectorPos);
				}
			}
			else
			{
				if (0 == rImageStruct.numberofbuffersrequired())
				{
					//new empty size, ignore it.
					continue;
				}
				//new size, add it to the image size list
				pStructData = newImageStructList.add_list();
				pStructData->set_type(rImageStruct.type());
			}

			int requiredNumbers = rImageStruct.numberofbuffersrequired();
			pStructData->set_numberofbuffersrequired(requiredNumbers);
			pStructData->set_structid(rImageStruct.structid());

			
			SVMatroxBufferCreateStruct bufferStruct;
			MatroxImageProps bufferProps;
			fillBufferData(*pStructData, bufferStruct, bufferProps);
			neededBufferSpace += bufferProps.Bytesize * requiredNumbers;

			if (neededBufferSpace > m_rDataController.getMaxBufferSizeInBytes())
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
				std::string DebugString = SvUl::Format(_T("Failed because required memory size %lld\n"), neededBufferSpace / 1024 / 1024);
				::OutputDebugString(DebugString.c_str());
#endif
				m_rDataController.clearImageBuffer(true);
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%d"), static_cast<long>(neededBufferSpace / 1024 / 1024)));
				msgList.push_back(SvUl::Format(_T("%d"), static_cast<long>(m_rDataController.getMaxBufferSizeInBytes()/1024/1024)));
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetBuffer_TooMuch, msgList, SvStl::SourceFileParams(StdMessageParams));
				Exception.Throw();
			}

			assert(requiredNumbers > 0);
			if (pStructData->numberofbuffers() < requiredNumbers)
			{ //add more buffer of this size if necessary
				try
				{
					vectorPos = m_rDataController.createMilBufferinMemory(requiredNumbers, *pStructData, vectorPos);
				}
				catch (const SvStl::MessageContainer& rSvE)
				{
					m_rDataController.clearImageBuffer(true);
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
				pStructData->set_offset(newImageStructList.list(groupPos - 1).offset() + newImageStructList.list(groupPos - 1).numberofbuffers());
			}
			else
			{
				pStructData->set_offset(0);
			}

			groupPos++;
		}

#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		std::string DebugString = SvUl::Format(_T("Required memory size %lld\n"), neededBufferSpace / 1024 / 1024);
		::OutputDebugString(DebugString.c_str());
#endif

		//update structId to fit to the position in newImageStructList
		int i = 0;
		for (auto& rImageStructData : (*newImageStructList.mutable_list()))
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

		//resize the reference counter array if required and set the counter to 0
		try
		{
			m_rDataController.setImageStructList(std::move(newImageStructList));
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			m_rDataController.clearImageBuffer(true);
			//This is the topmost catch for MessageContainer exceptions
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
			Exception.setMessage(rSvE.getMessage());
			Exception.Throw();
		}
		m_rDataController.resetImageRefCounter();
	}
	else
	{
		try
		{
			m_rDataController.setImageStructList(std::move(rImageStructList));
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			m_rDataController.clearImageBuffer(true);
			//This is the topmost catch for MessageContainer exceptions
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
			Exception.setMessage(rSvE.getMessage());
			Exception.Throw();
		}
	}

	return moveVec;
}

bool ImageBufferController::increaseImageRefCounter(int pos)
{
	long* pImageCount = m_rDataController.getImageRefCountPtr(pos);
	if (nullptr != pImageCount)
	{
		InterlockedIncrement(pImageCount);
		return true;
	}
	return false;
}

bool ImageBufferController::decreaseImageRefCounter(int pos)
{
	long* pImageCount = m_rDataController.getImageRefCountPtr(pos);
	if (nullptr != pImageCount)
	{
		long value = InterlockedDecrement(pImageCount);
		if (0 > value)
		{
			assert(false);
			SvStl::MessageMgrStd e(SvStl::MsgType::Log);
			e.setMessage(SVMSG_TRC_GENERAL_WARNING, SvStl::Tid_TRC_Error_ImageRefCount, SvStl::SourceFileParams(StdMessageParams));
			InterlockedExchange(pImageCount, 0);
			return false;
		}
		return true;
	}

	return false;
}

IImagePtr ImageBufferController::getImage(int pos, long resetId, bool shouldUnlockAutomatically) const
{
	IImagePtr pImage;
	if (0 <= pos &&  m_rDataController.getBufferVectorRef().size() > pos)
	{
		SvOi::SVImageBufferHandlePtr pImagePtr = SvOi::SVImageBufferHandlePtr {new SVImageBufferHandleImage(m_rDataController.getBufferVectorRef()[pos])};
		pImage = IImagePtr {new Image(pImagePtr, resetId, pos, true, shouldUnlockAutomatically)};
	}
	return pImage;
}

IImagePtr ImageBufferController::getChildImage(int parentPos, const MatroxBufferChildDataStruct& rBufferDataStruct, long resetId, bool shouldUnlockAutomatically) const
{
	IImagePtr pImage;
	if (0 <= parentPos &&  m_rDataController.getBufferVectorRef().size() > parentPos)
	{
		SVMatroxBufferCreateChildStruct bufferStruct(m_rDataController.getBufferVectorRef()[parentPos]);
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
	const auto& rImageStructList = m_rDataController.getImageStructList();
	auto imageSizeIter = std::find_if(rImageStructList.list().begin(), rImageStructList.list().end(), [bufferStruct](const auto& rData)->bool
	{
		SVMatroxBufferCreateStruct bufferStruct2;
		memcpy(&bufferStruct2, rData.type().c_str(), sizeof(SVMatroxBufferCreateStruct));
		return bufferStruct == bufferStruct2;
	});
	if (rImageStructList.list().end() != imageSizeIter)
	{
		int imagePos = -1;
		return createNewImageHandle(imageSizeIter->structid(), imagePos, resetId, true);
	}
	return nullptr;
}

IImagePtr ImageBufferController::createNewImageHandle(int structId, int& rImagePos, long resetId, bool shouldUnlockAutomatically) const
{
	const auto& rList = m_rDataController.getImageStructList().list();
	if (0 <= structId &&  rList.size() > structId)
	{
		int offset = rList[structId].offset();
		for (int i = 0; i < rList[structId].numberofbuffers(); i++)
		{
			int imagePosTmp = i + offset;
			long* pValue = m_rDataController.getImageRefCountPtr(imagePosTmp);
			if (nullptr != pValue && 0 == *pValue)
			{
				if (InterlockedCompareExchange(pValue, 1, 0) == 0)
				{
					rImagePos = imagePosTmp;
					SvOi::SVImageBufferHandlePtr pImagePtr = SvOi::SVImageBufferHandlePtr {new SVImageBufferHandleImage(m_rDataController.getBufferVectorRef()[rImagePos])};
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
	SvPb::ImageStructList newImageStructList = m_rDataController.getImageStructList();
	for (const auto& rSizeData : imageList.list())
	{
		int id = rSizeData.structid();
		if (0 <= id && newImageStructList.list_size() > id)
		{
			auto* pImageStruct = newImageStructList.mutable_list(id);
			if (nullptr != pImageStruct)
			{
				assert(0 <= pImageStruct->numberofbuffersrequired() - numbers);
				pImageStruct->set_numberofbuffersrequired(pImageStruct->numberofbuffersrequired() - numbers);
			}
		}
	}
	try
	{
		m_rDataController.setImageStructList(std::move(newImageStructList));
	}
	catch (const SvStl::MessageContainer& rSvE)
	{
		//This is the topmost catch for MessageContainer exceptions
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
		Exception.setMessage(rSvE.getMessage());
	}	
}
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods
#pragma endregion Private Methods
} //namespace SvTrc
