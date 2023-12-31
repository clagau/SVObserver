//******************************************************************************
//* COPYRIGHT (c) 2005 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMemoryManager
//* .File Name       : $Workfile:   SVMemoryManager.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:36:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMemoryManager.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/TextDefinesSvDef.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#pragma endregion Includes

HRESULT SVMemoryPool::Create(__int64 lPoolSizeKBytes)
{
	m_lPoolSize = lPoolSizeKBytes * SvDef::cBytesPerKilobyte;
	return S_OK;
}

HRESULT SVMemoryPool::ReservePoolMemory(uint32_t ownerId, __int64 sizeInBytes)
{
	HRESULT hr = SVMSG_SVO_32_ARCHIVE_TOOL_OUT_OF_MEMORY;

	if (m_lUsed + sizeInBytes < m_lPoolSize)
	{
		hr = S_OK;
	}

	//Add the amount of memory even if you are over the limit.  The Archive Tool Adjustment Dialog will handle negative memory properly
	std::lock_guard<std::mutex> lock(m_rMemoryMutex);
	::InterlockedExchangeAdd64(const_cast <PLONGLONG> (&m_lUsed), sizeInBytes);
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY_POOL)
	
	::OutputDebugString(std::format(_T("Reserving {:8.2f} MB for '{}': "), sizeInBytes / (double)(SvDef::cBytesPerMegabyte), getCompleteObjectNameForId(ownerId).c_str()).c_str());
#endif
	m_mapEntries[ownerId].lSize += sizeInBytes;

	return hr;
}

HRESULT SVMemoryPool::ReleasePoolMemory(uint32_t ownerId)
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolEntryMap::iterator iter = m_mapEntries.find(ownerId);
	if (iter != m_mapEntries.end())
	{
		::InterlockedExchangeAdd64(const_cast <PLONGLONG> (&m_lUsed), -iter->second.lSize);

#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY_POOL)
		::OutputDebugString(std::format(_T("{:8.2f} MB released by '{}': "), iter->second.lSize/ (double)(SvDef::cBytesPerMegabyte), getCompleteObjectNameForId(ownerId)).c_str());
#endif
		std::lock_guard<std::mutex> lock(m_rMemoryMutex);
		m_mapEntries.erase(m_mapEntries.find(ownerId));
		hr = S_OK;
	}
	else
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE) || defined (TRACE_MEMORY_POOL)
		::OutputDebugString(std::format(_T("Cannot release memory - '{}' NOT FOUND\n"), getCompleteObjectNameForId(ownerId)).c_str());
#endif
	}
	return hr;
}

__int64 SVMemoryPool::remainingMemoryInBytes() const
{
	return m_lPoolSize - m_lUsed;
}

__int64 SVMemoryPool::SizeOfPoolBytes() const
{
	return m_lPoolSize;
}


std::vector<std::string> SVMemoryPool::poolInfoStrings() const
{
	auto first = std::format(SvDef::cAvailableImageMemory, static_cast<double>(m_lPoolSize) / SvDef::cBytesPerMegabyte);
	std::vector<std::string> memoryInformation {first};

	std::ranges::transform(m_mapEntries, back_inserter(memoryInformation), [](const auto& rOwnerAndSize)
		{
			auto info = std::format(SvDef::cMegabytesUsed, static_cast<double>(rOwnerAndSize.second.lSize) / SvDef::cBytesPerMegabyte, getCompleteObjectNameForId(rOwnerAndSize.first));
			return info;
		});

	std::string info = std::format(SvDef::cMegabytesRemaining, static_cast<double>(remainingMemoryInBytes()) / SvDef::cBytesPerMegabyte);

	memoryInformation.emplace_back(info);

	return memoryInformation;
}

bool SVMemoryPool::hasOwner(uint32_t ownerId)
{
	return m_mapEntries.find(ownerId) != m_mapEntries.end();
}

SVMemoryManager& SVMemoryManager::Instance()
{
	static SVMemoryManager object;

	return object;
}

void SVMemoryManager::InitializeMemoryManager(long bufferSize)
{
	//Get Amount of System Memory
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	DWORDLONG ramSizeInMB = statex.ullTotalPhys / SvDef::cBytesPerMegabyte;

	//Log amount of physical memory - may help in debugging issues in the future.
	SvDef::StringVector MessageList;
	MessageList.push_back(std::format(_T("{:d} MB"), ramSizeInMB));
	SvStl::MessageManager Msg(SvStl::MsgType::Log);
	Msg.setMessage(SVMSG_SVO_54_EMPTY, SvStl::Tid_AmountOfSystemMemoryText, MessageList, SvStl::SourceFileParams(StdMessageParams));

	m_pool.Create(bufferSize * SvDef::cBytesPerKilobyte);
}

HRESULT SVMemoryManager::ReserveMemory( uint32_t ownerId, __int64 sizeInBytes )
{
	HRESULT result = m_pool.ReservePoolMemory(ownerId, sizeInBytes);

#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY_POOL)
	::OutputDebugString(std::format(_T("{:8.2f} MB free"), SVMemoryManager::Instance().remainingMemoryInBytes() / (double)(SvDef::cBytesPerMegabyte)).c_str());

	if (S_OK != result)
	{
		::OutputDebugString(_T(": Failed"));
	}

	::OutputDebugString(_T(".\n"));
#endif

	return result;
}

HRESULT SVMemoryManager::ReleaseMemory( uint32_t ownerId )
{
	HRESULT result = m_pool.ReleasePoolMemory(ownerId);

#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY_POOL)
	::OutputDebugString(std::format(_T("{:8.2f} MB free"), SVMemoryManager::Instance().remainingMemoryInBytes() / (double)(SvDef::cBytesPerMegabyte)).c_str());

	if (S_OK != result)
	{
		::OutputDebugString(_T(": Failed"));
	}

	::OutputDebugString(_T(".\n"));
#endif

	return result;
}

__int64 SVMemoryManager::remainingMemoryInBytes() const
{
	auto lSize = m_pool.remainingMemoryInBytes();
	return lSize;
}

__int64 SVMemoryManager::TotalBytes() const
{
	return m_pool.SizeOfPoolBytes();
}


std::string SVMemoryManager::MemoryInfoByOwner() const
{
	auto poolInfo = m_pool.poolInfoStrings();
	return std::accumulate(poolInfo.begin(), poolInfo.end(), std::string(), [](const auto& a, const auto& b){return a + b; });
}