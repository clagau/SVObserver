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

constexpr int UseLargerArchiveMemoryPool = 16000;
constexpr int AsyncDefault4GB = 50;
constexpr int AsyncDefault16GB = 200;
constexpr int GoOfflineDefault4GB = 300;
constexpr int GoOfflineDefault16GB = 2000;
constexpr int cKiloByte = 1024;

HRESULT SVMemoryPool::Create(__int64 lPoolSizeKBytes)
{
	m_lPoolSize = lPoolSizeKBytes * 1024;
	return S_OK;
}

HRESULT SVMemoryPool::ReservePoolMemory(void* pOwner, __int64 lSizeInBytes)
{
	HRESULT hr = SVMSG_SVO_32_ARCHIVE_TOOL_OUT_OF_MEMORY;

	if (m_lUsed + lSizeInBytes < m_lPoolSize)
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
	::InterlockedExchangeAdd64(const_cast <PLONGLONG> (&m_lUsed), lSizeInBytes);
	iter->second.lSize += lSizeInBytes;

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

HRESULT SVMemoryPool::ReleasePoolMemory(void* pOwner, long lSizeInBytes)
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolEntryMap::iterator iter = m_mapEntries.find(pOwner);
	if (iter != m_mapEntries.end())
	{
		if (lSizeInBytes >= iter->second.lSize)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
			::OutputDebugString(_T("SVMemoryPool::ReleasePoolMemory %08X - %d >= %d\n"), owner, lSizeInBytes, iter->second.lSize);
#endif
		}

		::InterlockedExchangeAdd64(const_cast <PLONGLONG> (&m_lUsed), -lSizeInBytes);

		iter->second.lSize -= lSizeInBytes;
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(_T("SVMemoryPool::ReleasePoolMemory %08X - %d, remaining = %d\n"), owner, lSizeInBytes, iter->second.lSize);
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

void SVMemoryManager::InitializeMemoryManager(LPCTSTR strPoolName1, LPCTSTR strPoolName2, long goOfflineBufferSize, long asyncBufferSize)
{
	//Get Amount of System Memory
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	DWORDLONG AmountOfRam = (statex.ullTotalPhys / cKiloByte) / cKiloByte;

	// allocate pools in the memory manager

	//Log amount of physical memory - may help in debugging issues in the future.
	SvDef::StringVector MessageList;
	MessageList.push_back(SvUl::Format(_T("%d"), AmountOfRam));
	SvStl::MessageManager Msg(SvStl::MsgType::Log);
	Msg.setMessage(SVMSG_SVO_54_EMPTY, SvStl::Tid_AmountOfSystemMemoryText, MessageList, SvStl::SourceFileParams(StdMessageParams), SvStl::Memory_Log_45001);

	//if amount of physical memory is around 16 GigE allocate the larger memory pools.
	if (0 == goOfflineBufferSize)
	{
		goOfflineBufferSize = (AmountOfRam >= UseLargerArchiveMemoryPool) ? GoOfflineDefault16GB : GoOfflineDefault4GB;
	}
	if (0 == asyncBufferSize)
	{
		asyncBufferSize = (AmountOfRam >= UseLargerArchiveMemoryPool) ? AsyncDefault16GB : AsyncDefault4GB;
	}
	CreatePool(strPoolName1, goOfflineBufferSize * cKiloByte);
	CreatePool(strPoolName2, asyncBufferSize * cKiloByte);
}

HRESULT SVMemoryManager::CreatePool( LPCTSTR strPoolName, __int64 lPoolSizeKBytes )
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(strPoolName) );
	if ( iter == m_mapPools.end() )// ensure it doesn't already exist
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(SvUl::Format(_T("SVMemoryManager::CreatePool(%s, %d)\n"), strPoolName, lPoolSizeKBytes*1024).c_str());
#endif
		auto [it, success] = m_mapPools.insert({std::string(strPoolName), SVMemoryPool {m_memoryMutex}});
		hr = it->second.Create(lPoolSizeKBytes);
	}
	return hr;
}

HRESULT SVMemoryManager::ReservePoolMemory( LPCTSTR strPoolName, void* pOwner, __int64 lSizeInBytes )
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(strPoolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(SvUl::Format(_T("SVMemoryManager::ReservePoolMemory(%s, %08X, %d)\n"), strPoolName, owner, lSizeInBytes).c_str);
#endif
		hr = iter->second.ReservePoolMemory(pOwner, lSizeInBytes );
	}

	return hr;
}

HRESULT SVMemoryManager::ReleasePoolMemory( LPCTSTR strPoolName, void* pOwner )
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(strPoolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(SvUl::Format(_T("SVMemoryManager::ReleasePoolMemory(%s, %08X)\n"), strPoolName, owner).c_str());
#endif
		hr = iter->second.ReleasePoolMemory(pOwner);
	}

	return hr;
}

HRESULT SVMemoryManager::ReleasePoolMemory( LPCTSTR strPoolName, void* pOwner, long lSizeInBytes )
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(strPoolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(SvUl::Format(_T("SVMemoryManager::ReleasePoolMemory(%s, %08X, %d)\n"), strPoolName, owner, lSizeInBytes).c_str());
#endif
		hr = iter->second.ReleasePoolMemory(pOwner, lSizeInBytes );
	}

	return hr;
}

bool SVMemoryManager::CanReservePoolMemory( LPCTSTR strPoolName, __int64 lReserveSizeInBytes )
{
	__int64 lFreeBytes = FreeBytes( strPoolName );
	return lFreeBytes - lReserveSizeInBytes > 0;
}

__int64 SVMemoryManager::FreeBytes( LPCTSTR strPoolName )
{
	__int64 lSize = 0;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(strPoolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
		lSize = iter->second.FreeBytes( );
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		::OutputDebugString(SvUl::Format(_T("SVMemoryManager::FreeBytes(%s, %d)\n"), strPoolName, lSize).c_str());
#endif
	}

	return lSize;
}

__int64 SVMemoryManager::SizeOfPoolBytes( LPCTSTR strPoolName )
{
	__int64 lSize = 0;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(strPoolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
		lSize = iter->second.SizeOfPoolBytes();
	}

	return lSize;
}

