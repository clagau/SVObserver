//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file TriggerRecordController.cpp
/// All Rights Reserved
//*****************************************************************************
/// The TriggerRecordController: control the triggerRecords.
//******************************************************************************
#pragma once

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "ITriggerRecordControllerRW.h"
#include "ITriggerRecordRW.h"
#include "TriggerRecordController.h"
#include "SVProtoBuf\BasicStructureHelper.h"
#include "ImageBufferController.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#pragma endregion Includes

namespace SvTRC
{
#pragma region Constructor
	TriggerRecordController& TriggerRecordController::getTriggerRecordControllerInstance()
	{
		static TriggerRecordController controller;

		return controller;
	}

	TriggerRecordController::TriggerRecordController()
	{
	}

	TriggerRecordController::~TriggerRecordController()
	{
		if (m_bInit)
		{
			delete[] m_triggerRecords;
			m_bInit = false;
		}
	}
#pragma endregion Constructor

#pragma region Public Methods
	const SvPB::ImageList& TriggerRecordController::GetImageDefList()
	{
		if (!m_bReset && m_bInit)
		{
			return ImageBufferController::getImageBufferControllerInstance().getImageList();
		}
		throw false;
	}

	ITriggerRecordRPtr TriggerRecordController::CreateTriggerRecordObject(int trId)
	{
		assert(!m_bReset && m_bInit);
		if (0 <= trId)
		{
			for (int i = 0; i < m_TriggerRecordNumber; i++)
			{
				TriggerRecord::TriggerRecordData& rCurrentTR = getTRData(i);
				if (rCurrentTR.m_trId == trId)
				{
					if (rCurrentTR.m_referenceCount >= 0)
					{//@TODO[MZA][8.00][20.12.2017] check if here is thread-safe-function to add
						rCurrentTR.m_referenceCount++;
						return ITriggerRecordRPtr(new TriggerRecord(rCurrentTR));
					}
				}
			}
		}
		return nullptr;
	}

	ITriggerRecordRWPtr TriggerRecordController::CreateTriggerRecordObjectToWrite()
	{
		assert(!m_bReset && m_bInit);

		int currentPos = m_nextPosForFreeCheck;
		do
		{
			TriggerRecord::TriggerRecordData& rCurrentTR = getTRData(currentPos);
			int count = rCurrentTR.m_referenceCount;
			if (TriggerRecord::m_InvalidData == count || 0 == count)
			{//@TODO[MZA][8.00][20.12.2017] check if here is thread-safe-function to add
				rCurrentTR.m_referenceCount = TriggerRecord::m_WriteBlocked;
				rCurrentTR.m_trId = m_nextTRID++;
				m_lastTRID = rCurrentTR.m_trId;
				m_nextPosForFreeCheck = (currentPos + 1) % (m_TriggerRecordNumber);
				return ITriggerRecordRWPtr(new TriggerRecord(rCurrentTR));
			}
			currentPos++;
			currentPos = currentPos%m_TriggerRecordNumber;
		}
		while (currentPos != m_nextPosForFreeCheck);
		assert(false);
		return ITriggerRecordRWPtr();
	};

	bool TriggerRecordController::StartResetTriggerRecordStructure(int TriggerRecordSize)
	{
		if (m_bReset)
		{   //new start of reset is not allowed if reset is in progress.
			assert(false);
			return false;
		}

		bool Result = false;
		m_lastTRID = -1;
		if (TriggerRecordSize < m_maxTriggerRecords)
		{
			m_bReset = true;
			if (m_bInit)
			{
				delete[] m_triggerRecords;
				m_triggerRecords = nullptr;
				m_triggerRecordBufferSize = 0;
			}
			//clear imageList and reset imageSizeList
			m_imageListResetTmp.Clear();
			int sizeID = 0;
			m_imageSizeListResetTmp.Clear();
			m_imageSizeListResetTmp = ImageBufferController::getImageBufferControllerInstance().getImageSizeList();
			for( auto& rSizeData: (*m_imageSizeListResetTmp.mutable_list()) )
			{	
				rSizeData.set_numberofimage(0);
				rSizeData.set_sizeid(sizeID++);
			}
			m_TriggerRecordNumber = TriggerRecordSize + m_TriggerRecordAddOn;
			m_nextPosForFreeCheck = 0;
			m_bInit = true;
			Result = true;
		}
		return Result;
	}

