//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TriggerRecord.cpp
/// All Rights Reserved
//*****************************************************************************
/// The class have access to a TriggerRecord.
//******************************************************************************
#pragma once

#pragma region Includes
#include "stdafx.h"
#include "TriggerRecord.h"
#include "TriggerRecordData.h"
#include "ImageBufferController.h"
#include "ResetLocker.h"
#include "Definitions/GlobalConst.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateChildStruct.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVPoint.h"
#pragma endregion Includes
//#define TRACE_TRC

namespace SvTrc
{
TriggerRecord::TriggerRecord(int inspectionPos, int trPos, TriggerRecordData& rData, const SvPb::ImageList& rImageList, const SvPb::DataDefinitionList& rDataDefList, long dataListSize, long resetId)
: m_inspectionPos(inspectionPos)
, m_trPos(trPos)
, m_rData(rData)
, m_rImageList(rImageList)
, m_rDataDefList(rDataDefList)
, m_dataListSize(dataListSize)
, m_ResetId(resetId)
{
}

TriggerRecord::~TriggerRecord()
{
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		bool finishedTr = (TriggerRecordData::cWriteBlocked == m_rData.m_referenceCount);
		removeTrReferenceCount(m_inspectionPos, m_rData.m_referenceCount);
		if (finishedTr && !m_blockUpdateLastId)
		{
			auto* pTRC = getTriggerRecordControllerInstance();
			if (nullptr != pTRC)
			{
				pTRC->setLastFinishedTr(SvOi::TrEventData(m_inspectionPos, m_rData.m_trId));
			}
		}
	}
}

int TriggerRecord::getId() const 
{
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		return m_rData.m_trId;
	}
	return -1;
}

const SvOi::TriggerData& TriggerRecord::getTriggerData() const
{ 
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		return m_rData.m_triggerData;
	}
	static SvOi::TriggerData emptyData;
	return emptyData;
}

SvOi::ITRCImagePtr TriggerRecord::getImage(int pos, bool lockImage) const
{
	SvOi::ITRCImagePtr pImage;
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		auto& rImageController = getImageBufferControllerInstance();
		const auto& rImageList = m_rImageList.list();
		int* const pImagePos = m_rData.getImagePos();
		if (0 <= pos && rImageList.size() > pos)
		{
			if (0 <= pImagePos[pos])
			{
				pImage = rImageController.getImage(pImagePos[pos], m_ResetId, lockImage);
				if (lockImage)
				{
					rImageController.increaseImageRefCounter(pImagePos[pos]);
				}
			}
			else
			{
				//This part is needed if a buffer of an image with is not set yet. For this set a new buffer and clear it.
				pImage = rImageController.createNewImageHandle(rImageList[pos].structid(), pImagePos[pos], m_ResetId, lockImage);
				if (nullptr == pImage || pImage->isEmpty())
				{
					pImagePos[pos] = -1;
					SvDef::StringVector msgList;
					msgList.push_back(SvUl::Format(_T("%d"), pos));
					SvStl::MessageManager e(SvStl::MsgType::Log);
					e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NewBufferFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				}
				else
				{
					MbufClear(pImage->getHandle()->GetBuffer().GetIdentifier(), 0);
					if (lockImage)
					{
						rImageController.increaseImageRefCounter(pImagePos[pos]);
					}
				}
			}
		}
	}
	return pImage;
}

SvOi::ITRCImagePtr TriggerRecord::getChildImage(int childPos, bool lockImage) const
{
	SvOi::ITRCImagePtr pImage;
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock && 0 <= childPos && m_rImageList.childlist_size() > childPos)
	{
		auto& rImageController = getImageBufferControllerInstance();
		const auto& rChildDef = m_rImageList.childlist(childPos);
		int pos = SvOi::findObjectIdPos(m_rImageList.list(), rChildDef.parentimageid());

		MatroxBufferChildDataStruct bufferDataStruct;
		memcpy(&bufferDataStruct, rChildDef.type().c_str(), sizeof(MatroxBufferChildDataStruct));
		const int*const pImagePos = m_rData.getImagePos();
		if (0 <= pos && m_rImageList.list_size() > pos)
		{
			pImage = rImageController.getChildImage(pImagePos[pos], bufferDataStruct, m_ResetId, lockImage);
			if (lockImage)
			{
				rImageController.increaseImageRefCounter(pImagePos[pos]);
			}
		}
		else
		{
			int childPos2 = SvOi::findObjectIdPos(m_rImageList.childlist(), rChildDef.parentimageid());
			SvOi::ITRCImagePtr pChildImage = getChildImage(childPos2, lockImage);
			if (nullptr != pChildImage && !pChildImage->isEmpty())
			{
				SVMatroxBufferCreateChildStruct bufferStruct(pChildImage->getHandle()->GetBuffer());
				bufferStruct.m_data = bufferDataStruct;
				pImage = rImageController.getChildImage(bufferStruct, m_ResetId, pChildImage->getBufferPos(), lockImage);
			}
		}
	}
	return pImage;
}

