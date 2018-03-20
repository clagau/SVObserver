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
//Moved to precompiled header: #include <WinBase.h>
#include "ITriggerRecordControllerRW.h"
#include "ITriggerRecordRW.h"
#include "TriggerRecordController.h"
#include "SVProtoBuf\BasicStructureHelper.h"
#include "ImageBufferController.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#pragma endregion Includes

namespace SvTrc
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
		for (int i=0; i < m_IPDataNumber; i++)
		{
			ResetInspectionData(i);
		}
		delete m_pData;
	}
#pragma endregion Constructor

#pragma region Public Methods
	int TriggerRecordController::getLastTRId(int inspectionPos) const
	{
		int id = -1;
		if (0 <= inspectionPos && m_IPDataNumber > inspectionPos)
		{
			id = m_pData[inspectionPos].m_lastTRID;
		}
		return id;
	}

	const SvPb::ImageList& TriggerRecordController::GetImageDefList(int inspectionPos)
	{
		if (-1 == m_resetStarted4IP)
		{
			if (0 <= inspectionPos && m_IPDataNumber > inspectionPos && m_pData[inspectionPos].m_bInit)
			{
				if (nullptr != m_pData[inspectionPos].m_pImageList)
				{
					return *m_pData[inspectionPos].m_pImageList;
				}
			}
		}
		throw false;
	}

	ITriggerRecordRPtr TriggerRecordController::CreateTriggerRecordObject(int inspectionPos, int trId)
	{
		assert(-1 == m_resetStarted4IP);
		
		if (-1 == m_resetStarted4IP && 0 <= inspectionPos && m_IPDataNumber > inspectionPos 
			&& m_pData[inspectionPos].m_bInit && nullptr != m_pData[inspectionPos].m_pImageList)
		{
			if (0 <= trId)
			{
				for (int i = 0; i < m_pData[inspectionPos].m_TriggerRecordNumber; i++)
				{
					TriggerRecord::TriggerRecordData& rCurrentTR = getTRData(inspectionPos, i);
					if (rCurrentTR.m_trId == trId)
					{
						long refTemp = rCurrentTR.m_referenceCount;
						while (0 <= refTemp)
						{
							if (InterlockedCompareExchange(&(rCurrentTR.m_referenceCount), refTemp + 1, refTemp) == refTemp)
							{
								return ITriggerRecordRPtr(new TriggerRecord(rCurrentTR, *m_pData[inspectionPos].m_pImageList, m_resetTime));
							}
							refTemp = rCurrentTR.m_referenceCount;
						}
					}
				}
			}
		}
		return nullptr;
	}
	
	void TriggerRecordController::setInspections(const SvPb::InspectionList& rInspectionList)
	{
		m_inspectionList = rInspectionList;
		for (int i = 0; i < m_IPDataNumber; i++)
		{
			ResetInspectionData(i);
		}
		if (rInspectionList.inspectionid_size() != m_IPDataNumber && 0 < rInspectionList.inspectionid_size())
		{
			delete m_pData;
			m_IPDataNumber = rInspectionList.inspectionid_size();
			m_pData = new TRControllerDataPerIP[m_IPDataNumber];
		}
	}

	ITriggerRecordRWPtr TriggerRecordController::CreateTriggerRecordObjectToWrite(int inspectionPos)
	{
		assert(-1 == m_resetStarted4IP && 0 <= inspectionPos && m_IPDataNumber > inspectionPos && m_pData[inspectionPos].m_bInit);

		if (-1 == m_resetStarted4IP && 0 <= inspectionPos && m_IPDataNumber > inspectionPos 
			&& m_pData[inspectionPos].m_bInit && nullptr != m_pData[inspectionPos].m_pImageList)
		{
			int currentPos = m_pData[inspectionPos].m_nextPosForFreeCheck;
			do
			{
				TriggerRecord::TriggerRecordData& rCurrentTR = getTRData(inspectionPos, currentPos);
				long count = rCurrentTR.m_referenceCount;
				while (TriggerRecord::m_InvalidData == count || 0 == count)
				{
					if (InterlockedCompareExchange(&(rCurrentTR.m_referenceCount), TriggerRecord::m_WriteBlocked, count) == count)
					{
						rCurrentTR.m_trId = m_nextTRID++;
						m_pData[inspectionPos].m_lastTRID = rCurrentTR.m_trId;
						m_pData[inspectionPos].m_nextPosForFreeCheck = (currentPos + 1) % (m_pData[inspectionPos].m_TriggerRecordNumber);
						return ITriggerRecordRWPtr(new TriggerRecord(rCurrentTR, *m_pData[inspectionPos].m_pImageList, m_resetTime));
					}
				}
				currentPos++;
				currentPos = currentPos%m_pData[inspectionPos].m_TriggerRecordNumber;
			} while (currentPos != m_pData[inspectionPos].m_nextPosForFreeCheck);
			assert(false);
		}

		return ITriggerRecordRWPtr();
	};

	bool TriggerRecordController::StartResetTriggerRecordStructure(int inspectionPos, int TriggerRecordSize)
	{
		if (-1 != m_resetStarted4IP || 0 > inspectionPos || m_inspectionList.inspectionid_size() <= inspectionPos)
		{   //new start of reset is not allowed if reset is in progress.
			assert(false);
			return false;
		}

		bool Result = false;
		if (TriggerRecordSize < m_maxTriggerRecords)
		{
			m_resetStarted4IP = inspectionPos;

			//clear imageList and reset ImageStructList
			m_imageListResetTmp.Clear();
			int TRNumber = 0;
			if (nullptr != m_pData[m_resetStarted4IP].m_pImageList)
			{
				m_imageListResetTmp = *m_pData[m_resetStarted4IP].m_pImageList;
				TRNumber = m_pData[m_resetStarted4IP].m_TriggerRecordNumber;
			}
			int structId = 0;
			m_imageStructListResetTmp.Clear();
			m_imageStructListResetTmp = ImageBufferController::getImageBufferControllerInstance().getImageStructList();
			for( const auto& rSizeData: m_imageListResetTmp.list() )
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

			m_TriggerRecordNumberResetTmp = TriggerRecordSize + m_TriggerRecordAddOn;
			Result = true;
		}
		return Result;
	}

	bool TriggerRecordController::AddImage2TriggerRecordStructure(const GUID& imageId, SVMatroxBufferCreateStruct bufferStruct)
	{ 
		if (-1 == m_resetStarted4IP)
		{   //stop of reset is not allowed if reset is not started.
			assert(false);
			return false;
		}

		
		std::string ImageIdBytes;
		SvPb::SetGuidInProtoBytes(&ImageIdBytes, imageId);

		//check if image with this GUID already in list (this is not allowed.)
		auto imageIter = std::find_if(m_imageListResetTmp.list().begin(), m_imageListResetTmp.list().end(), [&ImageIdBytes](auto data)->bool
		{
			
			return (0 == data.imageid().compare(ImageIdBytes));
			
		});
		if (m_imageListResetTmp.list().end() != imageIter)
		{
			return false;
		}

		//add new image to imageList
		SvPb::ImageDefinition* pImageDefinition = m_imageListResetTmp.add_list();
		SvPb::SetGuidInProtoBytes(pImageDefinition->mutable_imageid(), imageId);
		

		//check if size already in sizeList
		auto pImageStructList = m_imageStructListResetTmp.mutable_list();
		auto imageSizeIter = std::find_if(pImageStructList->begin(), pImageStructList->end(), [bufferStruct](auto data)->bool
		{
			SVMatroxBufferCreateStruct bufferStruct2;
			memcpy(&bufferStruct2, data.type().c_str(), sizeof(SVMatroxBufferCreateStruct));
			return bufferStruct == bufferStruct2;
		});
		
		
		if (pImageStructList->end() != imageSizeIter)
		{   //use existing imageSize
			imageSizeIter->set_numberofbuffersrequired(imageSizeIter->numberofbuffersrequired() + m_TriggerRecordNumberResetTmp);
			pImageDefinition->set_type((*imageSizeIter).type());
			pImageDefinition->set_structid((*imageSizeIter).structid());
		}
		else
		{	//add new imageSize
			SvPb::ImageStructData* pImageStructData = m_imageStructListResetTmp.add_list();
			std::string typeStr(reinterpret_cast<const char*>(&bufferStruct), sizeof(bufferStruct));
			pImageStructData->set_type(typeStr);
			pImageStructData->set_numberofbuffers(0);
			pImageStructData->set_numberofbuffersrequired(m_TriggerRecordNumberResetTmp);
			pImageStructData->set_structid(m_imageStructListResetTmp.list_size()-1);
			pImageDefinition->set_type(pImageStructData->type());
			pImageDefinition->set_structid(pImageStructData->structid());
		}

		//add to list
		return true;
	}

	bool TriggerRecordController::FinishResetTriggerRecordStructure()
	{ 
		if (-1 == m_resetStarted4IP)
		{   //stop of reset is not allowed if reset is not started.
			assert(false);
			return false;
		}

		time(&m_resetTime);
		while (0 < m_resetLockCounter) {}; //wait if any other method have access of this structure
		
		ResetInspectionData(m_resetStarted4IP);
		m_pData[m_resetStarted4IP].m_TriggerRecordNumber = m_TriggerRecordNumberResetTmp;
		m_pData[m_resetStarted4IP].m_triggerRecordBufferSize = (sizeof(TriggerRecord::TriggerRecordData) + sizeof(int)*m_imageListResetTmp.list_size());
		m_pData[m_resetStarted4IP].m_pTriggerRecords = malloc (m_pData[m_resetStarted4IP].m_triggerRecordBufferSize*m_pData[m_resetStarted4IP].m_TriggerRecordNumber);
		memset(m_pData[m_resetStarted4IP].m_pTriggerRecords, -1, m_pData[m_resetStarted4IP].m_triggerRecordBufferSize*m_pData[m_resetStarted4IP].m_TriggerRecordNumber);
		for (int i=0; i<m_pData[m_resetStarted4IP].m_TriggerRecordNumber; i++)
		{	//initialize buffer
			getTRData(m_resetStarted4IP, i).init();
		}
		m_pData[m_resetStarted4IP].m_pImageList = new SvPb::ImageList(m_imageListResetTmp);

		//update structId to fit to the position in m_imageStructList
		std::vector<std::pair<int,int>> changeVect = ImageBufferController::getImageBufferControllerInstance().reset(m_imageStructListResetTmp);
		for (const auto& rChangePair : changeVect)
		{
			for (int i=0; i<m_IPDataNumber; i++)
			{
				auto* pImageList = m_pData[i].m_pImageList;
				if (nullptr != pImageList)
				{
					for (auto& rImageData : (*pImageList->mutable_list()))
					{
						if (rImageData.structid() == rChangePair.first)
						{
							rImageData.set_structid(rChangePair.second);
						}
					}
				}
			}
		}

		m_pData[m_resetStarted4IP].m_bInit = true;
		m_resetStarted4IP = -1;

		return true; 
	}
