//*****************************************************************************
/// \copyright COPYRIGHT (c) 2019 by Seidenader Maschinenbau GmbH
/// \file SharedMemoryStructs.cpp
/// All Rights Reserved
//*****************************************************************************
/// Structs for the shared memory data.
//******************************************************************************
#pragma once

namespace SvTrc
{

struct CommonDataStruct
{
	volatile long m_resetId = 0; //id of the last reset
	volatile long m_resetLockCounter = 0; //counter of current used methods of ITriggerRecordR-instance 
	int m_imageRefCountSize = 0; //the numbers of refCounts reserved in m_imageRefCountArray.
	int m_inspectionListPBSize = 0;
	int m_imageStructListPBSize = 0;
	volatile unsigned __int64 m_pauseTRofInterest[2] = {0};
};

struct SMData
{
	int m_maxImageListSize = 1000;
	int m_imageListSize = 0;
	int m_maxDataDefListSize = 10000;
	int m_dataDefListSize = 0;
	int m_maxTriggerRecordBufferSize = 100'000;
};

constexpr LPCTSTR cCommonParameterSM = _T("SVO-CommonParameter");
constexpr int cMaxInspectionPbSize = 1000;
constexpr int cMaxImageStructPbSize = 10'000;
}
