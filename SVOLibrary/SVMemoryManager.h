//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMemoryManager
//* .File Name       : $Workfile:   SVMemoryManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   23 Jul 2014 11:36:32  $
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

// currently SVMemoryPool is thread-safe dealing with multiple owners,
// but an individual owner is not thread-safe.
// this may be changed if needed.
// just make the iter->second.lSize operations use the InterlockedExchange APIs.
class SVMemoryPool // manages a single pool of memory
{
public:
	explicit SVMemoryPool(std::mutex& rMutex) : m_rMemoryMutex {rMutex} {}

	HRESULT Create(__int64 lPoolSizeKBytes);

	HRESULT ReservePoolMemory(void* pOwner, __int64 sizeInBytes);

	HRESULT ReleasePoolMemory(void* pOwner);
	HRESULT ReleasePoolMemory(void* pOwner, long sizeInBytes);

	__int64 FreeBytes();
	__int64 SizeOfPoolBytes();

private:
	// manages memory in a pool for one owner
	struct SVMemoryPoolEntry
	{
		__int64   lSize {0LL};
		SVMemoryPoolEntry() : lSize(0) {}
	};// end class SVMemoryPoolEntry
	typedef std::pair <void*, SVMemoryPoolEntry> SVMemoryPoolEntryPair;
	typedef std::map <void*, SVMemoryPoolEntry> SVMemoryPoolEntryMap;

	SVMemoryPoolEntryMap m_mapEntries;
	volatile __int64 m_lPoolSize {0LL};
	volatile __int64 m_lUsed {0LL};

public:
	SVMemoryPool(const SVMemoryPool& rhs) : m_rMemoryMutex {rhs.m_rMemoryMutex}, m_mapEntries(rhs.m_mapEntries)
	{
		m_lPoolSize = rhs.m_lPoolSize;
		m_lUsed = rhs.m_lUsed;
	}

	SVMemoryPool& operator = (const SVMemoryPool& rhs)
	{
		if (this != &rhs)
		{
			m_mapEntries = rhs.m_mapEntries;
			m_lPoolSize = rhs.m_lPoolSize;
			m_lUsed = rhs.m_lUsed;
		}
		return *this;
	}

	std::mutex& m_rMemoryMutex;
};// end class SVMemoryPool

// manages multiple virtual pools of memory
// no memory is actually allocated
// this just enables the app to put a cap on
// the amount of memory that function X / tool X uses
class SVMemoryManager
{
public:
	static SVMemoryManager& Instance();

	void InitializeMemoryManager(LPCTSTR poolName, long bufferSize);
	HRESULT CreatePool( LPCTSTR poolName, __int64 lPoolSizeKBytes );
	HRESULT ReservePoolMemory( LPCTSTR poolName, void* pOwner, __int64 sizeInBytes );
	bool    CanReservePoolMemory( LPCTSTR poolName, __int64 sizeInBytes );
	HRESULT ReleasePoolMemory( LPCTSTR poolName, void* pOwner );
	HRESULT ReleasePoolMemory( LPCTSTR poolName, void* pOwner, long sizeInBytes );
	__int64 FreeBytes( LPCTSTR poolName );
	__int64 SizeOfPoolBytes( LPCTSTR poolName );
	__int64 ReservedBytes( LPCTSTR poolName ){ return SizeOfPoolBytes(poolName) - FreeBytes(poolName);}

private:
	typedef std::map <std::string, SVMemoryPool> SVMemoryPoolMap;
	SVMemoryPoolMap m_mapPools;
	std::mutex m_memoryMutex;
};// end class SVMemoryManager

