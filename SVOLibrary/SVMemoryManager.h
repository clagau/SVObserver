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

class SVObjectClass;

class SVMemoryPool // manages a single pool of memory 
{
public:
	explicit SVMemoryPool(std::mutex& rMutex) : m_rMemoryMutex {rMutex} {}

	HRESULT Create(__int64 lPoolSizeKBytes);

	HRESULT ReservePoolMemory(uint32_t ownerId, __int64 sizeInBytes);
	HRESULT ReleasePoolMemory(uint32_t ownerId);
	std::vector<std::string> poolInfoStrings() const;
	bool hasOwner(uint32_t ownerId);

	__int64 remainingMemoryInBytes() const;
	__int64 SizeOfPoolBytes() const;

private:
	// manages memory in a pool for one owner
	struct SVMemoryPoolEntry
	{
		__int64   lSize {0LL};
		SVMemoryPoolEntry() : lSize(0) {}
	};// end class SVMemoryPoolEntry
	typedef std::map <uint32_t, SVMemoryPoolEntry> SVMemoryPoolEntryMap;

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

	SVMemoryManager():m_pool {m_memoryMutex} {};

	void InitializeMemoryManager(long bufferSize);
	HRESULT ReserveMemory( uint32_t ownerId, __int64 sizeInBytes );
	HRESULT ReleaseMemory( uint32_t ownerId );
	bool hasOwner(uint32_t ownerId) {return m_pool.hasOwner(ownerId);}
	__int64 remainingMemoryInBytes() const ;
	__int64 TotalBytes() const;
	__int64 ReservedBytes()const { return TotalBytes() - remainingMemoryInBytes();}
	std::string MemoryInfoByOwner() const;

private:

	std::mutex m_memoryMutex;
	SVMemoryPool m_pool;

};// end class SVMemoryManager