#pragma endregion Public Methods

#pragma region Private Methods
	TriggerRecord::TriggerRecordData& TriggerRecordController::getTRData(int inspectionPos, int pos) const
	{
		//do not check of the input parameter, because this is are private method and the parameter will be checked before in the calling method.
		assert(0 <= pos && 0 <= inspectionPos && m_IPDataNumber > inspectionPos && m_pData[inspectionPos].m_TriggerRecordNumber > pos);
		
		void* tmp = static_cast<char*>(m_pData[inspectionPos].m_pTriggerRecords) + (pos*m_pData[inspectionPos].m_triggerRecordBufferSize);
		TriggerRecord::TriggerRecordData* tr = reinterpret_cast<TriggerRecord::TriggerRecordData*>(tmp);
		return *tr;
	}

	void TriggerRecordController::ResetInspectionData(int inspectionID)
	{
		if (0 <= inspectionID && m_IPDataNumber > inspectionID)
		{
			m_pData[inspectionID].m_lastTRID = -1;
			if (m_pData[inspectionID].m_bInit)
			{
				delete[] m_pData[inspectionID].m_pTriggerRecords;
				m_pData[inspectionID].m_pTriggerRecords = nullptr;
				m_pData[inspectionID].m_triggerRecordBufferSize = 0;
				delete m_pData[inspectionID].m_pImageList;
				m_pData[inspectionID].m_pImageList = nullptr;
			}
			m_pData[m_resetStarted4IP].m_nextPosForFreeCheck = 0;
		}
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
} //namespace SvTrc
