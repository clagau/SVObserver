//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

constexpr int cBytesPerKiloByte = 1024;

HRESULT SVMemoryPool::Create(__int64 lPoolSizeKBytes)
{
	m_lPoolSize = lPoolSizeKBytes * 1024;
	return S_OK;
}

HRESULT SVMemoryPool::ReservePoolMemory(void* pOwner, __int64 sizeInBytes)
{
	HRESULT hr = SVMSG_SVO_32_ARCHIVE_TOOL_OUT_OF_MEMORY;

	if (m_lUsed + sizeInBytes < m_lPoolSize)
	{
		hr = S_OK;
	}

	//Add the amount of Memory even if you are over the limit.  The Archive Tool Dialog will handle negative memory properly
	typename SVMemoryPoolEntryMap::iterator iter = m_mapEntries.find(pOwner);
	if (iter == m_mapEntries.end())
	{
		std::lock_guard<std::mutex> lock(m_rMemoryMutex);
		iter = m_mapEntries.insert(SVMemoryPoolEntryPair(pOwner, SVMemoryPoolEntry())).first;
	}
	::InterlockedExchangeAdd64(const_cast <PLONGLONG> (&m_lUsed), sizeInBytes);
	iter->second.lSize += sizeInBytes;

	return hr;
}

HRESULT SVMemoryPool::ReleasePoolMemory(void* pOwner)
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolEntryMap::iterator iter = m_mapEntries.find(pOwner);
	if (iter != m_mapEntries.end())
	{
		::InterlockedExchangeAdd64(const_cast <PLONGLONG> (&m_lUsed), -iter->second.lSize);

#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(_T("SVMemoryPool::ReleasePoolMemory %08X - %d\n"), owner, iter->second.lSize);
#endif
		std::lock_guard<std::mutex> lock(m_rMemoryMutex);
		m_mapEntries.erase(m_mapEntries.find(pOwner));
		hr = S_OK;
	}
	else
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(_T("SVMemoryPool::ReleasePoolMemory %08X - NOT FOUND\n"), owner);
#endif
	}
	return hr;
}

HRESULT SVMemoryPool::ReleasePoolMemory(void* pOwner, long sizeInBytes)
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolEntryMap::iterator iter = m_mapEntries.find(pOwner);
	if (iter != m_mapEntries.end())
	{
		if (sizeInBytes >= iter->second.lSize)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
			::OutputDebugString(_T("SVMemoryPool::ReleasePoolMemory %08X - %d >= %d\n"), owner, sizeInBytes, iter->second.lSize);
#endif
		}

		::InterlockedExchangeAdd64(const_cast <PLONGLONG> (&m_lUsed), -sizeInBytes);

		iter->second.lSize -= sizeInBytes;
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(_T("SVMemoryPool::ReleasePoolMemory %08X - %d, remaining = %d\n"), owner, sizeInBytes, iter->second.lSize);
#endif
		if (iter->second.lSize <= 0)	// check less than for safety
		{
			std::lock_guard<std::mutex> lock(m_rMemoryMutex);
			m_mapEntries.erase(iter);
			hr = S_OK;
		}
	}
	else
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		::OutputDebugString(SvUl::Format(_T("SVMemoryPool::ReleasePoolMemory %08X - NOT FOUND\n"), pOwner).c_str());
#endif
	}
	return hr;
}

__int64 SVMemoryPool::FreeBytes()
{
	return m_lPoolSize - m_lUsed;
}

__int64 SVMemoryPool::SizeOfPoolBytes()
{
	return m_lPoolSize;
}

SVMemoryManager& SVMemoryManager::Instance()
{
	static SVMemoryManager object;

	return object;
}

void SVMemoryManager::InitializeMemoryManager(LPCTSTR poolName, long bufferSize)
{
	//Get Amount of System Memory
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	DWORDLONG ramSizeInMB = (statex.ullTotalPhys / cBytesPerKiloByte) / cBytesPerKiloByte;

	// 'allocate' a pool in the 'memory manager'

	//Log amount of physical memory - may help in debugging issues in the future.
	SvDef::StringVector MessageList;
	MessageList.push_back(SvUl::Format(_T("%d MB"), ramSizeInMB));
	SvStl::MessageManager Msg(SvStl::MsgType::Log);
	Msg.setMessage(SVMSG_SVO_54_EMPTY, SvStl::Tid_AmountOfSystemMemoryText, MessageList, SvStl::SourceFileParams(StdMessageParams), SvStl::Memory_Log_45001);

	CreatePool(poolName, bufferSize * cBytesPerKiloByte);
}

HRESULT SVMemoryManager::CreatePool( LPCTSTR poolName, __int64 lPoolSizeKBytes )
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(poolName) );
	if ( iter == m_mapPools.end() )// ensure it doesn't already exist
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(SvUl::Format(_T("SVMemoryManager::CreatePool(%s, %d)\n"), poolName, lPoolSizeKBytes*1024).c_str());
#endif
		auto [it, success] = m_mapPools.insert({std::string(poolName), SVMemoryPool {m_memoryMutex}});
		hr = it->second.Create(lPoolSizeKBytes);
	}
	return hr;
}

HRESULT SVMemoryManager::ReservePoolMemory( LPCTSTR poolName, void* pOwner, __int64 sizeInBytes )
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(poolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(SvUl::Format(_T("SVMemoryManager::ReservePoolMemory(%s, %08X, %d)\n"), poolName, owner, sizeInBytes).c_str);
#endif
		hr = iter->second.ReservePoolMemory(pOwner, sizeInBytes );
	}

	return hr;
}

HRESULT SVMemoryManager::ReleasePoolMemory( LPCTSTR poolName, void* pOwner )
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(poolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(SvUl::Format(_T("SVMemoryManager::ReleasePoolMemory(%s, %08X)\n"), poolName, owner).c_str());
#endif
		hr = iter->second.ReleasePoolMemory(pOwner);
	}

	return hr;
}

HRESULT SVMemoryManager::ReleasePoolMemory( LPCTSTR poolName, void* pOwner, long sizeInBytes )
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(poolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(SvUl::Format(_T("SVMemoryManager::ReleasePoolMemory(%s, %08X, %d)\n"), poolName, owner, sizeInBytes).c_str());
#endif
		hr = iter->second.ReleasePoolMemory(pOwner, sizeInBytes );
	}

	return hr;
}

bool SVMemoryManager::CanReservePoolMemory( LPCTSTR poolName, __int64 lReserveSizeInBytes )
{
	__int64 lFreeBytes = FreeBytes( poolName );
	return lFreeBytes - lReserveSizeInBytes > 0;
}

__int64 SVMemoryManager::FreeBytes( LPCTSTR poolName )
{
	__int64 lSize = 0;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(poolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
		lSize = iter->second.FreeBytes( );
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(SvUl::Format(_T("SVMemoryManager::FreeBytes(%s, %d)\n"), poolName, lSize).c_str());
#endif
	}

	return lSize;
}

__int64 SVMemoryManager::SizeOfPoolBytes( LPCTSTR poolName )
{
	__int64 lSize = 0;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(poolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
		lSize = iter->second.SizeOfPoolBytes();
	}

	return lSize;
}