	bool TriggerRecordController::AddImage2TriggerRecordStructure(const GUID& imageId, SVMatroxBufferCreateStruct bufferStruct)
	{ 
		if (!m_bReset)
		{   //stop of reset is not allowed if reset is not started.
			assert(false);
			return false;
		}

		SvPB::UUID imageUuid = SvPB::setGuidToMessage(imageId);
		//check if image with this GUID already in list (this is not allowed.)
		auto imageIter = std::find_if(m_imageListResetTmp.list().begin(), m_imageListResetTmp.list().end(), [imageUuid](auto data)->bool
		{
			return data.imageid().part1() == imageUuid.part1() && data.imageid().part2() == imageUuid.part2();
		});
		if (m_imageListResetTmp.list().end() != imageIter)
		{
			return false;
		}

		//add new image to imageList
		SvPB::ImageDefinition* pImageDefinition = m_imageListResetTmp.add_list();
		pImageDefinition->mutable_imageid()->CopyFrom(imageUuid);
		

		//check if size already in sizeList
		auto pImageSizeList = m_imageSizeListResetTmp.mutable_list();
		auto imageSizeIter = std::find_if(pImageSizeList->begin(), pImageSizeList->end(), [bufferStruct](auto data)->bool
		{
			SVMatroxBufferCreateStruct bufferStruct2;
			memcpy(&bufferStruct2, data.type().c_str(), sizeof(SVMatroxBufferCreateStruct));
			return bufferStruct == bufferStruct2;
		});
		
		
		if (pImageSizeList->end() != imageSizeIter)
		{   //use existing imageSize
			imageSizeIter->set_numberofimage(imageSizeIter->numberofimage() + 1);
			pImageDefinition->set_type((*imageSizeIter).type());
			pImageDefinition->set_sizeid((*imageSizeIter).sizeid());
		}
		else
		{	//add new imageSize
			SvPB::ImageSizeData* pImageSizeData = m_imageSizeListResetTmp.add_list();
			std::string typeStr(reinterpret_cast<const char*>(&bufferStruct), sizeof(bufferStruct));
			pImageSizeData->set_type(typeStr);
			pImageSizeData->set_numberofbuffers(0);
			pImageSizeData->set_numberofimage(1);
			pImageSizeData->set_sizeid(m_imageSizeListResetTmp.list_size()-1);
			pImageDefinition->set_type(pImageSizeData->type());
			pImageDefinition->set_sizeid(pImageSizeData->sizeid());
		}

		//add to list
		return true;
	}

	bool TriggerRecordController::FinishResetTriggerRecordStructure()
	{ 
		if (!m_bReset)
		{   //stop of reset is not allowed if reset is not started.
			assert(false);
			return false;
		}
		m_triggerRecordBufferSize = (sizeof(TriggerRecord::TriggerRecordData) + sizeof(int)*m_imageListResetTmp.list_size());
		m_triggerRecords = malloc (m_triggerRecordBufferSize*m_TriggerRecordNumber);
		memset(m_triggerRecords, -1, m_triggerRecordBufferSize*m_TriggerRecordNumber);
		for (int i=0; i<m_TriggerRecordNumber; i++)
		{	//initialize buffer
			getTRData(i).init();
		}

		ImageBufferController::getImageBufferControllerInstance().reset(m_TriggerRecordNumber, m_imageSizeListResetTmp, m_imageListResetTmp);

		m_bReset = false;

		return true; 
	}
#pragma endregion Public Methods

#pragma region Private Methods
	TriggerRecord::TriggerRecordData& TriggerRecordController::getTRData(int pos) const
	{
		assert(0 <= pos && m_TriggerRecordNumber > pos);
		
		void* tmp = static_cast<char*>(m_triggerRecords) + (pos*m_triggerRecordBufferSize);
		TriggerRecord::TriggerRecordData* tr = reinterpret_cast<TriggerRecord::TriggerRecordData*>(tmp);
		return *tr;
	}
#pragma endregion Private Methods

	ITriggerRecordControllerRW& getTriggerRecordControllerRWInstance()
	{
		return TriggerRecordController::getTriggerRecordControllerInstance();
	}

	ITriggerRecordControllerR& getTriggerRecordControllerRInstance()
	{
		return TriggerRecordController::getTriggerRecordControllerInstance();
	}
} //namespace SvTRC