_variant_t TriggerRecord::getDataValue(int pos) const
{
	_variant_t result;
	if (-1 < pos)
	{
		auto pLock = ResetLocker::lockReset(m_ResetId);
		if (nullptr != pLock)
		{
			std::atomic_int* pSize = reinterpret_cast<std::atomic_int*> (m_rData.getValueData());
			int DataSize = *pSize;
			if (DataSize > 0)
			{
				if (m_rDataDefList.list_size() > pos)
				{
					long memOffset = m_rDataDefList.list()[pos].memoffset();
					if(DataSize > memOffset)
					{
						long arraySize = m_rDataDefList.list()[pos].arraysize();
						VARTYPE vtType = static_cast<VARTYPE> (m_rDataDefList.list()[pos].vttype());
						BYTE* pMemBlock = reinterpret_cast<BYTE*> (pSize + 1);
						pMemBlock += memOffset;
						//If it is an array then the first value is the result size
						if(1 == arraySize && m_rDataDefList.list()[pos].type() != SvPb::SVObjectSubTypeEnum::DoubleSortValueObjectType)
						{
							result = readValue(vtType, pMemBlock);
						}
						else if(arraySize >0  )
						{
							result = readArrayValue(vtType, pMemBlock);
						}
					}
				}
				else
				{
					SvDef::StringVector msgList;
					msgList.push_back(SvUl::Format(_T("%d"), pos));
					SvStl::MessageManager e(SvStl::MsgType::Log);
					e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetValueWrongPos, msgList, SvStl::SourceFileParams(StdMessageParams));
				}
			}
			else
			{
				SvStl::MessageManager e(SvStl::MsgType::Log);
				e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_ValueNotReady, SvStl::SourceFileParams(StdMessageParams));
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
				std::string DebugString = SvUl::Format(_T("value not ready; %d\n"), m_rData.m_trId);
				::OutputDebugString(DebugString.c_str());
#endif
			}	
		}
		else
		{
			SvStl::MessageManager e(SvStl::MsgType::Log);
			e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_InvalidResetState, SvStl::SourceFileParams(StdMessageParams));
		}
	}
	else
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), pos));
		SvStl::MessageManager e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_GetValueWrongPos, msgList, SvStl::SourceFileParams(StdMessageParams));
	}

	return result;
}

bool TriggerRecord::isValueDataValid() const
{
	bool retValue = false;
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		std::atomic_int* pSize = reinterpret_cast<std::atomic_int*> (m_rData.getValueData());
		//Data is valid when the data size > 0
		retValue = (0 < *pSize);
	}
	return retValue;
}

bool TriggerRecord::isObjectUpToTime() const
{
	auto pLock = ResetLocker::lockReset(m_ResetId);
	return (nullptr != pLock);
}

void TriggerRecord::setTriggerData(const SvOi::TriggerData& data)
{ 
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		m_rData.m_triggerData = data;
	}
}

void TriggerRecord::initImages()
{
	auto& rImageController = getImageBufferControllerInstance();
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		for (int i = 0; i < rImageList.size(); i++)
		{
			if (-1 == pImagePos[i])
			{
				SvOi::ITRCImagePtr pImage = rImageController.createNewImageHandle(rImageList[i].structid(), pImagePos[i], m_ResetId, false);
				if (nullptr == pImage)
				{
					SvDef::StringVector msgList;
					msgList.push_back(SvUl::Format(_T("%d"), pImagePos[i]));
					SvStl::MessageManager e(SvStl::MsgType::Log);
					e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NewBufferFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				}
				else
				{
					if (!pImage->isEmpty())
					{
						SVMatroxBufferInterface::ClearBuffer(pImage->getHandle()->GetBuffer(), 0);
					}
				}
			}
		}
	}
}

