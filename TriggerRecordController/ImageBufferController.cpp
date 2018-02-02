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
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "Image.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvTRC
{
#pragma region Constructor
	ImageBufferController& ImageBufferController::getImageBufferControllerInstance()
	{
		static ImageBufferController controller;

		return controller;
	}

	ImageBufferController::ImageBufferController()
	{
	}

	ImageBufferController::~ImageBufferController()
	{
		m_bufferVector.clear();
		delete m_imageRefCountArray;
	}
#pragma endregion Constructor

#pragma region Public Methods
	std::vector<std::pair<int, int>> ImageBufferController::reset(const SvPB::ImageStructList& rImageStructList)
	{
		int groupPos = 0;
		int vectorPos = 0;

		assert(rImageStructList.list_size() >= m_imageStructList.list_size());
		assert(0 == m_imageStructList.list_size() || rImageStructList.list(m_imageStructList.list_size() - 1).structid() == m_imageStructList.list(m_imageStructList.list_size() - 1).structid());

		//step through new imageSize list to fit buffer to the new required structure
		for (const auto& rImageStruct : rImageStructList.list() )
		{
			SvPB::ImageStructData* pStructData = nullptr;
			if (m_imageStructList.list_size() > groupPos)
			{
				pStructData = m_imageStructList.mutable_list(groupPos);
				if (0 == rImageStruct.numberofbuffersrequired())
				{
					//this size is not longer used, delete the buffers.
					auto bufferIter = m_bufferVector.begin() + vectorPos;
					m_bufferVector.erase(bufferIter, bufferIter + pStructData->numberofbuffers());
					m_imageStructList.mutable_list()->erase(m_imageStructList.list().begin() + groupPos);
					continue;
				}
				else
				{	//this size is used, move vectorPos to the end of this bufferSize in Vector.
					vectorPos += pStructData->numberofbuffers();
				}
			}
			else
			{	//new size, add it to the image size list
				pStructData = m_imageStructList.add_list();
				pStructData->set_type(rImageStruct.type());
			}

			pStructData->set_numberofbuffersrequired(rImageStruct.numberofbuffersrequired());
			pStructData->set_structid(rImageStruct.structid());

			if (pStructData->numberofbuffers() < rImageStruct.numberofbuffersrequired())
			{ //add more buffer of this size if necessary
				SVMatroxBufferCreateStruct bufferStruct;
				memcpy(&bufferStruct, pStructData->type().c_str(), sizeof(SVMatroxBufferCreateStruct));
				for (int i = pStructData->numberofbuffers(); i < rImageStruct.numberofbuffersrequired(); i++)
				{
					SVMatroxBuffer buffer;
					SVMatroxBufferInterface::Create(buffer, bufferStruct);
					if (m_bufferVector.size() <= vectorPos)
					{
						m_bufferVector.push_back(buffer);
					}
					else
					{
						m_bufferVector.insert(m_bufferVector.begin() + vectorPos, buffer);
					}
					vectorPos++;
				}
				pStructData->set_numberofbuffers(rImageStruct.numberofbuffersrequired());
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
				moveVec.push_back({ oldId, i });
				rImageStructData.set_structid(i);
			}
			i++;
		}

		return moveVec;
	}

	bool ImageBufferController::IncreaseRefCounter(int pos)
	{
		if (0 <= pos &&  m_imageRefCountSize > pos)
		{
			long value = m_imageRefCountArray[pos];
			while (InterlockedCompareExchange(&(m_imageRefCountArray[pos]), value + 1, value) != value)
			{
				value = m_imageRefCountArray[pos];
			}
			return true;
		}
		return false;
	}

	bool ImageBufferController::DecreaseRefCounter(int pos)
	{
		if (0 <= pos &&  m_imageRefCountSize > pos)
		{
			long value = m_imageRefCountArray[pos];
			if (0 < value)
			{
				while (InterlockedCompareExchange(&(m_imageRefCountArray[pos]), value - 1, value) != value)
				{
					assert(m_imageRefCountArray[pos] > 0);
					value = m_imageRefCountArray[pos];
				}
				return true;
			}
			assert(false);
		}

		return false;
	}

	IImagePtr ImageBufferController::getImage(int pos) const
	{
		IImagePtr pImage;
		if (0 <= pos &&  m_bufferVector.size() > pos)
		{
			SvOi::SVImageBufferHandlePtr pImagePtr = SvOi::SVImageBufferHandlePtr{ new SVImageBufferHandleImage(m_bufferVector[pos]) };
			pImage = IImagePtr{ new Image(pImagePtr) };
		}
		return pImage;
	}

	IImagePtr ImageBufferController::createNewImageHandle(int structId, int& imagePos) const
	{
		IImagePtr pImage;
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
						imagePos = imagePosTmp;
						SvOi::SVImageBufferHandlePtr pImagePtr = SvOi::SVImageBufferHandlePtr{ new SVImageBufferHandleImage(m_bufferVector[imagePos]) };
						return IImagePtr{ new Image(pImagePtr, false) };
					}
				}
			}			
		}
		return pImage;
	}
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods
#pragma endregion Private Methods
} //namespace SvTRC
