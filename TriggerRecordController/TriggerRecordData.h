//*****************************************************************************
/// \copyright COPYRIGHT (c) 2019 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file TriggerRecordData.h
/// All Rights Reserved
//*****************************************************************************
/// The TriggerRecordData: class to handle the triggerRecord data.
//******************************************************************************
#pragma once
#include "ObjectInterfaces/ITriggerRecordR.h"


namespace SvTrc
{
struct TriggerRecordData
{
	//All constructor are deleted, because this struct should not create with the normal way. 
	// It must be allocated a buffer with space behind this struct for further data (image position and data)
	TriggerRecordData() = delete;
	TriggerRecordData(const TriggerRecordData&) = delete;
	TriggerRecordData& operator = (const TriggerRecordData&) = delete;

	void init(int imageCount, int hiddenImageCount)
	{
		m_trId = cInvalidTrId;
		m_referenceCount = cInvalidData;
		m_isInterest = false;
		m_ImageCount = imageCount;
		m_HiddenImageCount = hiddenImageCount;
		m_triggerData = SvOi::TriggerData();
	}

	int* const getImagePos()
	{
		return reinterpret_cast<int*const>(reinterpret_cast<char*>(this) + sizeof(TriggerRecordData));
	}

	const int* const getImagePos() const
	{
		return reinterpret_cast<const int*const>(reinterpret_cast<const char*>(this) + sizeof(TriggerRecordData));
	}

	int* const getHiddenImagePos()
	{
		return reinterpret_cast<int* const>(reinterpret_cast<char*>(this) + sizeof(TriggerRecordData) + sizeof(int) * m_ImageCount);
	}

	const int* const getHiddenImagePos() const
	{
		return reinterpret_cast<const int* const>(reinterpret_cast<const char*>(this) + sizeof(TriggerRecordData) + sizeof(int) * m_ImageCount);
	}

	uint8_t* getValueData()
	{
		uint8_t* pResult = reinterpret_cast<uint8_t*>(this) + sizeof(TriggerRecordData);
		pResult += sizeof(int) * (m_ImageCount+m_HiddenImageCount);
		return pResult;
	}

	int m_trId = cInvalidTrId; //This should be unique ID for the trigger record
	long m_referenceCount = cInvalidData; //The ReferenceCount for this TriggerRecord. m_InvalidData = invalid; m_WriteBlocked = write blocked; 0 = readable, but can be "deleted"; >0 = readable and blocked 
	int m_ImageCount {0};
	int m_HiddenImageCount {0};
	bool m_isInterest{ false };
	SvOi::TriggerData m_triggerData;

	static const int cInvalidTrId = -1;
	static const int cInvalidData = -1;
	static const int cWriteBlocked = -2;
};
}