void TriggerRecord::setImages(const ITriggerRecordR& rDestTr)
{
	auto& rImageController = getImageBufferControllerInstance();
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock && rDestTr.isObjectUpToTime())
	{
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		const int*const pImagePosOld = dynamic_cast<const TriggerRecord&>(rDestTr).getTrData().getImagePos();
		for (int i = 0; i < rImageList.size(); i++)
		{
			if (-1 != pImagePosOld[i])
			{
				if (pImagePos[i] != pImagePosOld[i])
				{
					rImageController.decreaseImageRefCounter(pImagePos[i]);
					rImageController.increaseImageRefCounter(pImagePosOld[i]);
					pImagePos[i] = pImagePosOld[i];
				}
			}
			else
			{
				if (-1 == pImagePos[i])
				{
					SvOi::ITRCImagePtr pImage = rImageController.createNewImageHandle(rImageList[i].structid(), pImagePos[i], m_ResetId, false);
					if (nullptr == pImage)
					{
						SvDef::StringVector msgList;
						msgList.push_back(SvUl::Format(_T("%d"), pImagePos[i]));
						SvStl::MessageManager e(SvStl::MsgType::Log);
						e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NewBufferFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
					}
				}
			}
		}
	}
	else
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_TRC_GENERAL_WARNING, SvStl::Tid_TRC_Error_SetImages_SourceTROutOfData, SvStl::SourceFileParams(StdMessageParams));
		initImages();
	}
}

void TriggerRecord::setImage(int pos, const SvOi::ITRCImagePtr& pImage)
{
	setImage(pos, pImage->getBufferPos());
}

void TriggerRecord::setImage(int pos, int bufferPos)
{
	auto& rImageController = getImageBufferControllerInstance();
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		if (0 <= pos && rImageList.size() > pos)
		{
			int typePos = rImageList[pos].structid();
			auto& pStructList = rImageController.getImageStructList().list();
			if (0 <= typePos && pStructList.size() > typePos)
			{
				const auto& rSizeList = pStructList[typePos];
				if (bufferPos >= rSizeList.offset() && bufferPos < rSizeList.offset() + rSizeList.numberofbuffers())
				{
					if (pImagePos[pos] != bufferPos)
					{
						rImageController.decreaseImageRefCounter(pImagePos[pos]);
						rImageController.increaseImageRefCounter(bufferPos);
						pImagePos[pos] = bufferPos;
					}
				}
				else if (-1 == bufferPos)
				{
					if (-1 != pImagePos[pos])
					{
						rImageController.decreaseImageRefCounter(pImagePos[pos]);
						pImagePos[pos] = -1;
					}
				}
				else
				{
					rImageController.decreaseImageRefCounter(pImagePos[pos]);
					pImagePos[pos] = -1;
					assert(false);
					SvStl::MessageManager Exception(SvStl::MsgType::Data);
					Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_SetImage_TypeNotFit, SvStl::SourceFileParams(StdMessageParams));
					Exception.Throw();
				}
			}
			else
			{
				assert(false);
				SvStl::MessageManager Exception(SvStl::MsgType::Data);
				Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_SetImage_InvalidType, SvStl::SourceFileParams(StdMessageParams));
				Exception.Throw();
			}
		}
		else
		{
			assert(false);
			SvStl::MessageManager Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_SetImage_InvalidPos, SvStl::SourceFileParams(StdMessageParams));
			Exception.Throw();
		}
	}
}

