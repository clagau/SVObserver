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
	void ImageBufferController::reset(int trNumbers, const SvPB::ImageSizeList& rImageSizeList, const SvPB::ImageList& rImageList)
	{
		int groupPos = 0;
		int vectorPos = 0;
		//step through new imageSize list to fit buffer to the new required structure
		for (const auto& rImageSize : rImageSizeList.list() )
		{
			SvPB::ImageSizeData* pSizeData = nullptr;
			if (m_imageSizeList.list_size() > groupPos)
			{
				pSizeData = m_imageSizeList.mutable_list(groupPos);
				if (0 == rImageSize.numberofimage())
				{
					//this size is not longer used, delete the buffers.
					auto bufferIter = m_bufferVector.begin() + vectorPos;
					m_bufferVector.erase(bufferIter, bufferIter + pSizeData->numberofbuffers());
					m_imageSizeList.mutable_list()->erase(m_imageSizeList.list().begin() + groupPos);
					continue;
				}
				else
				{	//this size is used, move vectorPos to the end of this bufferSize in Vector.
					vectorPos += pSizeData->numberofbuffers();
				}
			}
			else
			{	//new size, add it to the image size list
				pSizeData = m_imageSizeList.add_list();
				pSizeData->set_type(rImageSize.type());
			}

			pSizeData->set_numberofimage(rImageSize.numberofimage());
			pSizeData->set_sizeid(rImageSize.sizeid());

			if (pSizeData->numberofbuffers() < rImageSize.numberofimage() * trNumbers)
			{ //add more buffer of this size if necessary
				SVMatroxBufferCreateStruct bufferStruct;
				memcpy(&bufferStruct, pSizeData->type().c_str(), sizeof(SVMatroxBufferCreateStruct));
				for (int i = pSizeData->numberofbuffers(); i < rImageSize.numberofimage() * trNumbers; i++)
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
				pSizeData->set_numberofbuffers(rImageSize.numberofimage() * trNumbers);
			}

			//correct offset
			if (groupPos > 0)
			{
				pSizeData->set_offset(m_imageSizeList.list(groupPos - 1).offset() + m_imageSizeList.list(groupPos - 1).numberofbuffers());
			}
			else
			{
				pSizeData->set_offset(0);
			}

			groupPos++;
		}

		//resize the reference counter array if required and set the counter to 0
		if (m_bufferVector.size() > m_imageRefCountSize)
		{
			delete[] m_imageRefCountArray;
			m_imageRefCountSize = static_cast<int>(vectorPos);
			m_imageRefCountArray = new int[m_imageRefCountSize];
		}
		memset(m_imageRefCountArray, 0, sizeof(int)*m_imageRefCountSize);

		//update sizeId to fit to the position in m_imageSizeList
		m_imageList = rImageList;
		int i = 0;
		for (auto& rImageSizeData : (*m_imageSizeList.mutable_list()))
		{
			//old sizeId must be equal or higher (it is higher if an size-entry was deleted.
			assert(rImageSizeData.sizeid() >= i);
			if (rImageSizeData.sizeid() > i)
			{
				//Because oldID is 
				int oldId = rImageSizeData.sizeid();
				for (auto& imageData : (*m_imageList.mutable_list()))
				{
					if (imageData.sizeid() == oldId)
					{
						imageData.set_sizeid(i);
					}
				}
				rImageSizeData.set_sizeid(i);
			}
			i++;
		}
	}

	bool ImageBufferController::IncreaseRefCounter(int pos)
	{
		if (0 <= pos &&  m_imageRefCountSize > pos)
		{//@TODO[MZA][8.00][18.01.2018] check if here is thread-safe-function to add
			m_imageRefCountArray[pos]++;
			return true;
		}
		return false;
	}

	bool ImageBufferController::DecreaseRefCounter(int pos)
	{
		if (0 <= pos &&  m_imageRefCountSize > pos)
		{//@TODO[MZA][8.00][18.01.2018] check if here is thread-safe-function to add
			assert(m_imageRefCountArray[pos] > 0);
			if (m_imageRefCountArray[pos] > 0)
			{
				m_imageRefCountArray[pos]--;
				return true;
			}
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

	IImagePtr ImageBufferController::createNewImageHandle(int sizeID, int& imagePos) const
	{
		IImagePtr pImage;
		auto& rList = m_imageSizeList.list();
		if (0 <= sizeID &&  rList.size() > sizeID)
		{
			int offset = rList[sizeID].offset();
			for (int i = 0; i < rList[sizeID].numberofbuffers(); i++)
			{
				if (0 == m_imageRefCountArray[i + offset])
				{
					imagePos = i + offset;
					m_imageRefCountArray[imagePos]++;
					SvOi::SVImageBufferHandlePtr pImagePtr = SvOi::SVImageBufferHandlePtr{ new SVImageBufferHandleImage(m_bufferVector[imagePos]) };
					pImage = IImagePtr{ new Image(pImagePtr, false) };					
					break;
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