SvOi::ITRCImagePtr TriggerRecord::createNewImageHandle(int pos)
{
	SvOi::ITRCImagePtr pImage;
	auto& rImageController = getImageBufferControllerInstance();
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		const auto& rImageList = m_rImageList.list();
		int*const pImagePos = m_rData.getImagePos();
		if (0 <= pos && rImageList.size() > pos)
		{
			rImageController.decreaseImageRefCounter(pImagePos[pos]);
			pImage = rImageController.createNewImageHandle(rImageList[pos].structid(), pImagePos[pos], m_ResetId);
			if (nullptr == pImage)
			{
				pImagePos[pos] = -1;
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%d"), pos));
				SvStl::MessageManager e(SvStl::MsgType::Log);
				e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_NewBufferFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
	return pImage;
}

void TriggerRecord::initValueData()
{
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		//It is sufficient to set the data size to 0 to invalidate the complete trigger record value data
		std::atomic_int* pSize = reinterpret_cast<std::atomic_int*> (m_rData.getValueData());
		*pSize = 0;
	}
}

void TriggerRecord::writeValueData(const uint8_t* pMemSource, int32_t memBytes)
{
	auto pLock = ResetLocker::lockReset(m_ResetId);
	if (nullptr != pLock)
	{
		uint8_t* pData = m_rData.getValueData();
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		std::string DebugString = SvUl::Format(_T("writeValueData; %d\n"), getId());
		::OutputDebugString(DebugString.c_str());
#endif
		if (nullptr != pData && memBytes < m_dataListSize)
		{
			//The destination start is the size 
			std::atomic_long* pDataSize = reinterpret_cast<std::atomic_long*> (pData);
			//The next position is where the value data list is streamed
			pData = reinterpret_cast<uint8_t*> (pDataSize + 1);
			memcpy(pData, pMemSource, memBytes);
			//The data size needs to be set after the memcpy as only when this is not 0 is the TR valid
			*pDataSize = memBytes;
		}
		else
		{
			SvStl::MessageManager e(SvStl::MsgType::Log);
			e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_CopyValueObjData, SvStl::SourceFileParams(StdMessageParams));
		}
	}
	else
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		std::string DebugString = SvUl::Format(_T("writeValueData: tr not lockable\n"));
		::OutputDebugString(DebugString.c_str());
#endif
		SvStl::MessageManager e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_TRC_GENERAL_ERROR, SvStl::Tid_TRC_Error_CopyValueObjData, SvStl::SourceFileParams(StdMessageParams));
	}
}

_variant_t TriggerRecord::readValue(VARTYPE vtType, const BYTE* pMemBlock) const
{
	_variant_t result;

	result.vt = vtType;
	switch (vtType)
	{
		case VT_BOOL:
			result = _variant_t((0 == *(reinterpret_cast<const short*> (pMemBlock))) ? false : true);
			break;
		case VT_I1:
			result.bVal = *(reinterpret_cast<const char*> (pMemBlock));
			break;
		case VT_I2:
			result.iVal = *(reinterpret_cast<const short*> (pMemBlock));
			break;
		case VT_I4:
			result.lVal = *(reinterpret_cast<const long*> (pMemBlock));
			break;
		case VT_I8:
			result.llVal = *(reinterpret_cast<const long long*> (pMemBlock));
			break;
		case VT_INT:
			result.intVal = *(reinterpret_cast<const int*> (pMemBlock));
			break;
		case VT_UI1:
			result.cVal = *(reinterpret_cast<const unsigned char*> (pMemBlock));
			break;
		case VT_UI2:
			result.uiVal = *(reinterpret_cast<const unsigned short*> (pMemBlock));
			break;
		case VT_UI4:
			result.ulVal = *(reinterpret_cast<const unsigned long*> (pMemBlock));
			break;
		case VT_UI8:
			result.ullVal = *(reinterpret_cast<const unsigned long long*> (pMemBlock));
			break;
		case VT_UINT:
			result.uintVal = *(reinterpret_cast<const unsigned int*> (pMemBlock));
			break;
		case VT_R4:
			result.fltVal = *(reinterpret_cast<const float*> (pMemBlock));
			break;
		case VT_R8:
			result.dblVal = *(reinterpret_cast<const double*> (pMemBlock));
			break;
		case VT_BSTR:
		{
			//For variant set back to VT_EMPTY otherwise SetString will try and delete the string which is not existing 
			result.vt = VT_EMPTY;
			std::string tempString = reinterpret_cast<const char*> (pMemBlock);
			result.SetString(tempString.c_str());
			break;
		}
		//!Note these are special SVObserver variant VT types
		case SvDef::VT_DPOINT:
		{
			result.vt = VT_EMPTY;
			//Type double point has x and y value directly behind each other in memory
			SVPoint<double> point;
			point.m_x = *(reinterpret_cast<const double*> (pMemBlock));
			point.m_y = *(reinterpret_cast<const double*> (pMemBlock + sizeof(double)));
			result.SetString(point.toString().c_str());
			break;
		}
		case SvDef::VT_POINT:
		{
			result.vt = VT_EMPTY;
			//Type long point has x and y value directly behind each other in memory
			SVPoint<long> point;
			point.m_x = *(reinterpret_cast<const long*> (pMemBlock));
			point.m_y = *(reinterpret_cast<const long*> (pMemBlock + sizeof(long)));
			result.SetString(point.toString().c_str());
			break;
		}
		default:
			break;
	}
	return result;
 }

_variant_t TriggerRecord::readArrayValue(VARTYPE vtType, const BYTE* pMemBlock) const
{
	_variant_t result;
	if (vtType == VT_EMPTY)
	{
		return result;
	}
	//For array values the first value is the result size
	int resultSize = *(reinterpret_cast<const int*> (pMemBlock));
	pMemBlock += sizeof(int);
	if (resultSize > 0)
	{
		int elementSize = getElementSize(vtType);

		VARTYPE vtArrayType{vtType};
		if(vtArrayType == SvDef::VT_DPOINT || vtArrayType == SvDef::VT_POINT)
		{
			vtArrayType = VT_BSTR | VT_ARRAY;
		}
		else
		{
			vtArrayType |= VT_ARRAY;
		}
		
		SAFEARRAYBOUND arrayBound;
		arrayBound.lLbound = 0;
		arrayBound.cElements = resultSize;
		result.parray = ::SafeArrayCreate(vtType, 1, &arrayBound);
		result.vt = vtArrayType;
		if (nullptr != result.parray)
		{
			for (long i = 0; i < resultSize; ++i)
			{
				switch(vtType)
				{
					case VT_BSTR:
					{
						std::string tempString = reinterpret_cast<const char*> (pMemBlock);
						_variant_t variantString;
						variantString.SetString(tempString.c_str());
						::SafeArrayPutElement(result.parray, &i, static_cast<void*> V_BSTR(variantString.GetAddress()));
						pMemBlock += (tempString.size() + 1);
						break;
					}
					case SvDef::VT_DPOINT:
					{
						//Type double point has x and y value directly behind each other in memory
						SVPoint<double> point;
						point.m_x = *(reinterpret_cast<const double*> (pMemBlock));
						point.m_y = *(reinterpret_cast<const double*> (pMemBlock + sizeof(double)));
						_variant_t variantString;
						variantString.SetString(point.toString().c_str());
						::SafeArrayPutElement(result.parray, &i, static_cast<void*> V_BSTR(variantString.GetAddress()));
						pMemBlock += elementSize;
						break;
					}
					case SvDef::VT_POINT:
					{
						//Type long point has x and y value directly behind each other in memory
						SVPoint<long> point;
						point.m_x = *(reinterpret_cast<const long*> (pMemBlock));
						point.m_y = *(reinterpret_cast<const long*> (pMemBlock + sizeof(long)));
						_variant_t variantString;
						variantString.SetString(point.toString().c_str());
						::SafeArrayPutElement(result.parray, &i, static_cast<void*> V_BSTR(variantString.GetAddress()));
						pMemBlock += elementSize;
						break;
					}
					default:
					{
						::SafeArrayPutElement(result.parray, &i, static_cast<void*> (const_cast<BYTE*>(pMemBlock)));
						pMemBlock += elementSize;
						break;
					}
				}
			}
		}
	}

	return result;
}

int TriggerRecord::getElementSize(VARTYPE vtType) const
{
	int result{0};

	switch (vtType)
	{
		case VT_BOOL:
		case VT_I2:
		case VT_UI2:
			result = sizeof(short);
			break;
		case VT_I1:
		case VT_UI1:
			result = sizeof(char);
			break;
		case VT_I4:
		case VT_UI4:
		case VT_INT:
		case VT_UINT:
			result = sizeof(long);
			break;
		case VT_I8:
		case VT_UI8:
			result = sizeof(long long);
			break;
		case VT_R4:
			result = sizeof(float);
			break;
		case VT_R8:
			result = sizeof(double);
			break;
		case SvDef::VT_DPOINT:
			//Type double point has x and y value directly behind each other in memory
			result = 2 * sizeof(double);
			break;
		case SvDef::VT_POINT:
			//Type long point has x and y value directly behind each other in memory
			result = 2 * sizeof(long);
			break;
		default:
			break;
	}
	return result;
}


void removeTrReferenceCount(int ipPos, long& rReferenceCount)
{
	long value = InterlockedDecrement(&rReferenceCount);
	if (0 >= value)
	{
		auto* pTRC = getTriggerRecordControllerInstance();
		if (nullptr != pTRC)
		{
			pTRC->increaseNumberOfFreeTr(ipPos);
		}
	}
	if (0 > value)
	{
		InterlockedExchange(&rReferenceCount, 0);
	}
}
} //namespace SvTrc
